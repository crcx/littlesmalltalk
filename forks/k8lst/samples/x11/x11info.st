Requires [ x11 ]


{
  X11Singleton initialize.
  ('black: ' + X11Singleton blackPixel) printNl.
  ('white: ' + X11Singleton whitePixel) printNl.
  ('all planes: ' + X11Singleton allPlanes) printNl.
  ('fd: ' + X11Singleton connectionFd) printNl.
  ('depth: ' + X11Singleton depth) printNl.
  ('planes: ' + X11Singleton planes) printNl.
  ('name: ' + X11Singleton name) printNl.
  ('vendor: ' + X11Singleton vendor) printNl.
  ('release: ' + X11Singleton release) printNl.
  ('width: ' + X11Singleton width) printNl.
  ('height: ' + X11Singleton height) printNl.
  X11Singleton deinitialize.
}
