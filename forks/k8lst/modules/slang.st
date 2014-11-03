"
 Little Smalltalk, Version 5

 Copyright (C) 1987-2005 by Timothy A. Budd
 Copyright (C) 2007 by Charles R. Childers
 Copyright (C) 2005-2007 by Danny Reinhold
 Copyright (C) 2010 by Ketmar // Vampire Avalon

 ============================================================================
 This license applies to the virtual machine and to the initial image of
 the Little Smalltalk system and to all files in the Little Smalltalk
 packages except the files explicitly licensed with another license(s).
 ============================================================================
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the 'Software'), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
"
Requires [
  ffi
]

Package [
  SLang
]


class: SLang
  | lib
    fnInitTty
    fnResetTty
    fnGetTermInfo
    fnKpInit
    fnSmgInit
    fnSmgReset
    fnSetCursorVisibility
    fnScreenCols
    fnScreenRows
    fnRefresh
    fnGotoXY
    fnGetKey
    fnWriteChar
    fnWriteString
    fnCls
    fnInputPending
    fnCharSet
    fnDrawBox
  |
[
  ^initialize [
    lib ifNil: [ lib := FfiLibrary new: 'libslang' ].
  ]

  ^slInitTty [
    (fnInitTty ifNil: [ fnInitTty := FfiFunction new: lib name: 'SLang_init_tty' ])
      retType: nil with: -1 with: 0 with: 0.  "abortChar, flowCtrl, opost"
  ]

  ^slResetTty [
    (fnResetTty ifNil: [ fnResetTty := FfiFunction new: lib name: 'SLang_reset_tty' ])
      call.
  ]

  ^slGetTermInfo [
    (fnGetTermInfo ifNil: [ fnGetTermInfo := FfiFunction new: lib name: 'SLtt_get_terminfo' ])
      call.
  ]

  ^slKpInit [
    (fnKpInit ifNil: [ fnKpInit := FfiFunction new: lib name: 'SLkp_init' ])
      call.
  ]

  ^slSmgInit [
    (fnSmgInit ifNil: [ fnSmgInit := FfiFunction new: lib name: 'SLsmg_init_smg' ])
      call.
  ]

  ^slSmgReset [
    (fnSmgReset ifNil: [ fnSmgReset := FfiFunction new: lib name: 'SLsmg_reset_smg' ])
      call.
  ]

  ^init [
    self;
      initialize;
      slInitTty;
      slGetTermInfo;
      slKpInit;
      slSmgInit;
      cursor: false.
  ]

  ^reset [
    self;
      cursor: true;
      normalCharSet;
      slSmgReset;
      slResetTty.
  ]

  ^screenCols [
    ^(fnScreenCols ifNil: [ fnScreenCols := FfiValue new: lib name: 'SLtt_Screen_Cols' ])
      as: SmallInt.
  ]

  ^screenRows [
    ^(fnScreenRows ifNil: [ fnScreenRows := FfiValue new: lib name: 'SLtt_Screen_Rows' ])
      as: SmallInt.
  ]

  ^cursor: aVisFlag [
    (fnSetCursorVisibility ifNil: [ fnSetCursorVisibility := FfiFunction new: lib name: 'SLtt_set_cursor_visibility' ])
      retType: nil with: aVisFlag.
  ]

  ^refresh [
    (fnRefresh ifNil: [ fnRefresh := FfiFunction new: lib name: 'SLsmg_refresh' ])
      retType: nil
  ]

  ^cls [
    (fnCls ifNil: [ fnCls := FfiFunction new: lib name: 'SLsmg_cls' ])
      retType: nil
  ]

  ^gotoX: x y: y [
    (fnGotoXY ifNil: [ fnGotoXY := FfiFunction new: lib name: 'SLsmg_gotorc' ])
      retType: nil with: y with: x
  ]

  ^writeChar: s [
    (fnWriteChar ifNil: [ fnWriteChar := FfiFunction new: lib name: 'SLsmg_write_char' ])
      retType: nil with: s
  ]

  ^writeString: s [
    (s class == Char) ifTrue: [ ^self writeChar: s ].
    (fnWriteString ifNil: [ fnWriteString := FfiFunction new: lib name: 'SLsmg_write_string' ])
      retType: nil with: s
  ]

  ^getKey [
    ^(fnGetKey ifNil: [ fnGetKey := FfiFunction new: lib name: 'SLkp_getkey' ])
      retType: SmallInt
  ]

  ^inputPending: aWaitSec [
    "aWaitSec either float or int or nil"
    aWaitSec ifNil: [ aWaitSec := 0 ]
      ifNotNil: [
        aWaitSec isFloat
          ifTrue: [ aWaitSec := (aWaitSec * 10) asSmallInt ]
          ifFalse: [ aWaitSec := aWaitSec * 10 ]].
    ^(fnInputPending ifNil: [ fnInputPending := FfiFunction new: lib name: 'SLang_input_pending' ])
      retType: SmallInt with: aWaitSec
  ]

  ^inputPending [
    ^self inputPending: nil
  ]

  ^charSet: idx [
    (fnCharSet ifNil: [ fnCharSet := FfiFunction new: lib name: 'SLsmg_set_char_set' ])
      retType: nil with: idx
  ]

  ^normalCharSet [
    ^self charSet: 0
  ]

  ^lineCharSet [
    ^self charSet: 1
  ]

  ^drawBoxX0: x0 y0: y0 x1: x1 y1: y1 [
    (fnDrawBox ifNil: [ fnDrawBox := FfiFunction new: lib name: 'SLsmg_draw_box' ])
      retType: nil with: x0 with: y0 with: x1 with: y1
  ]

  ^writeStringX: x y: y text: text [
    ^self; gotoX: x y: y; writeString: text
  ]

  ^writeCharX: x y: y char: char [
    ^self; gotoX: x y: y; writeChar: char
  ]
]


"
flushInput [
  [(self pendingInput: 0) ~= 0] whileTrue: [self getKey].
]
"


"
{
  | c |
  SLang init.
  SLang cls.
  SLang drawBoxX0: 7 y0: 5 x1: 10 y1: 15.
  SLang gotoX: 2 y: 4.
  SLang writeString: 'test!'.
  SLang refresh.
  c := SLang getKey.
  SLang reset.
  SLang screenCols print. 'x' print. SLang screenRows printNl.
  c printNl.
}
"
