/*
 * coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * Understanding is not required. Only obedience.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#ifndef LST_X11_PRIMITIVES_H
#define LST_X11_PRIMITIVES_H

#ifdef __cplusplus
extern "C" {
#endif


extern void lstInitX11Library (void);

extern void lstInitPrimitivesX11 (void);

extern void x11LoopStep (void);
extern int x11HasEvent (void);

extern lstObject *x11GetEvent (void);


#ifdef __cplusplus
}
#endif
#endif
