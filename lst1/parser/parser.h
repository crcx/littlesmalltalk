/*
	Little Smalltalk
		definitions used by parser
*/


enum vartypes {instvar, argvar, tempvar};

struct varstruct {
        struct varstruct *nextvar;
        enum vartypes vtype;
        char *text;
        short position;
        };

enum objtypes {classobj, varobj, instvarobj, contvarobj, 
	litobj, pseuobj, primobj, exprobj, blockobj};

struct objstruct {
        enum objtypes objtype;
        union {
		char *varname;
		int varoffset;
                struct litstruct *litinfo;
                enum   pseuvars pseuinfo;
		struct primstruct *priminfo;
                struct statestruct *stateinfo;
                struct blockstruct *blockinfo;
             } ee;
        };

struct blockstruct {
	int arglocation;
	int numargs;
	struct statestruct *bstates;
	};

enum littypes {numlit, fnumlit, charlit, strlit, symlit, arlit, bytelit};

struct litstruct {
        enum littypes littype;
        union {
                int litint;
                char litchar;
                char *litstr;
                char *litsym;
                struct litlist *litarry;
             } ll;
        };

struct litlist {
        struct litstruct *litele;
        struct litlist *nextlit;
        int    litposition;
        };

struct primstruct {
	int primnumber;
	struct primlist *plist;
	} ;

struct primlist {
	struct primlist *nextprim;
	struct objstruct *pobject;
	};

enum cmdtypes {reccmd, uncmd, bincmd, keycmd, semistart, semiend};

struct exprstruct {
        enum   cmdtypes cmdtype;
        char   *cmdname;
        struct exprstruct *receiver;
        union {
                struct exprstruct *argument;
                struct keylist *keys;
                struct objstruct *recobj;
              } cc;
        struct exprstruct *nextcmd;
        };

enum statetypes {blkupar, upar, asgn, iasgn, casgn, expr};

struct statestruct {
        enum statetypes statetype;
        struct statestruct *nextstate;
        union {
                struct varstruct *variable;
		int varpos;
              } mm;
        union {
                struct statestruct *stateexpr;
                struct exprstruct *cmd;
              } nn;
        };

struct keylist {
        char   *keyword;
        struct exprstruct *arg;
        struct keylist *nextkey;
        };

struct methodstruct {
	struct exprstruct *pattern;
	int    numtempvars;
        struct statestruct *states;
        struct methodstruct *nextmethod;
        };

struct classstruct {
        char *name;
        char *super;
        };

# define structalloc(type) (struct type *) alloc(sizeof (struct type ))

extern struct varstruct *instvars;
extern struct varstruct *contextvars;

# define addinst(x) (instvars = addvlist(mkvar(x, instvar), instvars))

extern char *walloc();
