# define LITNUM 257
# define LITFNUM 258
# define LITCHAR 259
# define LITSTR 260
# define LITSYM 261
# define CLASS 262
# define ASSIGN 263
# define BINARY 264
# define PRIMITIVE 265
# define NAMEDPRIM 266
# define PSEUDO 267
# define UPPERCASEVAR 268
# define LOWERCASEVAR 269
# define COLONVAR 270
# define KEYWORD 271
# define LP 272
# define RP 273
# define LB 274
# define RB 275
# define PERIOD 276
# define BAR 277
# define MBAR 278
# define SEMI 279
# define UPARROW 280
# define PS 281
# define MINUS 282
# define PE 283

# line 26 "parser.y"
# include "env.h"
# include "drive.h"
# include "parser.h"

# line 31 "parser.y"
typedef union  {
	struct litlist 		*a;
	struct blockstruct 	*b;
	char 			*c;
	struct exprstruct 	*e;
	int  			 i;
	struct keylist 		*k;
	struct classstruct 	*l;
	struct methodstruct 	*m;
	struct objstruct 	*o;
	enum pseuvars 		 p;
	struct primlist 	*r;
	struct statestruct 	*s;
	struct litstruct 	*t;
	struct primstruct 	*u
	} YYSTYPE;

# line 49 "parser.y"
extern struct blockstruct *mkblock();
extern struct classstruct *mkclass();
extern struct varstruct *mkvar(), *addvlist(), *invlist();
extern struct methodstruct *mkmethod();
extern struct exprstruct *mkexpr(), *mkkey();
extern struct keylist *mkklist();
extern struct statestruct *mkstate();
extern struct objstruct *mkobj();
extern struct primstruct *mkprim();
extern struct primlist *addprim();
extern struct litstruct *mklit();
extern struct litlist *addlit();
extern char   *bincat();

struct varstruct *instvars;
struct varstruct *contextvars;

int bytetop = 0;
uchar bytearray[1000];

YYSTYPE e;
int errorcount = 0;
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 350 "parser.y"

# include <stdio.h>

char *filename;
FILE *fp;
FILE *ofd;

# include "lex.yy.c"

main(argc, argv) 
int argc;
char **argv;
{	
	if (argc != 2) quiter("parser: wrong number of arguments");
	filename = argv[1];
	fp = fopen(filename, "r");
	if (fp == NULL) {
		yerr("cannot open input file %s", filename);
		quiter("parser quits");
		}
	ofd = stdout;
	return(yyparse());
}

quiter(s) char *s; {fprintf(stderr,"%s\n", s); exit(1);}

yywarn(s, v) char *s, *v; {
   fprintf(stderr, "%s: line %d: Warning ", filename, linenum);
   fprintf(stderr, s, v);
   fprintf(stderr,"\n");
}

yyerror(s) char *s; {yerr(s, "");}

yerr(s, v) 
char *s, *v; 
{
   fprintf(stderr, "%s: line %d: ", filename, linenum);
   fprintf(stderr, s, v);
   fprintf(stderr,"\n");
   if (errorcount++ > 10) quiter("too many errors, goodby");
}

expect(str) char *str;
{  char buffer[100];

   sprintf(buffer,"Expected %%s found %s", yytext);
   yerr(buffer, str);
}

int yywrap() { return(1);}

char *alloc(size) int size;      /* allocate a block of storage */
{  char *p, *malloc();

   p = malloc( (unsigned) size);
   if (p == (char *) 0) yyerror("out of free space");
   return(p);
}

