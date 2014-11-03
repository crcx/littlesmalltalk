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
" browser.st
  Utility to interactively examine data structures"

Object subclass: Browser [
| stack |

^on: obj [
  "Instantiate a browser on the named object"
  self new on: obj
]

showPos [
  "Display stack of objects we're inspecting"
  | cnt |
  ' == stack -> ' print.
  cnt <- 0.
  stack do: [:obj|
    (cnt > 0) ifTrue: [ ' / ' print ].
    obj print.
    cnt <- cnt + 1
  ].
  '' printNl
]

ivars: obj [
  "Dump instance variables of named object"
  | iv |
  iv <- obj class instanceVariables.
  1 to: iv size do: [:idx|
    idx print. Char tab print.
    (iv at: idx) print. ': ' print.
    (Object in: obj at: idx) printNl
  ]
]

run: args on: obj [
  "Compile and run Smalltalk expression on object"
  | t cl |
  "Assemble arguments into single String"
  t := nil.
  args do: [:arg |
    t ifNil: [ t <- arg ]
      ifNotNil: [ t <- t + ' ' + arg ]
  ].
  "Compile into xxBrowse Method for object's class"
  cl <- obj class.
  t <- 'xxBrowse' + String newline + String tab + '^ ' + t.
  (cl addMethod: t) ifNotNil: [
    "Compile OK, run and print result"
    obj xxBrowse printNl.
    "Now clean up (remove) browse method"
    cl removeMethod: #xxBrowse
  ]
]

on: top [
  "Main UI loop for browser"
  | cmd done last obj args idx |
  stack <- Array with: top.
  [true] whileTrue: [
    "Show where we are and get next command"
    self showPos.
    "Get next command, explode into cmd/args"
    'Browse> ' print.
    cmd <- String input.
    cmd ifNil: [ ^ nil ].
    args <- cmd break: ' '.
    cmd <- args at: 1.
    args <- args from: 2.
    "Get top of stack in its own variable"
    obj <- stack at: (stack size).
    done <- false.
    "Index with at:"
    ((cmd = 'i') or: [cmd = 'index']) ifTrue: [
      last <- stack at: (stack size).
      idx <- (args at: 1) asNumber.
      last <- last at: idx.
      last printNl.
      done <- true
    ].
    "Nest to last displayed object"
    ((cmd = 'n') or: [cmd = 'nest']) ifTrue: [
      stack <- stack with: last.
      done <- true
    ].
    "Show class"
    ((cmd = 'c') or: [cmd = 'class']) ifTrue: [
      obj class printNl.
      done <- true
    ].
    "Pop up a stack level"
    ((cmd = 'u') or: [cmd = 'up']) ifTrue: [
      (stack size < 2) ifTrue: [
        'Stack is empty' printNl
      ] ifFalse: [
        stack <- stack from: 1 to: (stack size - 1)
      ].
      done <- true
    ].
    "Raw index"
    (cmd = 'ri') ifTrue: [
      idx <- (args at: 1) asNumber.
      last <- Object in: obj at: idx.
      last printNl.
      done <- true
    ].
    "List instance variables"
    ((cmd = 'iv') or: [cmd = 'ivars']) ifTrue: [
      self ivars: obj.
      done <- true
    ].
    "Show size"
    ((cmd = 'bs') or: [cmd = 'bsize']) ifTrue: [
      obj basicSize printNl.
      done <- true
    ].
    ((cmd = 'sz') or: [cmd = 'size']) ifTrue: [
      obj size printNl.
      done <- true
    ].
    "Print arbitrary expression"
    ((cmd = 'p') or: [cmd = 'print']) ifTrue: [
      self run: args on: obj.
      done <- true
    ].
    ((cmd = 'h') or: [cmd = 'help']) ifTrue: [
      'browser commands' printNl.
      '----------------' printNl.
      'i  index   index with at:' printNl.
      'n  nest    nest to last displayed object' printNl.
      'c  class   show class' printNl.
      'u  up      pop up a stack level' printNl.
      'ri         raw index' printNl.
      'iv ivars   list instance variables' printNl.
      'bs bsize   show size' printNl.
      'sz size    show size' printNl.
      'p  print   print expression' printNl.
      done := true.
    ].
    "All done"
    ((cmd = 'q') or: [cmd = 'quit']) ifTrue: [
      ^ nil
    ].
    "Unknown command?"
    done ifFalse: [ ('Unknown command: ' + cmd) printNl ]
  ]
]
]


{ REPL new REPL }
