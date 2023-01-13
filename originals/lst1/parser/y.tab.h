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
extern YYSTYPE yylval;
