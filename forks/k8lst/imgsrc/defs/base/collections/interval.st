COMMENTS
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
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
ENDCOMMENTS


COMMENT ----------Interval------------
METHODS FOR Interval
^from: l to: h step: s [
  | newInterval |
  newInterval := self in: self new at: 1 put: l.
  self in: newInterval at: 2 put: h.
  self in: newInterval at: 3 put: s.
  ^newInterval
]

do: aBlock [
  | current |
  current := low.
  step < 0
    ifTrue: [[ high > current ] whileFalse: [ aBlock value: current. current := current + step ]]
    ifFalse: [[ current > high ] whileFalse: [ aBlock value: current. current := current + step ]]
]

low: l [
  low := l
]

high: h [
  high := h
]

low [
  ^low
]

high [
  ^high
]

includes: val [
  "Check within range first"
  ((val < low) or: [val > high]) ifTrue: [ ^false ].
  "Then check if in equivalence class of interval"
  ^((val - low) % step) = 0
]

printString [
  | s |
  s := (self class printString) + ' <' + low printString + '..' + high printString.
  (step ~= 1) ifTrue: [ s := s + ' by ' + step printString ].
  ^s + '>'
]

atRandom [
  "Return a random element from our sequence"
  | ret |
  ret := (SmallInt atRandom) % ((high - low + 1) / step).
  ^low + (ret * step)
]
!