char *bincat(s1, s2)
char *s1, *s2;
{	char *p;

	p = alloc(strlen(s1) + strlen(s2) + 1);
	strcpy(p, s1);
	strcat(p, s2);
	return(p);
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 120
# define YYLAST 370
short yyact[]={

  82,  80,  76,  77,  78, 129,  93,  25, 160, 155,
 145, 126, 125, 128, 127, 133, 159, 144, 158,  36,
  26,  90,  37,  28, 132, 134,  29,  82,  80,  76,
  77,  78, 129, 152,  25, 150, 157, 109, 126, 125,
 128, 127, 133, 122, 121, 116,  34,  26,  40, 107,
  28, 132, 134,  29,  82,  80,  76,  77,  78,  15,
  43,  48,  71,  72,  65,  14, 118,  32,  33,  67,
 108,  73, 109,  42,  47,  99,  10,  35,  79,  81,
 151,  82,  80,  76,  77,  78,  15, 100,  85,  71,
  72,  65,  14, 118,   9,  92,  67,   6,  73, 111,
 110, 147, 103,   5, 140,  79,  81, 136,  82,  80,
  76,  77,  78,  15, 102,  13,  71,  72,  65,  14,
  54,  15,  24,  67,  69,  73,  62,  14, 104,  17,
 131, 142,  79,  81,  82,  80,  76,  77,  78,  15,
  60,  53,  71,  72,  65,  14,  54,  45,  58,  67,
  51,  73,   2, 146,   7,  70,  84,  52,  79,  81,
  82,  80,  76,  77,  78,  15,  89,  49,  71,  72,
  65,  14,  54,  94,  46,  67,  30,  73,  82,  80,
  76,  77,  78,  15,  79,  81,  71,  72,  65,  14,
 118,  22,  25,  67,  91,  73,  63,   4,   8,  25,
  34,  12,  79,  81,  21,  26,  23,  25,  28, 101,
  27,  29,  26,  85,  96,  28,  34,  27,  29,  95,
  26,  32,  33,  28,  31,  27,  29, 124,  20,  88,
   1, 137, 135,  97, 115,  41, 117,  32,  33, 120,
 113, 114,  66,  39, 153, 119, 149, 141, 123,  64,
 143, 139, 138,  50,  16,   3,  11,  20,  19,  44,
  75, 105, 137,  83, 106, 148,  38,  61,  59,  57,
  55,  87,  56,  18, 124,  74,  86,  68,   0,  98,
   0, 154,   0,   0, 156,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 112,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 130,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0, 130 };
short yypact[]={

-159,-159,-1000,-180,-141,-1000,-1000,-1000, -65,-1000,
-1000,-210,-193,-1000,-1000,-1000,-256,-1000,-210,-223,
-196,-1000,-1000,-196, -72,-1000,-1000,-1000,-1000,-1000,
-1000,-195,-1000,-1000,-1000,-1000,-1000, -65,-123,-181,
-196,-1000,-1000,-1000,-1000,-1000, -40,-1000,-1000,-1000,
-255,-1000, -97,-1000,-168,-273,-1000,-1000, -57, -57,
-194,-182,-1000,-1000,-1000,-1000,-1000, -97,-1000,-1000,
-1000,-155,-1000,-221,-1000,-1000,-1000,-1000,-1000,-202,
-1000,-158,-1000, -56,-1000,-1000,-1000,-1000,-1000,-1000,
-123,-1000, -97,-1000,-226, -79, -79,-226, -79,-1000,
-1000,-229,-230,-1000,-176,-149,-260,-1000,-1000,-156,
-1000,-1000,-1000,-1000,-1000,-1000, -79,-194,-1000, -72,
-194,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-237,-1000,-158,-203,-1000,-1000,-242,-149,
-1000,-267, -97,-1000,-1000,-1000,-239,-1000, -72,-257,
-1000,-1000,-1000,-1000,-268,-1000,-1000,-1000,-1000,-1000,
-1000 };
short yypgo[]={

   0, 114, 277, 196, 219, 122, 275, 273, 272, 270,
 148, 140, 269, 268, 267, 266, 264, 263, 261, 260,
 258, 173, 256, 255, 129, 254, 126, 128, 253, 150,
 141, 252, 251, 104, 250, 249, 124, 248, 242, 230,
 152, 198, 197, 176, 224, 174, 166, 235, 156, 155,
 153, 130 };
short yyr1[]={

   0,  39,  39,  40,  41,  41,  23,  42,  42,  22,
  22,  22,   3,   3,  43,  43,  45,  45,  45,  25,
  25,  24,   7,   7,   7,   7,  20,  20,  47,  47,
  15,  15,  44,  44,  44,  17,  17,  48,  28,  28,
  46,  46,  29,  29,  30,  30,   9,   9,  12,  12,
  13,  13,  14,  14,   8,   8,  21,  21,  10,  10,
   4,   4,   5,   5,   5,   5,   5,  11,  11,  26,
  26,  26,  26,  26,  26,  26,  38,  38,  27,  27,
   2,  18,  18,  16,  16,  31,  31,  33,  33,  34,
  34,  32,  32,  35,  35,  49,  36,  36,  36,  36,
  36,  36,   6,   6,  19,  19,  37,  37,  37,  37,
  37,  37,  37,  37,  51,  51,   1,   1,  50,  50 };
short yyr2[]={

   0,   1,   2,   4,   1,   1,   3,   1,   1,   1,
   2,   1,   1,   1,   0,   3,   1,   2,   1,   1,
   3,   4,   1,   2,   1,   1,   2,   3,   1,   1,
   0,   3,   1,   1,   1,   1,   2,   1,   1,   3,
   0,   1,   2,   1,   3,   1,   1,   1,   1,   2,
   1,   3,   2,   2,   1,   2,   2,   3,   1,   3,
   1,   2,   1,   1,   1,   1,   1,   1,   2,   1,
   1,   1,   1,   1,   3,   1,   4,   3,   0,   2,
   4,   0,   2,   1,   2,   2,   1,   2,   1,   0,
   1,   2,   3,   1,   3,   2,   1,   1,   1,   1,
   1,   4,   1,   2,   1,   2,   1,   1,   1,   1,
   1,   1,   1,   3,   2,   1,   0,   2,   1,   2 };
short yychk[]={

-1000, -39, -40, -23, -42, 262, 256, -40, -41, 274,
 256, -22,  -3, 256, 268, 262, -25, -24,  -7, -20,
  -4, 269, 256, 271,  -5, 264, 277, 282, 280, 283,
 -43, -44, 277, 278, 256, 270, 275, 278, -15, -44,
 271, -47, 269, 256, -47,  -5, -45, 269, 256, -24,
 -28, -29, 280, -30, 269,  -9,  -8, -12, -10, -13,
 -11, -14, -26,  -3, -35, 267, -38, 272,  -2, -36,
 -49, 265, 266, 274,  -6, -19, 259, 260, 261, 281,
 258, 282, 257, -17, -48, 269, -47, -44, 269, -46,
 276, -30, 263, 279, -21,  -4, 271, -21,  -4, 269,
 269, -30,  -1, 257, -27, -18, -16, 270, 272, 274,
 258, 257, -44, -48, -29, -30, 271, -11, 269, -10,
 -11, 273, 273, -37, -36, 269, 268, 271, 270, 262,
  -4, -51, 281, 272, 282, -27, 283, -26, -31, -32,
 -33, -30, 280, -34, 277, 270, -50, 257, -10,  -1,
 272, 283, 275, -33, -30, 276, -30, 275, 257, 273,
 276 };
short yydef[]={

   0,  -2,   1,   0,   0,   7,   8,   2,   0,   4,
   5,  14,   9,  11,  12,  13,   0,  19,  30,  22,
   0,  24,  25,   0,  60,  62,  63,  64,  65,  66,
   6,   0,  32,  33,  34,  10,   3,   0,   0,   0,
   0,  23,  28,  29,  26,  61,   0,  16,  18,  20,
  40,  38,   0,  43,  70,  45,  46,  47,  54,  48,
  58,  50,  67,  69,  71,  72,  73,   0,  75,  93,
 116,   0,  78,  81,  96,  97,  98,  99, 100,   0,
 102,   0, 104,   0,  35,  37,  27,  15,  17,  21,
  41,  42,   0,  52,  55,   0,   0,  49,   0,  68,
  53,   0,   0,  78,   0,  89,   0,  83,  95,   0,
 103, 105,  31,  36,  39,  44,   0,  59,  70,  56,
  51,  74,  94, 117, 106, 107, 108, 109, 110, 111,
 112, 116,   0, 115,  64,   0,  77,  79,   0,  89,
  86,  90,   0,  88,  82,  84,   0, 118,  57,   0,
 114,  76,  80,  85,  90,  91,  87, 101, 119, 113,
  92 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:
# line 103 "parser.y"
{if (errorcount == 0) genclass(yypvt[-3].l, yypvt[-1].m);} break;
case 5:
# line 107 "parser.y"
{if ((yytext[0] == ':') ||
                             isalpha(yytext[0])) expect(":SuperClass");
                         else expect("open brace [");} break;
case 6:
# line 112 "parser.y"
{yyval.l = yypvt[-1].l;} break;
case 8:
# line 116 "parser.y"
{expect("keyword Class");} break;
case 9:
# line 119 "parser.y"
{yyval.l = mkclass(yypvt[-0].c, (char *) 0);} break;
case 10:
# line 120 "parser.y"
{yyval.l = mkclass(yypvt[-1].c, yypvt[-0].c);} break;
case 11:
# line 121 "parser.y"
{expect("Classname :Superclass");
                                         yyval.l = mkclass("Error", (char *) 0);} break;
case 16:
# line 133 "parser.y"
{addinst(yypvt[-0].c);} break;
case 17:
# line 134 "parser.y"
{addinst(yypvt[-0].c);} break;
case 18:
# line 135 "parser.y"
{expect("instance variable");} break;
case 20:
# line 140 "parser.y"
{yypvt[-0].m->nextmethod = yypvt[-2].m; yyval.m = yypvt[-0].m;} break;
case 21:
# line 144 "parser.y"
{deltemps(yypvt[-2].i); yyval.m = mkmethod(yypvt[-3].e, yypvt[-2].i, yypvt[-1].s);} break;
case 22:
# line 148 "parser.y"
{yyval.e = mkkey((struct exprstruct *) 0, yypvt[-0].k);} break;
case 23:
# line 150 "parser.y"
{yyval.e = mkexpr((struct exprstruct *) 0, bincmd, yypvt[-1].c, (struct exprstruct *) 0);} break;
case 24:
# line 152 "parser.y"
{yyval.e = mkexpr((struct exprstruct *) 0, uncmd, yypvt[-0].c, (struct exprstruct *) 0);} break;
case 25:
# line 153 "parser.y"
{expect("method pattern");
yyval.e = mkexpr((struct exprstruct *) 0, uncmd, "", (struct exprstruct *) 0);} break;
case 26:
# line 158 "parser.y"
{yyval.k = mkklist((struct keylist *) 0, yypvt[-1].c, (struct exprstruct *) 0);} break;
case 27:
# line 160 "parser.y"
{yyval.k = mkklist(yypvt[-2].k, yypvt[-1].c, (struct exprstruct *) 0);} break;
case 28:
# line 163 "parser.y"
{addtemp(yypvt[-0].c, argvar);} break;
case 29:
# line 164 "parser.y"
{expect("argument variable");} break;
case 30:
# line 167 "parser.y"
{yyval.i = 0;} break;
case 31:
# line 168 "parser.y"
{yyval.i = yypvt[-1].i;} break;
case 34:
# line 173 "parser.y"
{expect("| (vertical bar)");} break;
case 35:
# line 176 "parser.y"
{yyval.i = 1;} break;
case 36:
# line 177 "parser.y"
{yyval.i = yypvt[-1].i + 1;} break;
case 37:
# line 180 "parser.y"
{addtemp(yypvt[-0].c, tempvar);} break;
case 38:
# line 183 "parser.y"
{yyval.s = yypvt[-0].s;} break;
case 39:
# line 184 "parser.y"
{yypvt[-0].s->nextstate = yypvt[-2].s; yyval.s = yypvt[-0].s;} break;
case 42:
# line 191 "parser.y"
{yyval.s = mkstate(upar, (char *) 0, yypvt[-0].s);} break;
case 44:
# line 196 "parser.y"
{yyval.s = mkstate(asgn, yypvt[-2].c, yypvt[-0].s);} break;
case 45:
# line 198 "parser.y"
{yyval.s = mkstate(expr, (char *) 0, (struct statestruct *) yypvt[-0].e);} break;
case 47:
# line 202 "parser.y"
{yyval.e = mkexpr(yypvt[-0].e, semiend, 0, 0);} break;
case 49:
# line 206 "parser.y"
{yyval.e = mkkey(yypvt[-1].e, yypvt[-0].k);} break;
case 51:
# line 211 "parser.y"
{yyval.e = mkexpr(yypvt[-2].e, bincmd, yypvt[-1].c, yypvt[-0].e);} break;
case 52:
# line 214 "parser.y"
{yyval.e = mkexpr(yypvt[-1].e, semistart, 0, 0);} break;
case 53:
# line 216 "parser.y"
{yyval.e = mkexpr(yypvt[-1].e, uncmd, yypvt[-0].c, (struct exprstruct *) 0);} break;
case 54:
# line 219 "parser.y"
{yyval.e = yypvt[-0].e;} break;
case 55:
# line 220 "parser.y"
{yyval.e = mkkey(yypvt[-1].e, yypvt[-0].k);} break;
case 56:
# line 224 "parser.y"
{yyval.k = mkklist((struct keylist *) 0, yypvt[-1].c, yypvt[-0].e);} break;
case 57:
# line 226 "parser.y"
{yyval.k = mkklist(yypvt[-2].k, yypvt[-1].c, yypvt[-0].e);} break;
case 58:
# line 229 "parser.y"
{yyval.e = yypvt[-0].e;} break;
case 59:
# line 230 "parser.y"
{yyval.e = mkexpr(yypvt[-2].e, bincmd, yypvt[-1].c, yypvt[-0].e);} break;
case 60:
# line 233 "parser.y"
{yyval.c = yypvt[-0].c;} break;
case 61:
# line 234 "parser.y"
{yyval.c = bincat(yypvt[-1].c, yypvt[-0].c);} break;
case 67:
# line 245 "parser.y"
{yyval.e = mkexpr((struct exprstruct *) 0, reccmd, (char *) 0, 
					(struct exprstruct *) yypvt[-0].o);} break;
case 68:
# line 248 "parser.y"
{yyval.e = mkexpr(yypvt[-1].e, uncmd, yypvt[-0].c, (struct exprstruct *) 0);} break;
case 69:
# line 251 "parser.y"
{e.c = yypvt[-0].c; yyval.o = mkobj(classobj, &e);} break;
case 70:
# line 252 "parser.y"
{e.c = yypvt[-0].c; yyval.o = mkobj(varobj, &e);} break;
case 71:
# line 253 "parser.y"
{e.t = yypvt[-0].t; yyval.o = mkobj(litobj, &e);} break;
case 72:
# line 254 "parser.y"
{e.p = yypvt[-0].p; yyval.o = mkobj(pseuobj, &e);} break;
case 73:
# line 255 "parser.y"
{e.u = yypvt[-0].u; yyval.o = mkobj(primobj, &e);} break;
case 74:
# line 256 "parser.y"
{e.s = yypvt[-1].s; yyval.o = mkobj(exprobj, &e);} break;
case 75:
# line 257 "parser.y"
{e.b = yypvt[-0].b; yyval.o = mkobj(blockobj, &e);} break;
case 76:
# line 261 "parser.y"
{yyval.u = mkprim(yypvt[-2].i, yypvt[-1].r);} break;
case 77:
# line 263 "parser.y"
{yyval.u = mkprim(yypvt[-2].i, yypvt[-1].r);} break;
case 78:
# line 266 "parser.y"
{yyval.r = (struct primlist *) 0;} break;
case 79:
# line 267 "parser.y"
{yyval.r = addprim(yypvt[-1].r, yypvt[-0].o);} break;
case 80:
# line 271 "parser.y"
{yyval.b = mkblock(yypvt[-2].i, yypvt[-1].s);
					deltemps(yypvt[-2].i);} break;
case 81:
# line 275 "parser.y"
{yyval.i = 0;} break;
case 82:
# line 276 "parser.y"
{yyval.i = yypvt[-1].i;} break;
case 83:
# line 279 "parser.y"
{addtemp(yypvt[-0].c, argvar); yyval.i = 1;} break;
case 84:
# line 280 "parser.y"
{addtemp(yypvt[-0].c, argvar); yyval.i = yypvt[-1].i + 1;} break;
case 85:
# line 283 "parser.y"
{yypvt[-0].s->nextstate = yypvt[-1].s; yyval.s = yypvt[-0].s;} break;
case 86:
# line 284 "parser.y"
{yyval.s = yypvt[-0].s;} break;
case 87:
# line 287 "parser.y"
{yyval.s = mkstate(blkupar, (char *) 0, yypvt[-0].s);} break;
case 88:
# line 288 "parser.y"
{yyval.s = mkstate(upar, (char *) 0, yypvt[-0].s);} break;
case 89:
# line 292 "parser.y"
{e.p = nilvar;
yyval.s = mkstate(expr, (char *) 0,
(struct statestruct *) mkexpr((struct exprstruct *) 0, reccmd, (char *) 0,
	(struct exprstruct *) mkobj(pseuobj, &e)));} break;
case 90:
# line 296 "parser.y"
{yyval.s = yypvt[-0].s;} break;
case 91:
# line 299 "parser.y"
{yyval.s = yypvt[-1].s;} break;
case 92:
# line 301 "parser.y"
{yypvt[-1].s->nextstate = yypvt[-2].s; yyval.s = yypvt[-1].s;} break;
case 93:
# line 304 "parser.y"
{yyval.t = yypvt[-0].t;} break;
case 94:
# line 305 "parser.y"
{e.a = yypvt[-1].a; yyval.t = mklit(arlit, &e);} break;
case 96:
# line 311 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(fnumlit, &e);} break;
case 97:
# line 312 "parser.y"
{e.i = yypvt[-0].i; yyval.t = mklit(numlit, &e);} break;
case 98:
# line 313 "parser.y"
{e.i = yypvt[-0].i; yyval.t = mklit(charlit, &e);} break;
case 99:
# line 314 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(strlit, &e);} break;
case 100:
# line 315 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 101:
# line 316 "parser.y"
{bytearray[bytetop] = '\0';
                                         yyval.t = mklit(bytelit, &e);} break;
case 102:
# line 320 "parser.y"
{yyval.c = yypvt[-0].c;} break;
case 103:
# line 321 "parser.y"
{yyval.c = bincat("-", yypvt[-0].c);} break;
case 104:
# line 324 "parser.y"
{yyval.i = yypvt[-0].i;} break;
case 105:
# line 325 "parser.y"
{yyval.i = - yypvt[-0].i;} break;
case 106:
# line 328 "parser.y"
{yyval.t = yypvt[-0].t;} break;
case 107:
# line 329 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 108:
# line 330 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 109:
# line 331 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 110:
# line 332 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 111:
# line 333 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 112:
# line 334 "parser.y"
{e.c = yypvt[-0].c; yyval.t = mklit(symlit, &e);} break;
case 113:
# line 335 "parser.y"
{e.a = yypvt[-1].a; yyval.t = mklit(arlit, &e);} break;
case 116:
# line 342 "parser.y"
{yyval.a = (struct litlist *) 0;} break;
case 117:
# line 343 "parser.y"
{yyval.a = addlit(yypvt[-1].a, yypvt[-0].t);} break;
case 118:
# line 346 "parser.y"
{bytetop = 0;
                                         bytearray[bytetop++] = itouc(yypvt[-0].i);} break;
case 119:
# line 348 "parser.y"
{bytearray[bytetop++] = itouc(yypvt[-0].i);} break;
		}
		goto yystack;  /* stack new state and value */

	}
