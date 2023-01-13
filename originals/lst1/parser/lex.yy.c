# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin ={stdin}, *yyout ={stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*
        Little Smalltalk lexical analyzer
*/
# include <math.h>
# include "primnum.h"

# undef input
# undef unput

double atof();
int linenum = 1;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
                         {;}
break;
case 2:
                             {linenum++;}
break;
case 3:
                             {readcomment();}
break;
case 4:
                           {return(ASSIGN);}
break;
case 5:
                           {return(ASSIGN);}
break;
case 6:
                          {return(lexsave(CLASS));}
break;
case 7:
                           {yylval.p = selfvar;  return(PSEUDO);}
break;
case 8:
		{yylval.p = procvar;  return(PSEUDO);}
break;
case 9:
                          {yylval.p = supervar; return(PSEUDO);}
break;
case 10:
                            {yylval.p = nilvar;   return(PSEUDO);}
break;
case 11:
                           {yylval.p = truevar;  return(PSEUDO);}
break;
case 12:
                          {yylval.p = falsevar; return(PSEUDO);}
break;
case 13:
                      {yylval.p = smallvar; return(PSEUDO);}
break;
case 14:
                            {yylval.i = yytext[1]; return(LITCHAR);}
break;
case 15:
                              {return(PS);}
break;
case 16:
{return(lexsave(LITFNUM));}
break;
case 17:
                         {yylval.i = atoi(yytext); return(LITNUM);}
break;
case 18:
  {return(lexsave(LITFNUM));}
break;
case 19:
                        {char c; unput(c = input());
                                 if (c == '\'') yymore();
                                 else return(lexlstr());}
break;
case 20:
                 {return(varlex());}
break;
case 21:
                  {return(slexsave(COLONVAR));}
break;
case 22:
                {return(slexsave(LITSYM));}
break;
case 23:
                            {return(lexsave(MINUS));}
break;
case 24:
                            {return(LP);}
break;
case 25:
                            {return(RP);}
break;
case 26:
                            {return(LB);}
break;
case 27:
                            {return(RB);}
break;
case 28:
                            {return(PERIOD);}
break;
case 29:
			{return(lexsave(MBAR));}
break;
case 30:
			{return(lexsave(MBAR));}
break;
case 31:
                            {return(lexsave(BAR));}
break;
case 32:
                            {return(lexsave(BAR));}
break;
case 33:
                            {return(SEMI);}
break;
case 34:
                            {return(lexsave(UPARROW));}
break;
case 35:
			{return(lexsave(PE));}
break;
case 36:
              {return(lexsave(BINARY));}
break;
case 37:
  			{return(PRIMITIVE);}
break;
case 38:
		{yylval.i = prim_number(&yytext[1]); return(NAMEDPRIM);}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
static int ocbuf = 0;
static int pbbuf[400];

static int input()
{	int c;

	if (ocbuf) {c = pbbuf[--ocbuf]; }
	else {
		c = getc(fp);
		if (c == EOF) c = 0;
		}
	return(c);
}

static unput(c)
char c;
{
	if (c) pbbuf[ocbuf++] = c;
}

# include <ctype.h>

static readcomment()
{  char c;

   while ((c = input()) && c != '\"')
	if (c == '\n') linenum++;
   if (!c) yyerror("unterminated comment");
}

char *walloc(s) char *s;
{  char *p, *malloc();

   p = malloc((unsigned) (strlen(s) + 1));
   if (p == (char *) 0) yyerror("out of variable string space");
   strcpy(p, s);
   return(p);
}

static int slexsave(type)
int type;
{

	yylval.c = walloc(&yytext[1]);
	if (yylval.c == 0) yerr("cannot create symbol %s", yytext);
	return(type);
}

static int lexsave(type)
int type;
{

	yylval.c = walloc(yytext);
	if (yylval.c == 0) yerr("cannot create string %s", yytext);
	return(type);
}

static int varlex()
{  

   lexsave(0);
   if (yytext[yyleng-1] == ':') return(KEYWORD);
   else if (islower(yytext[0])) return(LOWERCASEVAR);
   else return(UPPERCASEVAR);
}

