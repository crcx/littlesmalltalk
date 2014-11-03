#ifndef LST_IUP_H
#define LST_IUP_H

#include <stdio.h>
#include <stdlib.h>

#include "lstcore/k8lst.h"


/*extern LST_PRIMFN_NONSTATIC(lpFLTKAction);*/
extern void guiInit (int *argc, char ***argv);
extern void guiShutdown (void);

extern void guiModalMessage (const char *title, const char *message);

extern void lstInitPrimitivesIUP (void);

extern void guiLoopStep (void);
extern int guiHasEvent (void);

#endif
