/*
	Little Smalltalk 
		pass 2 of the parser

		timothy a. budd, 10/84

*/
/*
	The source code for the Little Smalltalk System may be freely
	copied provided that the source of all files is acknowledged
	and that this condition is copied with each file.

	The Little Smalltalk System is distributed without responsibility
	for the performance of the program and without any guarantee of
	maintenance.

	All questions concerning Little Smalltalk should be addressed to:

		Professor Tim Budd
		Department of Computer Science
		Oregon State University
		Corvallis, Oregon
		97331
		USA
*/
# include <stdio.h>
# include "env.h"
# include "drive.h"
# include "cmds.h"
# include "parser.h"
# include "y.tab.h"

extern int maxcontext;
extern char *filename;
extern FILE *ofd;

static int inblock = 0;
static int topstack = 0;
static int maxstack = 0;
# define bumpstk() if (++topstack > maxstack) maxstack = topstack
# define popstk(i) topstack -= i

genclass(clinfo, mlist)
struct classstruct *clinfo;
struct methodstruct *mlist;
{	int i;
	struct methodstruct *m, *n;

	topstack = 0;
	maxstack = 0;

	/* first find out how many methods have been declared */
	/* also check for multiply defined methods */
	for (m = mlist, i = 0; m; i++, m = m->nextmethod)
		for (n = m->nextmethod; n; n = n->nextmethod)
			if (streq((m->pattern)->cmdname ,
				(n->pattern)->cmdname))
				yerr("%s multiply defined",
					(m->pattern)->cmdname);

	fprintf(ofd,"temp <- <primitive 110 %d >\n", i);

	/* next print out each method */
	for (m = mlist, i = 1; m; i++, m = m->nextmethod) {
		fprintf(ofd,"<primitive 112 temp %d\t\t\" %s \" \\\n", 
			i, (m->pattern)->cmdname);
		topstack = 0;
		genmeth(m);
		}

	/* finally print out class definition stuff */
	fprintf(ofd,"<primitive 98 #%s \\\n", clinfo->name);
	fprintf(ofd,"\t<primitive 97 #%s #%s #%s \\\n", 
		clinfo->name, clinfo->super, filename);
	fprintf(ofd,"\t#( ");
	if (instvars) prvars(instvars);
	fprintf(ofd," ) \\\n");
	fprintf(ofd,"\t#( ");
	for (m = mlist; m; m = m->nextmethod)
		fprintf(ofd,"#%s ", (m->pattern)->cmdname);
	fprintf(ofd," ) \\\n");
	fprintf(ofd,"\ttemp %d %d > >\n\n", 1 + maxcontext, 1 + maxstack);
}

prvars(v)
struct varstruct *v;
{
	if (v->nextvar)
		prvars(v->nextvar);
	fprintf(ofd," #%s", v->text);
}

static int codetop = 0;
static uchar code[1000];

static gencode(value)
int value;
{
   if (value >= 256) {
	yerr("code word too big: %d", value);
	}
   code[codetop++] = itouc(value);
}

static genhighlow(high, low)
int high, low;
{
   if (high < 0 || high > 16)
	yerr("genhighlow error: %d", high);
   if (low < 16) gencode(high * 16 + low);
   else {
      gencode(TWOBIT * 16 + high);
      gencode(low);
      }
}

static struct litstruct *literals[100];
int littop = 0;

int litcomp(l1, l2)
struct litstruct *l1, *l2;
{
   if (l1->littype != l2->littype) return(0);
   switch(l1->littype) {
      case charlit: if (l1->ll.litchar != l2->ll.litchar) return(0); break;
      case numlit: if (l1->ll.litint != l2->ll.litint) return(0); break;
      case fnumlit: if (l1->ll.litstr != l2->ll.litstr) return(0); break;
      case strlit:  if (strcmp(l1->ll.litstr, l2->ll.litstr)) return(0); break;
      case symlit:  if (strcmp(l1->ll.litsym, l2->ll.litsym)) return(0); break;
      default: return(0);
      }
   return(1);
}

int genlitix(l)
struct litstruct *l;
{	int i;

	for (i = 0; i < littop; i++)
		if (litcomp(l, literals[i]))
			return(i);
	i = littop;
	literals[littop++] = l;
	return(i);
}

