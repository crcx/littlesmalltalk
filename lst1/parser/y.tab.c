/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LITNUM = 258,
     LITFNUM = 259,
     LITCHAR = 260,
     LITSTR = 261,
     LITSYM = 262,
     CLASS = 263,
     ASSIGN = 264,
     BINARY = 265,
     PRIMITIVE = 266,
     NAMEDPRIM = 267,
     PSEUDO = 268,
     UPPERCASEVAR = 269,
     LOWERCASEVAR = 270,
     COLONVAR = 271,
     KEYWORD = 272,
     LP = 273,
     RP = 274,
     LB = 275,
     RB = 276,
     PERIOD = 277,
     BAR = 278,
     MBAR = 279,
     SEMI = 280,
     UPARROW = 281,
     PS = 282,
     MINUS = 283,
     PE = 284
   };
#endif
/* Tokens.  */
#define LITNUM 258
#define LITFNUM 259
#define LITCHAR 260
#define LITSTR 261
#define LITSYM 262
#define CLASS 263
#define ASSIGN 264
#define BINARY 265
#define PRIMITIVE 266
#define NAMEDPRIM 267
#define PSEUDO 268
#define UPPERCASEVAR 269
#define LOWERCASEVAR 270
#define COLONVAR 271
#define KEYWORD 272
#define LP 273
#define RP 274
#define LB 275
#define RB 276
#define PERIOD 277
#define BAR 278
#define MBAR 279
#define SEMI 280
#define UPARROW 281
#define PS 282
#define MINUS 283
#define PE 284




/* Copy the first part of user declarations.  */
#line 25 "parser.y"

# include <stdlib.h>
# include <string.h>
# include "env.h"
# include "drive.h"
# include "parser.h"


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 33 "parser.y"
typedef union YYSTYPE {
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
	struct primstruct 	*u;
	} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 168 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 50 "parser.y"

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
extern char yytext[];

YYSTYPE e;
int errorcount = 0;