static int lexlstr()
{  char *p, *q;

   yylval.c = p = walloc(&yytext[1]);
   *(p + yyleng -2) = '\0';
   return(LITSTR);
}

static int prim_number(name)
char *name;
{	struct prim_names *p;

	for (p = prim_table; *(p->p_name); p++) {
		if (strcmp(p->p_name, name) == 0)
			return(p->p_number);
		}
	yerr("unknown primitive name %s", name);
	return(0);
}
int yyvstop[] ={
0,

36,
0,

1,
0,

2,
0,

32,
36,
0,

3,
36,
0,

15,
36,
0,

36,
0,

36,
0,

24,
36,
0,

25,
36,
0,

23,
36,
0,

28,
36,
0,

17,
18,
20,
0,

36,
0,

33,
36,
0,

36,
0,

35,
36,
0,

20,
0,

20,
0,

26,
36,
0,

27,
36,
0,

34,
36,
0,

20,
0,

20,
0,

20,
0,

20,
0,

31,
36,
0,

30,
32,
36,
0,

29,
31,
36,
0,

22,
0,

14,
0,

19,
0,

20,
0,

20,
0,

20,
0,

21,
0,

4,
0,

5,
0,

38,
0,

38,
0,

20,
0,

20,
0,

20,
0,

20,
0,

20,
0,

20,
0,

20,
0,

18,
0,

18,
20,
0,

16,
20,
0,

38,
0,

20,
0,

20,
0,

10,
20,
0,

20,
0,

20,
0,

20,
0,

20,
0,

18,
0,

16,
0,

20,
0,

38,
0,

20,
0,

20,
0,

7,
20,
0,

20,
0,

20,
0,

11,
20,
0,

16,
0,

16,
20,
0,

38,
0,

6,
20,
0,

12,
20,
0,

20,
0,

20,
0,

9,
20,
0,

16,
0,

38,
0,

20,
0,

20,
0,

38,
0,

20,
0,

20,
0,

38,
0,

20,
0,

20,
0,

38,
0,

20,
0,

13,
20,
0,

37,
38,
0,

20,
0,

8,
20,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] ={
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	4,4,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,7,	
1,8,	1,9,	4,4,	0,0,	
1,10,	1,11,	1,12,	0,0,	
1,3,	0,0,	1,13,	1,14,	
0,0,	1,15,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,16,	
1,17,	1,18,	0,0,	1,19,	
0,0,	0,0,	1,20,	0,0,	
1,21,	0,0,	0,0,	65,53,	
0,0,	65,53,	0,0,	2,30,	
2,7,	2,8,	2,9,	0,0,	
0,0,	8,32,	0,0,	2,12,	
0,0,	0,0,	0,0,	2,13,	
2,14,	8,0,	8,0,	0,0,	
1,22,	0,0,	1,23,	1,24,	
0,0,	0,0,	1,20,	9,33,	
2,16,	2,17,	2,18,	1,25,	
2,19,	25,46,	45,58,	9,33,	
9,0,	2,21,	21,45,	1,26,	
8,0,	26,47,	46,59,	47,60,	
1,27,	1,28,	28,51,	8,32,	
8,0,	8,0,	10,34,	8,32,	
27,48,	1,29,	8,0,	44,57,	
8,32,	48,61,	10,34,	10,34,	
27,49,	2,22,	49,62,	2,23,	
2,24,	9,33,	9,33,	50,63,	
27,50,	9,33,	51,64,	52,65,	
2,25,	8,32,	9,33,	57,70,	
58,71,	59,72,	61,73,	62,74,	
2,26,	63,75,	64,76,	70,81,	
71,82,	2,27,	2,28,	67,68,	
10,35,	10,34,	72,83,	9,33,	
10,34,	73,84,	2,31,	74,85,	
75,86,	10,34,	78,77,	8,0,	
81,88,	84,89,	85,90,	88,91,	
89,92,	8,32,	90,93,	91,94,	
92,95,	93,96,	94,97,	95,98,	
96,99,	97,100,	10,34,	98,101,	
101,102,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	9,33,	
0,0,	0,0,	0,0,	0,0,	
15,36,	0,0,	15,15,	15,15,	
15,15,	15,15,	15,15,	15,15,	
15,15,	15,15,	15,15,	15,15,	
15,37,	0,0,	67,77,	0,0,	
0,0,	0,0,	10,34,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	15,20,	
15,20,	15,20,	15,20,	15,38,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,39,	15,20,	15,20,	15,20,	
15,20,	15,20,	15,20,	15,20,	
15,20,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	0,0,	
0,0,	0,0,	16,41,	0,0,	
0,0,	0,0,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
16,40,	16,40,	16,40,	16,40,	
18,42,	0,0,	0,0,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,44,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	18,43,	18,43,	
18,43,	18,43,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,37,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	20,20,	20,20,	20,20,	
20,20,	32,0,	32,0,	36,52,	
36,52,	36,52,	36,52,	36,52,	
36,52,	36,52,	36,52,	36,52,	
36,52,	53,66,	53,66,	53,66,	
53,66,	53,66,	53,66,	53,66,	
53,66,	53,66,	53,66,	0,0,	
32,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
32,0,	32,0,	38,53,	0,0,	
38,53,	0,0,	32,0,	38,54,	
38,54,	38,54,	38,54,	38,54,	
38,54,	38,54,	38,54,	38,54,	
38,54,	54,54,	54,54,	54,54,	
54,54,	54,54,	54,54,	54,54,	
54,54,	54,54,	54,54,	0,0,	
0,0,	0,0,	0,0,	0,0,	
39,55,	0,0,	0,0,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	32,0,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	39,56,	39,56,	
39,56,	39,56,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	40,40,	40,40,	40,40,	
40,40,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
43,43,	43,43,	43,43,	43,43,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	55,67,	
55,67,	55,67,	55,67,	56,68,	
0,0,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
56,56,	56,56,	56,56,	56,56,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	56,69,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	68,78,	
68,78,	68,78,	68,78,	69,79,	
0,0,	69,79,	0,0,	0,0,	
69,80,	69,80,	69,80,	69,80,	
69,80,	69,80,	69,80,	69,80,	
69,80,	69,80,	77,79,	0,0,	
77,79,	0,0,	0,0,	77,87,	
77,87,	77,87,	77,87,	77,87,	
77,87,	77,87,	77,87,	77,87,	
77,87,	79,87,	79,87,	79,87,	
79,87,	79,87,	79,87,	79,87,	
79,87,	79,87,	79,87,	80,80,	
80,80,	80,80,	80,80,	80,80,	
80,80,	80,80,	80,80,	80,80,	
80,80,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] ={
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-42,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+6,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+5,
yycrank+0,	0,		yyvstop+7,
yycrank+0,	0,		yyvstop+10,
yycrank+-80,	0,		yyvstop+13,
yycrank+-98,	0,		yyvstop+16,
yycrank+-121,	0,		yyvstop+18,
yycrank+0,	0,		yyvstop+20,
yycrank+0,	0,		yyvstop+23,
yycrank+0,	0,		yyvstop+26,
yycrank+0,	0,		yyvstop+29,
yycrank+154,	0,		yyvstop+32,
yycrank+229,	0,		yyvstop+36,
yycrank+0,	0,		yyvstop+38,
yycrank+307,	0,		yyvstop+41,
yycrank+0,	0,		yyvstop+43,
yycrank+382,	0,		yyvstop+46,
yycrank+2,	yysvec+20,	yyvstop+48,
yycrank+0,	0,		yyvstop+50,
yycrank+0,	0,		yyvstop+53,
yycrank+0,	0,		yyvstop+56,
yycrank+8,	yysvec+20,	yyvstop+59,
yycrank+8,	yysvec+20,	yyvstop+61,
yycrank+23,	yysvec+20,	yyvstop+63,
yycrank+4,	yysvec+20,	yyvstop+65,
yycrank+0,	0,		yyvstop+67,
yycrank+0,	0,		yyvstop+70,
yycrank+0,	0,		yyvstop+74,
yycrank+-496,	yysvec+8,	yyvstop+78,
yycrank+0,	0,		yyvstop+80,
yycrank+0,	yysvec+10,	0,	
yycrank+0,	0,		yyvstop+82,
yycrank+459,	0,		0,	
yycrank+0,	0,		yyvstop+84,
yycrank+495,	yysvec+20,	yyvstop+86,
yycrank+523,	yysvec+20,	yyvstop+88,
yycrank+566,	0,		yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+0,	0,		yyvstop+94,
yycrank+641,	0,		yyvstop+96,
yycrank+13,	yysvec+43,	yyvstop+98,
yycrank+9,	yysvec+20,	yyvstop+100,
yycrank+6,	yysvec+20,	yyvstop+102,
yycrank+7,	yysvec+20,	yyvstop+104,
yycrank+21,	yysvec+20,	yyvstop+106,
yycrank+37,	yysvec+20,	yyvstop+108,
yycrank+27,	yysvec+20,	yyvstop+110,
yycrank+25,	yysvec+20,	yyvstop+112,
yycrank+42,	yysvec+36,	yyvstop+114,
yycrank+469,	0,		0,	
yycrank+505,	yysvec+20,	yyvstop+116,
yycrank+716,	0,		0,	
yycrank+761,	yysvec+20,	yyvstop+119,
yycrank+42,	yysvec+43,	yyvstop+122,
yycrank+33,	yysvec+20,	yyvstop+124,
yycrank+34,	yysvec+20,	yyvstop+126,
yycrank+0,	yysvec+20,	yyvstop+128,
yycrank+48,	yysvec+20,	yyvstop+131,
yycrank+43,	yysvec+20,	yyvstop+133,
yycrank+52,	yysvec+20,	yyvstop+135,
yycrank+53,	yysvec+20,	yyvstop+137,
yycrank+28,	yysvec+53,	0,	
yycrank+0,	yysvec+53,	yyvstop+139,
yycrank+113,	yysvec+55,	yyvstop+141,
yycrank+804,	0,		0,	
yycrank+852,	yysvec+20,	yyvstop+143,
yycrank+46,	yysvec+43,	yyvstop+145,
yycrank+41,	yysvec+20,	yyvstop+147,
yycrank+61,	yysvec+20,	yyvstop+149,
yycrank+85,	yysvec+20,	yyvstop+151,
yycrank+59,	yysvec+20,	yyvstop+154,
yycrank+54,	yysvec+20,	yyvstop+156,
yycrank+0,	yysvec+20,	yyvstop+158,
yycrank+867,	0,		0,	
yycrank+69,	yysvec+68,	yyvstop+161,
yycrank+877,	0,		0,	
yycrank+887,	yysvec+20,	yyvstop+163,
yycrank+67,	yysvec+43,	yyvstop+166,
yycrank+0,	yysvec+20,	yyvstop+168,
yycrank+0,	yysvec+20,	yyvstop+171,
yycrank+59,	yysvec+20,	yyvstop+174,
yycrank+58,	yysvec+20,	yyvstop+176,
yycrank+0,	yysvec+20,	yyvstop+178,
yycrank+0,	yysvec+79,	yyvstop+181,
yycrank+59,	yysvec+43,	yyvstop+183,
yycrank+65,	yysvec+20,	yyvstop+185,
yycrank+81,	yysvec+20,	yyvstop+187,
yycrank+74,	yysvec+43,	yyvstop+189,
yycrank+81,	yysvec+20,	yyvstop+191,
yycrank+73,	yysvec+20,	yyvstop+193,
yycrank+64,	yysvec+43,	yyvstop+195,
yycrank+82,	yysvec+20,	yyvstop+197,
yycrank+77,	yysvec+20,	yyvstop+199,
yycrank+84,	yysvec+43,	yyvstop+201,
yycrank+72,	yysvec+20,	yyvstop+203,
yycrank+0,	yysvec+20,	yyvstop+205,
yycrank+0,	yysvec+43,	yyvstop+208,
yycrank+73,	yysvec+20,	yyvstop+211,
yycrank+0,	yysvec+20,	yyvstop+213,
0,	0,	0};
struct yywork *yytop = yycrank+944;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,01  ,01  ,01  ,047 ,
'(' ,'(' ,01  ,'+' ,01  ,'+' ,'(' ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'(' ,01  ,01  ,01  ,01  ,
01  ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] ={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	ncform	4.1	83/08/11	*/

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
