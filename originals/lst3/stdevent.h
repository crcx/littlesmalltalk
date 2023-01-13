/* STDWIN -- EVENT STRUCT DEFINITION. */

struct event {
	int type;
	WINDOW *window;
	union {
	/* case WE_CHAR: */
		int character;
	/* case WE_COMMAND: */
		int command;
	/* case WE_MENU: */
		struct { int id; int item; } m;
	/* case WE_DRAW: */
		struct { int left, top, right, bottom; } area;
	/* case WE_MOUSE_DOWN, WE_MOUSE_MOVE, WE_MOUSE_UP: */
		struct {
			int v;
			int h;
			int clicks;
			int button;
			int mask;
		} where;
	} u;
};

#define EVENT struct event

/* Event types (should be grouped differently). */

#define WE_NULL		0	/* (Used internally) */
#define WE_ACTIVATE	1	/* Window became active */
#define WE_CHAR		2	/* Character typed at keyboard */
#define WE_COMMAND	3	/* Special command, function key etc. */
#define WE_MOUSE_DOWN	4	/* Mouse button pressed */
#define WE_MOUSE_MOVE	5	/* Mouse moved with button down */
#define WE_MOUSE_UP	6	/* Mouse button released */
#define WE_MENU		7	/* Menu item selected */
#define WE_SIZE		8	/* Window size changed */
#define WE_MOVE		9	/* (Reserved) */
#define WE_DRAW		10	/* Request to redraw part of window */
#define WE_TIMER	11	/* Window's timer went off */
#define WE_DEACTIVATE	12	/* Window became inactive */

/* Command codes for WE_COMMAND.
   Special ways of entering these are usually available,
   such as clicking icons, standard menu items or special keys.
   Some ASCII keys are also passed back as commands since they
   more often than not need special processing. */

#define WC_CLOSE	1	/* Should become a separate event! */
/* The following four are arrow keys */
#define WC_LEFT		2
#define WC_RIGHT	3
#define WC_UP		4
#define WC_DOWN		5
/* ASCII keys */
#define WC_CANCEL	6
#define WC_BACKSPACE	7
#define WC_TAB		8
#define WC_RETURN	9
/* IBM-PC keys -- not in all implementations */
#define WC_HOME		10
#define WC_END		11
#define WC_CLEAR	12
#define WC_INS		13
#define WC_DEL		14
#define WC_PAGE_UP	15
#define WC_PAGE_DOWN	16
#define WC_META_LEFT	17
#define WC_META_RIGHT	18
#define WC_META_HOME	19
#define WC_META_END	20
#define WC_META_PAGE_UP	21
#define WC_META_PAGE_DOWN	22
/* Should have entries for Alt-letter and F1-F10 etc. ? */