static printalit(lit)
struct litlist *lit;
{
   if (lit) {
      if(lit->nextlit)
        printalit(lit->nextlit);
      printlit(lit->litele);
      }
}

printlit(lit)
struct litstruct *lit;
{
   if (lit)
      switch(lit->littype) {
         case numlit: fprintf(ofd,"%d ", lit->ll.litint); break;
         case fnumlit: fprintf(ofd,"%s ", lit->ll.litstr); break;
         case charlit: fprintf(ofd,"$%c ", lit->ll.litchar); break;
         case strlit:  fprintf(ofd,"\'%s\' ", lit->ll.litstr); break;
         case symlit:  fprintf(ofd,"#%s ", lit->ll.litsym); break;
         case arlit:   fprintf(ofd,"#( ");
                       printalit(lit->ll.litarry);
                       fprintf(ofd,") ");
                       break;
         default: yerr("unknown literal type %d", lit->littype);
         }
}

genmeth(m)
struct methodstruct *m;
{ 	int i;

	fprintf(ofd,"\t#( #[");
	codetop = littop = 0;
	genstate(m->states, 1);
	genhighlow(SPECIAL, SELFRETURN);
	for (i = 0; i < codetop; i++){
	   	fprintf(ofd," %d", uctoi(code[i]));
		if (i % 15 == 14) fprintf(ofd," \\\n");
		}
	fprintf(ofd,"] \\\n");
	fprintf(ofd,"\t#( ");
	for (i = 0; i < littop; i++)
		printlit(literals[i]);
	fprintf(ofd," ) ) >\n\n");
}

genstate(s, doret)
struct statestruct *s;
int doret;
{
	if (s->nextstate)
		genstate(s->nextstate, doret);
	switch(s->statetype) {
		default:
			yerr("unknown case in genstate %d", s->statetype);
		case blkupar:
			gensexpr(s->nn.stateexpr);
			if (inblock)
				genhighlow(SPECIAL, BLOCKRETURN);
			else
				genhighlow(SPECIAL, RETURN);
			popstk(1);
			break;
		case upar:
			gensexpr(s->nn.stateexpr);
			if (doret)
				genhighlow(SPECIAL, RETURN);
			popstk(1);
			break;
		case iasgn:
			gensexpr(s->nn.stateexpr);
			genhighlow(POPINSTANCE, s->mm.varpos);
			popstk(1);
			break;
		case casgn:
			gensexpr(s->nn.stateexpr);
			genhighlow(POPTEMP, s->mm.varpos);
			popstk(1);
			break;
		case expr:
			genexpr(s->nn.cmd);
			genhighlow(SPECIAL, POPSTACK);
			popstk(1);
			break;
		}
}

gensexpr(s)
struct statestruct *s;
{
	switch(s->statetype) {
		default:
			yerr("unknown state in gensexpr %d", s->statetype);
		case iasgn:
			gensexpr(s->nn.stateexpr);
			genhighlow(SPECIAL, DUPSTACK);
			bumpstk();
			genhighlow(POPINSTANCE, s->mm.varpos);
			popstk(1);
			break;
		case casgn:
			gensexpr(s->nn.stateexpr);
			genhighlow(SPECIAL, DUPSTACK);
			bumpstk();
			genhighlow(POPTEMP, s->mm.varpos);
			popstk(1);
			break;
		case expr:
			genexpr(s->nn.cmd);
			break;
		}
}

int supertest(rec)
struct exprstruct *rec;
{	struct objstruct *o;

	if (rec->cmdtype != reccmd)
		return(0);
	o = rec->cc.recobj;
	if (o->objtype != pseuobj)
		return(0);
	if (o->ee.pseuinfo == supervar)
		return(1);
	return(0);
}

int isblock(e)
struct exprstruct *e;
{	struct objstruct *o;

	if (e->cmdtype != reccmd)
		return(0);
	o = e->cc.recobj;
	if (o->objtype != blockobj)
		return(0);
	return(1);
}

genbarg(e)
struct exprstruct *e;
{
	if (isblock(e)) {
		genstate(((e->cc.recobj)->ee.blockinfo)->bstates, 0);
		}
	else {
		genexpr(e);
		genhighlow(UNSEND, VALUECMD);
		}
}

fixjump(loc)
int loc;
{	int size;
	
	size = (codetop - loc) - 1;
	if (size > 255)
		yerr("block too big %d", size);
	code[loc] = itouc(size);
}

