/*
	Little Smalltalk 
		pass 1 of the parser

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
#include <stdio.h>
#include <string.h>

# include "env.h"
# include "drive.h"
# include "parser.h"
# include "y.tab.h"

extern char *alloc();

int maxcontext = 0;

struct classstruct *mkclass(classname, supername)
char *classname, *supername;
{	struct classstruct *new;
	struct varstruct *mkvar(), *addvlist();

	new = structalloc(classstruct);
	new->name = classname;
	if (supername)
		new->super = supername;
	else new->super = walloc("Object");
	instvars = (struct varstruct *) 0;
	contextvars = (struct varstruct *) 0;
	maxcontext = 0;
	addtemp("_self", (enum vartypes) 0);
	return(new);
}

struct varstruct *mkvar(text, vtype) char *text; enum vartypes vtype;
{  struct varstruct *p;

   p = structalloc(varstruct);
   p->vtype = vtype;
   p->text = text;
   p->nextvar = (struct varstruct *) 0;
   p->position = 17;
   return(p);
}

struct varstruct *addvlist(varnode, vlist)
struct varstruct *varnode, *vlist;
{
   varnode->nextvar = vlist;
   if (vlist) varnode->position = 1 + vlist->position;
   else varnode->position = 0;
   return(varnode);
}

addtemp(name, vtype)
char *name;
enum vartypes vtype;
{
	contextvars = addvlist(mkvar(name, vtype), contextvars);
	if (contextvars->position > maxcontext)
		maxcontext = contextvars->position;
}

struct varstruct *invlist(varnode, name)
struct varstruct *varnode;
char *name;
{
	for ( ; varnode; varnode = varnode->nextvar)
		if (strcmp(varnode->text, name) == 0)
			return(varnode);
	return((struct varstruct *) 0);
}

struct methodstruct *mkmethod(pat, temps, state)
struct exprstruct *pat;
int temps;
struct statestruct *state;
{	struct methodstruct *new;
	int i;

	new = structalloc(methodstruct);
	new->pattern = pat;
	new->numtempvars = temps;
	new->states = state;
	new->nextmethod = (struct methodstruct *) 0;
	switch(pat->cmdtype) {
		case uncmd: i = 0; break;
		case bincmd: i = 1; break;
		case keycmd: i = keycount(pat->cc.keys); break;
		}
	deltemps(i);
	return(new);
}

keycount(kl)
struct keylist *kl;
{
	if (kl->nextkey)
		return(1 + keycount(kl->nextkey));
	else return(1);
}

struct statestruct *mkstate(type, name, sexpr)
enum statetypes type;
char *name;
struct statestruct *sexpr;
{	struct statestruct *new;
	struct varstruct *v;

	new = structalloc(statestruct);
	new->statetype = type;
	new->nextstate = (struct statestruct *) 0;
	switch(type) {
		case upar: case blkupar:
			new->nn.stateexpr = sexpr;
			break;
		case expr:
			new->nn.cmd = (struct exprstruct *) sexpr;
			break;
		case asgn:
			new->nn.stateexpr = sexpr;
			v = invlist(instvars, name);
			if (v) {
				new->statetype = iasgn;
				new->mm.varpos = v->position;
				break;
				}
			v = invlist(contextvars, name);
			if (v) {
				new->statetype = casgn;
				new->mm.varpos = v->position;
				break;
				}
		default:
			yyerror("unknown variable or case in mkstate");
		}
	return(new);
}

struct exprstruct *mkexpr(receiver, type, name, args)
struct exprstruct *receiver, *args;
enum cmdtypes type;
char *name;
{	struct exprstruct *new;

	new = structalloc(exprstruct);
	new->cmdtype = type;
	new->cmdname = name;
	new->receiver = receiver;
	switch(type) {
		case reccmd:
			new->cc.recobj = (struct objstruct *) args;
			break;
		case uncmd:
			break;
		case bincmd:
			new->cc.argument = args;
			break;
		case keycmd:
			new->cc.keys = (struct keylist *) args;
			break;
		}
	return(new);
}

struct keylist *mkklist(kl, kw, ka)
struct keylist *kl;
char *kw;
struct exprstruct *ka;
{	struct keylist *new;

	new = structalloc(keylist);
	new->keyword = kw;
	new->arg = ka;
	new->nextkey = kl;
	return(new);
}
	
mkkname(kl, kb)
struct keylist *kl;
char *kb;
{
	if (kl->nextkey)
		mkkname(kl->nextkey, kb);
	strcat(kb, kl->keyword);
}

struct exprstruct *mkkey(receiver, keywordlist)
struct exprstruct *receiver;
struct keylist *keywordlist;
{	char kbuffer[500];

	kbuffer[0] = '\0';
	mkkname(keywordlist, kbuffer);
	return(mkexpr(receiver, keycmd, walloc(kbuffer), 
		(struct exprstruct *) keywordlist));
}

struct objstruct *mkobj(type, info)
enum objtypes type;
YYSTYPE *info;
{	struct objstruct *new;
	struct varstruct *v;
	struct litstruct *mklit();

	new = structalloc(objstruct);
	new->objtype = type;
	switch(type) {
		case classobj:
			new->ee.litinfo = mklit(symlit, info);
			break;
		case varobj:
			v = invlist(instvars, info->c);
			if (v) {
				new->objtype = instvarobj;
				new->ee.varoffset = v->position;
				return(new);
				}
			v = invlist(contextvars, info->c);
			if (v) {
				new->objtype = contvarobj;
				new->ee.varoffset = v->position;
				return(new);
				}
			yerr("unknown variable %s", info->c);
			break;
		case litobj:
			new->ee.litinfo = info->t;
			break;
		case pseuobj:
			new->ee.pseuinfo = info->p;
			break;
		case primobj:
			new->ee.priminfo = info->u;
			break;
		case exprobj:
			new->ee.stateinfo = info->s;
			break;
		case blockobj:
			new->ee.blockinfo = info->b;
			break;
	}
	return(new);
}

struct blockstruct *mkblock(numargs, bstates)
int numargs;
struct statestruct *bstates;
{	struct blockstruct *new;
	int i;

	new = structalloc(blockstruct);
	new->numargs = numargs;
	if (contextvars)
		i = (contextvars->position - numargs) +1;
	else i = 1;
	new->arglocation = i;
	new->bstates = bstates;
	return(new);
}


struct primstruct *mkprim(pnum, plist)
struct primlist *plist;
int pnum;
{	struct primstruct *new;

	new = structalloc(primstruct);
	new->primnumber = pnum;
	new->plist = plist;
	return(new);
}

struct primlist *addprim(plist, prim)
struct primlist *plist;
struct objstruct *prim;
{	struct primlist *new;

	new = structalloc(primlist);
	new->nextprim = plist;
	new->pobject = prim;
	return(new);
}

struct litlist *addlit(list, lit)
struct litlist *list;
struct litstruct *lit;
{	struct litlist *new;

	new = structalloc(litlist);
	new->litele = lit;
	new->nextlit = list;
	return(new);
}

struct litstruct *mklit(littype, e) 
enum littypes littype; 
YYSTYPE *e;
{  struct litstruct *p;

	p = structalloc(litstruct);
	p->littype = littype;
	switch(littype) {
		case numlit:  p->ll.litint = e->i; break;
		case fnumlit: p->ll.litstr = e->c; break;
		case charlit: p->ll.litchar = (char) e->i; break;
		case strlit:  p->ll.litstr = e->c; break;
		case symlit:  p->ll.litsym = e->c; break;
		case arlit:   p->ll.litarry = e->a; break;
      	}
	return(p);
}

deltemps(n)
int n;
{
	while (n--) {
		contextvars = contextvars->nextvar;
		}
}
