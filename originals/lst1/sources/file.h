/*
	Little Smalltalk

		class File definitions
		timothy a. budd, 11/84
*/
/*
	files use standard i/o package
*/

struct file_struct {
	int l_ref_count;
	int l_size;
	int file_mode;
	FILE *fp;
	};

typedef struct file_struct file;

extern object *new_file();
extern object *file_read();

/* files can be opened in one of three modes, modes are either
	0 - char mode - each read gets one char
	1 - string mode - each read gets a string
	2 - integer mode - each read gets an integer
*/
# define CHARMODE 0
# define STRMODE  1
# define INTMODE  2
