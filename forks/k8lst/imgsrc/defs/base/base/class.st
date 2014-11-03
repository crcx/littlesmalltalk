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


COMMENT ---------Class-------------------
METHODS FOR Class
new [
  "return a new instance of ourselves"
  <#NewObject self cvSize>.
  self primitiveFailed
]

basicNew [
  "Like new"
  <#NewObject self cvSize>.
  self primitiveFailed
]

package [
  ^cvPackage
]

package: pkg [
  "strictly for internal use by Little Smalltalk!"
  cvPackage := pkg.
  ^pkg
]

getName [
  ^cvName
]

name: n parent: c variables: v [
  "create a new class with the given characteristics"
  cvName := n.
  cvParentClass := c.
  cvMethods := Dictionary new.
  cvSize := v size + (c ifNotNil: [c size] ifNil: [0]).
  cvVariables := v.
  cvPackage := nil.
]

size [
  ^cvSize
]

superclass [
  ^cvParentClass
]

super [
  ^cvParentClass
]

parent [
  ^cvParentClass
]

variables [
  ^cvVariables
]

methods [
  "return the tree of methods"
  ^cvMethods
]

printString [
  "just return our name"
  ^cvName printString
]

asString [
  ^self printString
]

isMeta [
  ^(cvName asString from: 1 to: 4) = 'Meta'
]

^flushCache [
  <#FlushMethodCache>.
  self primitiveFailed
]

^invalidateCacheForMethod: anOldMth [
  <#FlushMethodCache anOldMth true>
]

subclasses [
  self subclasses: 0
]

subclass: nm [
  ^self subclass: nm variables: (Array new: 0) classVariables: (Array new: 0)
]

subclass: nm variables: v [
  ^self subclass: nm variables: v classVariables: (Array new: 0)
]

subclass: nm variables: v classVariables: cv [
  ^self subclass: nm variables: v classVariables: cv asProxy: false
]

subclass: nm variables: v classVariables: cv asProxy: isProxy [
  | meta aClass |
  ((((nm == #Class)
    or: [ nm == #Object ])
    or: [ nm == #Method ])
    or: [ ((nm asString) from: 1 to: 4) = 'Meta' ]) ifTrue: [ self error: 'invalid class name in subclassing: "' + nm asString + '"' ].
  meta := Class new name: ('Meta' + nm asString) asSymbol parent: self class variables: cv.
  Package addToCurrent: meta.
  aClass := (meta new name: nm parent: (isProxy ifTrue: [nil] ifFalse: [self]) variables: v).
  "FIXME: we don't need to kill all cache here"
  Class flushCache.
  Package addToCurrent: aClass.
  globals at: nm put: aClass.
  ^aClass
]

proxy: nm variables: v classVariables: cv [
  ^self subclass: nm variables: v classVariables: cv asProxy: true.
]

findMethod: aName ifAbsent: aBlock [
  ^cvMethods at: aName asSymbol ifAbsent: [ ^aBlock value ]
]

findMethod: aName [
  ^self findMethod: aName ifAbsent: [ ^self error: 'no such method: ' + aName printString ]
]

findMethodInAll: aName ifAbsent: aBlock [
  | meth |
  (meth := cvMethods at: aName ifAbsent: [
    cvParentClass ifNotNil: [ ^cvParentClass findMethodInAll: aName ifAbsent: aBlock ] ifNil: [ ^aBlock value ].
  ]) ifNil: [ ^aBlock value ].
  ^meth
]

findMethodInAll: aName [
  ^self findMethodInAll: aName ifAbsent: [ ^self error: 'no such method: ' + aName printString ]
]

addMethod: text withCompiler: compiler [
  | meth mold |
  text firstNonBlankChar == $^ ifTrue: [ ^self class addMethod: (text removeLeadingBlanks from: 2) ].
  meth := self compileMethod: text withCompiler: compiler.
  meth ifNotNil: [
    [
      mold := cvMethods at: meth name ifAbsent: [ nil ].
      cvMethods at: meth name put: meth.
      mold ifNotNil: [ Class invalidateCacheForMethod: mold ].
      "Class flushCache."
    ] runLocked.
    ^meth
  ].
  ^nil
]

replaceMethod: methName with: aMethod [
  | mold |
  [
    mold := self findMethod: methName ifAbsent: [ ^self error: 'no such method: ' + methName printString ].
    cvMethods at: methName put: aMethod.
    mold ifNotNil: [ Class invalidateCacheForMethod: mold ].
    "Class flushCache."
  ] runLocked.
  ^mold
]

removeMethod: methName [
  | mold |
  [
    mold := self findMethod: methName ifAbsent: [ ^self error: 'no such method: ' + methName printString ].
    cvMethods removeKey: methName ifAbsent: [ nil ].
    mold ifNotNil: [ Class invalidateCacheForMethod: mold ].
    "Class flushCache."
  ] runLocked.
  ^mold
]

allMethods [
  "return all our methods as Dictionary"
  | allMethods |
  cvParentClass
    ifNil: [ allMethods := Dictionary new ]
    ifNotNil: [ allMethods := cvParentClass allMethods ].
  cvMethods keysAndValuesDo: [:n :m | allMethods at: n put: m ].
  ^allMethods
]

instanceVariables [
  "return all our variable names as Array"
  | names |
  cvParentClass
    ifNotNil: [ names := cvParentClass instanceVariables ]
    ifNil: [ names := Array new: 0 ].
  (cvVariables isNil or: [ cvVariables isEmpty ])
    ifFalse: [ names := names + cvVariables ].
  ^names
]

noInstanceVarError: aName [
  ^self error: 'instance variable "' + aName asString + '" not found'.
]

ivarIndex: aName [
  "aName must be a symbol; returns 1-based index or fails"
  | idx cls v |
  cvVariables ifNotNil: [
    idx := cvVariables indexOf: aName ifAbsent: [
      cvParentClass ifNil: [ ^self noInstanceVarError: aName ].
      ^cvParentClass ivarIndex: aName.
    ].
    "now we have to add sizes of all previous ivar arrays"
    cls := cvParentClass.
    [ cls ] whileNotNil: [
      (v := cls variables) ifNotNil: [ idx := idx + v size ].
      cls := cls parent.
    ].
    ^idx
  ].
  cvParentClass ifNil: [ ^self noInstanceVarError: aName ].
  ^cvParentClass ivarIndex: aName.
]

methodText: nm  [
  | meth |
  meth := self findMethodInAll: nm ifAbsent: [ ^self error: 'no such method: ' + nm printString ].
  ^meth text.
]

COMMENT
COMMENT perform: aName withSelf: aSelf withArguments: a
COMMENT is defined in context.st
COMMENT

perform: aName withArguments: a [
  ^self perform aName withSelf: self withArguments: a
]

perform: aName [
  ^self perform: aName withArguments: nil
]


ifNil: aBlock [
  ^self
]

ifNotNil: aBlock [
  ^aBlock value
]

ifNil: aNilBlock ifNotNil: aNotBlock [
  ^aNotBlock value
]

ifNotNil: aNotBlock ifNil: aNilBlock [
  ^aNotBlock value
]
!
