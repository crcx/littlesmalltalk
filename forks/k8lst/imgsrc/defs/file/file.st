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


EXTPRIM FileExists
EXTPRIM FileOpen
EXTPRIM FilePOpen
EXTPRIM FileGetChar
EXTPRIM FilePutChar
EXTPRIM FileClose
EXTPRIM FileWriteImage
EXTPRIM FileSeek
EXTPRIM FileSize
EXTPRIM FileGetFD
EXTPRIM FileWriteByteArray
EXTPRIM FileReadByteArray
EXTPRIM FileReadLine

CLASS File  Stream  fileID fileName


COMMENT ---------File----------
METHODS FOR File
^exists: aName [
  <#FileExists aName>.
  self primitiveFailed
]

^close: id [
  <#FileClose id>.
  self primitiveFailed
]

^doOpen: nm mode: mode [
  <#FileOpen nm mode>.
  self primitiveFailed
]

^doPOpen: nm mode: mode [
  <#FilePOpen nm mode>.
  self primitiveFailed
]

^open: nm mode: mode [
  | obj |
  obj := self new.
  self;
    in: obj var: #fileID put: (self doOpen: nm mode: mode);
    in: obj var: #fileName put: nm.
  ^obj
]

^popen: nm mode: mode [
  | obj |
  obj := self new.
  self
    in: obj var: #fileID put: (self doPOpen: nm mode: mode);
    in: obj var: #fileName put: nm.
  ^obj
]

^openRead: nm [
  "open new file for reading"
  ^self open: nm mode: 'rb'
]

^openWrite: nm [
  "open new file for writing"
  ^self open: nm mode: 'wb'
]

^openUpdate: nm [
  "open new file for reading and writing"
  ^self open: nm mode: 'rb+'
]

^popenRead: nm [
  "open new file for reading"
  ^self popen: nm mode: 'r'
]

^popenWrite: nm [
  "open new file for writing"
  ^self popen: nm mode: 'w'
]

^fileIn: nm [
  self openRead: nm;
    opened ifFalse: [ ^self error: 'cannot open file ' + nm ];
    fileIn;
    close.
  ^true  "fileIn completed"
]

fileName [
  ^fileName
]

opened [
  ^fileID notNil
]

isOpen [
  ^fileID notNil
]

fd [
  fileID ifNil: [ ^nil ].
  <#FileGetFD fileID>.
  self primitiveFailed
]

close [
  "close file, return file descriptor"
  fileID ifNotNil: [
    self class close: fileID.
    fileID := nil
  ]
]

notOpened [
  self error: 'file is not open'
]

writeImage [
  "save the current image in a file"
  fileID ifNotNil: [ <#FileWriteImage fileID 0> ].
  ^nil
]

writeImageNoSrc [
  "save the current image in a file w/o sources"
  fileID ifNotNil: [ <#FileWriteImage fileID 1> ].
  ^nil
]

size [
  <#FileSize fileID>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

readCharValue [
  <#FileGetChar fileID>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

writeCharValue: n [
  <#FilePutChar fileID n>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

read: buf size: count [
  <#FileReadByteArray fileID buf count>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

write: buf size: count [
  <#FileWriteByteArray fileID buf count>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

readLine [
  <#FileReadLine fileID>.
  fileID ifNil: [ self notOpened ].
  self primitiveFailed
]

doReadChar [
  | c |
  (c := self readCharValue) ifNil: [ ^nil ].
  ^Char new: c
]

at: idx [
  <#FileSeek fileID idx>.
  self primitiveFailed
]

write: buf [
  ^self write: buf size: (buf size)
]

read: buf [
  ^self read: buf size: (buf size)
]

at: idx put: buf [
  self;
    at: idx;
    write: buf size: buf size
]

at: idx get: buf [
  | size |
  self at: idx.
  size := buf size.
  <#FileReadByteArray fileID buf size>.
  self primitiveFailed
]

at: idx size: count [
  | buf res |
  buf := ByteArray new: count.
  res := self at: idx get: buf.
  (res < count) ifTrue: [ buf := buf from: 1 to: res ].
  ^buf
]

newline [
  ^self write: (String newline)
]
!
