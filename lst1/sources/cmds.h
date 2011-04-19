extern char x_str[];
/*
	Little Smalltalk

	The following very common commands are given a concise description
	in bytecodes.

*/

static char *unspecial[] = {&x_str[2185], &x_str[1980], &x_str[2254], &x_str[2798], &x_str[914],
		&x_str[3015], &x_str[1502], &x_str[2221], &x_str[2360], &x_str[2366],
		&x_str[2848], &x_str[1122], &x_str[2250],

		/* after the first 16 - which should be the most common
		messages, order doesn't make as much difference so we
		might as well list things in alphabetical order */

		&x_str[421],
		&x_str[586], 
		&x_str[625], 
		&x_str[699], 
		&x_str[708], 
		&x_str[867],
		&x_str[955], 
		&x_str[968], 
		&x_str[1100],
		&x_str[1157],
		&x_str[1395], 
		&x_str[1464],
		&x_str[1508],
		&x_str[1594], 
		&x_str[1929],
		&x_str[1956],
		&x_str[1998],
		&x_str[2043],
		&x_str[2048],
		&x_str[2066], 
		&x_str[2194],
		&x_str[2250], 
		&x_str[2288],
		&x_str[2453],
		&x_str[2518],
		&x_str[2530],
		&x_str[2573],
		&x_str[2668],
		&x_str[2704], 
		&x_str[2757], 
		&x_str[2824],
		&x_str[2829], 
		&x_str[2837],
		&x_str[2865],
		&x_str[2976], 
		&x_str[3001],
		&x_str[3208], 
		&x_str[3225], 
		0 };

# define VALUECMD 5
# define PRNTCMD  8

static char *binspecial[] = {&x_str[2189], &x_str[740], &x_str[2943], &x_str[1283], &x_str[3021],
                      &x_str[28], &x_str[3250], &x_str[2930], &x_str[3166], &x_str[3154],
                      &x_str[1685], &x_str[1660], &x_str[1356], &x_str[425], 
		      &x_str[16], &x_str[928],

		      &x_str[419], 
		      &x_str[12], 
		      &x_str[18],
		      &x_str[474], 
		      &x_str[495], 
		      &x_str[507], 
		      &x_str[517], 
		      &x_str[807], 
		      &x_str[901],
		      &x_str[936],
		      &x_str[1166], 
		      &x_str[1600], 
		      &x_str[1605],
		      &x_str[1620],
		      &x_str[1718], 
		      &x_str[1709], 
		      &x_str[2023], 
		      &x_str[2069], 
		      &x_str[2102], 
		      &x_str[2205],
		      &x_str[2269],
		      &x_str[2403],
		      &x_str[2469],
		      &x_str[2482], 
		      &x_str[2542], 
		      &x_str[2656],
		      &x_str[2675],
		      &x_str[2695], 
		      &x_str[2725],
		      &x_str[2769], 
		      &x_str[2818], 
		      &x_str[2905],
		      &x_str[2964],
		      &x_str[3201],
		      &x_str[3210], 
		      &x_str[3227], 
		      &x_str[1728],
		      0};

static char *arithspecial[] = {&x_str[10], &x_str[14], &x_str[8], &x_str[412],
                        &x_str[849], &x_str[817], &x_str[842],
                        &x_str[21], &x_str[23], &x_str[26], &x_str[3247], &x_str[33], &x_str[31], 
			&x_str[2477], &x_str[2383], &x_str[2120], &x_str[2085], 
			0};

static char *keyspecial[] = {&x_str[757], &x_str[1693], &x_str[1669],
                        &x_str[3028], &x_str[2947], &x_str[744],
			&x_str[1750], &x_str[1881], 
			&x_str[2490], &x_str[2553], 
			&x_str[794], 
			&x_str[1444], &x_str[1483],
			&x_str[1332],
			&x_str[1409],
			&x_str[1184],
			0};

/*	The classes included in the standard prelude
	also have a very concise description in bytecode representation
*/

static char *classpecial[] = {&x_str[38], &x_str[44], 
	&x_str[70], &x_str[74], &x_str[80], &x_str[88],
	&x_str[98], &x_str[103], &x_str[109], &x_str[120],
	&x_str[128], &x_str[139], &x_str[145], &x_str[150],
	&x_str[156], &x_str[164], &x_str[176],
	&x_str[185], &x_str[201], &x_str[223], &x_str[238],
	&x_str[245], &x_str[252], &x_str[270],
	&x_str[290], &x_str[297],
	&x_str[324], &x_str[347], &x_str[361], &x_str[368],
	&x_str[386], &x_str[391],
	0 };
