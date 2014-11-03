Requires [ x11 ]


{
  X11Singleton initialize.
  X11Singleton XSetWindowBackground: nil color: (X11Color new: 0 g: 0 b: 0) color.
  X11Singleton deinitialize.
}