int gencond(message, e)
char *message;
struct exprstruct *e;
{	struct keylist *k;
	int i, j;

 	k = e->cc.keys;
	i = 0;
	if ((i = streq(message, "ifTrue:")) || streq(message, "ifFalse:")) {
		genhighlow(SPECIAL, i ? SKIPFALSEPUSH : SKIPTRUEPUSH);
		i = codetop;
		gencode(0);
		genbarg(k->arg);
		fixjump(i);
		return(1);
		}
	if ((i = streq(message, "ifTrue:ifFalse:")) || 
			streq(message, "ifFalse:ifTrue:")) {
		genhighlow(SPECIAL, i ? SKIPFALSEPUSH : SKIPTRUEPUSH);
		i = codetop;
		gencode(0);
		genbarg((k->nextkey)->arg);
		genhighlow(SPECIAL, SKIPFORWARD);
		j = codetop;
		gencode(0);
		fixjump(i);
		genhighlow(SPECIAL, POPSTACK);
		popstk(1);
		genbarg(k->arg);
		fixjump(j);
		return(1);
		}
	if ((i = streq(message, "and:")) || streq(message, "or:")) {
		genhighlow(SPECIAL, i ? SKIPF : SKIPT);
		i = codetop;
		gencode(0);
		genbarg(k->arg);
		fixjump(i);
		return(1);
		}
	if ((j = streq(message, "whileTrue:")) ||
			streq(message, "whileFalse:")) {
		i = codetop;
		genbarg(e->receiver);
		genhighlow(SPECIAL, j ? SKIPFALSEPUSH : SKIPTRUEPUSH);
		j = codetop;
		gencode(0);
		genbarg(k->arg);
		genhighlow(SPECIAL, POPSTACK);
		popstk(1);
		genhighlow(SPECIAL, SKIPBACK);
		/* add one because bytecount pointer already advanced */
		gencode((codetop - i) + 1);
		fixjump(j);
		return(1);
		}
	return(0);
}

int genexpr(e)
struct exprstruct *e;
{	char *message = e->cmdname;
	char **p;
	int  i, numargs, s;
	YYSTYPE ex;
	struct litstruct *mklit();

	if (e->cmdtype != reccmd)
		s = supertest(e->receiver);
	switch(e->cmdtype) {
		default:
			yerr("unknown state in genexpr %d", e->cmdtype);
		case reccmd:
			genobj(e->cc.recobj);
			return 0;
		case semiend:
			genexpr(e->receiver);
			genhighlow(SPECIAL, POPSTACK);
			popstk(1);
			return 0;
		case semistart:
			genexpr(e->receiver);
			genhighlow(SPECIAL, DUPSTACK);
			bumpstk();
			return 0;
		case uncmd:
			genexpr(e->receiver);
			numargs = 0;
			break;
		case bincmd:
			genexpr(e->receiver);
			numargs = 1;
			genexpr(e->cc.argument);
			break;
		case keycmd:
			if ((!s) && isblock(e->receiver) &&
				(streq(message, "whileTrue:") ||
				 streq(message, "whileFalse:")))
				 if (gencond(message, e))
					return 0;
			genexpr(e->receiver);
			if ((!s) && ((streq(message, "ifTrue:")) ||
				(streq(message, "ifFalse:")) ||
				(streq(message, "and:")) ||
				(streq(message, "or:")) ||
				(streq(message, "ifTrue:ifFalse:")) ||
				(streq(message, "ifFalse:ifTrue:"))))
				if (gencond(message, e))
					return 0;
			numargs = genkargs(e->cc.keys);
			break;
		}
	if (s) {	/* message to super */
		genhighlow(SUPERSEND, numargs);
		popstk(numargs - 1);
		ex.c = message;
		gencode(genlitix(mklit(symlit, &ex)));
		return 0;
		}
	for (p = unspecial, i = 0; *p; i++, p++)
		if (strcmp(*p, message) == 0) {
			genhighlow(UNSEND, i);
			return 0;
			}
	for (p = binspecial, i = 0; *p; i++, p++)
		if (strcmp(*p, message) == 0) {
			genhighlow(BINSEND, i);
			popstk(1);
			return 0;
			}
	for (p = arithspecial, i = 0; *p; i++, p++)
		if (strcmp(*p, message) == 0) {
			genhighlow(ARITHSEND, i);
			popstk(1);
			return 0;
			}
	for (p = keyspecial, i = 0; *p; i++, p++)
		if (strcmp(*p, message) == 0) {
			genhighlow(KEYSEND, i);
			popstk(2);
			return 0;
			}
	genhighlow(SEND, numargs);
	popstk(numargs - 1);
	ex.c = message;
	gencode(genlitix(mklit(symlit, &ex)));
}

