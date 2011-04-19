/*
	Little Smalltalk

        defines used by both parser and driver

*/

# define TWOBIT         0
# define PUSHINSTANCE   1
# define PUSHTEMP       2
# define PUSHLIT        3
# define PUSHCLASS      4
# define PUSHSPECIAL    5
# define POPINSTANCE    6
# define POPTEMP        7
# define SEND           8
# define SUPERSEND      9
# define UNSEND        10
# define BINSEND       11
# define ARITHSEND     12
# define KEYSEND       13
# define BLOCKCREATE   14
# define SPECIAL       15

/* arguments for special */

# define NOOP           0
# define DUPSTACK       1
# define POPSTACK       2
# define RETURN         3
# define BLOCKRETURN    4
# define SELFRETURN     5
# define SKIPTRUEPUSH   6
# define SKIPFALSEPUSH  7
# define SKIPFORWARD    8
# define SKIPBACK       9
# define PRIMCMD       10
# define SKIPT         11
# define SKIPF         12

enum pseuvars {nilvar, truevar, falsevar, selfvar, supervar, smallvar,
		procvar};

# define streq(a,b) (strcmp(a,b) == 0)

/* only include driver code in driver, keeps both lint and the 11/70 quiet */
# ifdef DRIVECODE

 enum lextokens { nothing, LITNUM , LITFNUM , LITCHAR , LITSTR , LITSYM , 
	LITARR , LITBYTE , ASSIGN , BINARY , PRIMITIVE , PSEUDO , 
	UPPERCASEVAR , LOWERCASEVAR , COLONVAR , KEYWORD ,
 	LP , RP , LB , RB , PERIOD , BAR , SEMI , PS , MINUS , PE , NL };

typedef union  {
	char 		*c;
	double 		 f;
	int  		 i;
	enum pseuvars 	 p;
	} tok_type;

extern tok_type t;

# endif
