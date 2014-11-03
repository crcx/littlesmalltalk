{
  | fo maxArgs |
  maxArgs := 32.

  fo := File openWrite: '../lstpl_ffi_fdef.c'.
  fo write: 'typedef void *(*Func00) (void);\n'.
  fo write: 'typedef void *(*Func01) (intptr_t);\n'.
  2 to: maxArgs do: [:i |
    fo write: 'typedef void *(*Func'.
    fo write: (i printWidth: 2).
    fo write: ') (intptr_t'.
    2 to: i do: [ fo write: ', intptr_t' ].
    fo write: ');\n'.
  ].
  fo close.

  fo := File openWrite: '../lstpl_ffi_switch.c'.
  fo write: '    case 0: fres = ((Func00)func)(); break;\n'.
  fo write: '    case 1: fres = ((Func01)func)(lstMakeArg(LST_PRIMARG(2))); break;\n'.
  2 to: maxArgs do: [:i |
    fo write: '    case '.
    fo write: i printString.
    fo write: ': fres = ((Func'.
    fo write: (i printWidth: 2).
    fo write: ')func)(lstMakeArg(LST_PRIMARG(2))'.
    2 to: i do: [:n |
      fo write: ', lstMakeArg(LST_PRIMARG('.
      fo write: (n+1) printString.
      fo write: '))'.
    ].
    fo write: '); break;\n'.
  ].
  fo close.


  fo := File openWrite: 'fficall.st'.
  fo write: '"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [
  FFI
]


FfiFunction extend [\n'.

  1 to: maxArgs do: [:i |
    fo write: '  retType: aType'.
    1 to: i do: [:n |
      fo write: ' with: anArg'.
      fo write: n printString.
    ].
    fo write: ' [\n'.
    fo write: '    <#FFICall func aType'.
    1 to: i do: [:n |
      fo write: ' anArg'.
      fo write: n printString.
    ].
    fo write: '>.\n'.
    fo write: '    self primitiveFailed.\n'.
    fo write: '  ]\n'.
  ].
  fo write: ']\n'.
  fo close.
}
