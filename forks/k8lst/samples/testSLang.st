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
  slang
]


{
  | k |
  SLang;
    init;
    cls;
    writeStringX: 0 y: 1 text: 'Enter ctrl-g to quit';
    writeStringX: 0 y: 3 text: (SLang screenRows asString + 'x' + SLang screenCols asString).

  SLang drawBoxX0: 7 y0: 5 x1: 10 y1: 15.

  SLang writeCharX: 20 y: 7 char: $^.
  SLang lineCharSet.
  8 to: 15 do: [:r | SLang writeCharX: 20 y: r char: $a ].
  SLang normalCharSet.
  SLang writeCharX: 20 y: 16 char: $v.

  SLang refresh.

  [ (k := SLang getKey) = 7 or: [ k = 65535 ] ] whileFalse: [
    SLang writeStringX: 0 y: 30 text: ('you pressed ' + k asString + '    ').
    SLang refresh ].

  SLang reset.
}