int genkargs(kl)
struct keylist *kl;
{	int i;
	
	if (kl->nextkey)
		i = genkargs(kl->nextkey);
	else i = 0;
	genexpr(kl->arg);
	return(i + 1);
}

genobj(o)
struct objstruct *o;
{
	switch(o->objtype) {
		default:
			yerr("unknown state in genobj %d", o->objtype);
		case classobj:
			genspclass(o->ee.litinfo);
			break;
		case instvarobj:
			genhighlow(PUSHINSTANCE, o->ee.varoffset);
			bumpstk();
			break;
		case contvarobj:
			genhighlow(PUSHTEMP, o->ee.varoffset);
			bumpstk();
			break;
		case litobj:
			genlit(o->ee.litinfo);
			break;
		case pseuobj:
			genpseu(o->ee.pseuinfo);
			break;
		case primobj:
			genprim(o->ee.priminfo);
			break;
		case exprobj:
			gensexpr(o->ee.stateinfo);
			break;
		case blockobj:
			genblock(o->ee.blockinfo);
			break;
		}
}

int genspclass(litinfo)
struct litstruct *litinfo;
{	int i;
	char **p, *name;

	if (litinfo->littype != symlit)
		yerr("can't happen in genspclass %d", litinfo->littype);
	name = litinfo->ll.litsym;
	for (p = classpecial, i = 30; *p; i++, p++)
		if (strcmp(name, *p) == 0) {
			genhighlow(PUSHSPECIAL, i);
			bumpstk();
			return 0;
			}
	genhighlow(PUSHCLASS, genlitix(litinfo));
	bumpstk();
}

genpseu(p)
enum pseuvars p;
{

	switch(p) {
		default: yerr("unknown state in genpseu %d", p);
		case truevar:  genhighlow(PUSHSPECIAL, 11); break;
		case falsevar: genhighlow(PUSHSPECIAL, 12); break;
		case nilvar:   genhighlow(PUSHSPECIAL, 13); break;
		case smallvar: genhighlow(PUSHSPECIAL, 14); break;
		case procvar:  genhighlow(PUSHSPECIAL, 15); break;
		case supervar:
		case selfvar:  genhighlow(PUSHTEMP, 0); break;
		}
	bumpstk();
}

int genpargs(p)
struct primlist *p;
{	int i;

	if (p) {
		i = 1 + genpargs(p->nextprim);
		genobj(p->pobject);
		}
	else i = 0;
	return(i);
}

genprim(p)
struct primstruct *p;
{	int i;

	i = genpargs(p->plist);
	genhighlow(SPECIAL, PRIMCMD);
	popstk(i-1);
	gencode(i);
	/*genhighlow(9, i); TEST- DEBUG*/
	gencode(p->primnumber);
}

genlit(l)
struct litstruct *l;
{	int i;

	if (l->littype == numlit) {
		i = l->ll.litint;
		if (i == -1)
			genhighlow(PUSHSPECIAL, 10);
		else if (i >= 0 && i < 10)
			genhighlow(PUSHSPECIAL, i);
		else if ((i > 15) && (i < 30))
			genhighlow(PUSHSPECIAL, i);
		else if ((i > 60) && (i < 256))
			genhighlow(PUSHSPECIAL, i);
		else
			genhighlow(PUSHLIT, genlitix(l));

		}
	else
		genhighlow(PUSHLIT, genlitix(l));
	bumpstk();
}

genblock(b)
struct blockstruct *b;
{	int i, size, bsave;

	bsave = inblock;
	inblock |= 1;
	genhighlow(BLOCKCREATE, b->numargs);
	bumpstk();
	if (b->numargs)
		gencode(b->arglocation);
	i = codetop;
	gencode(0); /* block size */
	genstate(b->bstates, 1);
	size = (codetop - i) - 1;
	if (size > 255)
		yerr("block too big %d", size);
	code[i] = size;
	inblock = bsave;
}
