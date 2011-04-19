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
/* Line 1447 of yacc.c.  */
#line 113 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



