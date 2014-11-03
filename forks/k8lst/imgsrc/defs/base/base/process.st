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


CLASS Process  Object  context state result running

COMMENT -----------Process--------------
METHODS FOR Process
^newWithMethod: aMethodName class: aClass args: aArgs [
  | ctx proc |
  aClass ifNil: [ aClass := UndefinedObject ].
  aArgs := (aArgs ifNil: [ Array with: aClass ] ifNotNil: [ aArgs withFirst: aClass ]).
  (ctx := Context new) setup: (aClass findMethod: aMethodName asSymbol) withArguments: aArgs.
  (proc := Process new) context: ctx.
  ^proc
]

^newWithMethod: aMethodName class: aClass [
  ^self newWithMethod: aMethodName class: aClass args: nil
]

context: aContext [
  aContext setProcessOwner: self.
  context := aContext.
]

running [
  running ifNil: [ ^false ].
  ^true
]

context [
  ^context
]

state [
  ^state
]

result [
  ^result
]

isComplete [
  ^context isNil
]

^lock [
  <#LockUnlockSheduler true>
]

^unlock [
  <#LockUnlockSheduler false>
]

^lockState [
  <#LockUnlockSheduler>
]

^ticks [
  <#TicksGetSet>
]

^ticks: ticks [
  <#TicksGetSet ticks>.
  self primitiveFailed
]

^freeSlice [
  self lock.
  self ticks > 0 ifTrue: [ self ticks: 1 ].
  self unlock.
]

doExecute: ticks [
  <#ExecuteNewProcessAndWait self ticks>.
  self primitiveFailed.
]

COMMENT  lstReturnError = 2,   /* 2: error: called */
COMMENT  lstReturnBadMethod,   /* 3: Unknown method for object */
COMMENT  lstReturnReturned,    /* 4: Top level method returned */
COMMENT  lstReturnTimeExpired, /* 5: Time quantum exhausted */
COMMENT  lstReturnBreak        /* 6: lstBXBreakpoint instruction */

errorReport: res [
  (res = 3) ifTrue: [
    "Note: state field is filled in with arguments on error"
    (state at: 1) print.
    ' (class ' print. (state at: 1) class print. ') ' print.
    'does not understand: ' print.  result printNl
  ].
  ((res = 4) or: [ res = 7 ]) ifFalse: [ '=============\nbacktrace:' printNl. context backtrace ]
]

execute [
  | res |
  res := self doExecute: 0.
  ((res = 4) or: [ res = 7 ]) ifTrue: [ ^result ].
  [ self errorReport: res ] runLocked.
  ^nil
]
!