/* Line 219 of yacc.c.  */
#line 205 "y.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   309

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  30
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  52
/* YYNRULES -- Number of rules. */
#define YYNRULES  120
/* YYNRULES -- Number of states. */
#define YYNSTATES  162

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    13,    15,    17,    21,    23,
      25,    27,    30,    32,    34,    36,    37,    41,    43,    46,
      48,    50,    54,    59,    61,    64,    66,    68,    71,    75,
      77,    79,    80,    84,    86,    88,    90,    92,    95,    97,
      99,   103,   104,   106,   109,   111,   115,   117,   119,   121,
     123,   126,   128,   132,   135,   138,   140,   143,   146,   150,
     152,   156,   158,   161,   163,   165,   167,   169,   171,   173,
     176,   178,   180,   182,   184,   186,   190,   192,   197,   201,
     202,   205,   210,   211,   214,   216,   219,   222,   224,   227,
     229,   230,   232,   235,   239,   241,   245,   248,   250,   252,
     254,   256,   258,   263,   265,   268,   270,   273,   275,   277,
     279,   281,   283,   285,   287,   291,   294,   296,   297,   300,
     302
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      31,     0,    -1,    32,    -1,    31,    32,    -1,    34,    33,
      40,    21,    -1,    20,    -1,     1,    -1,    35,    36,    38,
      -1,     8,    -1,     1,    -1,    37,    -1,    37,    16,    -1,
       1,    -1,    14,    -1,     8,    -1,    -1,    46,    39,    46,
      -1,    15,    -1,    39,    15,    -1,     1,    -1,    41,    -1,
      40,    24,    41,    -1,    42,    45,    49,    50,    -1,    43,
      -1,    60,    44,    -1,    15,    -1,     1,    -1,    17,    44,
      -1,    43,    17,    44,    -1,    15,    -1,     1,    -1,    -1,
      46,    47,    46,    -1,    23,    -1,    24,    -1,     1,    -1,
      48,    -1,    47,    48,    -1,    15,    -1,    51,    -1,    49,
      22,    51,    -1,    -1,    22,    -1,    26,    52,    -1,    52,
      -1,    15,     9,    52,    -1,    53,    -1,    57,    -1,    54,
      -1,    55,    -1,    55,    58,    -1,    56,    -1,    55,    60,
      62,    -1,    53,    25,    -1,    56,    15,    -1,    59,    -1,
      59,    58,    -1,    17,    59,    -1,    58,    17,    59,    -1,
      62,    -1,    59,    60,    62,    -1,    61,    -1,    61,    61,
      -1,    10,    -1,    23,    -1,    28,    -1,    26,    -1,    29,
      -1,    63,    -1,    62,    15,    -1,    37,    -1,    15,    -1,
      73,    -1,    13,    -1,    64,    -1,    18,    52,    19,    -1,
      66,    -1,    11,     3,    65,    29,    -1,    12,    65,    29,
      -1,    -1,    65,    63,    -1,    20,    67,    69,    21,    -1,
      -1,    68,    23,    -1,    16,    -1,    68,    16,    -1,    72,
      70,    -1,    70,    -1,    26,    52,    -1,    71,    -1,    -1,
      52,    -1,    52,    22,    -1,    72,    52,    22,    -1,    75,
      -1,    74,    80,    19,    -1,    27,    18,    -1,    76,    -1,
      77,    -1,     5,    -1,     6,    -1,     7,    -1,    27,    20,
      81,    21,    -1,     4,    -1,    28,     4,    -1,     3,    -1,
      28,     3,    -1,    75,    -1,    15,    -1,    14,    -1,    17,
      -1,    16,    -1,     8,    -1,    60,    -1,    79,    80,    19,
      -1,    27,    18,    -1,    18,    -1,    -1,    80,    78,    -1,
       3,    -1,    81,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   101,   101,   102,   105,   109,   110,   114,   117,   118,
     121,   122,   123,   127,   128,   131,   132,   135,   136,   137,
     140,   141,   145,   149,   151,   153,   155,   159,   161,   165,
     166,   169,   170,   173,   174,   175,   178,   179,   182,   185,
     186,   189,   190,   193,   194,   197,   199,   203,   204,   207,
     208,   211,   212,   216,   217,   221,   222,   225,   227,   231,
     232,   235,   236,   239,   240,   241,   242,   243,   246,   249,
     253,   254,   255,   256,   257,   258,   259,   262,   264,   268,
     269,   272,   277,   278,   281,   282,   285,   286,   289,   290,
     294,   298,   301,   302,   306,   307,   310,   313,   314,   315,
     316,   317,   318,   322,   323,   326,   327,   330,   331,   332,
     333,   334,   335,   336,   337,   340,   341,   344,   345,   348,
     350
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LITNUM", "LITFNUM", "LITCHAR", "LITSTR",
  "LITSYM", "CLASS", "ASSIGN", "BINARY", "PRIMITIVE", "NAMEDPRIM",
  "PSEUDO", "UPPERCASEVAR", "LOWERCASEVAR", "COLONVAR", "KEYWORD", "LP",
  "RP", "LB", "RB", "PERIOD", "BAR", "MBAR", "SEMI", "UPARROW", "PS",
  "MINUS", "PE", "$accept", "file", "classdef", "lb", "classheading",
  "class", "super", "classname", "instancevars", "instvarlist",
  "methodlist", "method", "pattern", "keypattern", "argvariable",
  "tempvars", "bar", "namelist", "tvariable", "statelist", "op",
  "statement", "sexpression", "cexpression", "kcontinuation",
  "bcontinuation", "ucontinuation", "expression", "keywordlist", "binary",
  "binarysym", "binarychar", "unary", "primary", "primitive", "objlist",
  "block", "barglist", "cvarlist", "opmessagelist", "bstatement",
  "bexpression", "bstatelist", "literal", "alitstart", "iliteral",
  "fliteral", "nliteral", "aliteral", "ias", "litarray", "bytearray", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    30,    31,    31,    32,    33,    33,    34,    35,    35,
      36,    36,    36,    37,    37,    38,    38,    39,    39,    39,
      40,    40,    41,    42,    42,    42,    42,    43,    43,    44,
      44,    45,    45,    46,    46,    46,    47,    47,    48,    49,
      49,    50,    50,    51,    51,    52,    52,    53,    53,    54,
      54,    55,    55,    56,    56,    57,    57,    58,    58,    59,
      59,    60,    60,    61,    61,    61,    61,    61,    62,    62,
      63,    63,    63,    63,    63,    63,    63,    64,    64,    65,
      65,    66,    67,    67,    68,    68,    69,    69,    70,    70,
      71,    71,    72,    72,    73,    73,    74,    75,    75,    75,
      75,    75,    75,    76,    76,    77,    77,    78,    78,    78,
      78,    78,    78,    78,    78,    79,    79,    80,    80,    81,
      81
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     4,     1,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     0,     3,     1,     2,     1,
       1,     3,     4,     1,     2,     1,     1,     2,     3,     1,
       1,     0,     3,     1,     1,     1,     1,     2,     1,     1,
       3,     0,     1,     2,     1,     3,     1,     1,     1,     1,
       2,     1,     3,     2,     2,     1,     2,     2,     3,     1,
       3,     1,     2,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     3,     1,     4,     3,     0,
       2,     4,     0,     2,     1,     2,     2,     1,     2,     1,
       0,     1,     2,     3,     1,     3,     2,     1,     1,     1,
       1,     1,     4,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     0,     2,     1,
       2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     9,     8,     0,     2,     0,     0,     1,     3,     6,
       5,     0,    12,    14,    13,     0,    10,    26,    63,    25,
       0,    64,    66,    65,    67,     0,    20,     0,    23,     0,
      61,    35,    33,    34,     7,     0,    11,    30,    29,    27,
       4,     0,     0,     0,     0,    24,    62,    19,    17,     0,
      21,   105,   103,    99,   100,   101,     0,    79,    73,    71,
       0,    82,     0,     0,     0,    70,    41,    39,    44,    46,
      48,    49,    51,    47,    55,    59,    68,    74,    76,    72,
     117,    94,    97,    98,    38,     0,    36,    28,    18,    16,
      79,     0,     0,     0,    84,    90,     0,    43,    96,     0,
     106,   104,    42,    22,    53,     0,    50,     0,    54,    56,
       0,    69,     0,    32,    37,     0,    71,    78,    80,    45,
      75,     0,    91,     0,    87,    89,    90,    85,    83,   119,
       0,    40,    57,     0,    52,    60,   112,   109,   108,   111,
     110,   116,    95,     0,    65,   113,   107,   118,   117,    77,
      88,    92,    81,    91,    86,   120,   102,    58,   115,     0,
      93,   114
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,     4,    11,     5,     6,    15,    65,    34,    49,
      25,    26,    27,    28,    39,    42,    35,    85,    86,    66,
     103,    67,    68,    69,    70,    71,    72,    73,   106,    74,
     110,    30,    75,    76,    77,    91,    78,    95,    96,   123,
     124,   125,   126,    79,    80,    81,    82,    83,   147,   148,
     112,   130
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -111
static const short int yypact[] =
{
      73,  -111,  -111,    16,  -111,     8,    43,  -111,  -111,  -111,
    -111,     3,  -111,  -111,  -111,    18,    -6,  -111,  -111,  -111,
       6,  -111,  -111,  -111,  -111,    -9,  -111,    65,    -3,     6,
      30,  -111,  -111,  -111,  -111,     7,  -111,  -111,  -111,  -111,
    -111,     3,   204,    39,     6,  -111,  -111,  -111,  -111,    22,
    -111,  -111,  -111,  -111,  -111,  -111,    61,  -111,  -111,    78,
     256,    74,   256,    66,   109,  -111,    72,  -111,  -111,    79,
    -111,   280,    99,  -111,   280,   101,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,    24,  -111,  -111,  -111,  -111,
    -111,   150,   256,    98,  -111,   230,    59,  -111,  -111,   119,
    -111,  -111,   204,  -111,  -111,   274,   106,   274,  -111,   106,
     274,  -111,    92,  -111,  -111,   177,  -111,  -111,  -111,  -111,
    -111,   256,   102,   104,  -111,  -111,   230,  -111,  -111,  -111,
      40,  -111,    30,   274,   101,   101,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,    85,   109,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,   110,  -111,  -111,  -111,    30,  -111,   123,
    -111,  -111
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -111,  -111,   131,  -111,  -111,  -111,  -111,   129,  -111,  -111,
    -111,    95,  -111,  -111,    21,  -111,   -22,  -111,    58,  -111,
    -111,    42,   -59,  -111,  -111,  -111,  -111,  -111,    71,   -99,
     -11,   117,   -55,   -80,  -111,    69,  -111,  -111,  -111,  -111,
      34,  -111,  -111,  -111,  -111,  -110,  -111,  -111,  -111,  -111,
      19,  -111
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -32
static const short int yytable[] =
{
      29,    93,   146,    97,    17,    43,   132,    37,    47,     9,
      36,   118,    40,    18,    44,    41,     7,     1,    19,    31,
      20,    38,    48,    31,     2,    31,    21,    89,    10,    22,
      29,    23,    24,   119,   157,   118,   122,    88,   -15,    84,
      18,    32,    33,   155,    12,    32,    33,    32,    33,   146,
      45,    13,   134,    21,    84,   135,    22,    14,    23,    24,
     107,   156,   150,   113,    90,    87,    31,   153,   -31,   -31,
     -31,   -31,   -31,   -31,     1,   127,   -31,   -31,   -31,   -31,
     -31,     2,   128,   -31,    98,   -31,    99,    92,    32,    33,
      94,   -31,   -31,   -31,   102,    51,    52,    53,    54,    55,
     136,   145,    18,   158,   104,    99,   137,   138,   139,   140,
     141,   142,   100,   101,   108,    21,   111,   120,    22,   143,
     144,    24,   129,   133,   151,   152,    51,    52,    53,    54,
      55,   136,   160,    18,     8,    16,    50,   137,   138,   139,
     140,   141,   161,   114,   131,   109,    21,    46,   145,    22,
     143,   144,    24,    51,    52,    53,    54,    55,    13,   115,
     154,    56,    57,    58,    14,   116,     0,   159,    60,     0,
      61,     0,     0,     0,     0,     0,     0,    63,    64,   117,
      51,    52,    53,    54,    55,    13,     0,     0,    56,    57,
      58,    14,   116,     0,     0,    60,     0,    61,     0,     0,
       0,     0,     0,     0,    63,    64,   149,    51,    52,    53,
      54,    55,    13,     0,     0,    56,    57,    58,    14,    59,
       0,     0,    60,     0,    61,     0,     0,     0,     0,     0,
      62,    63,    64,    51,    52,    53,    54,    55,    13,     0,
       0,    56,    57,    58,    14,    59,     0,     0,    60,     0,
      61,     0,     0,     0,     0,     0,   121,    63,    64,    51,
      52,    53,    54,    55,    13,     0,     0,    56,    57,    58,
      14,    59,     0,     0,    60,     0,    61,    51,    52,    53,
      54,    55,    13,    63,    64,    56,    57,    58,    14,   116,
      18,     0,    60,     0,    61,     0,     0,   105,     0,     0,
       0,    63,    64,    21,     0,     0,    22,     0,    23,    24
};

static const short int yycheck[] =
{
      11,    60,   112,    62,     1,    27,   105,     1,     1,     1,
      16,    91,    21,    10,    17,    24,     0,     1,    15,     1,
      17,    15,    15,     1,     8,     1,    23,    49,    20,    26,
      41,    28,    29,    92,   133,   115,    95,    15,    20,    15,
      10,    23,    24,     3,     1,    23,    24,    23,    24,   159,
      29,     8,   107,    23,    15,   110,    26,    14,    28,    29,
      71,    21,   121,    85,     3,    44,     1,   126,     3,     4,
       5,     6,     7,     8,     1,    16,    11,    12,    13,    14,
      15,     8,    23,    18,    18,    20,    20,     9,    23,    24,
      16,    26,    27,    28,    22,     3,     4,     5,     6,     7,
       8,   112,    10,    18,    25,    20,    14,    15,    16,    17,
      18,    19,     3,     4,    15,    23,    15,    19,    26,    27,
      28,    29,     3,    17,    22,    21,     3,     4,     5,     6,
       7,     8,    22,    10,     3,     6,    41,    14,    15,    16,
      17,    18,    19,    85,   102,    74,    23,    30,   159,    26,
      27,    28,    29,     3,     4,     5,     6,     7,     8,    90,
     126,    11,    12,    13,    14,    15,    -1,   148,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
       3,     4,     5,     6,     7,     8,    -1,    -1,    11,    12,
      13,    14,    15,    -1,    -1,    18,    -1,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,     3,     4,     5,
       6,     7,     8,    -1,    -1,    11,    12,    13,    14,    15,
      -1,    -1,    18,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      26,    27,    28,     3,     4,     5,     6,     7,     8,    -1,
      -1,    11,    12,    13,    14,    15,    -1,    -1,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,     3,
       4,     5,     6,     7,     8,    -1,    -1,    11,    12,    13,
      14,    15,    -1,    -1,    18,    -1,    20,     3,     4,     5,
       6,     7,     8,    27,    28,    11,    12,    13,    14,    15,
      10,    -1,    18,    -1,    20,    -1,    -1,    17,    -1,    -1,
      -1,    27,    28,    23,    -1,    -1,    26,    -1,    28,    29
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     8,    31,    32,    34,    35,     0,    32,     1,
      20,    33,     1,     8,    14,    36,    37,     1,    10,    15,
      17,    23,    26,    28,    29,    40,    41,    42,    43,    60,
      61,     1,    23,    24,    38,    46,    16,     1,    15,    44,
      21,    24,    45,    46,    17,    44,    61,     1,    15,    39,
      41,     3,     4,     5,     6,     7,    11,    12,    13,    15,
      18,    20,    26,    27,    28,    37,    49,    51,    52,    53,
      54,    55,    56,    57,    59,    62,    63,    64,    66,    73,
      74,    75,    76,    77,    15,    47,    48,    44,    15,    46,
       3,    65,     9,    52,    16,    67,    68,    52,    18,    20,
       3,     4,    22,    50,    25,    17,    58,    60,    15,    58,
      60,    15,    80,    46,    48,    65,    15,    29,    63,    52,
      19,    26,    52,    69,    70,    71,    72,    16,    23,     3,
      81,    51,    59,    17,    62,    62,     8,    14,    15,    16,
      17,    18,    19,    27,    28,    60,    75,    78,    79,    29,
      52,    22,    21,    52,    70,     3,    21,    59,    18,    80,
      22,    19
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 106 "parser.y"
    {if (errorcount == 0) genclass((yyvsp[-3].l), (yyvsp[-1].m));}
    break;

  case 6:
#line 110 "parser.y"
    {if ((yytext[0] == ':') || isalpha(yytext[0])) expect(":SuperClass");
                         else expect("open brace [");}
    break;

  case 7:
#line 114 "parser.y"
    {(yyval.l) = (yyvsp[-1].l);}
    break;

  case 9:
#line 118 "parser.y"
    {expect("keyword Class");}
    break;

  case 10:
#line 121 "parser.y"
    {(yyval.l) = mkclass((yyvsp[0].c), (char *) 0);}
    break;

  case 11:
#line 122 "parser.y"
    {(yyval.l) = mkclass((yyvsp[-1].c), (yyvsp[0].c));}
    break;

  case 12:
#line 123 "parser.y"
    {expect("Classname :Superclass");
                                         (yyval.l) = mkclass("Error", (char *) 0);}
    break;

  case 17:
#line 135 "parser.y"
    {addinst((yyvsp[0].c));}
    break;

  case 18:
#line 136 "parser.y"
    {addinst((yyvsp[0].c));}
    break;

  case 19:
#line 137 "parser.y"
    {expect("instance variable");}
    break;

  case 21:
#line 142 "parser.y"
    {(yyvsp[0].m)->nextmethod = (yyvsp[-2].m); (yyval.m) = (yyvsp[0].m);}
    break;

  case 22:
#line 146 "parser.y"
    {deltemps((yyvsp[-2].i)); (yyval.m) = mkmethod((yyvsp[-3].e), (yyvsp[-2].i), (yyvsp[-1].s));}
    break;

  case 23:
#line 150 "parser.y"
    {(yyval.e) = mkkey((struct exprstruct *) 0, (yyvsp[0].k));}
    break;

  case 24:
#line 152 "parser.y"
    {(yyval.e) = mkexpr((struct exprstruct *) 0, bincmd, (yyvsp[-1].c), (struct exprstruct *) 0);}
    break;

  case 25:
#line 154 "parser.y"
    {(yyval.e) = mkexpr((struct exprstruct *) 0, uncmd, (yyvsp[0].c), (struct exprstruct *) 0);}
    break;

  case 26:
#line 155 "parser.y"
    {expect("method pattern");
(yyval.e) = mkexpr((struct exprstruct *) 0, uncmd, "", (struct exprstruct *) 0);}
    break;

  case 27:
#line 160 "parser.y"
    {(yyval.k) = mkklist((struct keylist *) 0, (yyvsp[-1].c), (struct exprstruct *) 0);}
    break;

  case 28:
#line 162 "parser.y"
    {(yyval.k) = mkklist((yyvsp[-2].k), (yyvsp[-1].c), (struct exprstruct *) 0);}
    break;

  case 29:
#line 165 "parser.y"
    {addtemp((yyvsp[0].c), argvar);}
    break;

  case 30:
#line 166 "parser.y"
    {expect("argument variable");}
    break;

  case 31:
#line 169 "parser.y"
    {(yyval.i) = 0;}
    break;

  case 32:
#line 170 "parser.y"
    {(yyval.i) = (yyvsp[-1].i);}
    break;

  case 35:
#line 175 "parser.y"
    {expect("| (vertical bar)");}
    break;

  case 36:
#line 178 "parser.y"
    {(yyval.i) = 1;}
    break;

  case 37:
#line 179 "parser.y"
    {(yyval.i) = (yyvsp[-1].i) + 1;}
    break;

  case 38:
#line 182 "parser.y"
    {addtemp((yyvsp[0].c), tempvar);}
    break;

  case 39:
#line 185 "parser.y"
    {(yyval.s) = (yyvsp[0].s);}
    break;

  case 40:
#line 186 "parser.y"
    {(yyvsp[0].s)->nextstate = (yyvsp[-2].s); (yyval.s) = (yyvsp[0].s);}
    break;

  case 43:
#line 193 "parser.y"
    {(yyval.s) = mkstate(upar, (char *) 0, (yyvsp[0].s));}
    break;

  case 45:
#line 198 "parser.y"
    {(yyval.s) = mkstate(asgn, (yyvsp[-2].c), (yyvsp[0].s));}
    break;

  case 46:
#line 200 "parser.y"
    {(yyval.s) = mkstate(expr, (char *) 0, (struct statestruct *) (yyvsp[0].e));}
    break;

  case 48:
#line 204 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[0].e), semiend, 0, 0);}
    break;

  case 50:
#line 208 "parser.y"
    {(yyval.e) = mkkey((yyvsp[-1].e), (yyvsp[0].k));}
    break;

  case 52:
#line 213 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[-2].e), bincmd, (yyvsp[-1].c), (yyvsp[0].e));}
    break;

  case 53:
#line 216 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[-1].e), semistart, 0, 0);}
    break;

  case 54:
#line 218 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[-1].e), uncmd, (yyvsp[0].c), (struct exprstruct *) 0);}
    break;

  case 55:
#line 221 "parser.y"
    {(yyval.e) = (yyvsp[0].e);}
    break;

  case 56:
#line 222 "parser.y"
    {(yyval.e) = mkkey((yyvsp[-1].e), (yyvsp[0].k));}
    break;

  case 57:
#line 226 "parser.y"
    {(yyval.k) = mkklist((struct keylist *) 0, (yyvsp[-1].c), (yyvsp[0].e));}
    break;

  case 58:
#line 228 "parser.y"
    {(yyval.k) = mkklist((yyvsp[-2].k), (yyvsp[-1].c), (yyvsp[0].e));}
    break;

  case 59:
#line 231 "parser.y"
    {(yyval.e) = (yyvsp[0].e);}
    break;

  case 60:
#line 232 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[-2].e), bincmd, (yyvsp[-1].c), (yyvsp[0].e));}
    break;

  case 61:
#line 235 "parser.y"
    {(yyval.c) = (yyvsp[0].c);}
    break;

  case 62:
#line 236 "parser.y"
    {(yyval.c) = bincat((yyvsp[-1].c), (yyvsp[0].c));}
    break;

  case 68:
#line 247 "parser.y"
    {(yyval.e) = mkexpr((struct exprstruct *) 0, reccmd, (char *) 0, 
					(struct exprstruct *) (yyvsp[0].o));}
    break;

  case 69:
#line 250 "parser.y"
    {(yyval.e) = mkexpr((yyvsp[-1].e), uncmd, (yyvsp[0].c), (struct exprstruct *) 0);}
    break;

  case 70:
#line 253 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.o) = mkobj(classobj, &e);}
    break;

  case 71:
#line 254 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.o) = mkobj(varobj, &e);}
    break;

  case 72:
#line 255 "parser.y"
    {e.t = (yyvsp[0].t); (yyval.o) = mkobj(litobj, &e);}
    break;

  case 73:
#line 256 "parser.y"
    {e.p = (yyvsp[0].p); (yyval.o) = mkobj(pseuobj, &e);}
    break;

  case 74:
#line 257 "parser.y"
    {e.u = (yyvsp[0].u); (yyval.o) = mkobj(primobj, &e);}
    break;

  case 75:
#line 258 "parser.y"
    {e.s = (yyvsp[-1].s); (yyval.o) = mkobj(exprobj, &e);}
    break;

  case 76:
#line 259 "parser.y"
    {e.b = (yyvsp[0].b); (yyval.o) = mkobj(blockobj, &e);}
    break;

  case 77:
#line 263 "parser.y"
    {(yyval.u) = mkprim((yyvsp[-2].i), (yyvsp[-1].r));}
    break;

  case 78:
#line 265 "parser.y"
    {(yyval.u) = mkprim((yyvsp[-2].i), (yyvsp[-1].r));}
    break;

  case 79:
#line 268 "parser.y"
    {(yyval.r) = (struct primlist *) 0;}
    break;

  case 80:
#line 269 "parser.y"
    {(yyval.r) = addprim((yyvsp[-1].r), (yyvsp[0].o));}
    break;

  case 81:
#line 273 "parser.y"
    {(yyval.b) = mkblock((yyvsp[-2].i), (yyvsp[-1].s));
					deltemps((yyvsp[-2].i));}
    break;

  case 82:
#line 277 "parser.y"
    {(yyval.i) = 0;}
    break;

  case 83:
#line 278 "parser.y"
    {(yyval.i) = (yyvsp[-1].i);}
    break;

  case 84:
#line 281 "parser.y"
    {addtemp((yyvsp[0].c), argvar); (yyval.i) = 1;}
    break;

  case 85:
#line 282 "parser.y"
    {addtemp((yyvsp[0].c), argvar); (yyval.i) = (yyvsp[-1].i) + 1;}
    break;

  case 86:
#line 285 "parser.y"
    {(yyvsp[0].s)->nextstate = (yyvsp[-1].s); (yyval.s) = (yyvsp[0].s);}
    break;

  case 87:
#line 286 "parser.y"
    {(yyval.s) = (yyvsp[0].s);}
    break;

  case 88:
#line 289 "parser.y"
    {(yyval.s) = mkstate(blkupar, (char *) 0, (yyvsp[0].s));}
    break;

  case 89:
#line 290 "parser.y"
    {(yyval.s) = mkstate(upar, (char *) 0, (yyvsp[0].s));}
    break;

  case 90:
#line 294 "parser.y"
    {e.p = nilvar;
(yyval.s) = mkstate(expr, (char *) 0,
(struct statestruct *) mkexpr((struct exprstruct *) 0, reccmd, (char *) 0,
	(struct exprstruct *) mkobj(pseuobj, &e)));}
    break;

  case 91:
#line 298 "parser.y"
    {(yyval.s) = (yyvsp[0].s);}
    break;

  case 92:
#line 301 "parser.y"
    {(yyval.s) = (yyvsp[-1].s);}
    break;

  case 93:
#line 303 "parser.y"
    {(yyvsp[-1].s)->nextstate = (yyvsp[-2].s); (yyval.s) = (yyvsp[-1].s);}
    break;

  case 94:
#line 306 "parser.y"
    {(yyval.t) = (yyvsp[0].t);}
    break;

  case 95:
#line 307 "parser.y"
    {e.a = (yyvsp[-1].a); (yyval.t) = mklit(arlit, &e);}
    break;

  case 97:
#line 313 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(fnumlit, &e);}
    break;

  case 98:
#line 314 "parser.y"
    {e.i = (yyvsp[0].i); (yyval.t) = mklit(numlit, &e);}
    break;

  case 99:
#line 315 "parser.y"
    {e.i = (yyvsp[0].i); (yyval.t) = mklit(charlit, &e);}
    break;

  case 100:
#line 316 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(strlit, &e);}
    break;

  case 101:
#line 317 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 102:
#line 318 "parser.y"
    {bytearray[bytetop] = '\0';
                                         (yyval.t) = mklit(bytelit, &e);}
    break;

  case 103:
#line 322 "parser.y"
    {(yyval.c) = (yyvsp[0].c);}
    break;

  case 104:
#line 323 "parser.y"
    {(yyval.c) = bincat("-", (yyvsp[0].c));}
    break;

  case 105:
#line 326 "parser.y"
    {(yyval.i) = (yyvsp[0].i);}
    break;

  case 106:
#line 327 "parser.y"
    {(yyval.i) = - (yyvsp[0].i);}
    break;

  case 107:
#line 330 "parser.y"
    {(yyval.t) = (yyvsp[0].t);}
    break;

  case 108:
#line 331 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 109:
#line 332 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 110:
#line 333 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 111:
#line 334 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 112:
#line 335 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 113:
#line 336 "parser.y"
    {e.c = (yyvsp[0].c); (yyval.t) = mklit(symlit, &e);}
    break;

  case 114:
#line 337 "parser.y"
    {e.a = (yyvsp[-1].a); (yyval.t) = mklit(arlit, &e);}
    break;

  case 117:
#line 344 "parser.y"
    {(yyval.a) = (struct litlist *) 0;}
    break;

  case 118:
#line 345 "parser.y"
    {(yyval.a) = addlit((yyvsp[-1].a), (yyvsp[0].t));}
    break;

  case 119:
#line 348 "parser.y"
    {bytetop = 0;
                                         bytearray[bytetop++] = itouc((yyvsp[0].i));}
    break;

  case 120:
#line 350 "parser.y"
    {bytearray[bytetop++] = itouc((yyvsp[0].i));}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1871 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 352 "parser.y"

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
{  char *p;

   p = (char *)malloc( (unsigned) size);
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

