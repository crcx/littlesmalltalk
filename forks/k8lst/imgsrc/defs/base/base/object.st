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


COMMENT -----------Object-------------------
METHODS FOR Object
^in: object at: index put: value [
  "change data field in object, used during initialization; returns the intialized object"
  <#Array#at:put: value object index>.
  self primitiveFailed
]

in: object at: index [
  "browse instance variable via debugger"
  <#Array#at: object index>.
  self primitiveFailed
]

^in: object var: aName put: value [
  "change data field in object, used during initialization; returns the intialized object"
  | idx |
  (idx := self varIndexIn: object name: aName) ifNil: [ self error: 'unknown instance variable: ' + aName asString ].
  <#Array#at:put: value object idx>.
  self primitiveFailed
]

in: object var: aName [
  "browse instance variable via debugger"
  | idx |
  (idx := self class varIndexIn: object name: aName) ifNil: [ self error: 'unknown instance variable: ' + aName asString ].
  <#Array#at: object idx>.
  self primitiveFailed
]

COMMENT ^varIndexIn: obj name: aName [
COMMENT   ^(obj class instanceVariables) indexOf: aName
COMMENT ]

^varIndexIn: obj name: aName [
  ^obj class ivarIndex: aName.
]

isNil [
  ^false
]

notNil [
  ^true
]

ifNil: aBlock [
  ^nil
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

== arg [
  <#ObjectIdentity self arg>
]

= arg [
  ^self == arg
]

~= arg [
  ^(self = arg) not
]

~~ arg [
  ^(self == arg) not
]

| aVal [
  ^self
]

& aVal [
  ^aVal
]

class [
  <#ObjectClass self>
]

printString [
  ^self class printString
]

asString [
  ^self printString
]

isMemberOf: aClass [
  ^self class == aClass
]

isKindOf: aClass [
  | clas |
  clas := self class.
  [ clas notNil ] whileTrue:
    [ clas == aClass ifTrue: [ ^true ].
      clas := clas superclass ].
  ^false
]

respondsTo: aMessage [
  ^self class allMethods includes: aMessage
]

addToBeFinalized [
  <#StFinalizeCtl self true>.
  self primitiveFailed
]

removeToBeFinalized [
  <#StFinalizeCtl self false>.
  self primitiveFailed
]

makeWeak [
  <#StWeakCtl self>.
  self primitiveFailed
]

finalize [
  "so each object will have this method"
]

mourn [
  "so each object will have this method"
]

print [
  System printBaseObject: self printString.
]

printNl [
  System printBaseObjectNl: self printString.
]

error: str [
  System error: str.
]

lastFailedPrim [
  <#LastFailedPrim>.
]

primitiveFailed [
  self error: 'Primitive failed (' + self lastFailedPrim + '): ' + (System nameOfPrimitive: self lastFailedPrim)
]

species [
  "By default, we re-instantiate ourselves as our own Class"
  ^self class
]

hash [
  "Most objects should generate something based on their value"
  ^self class printString hash
]

become: other [
  "Exchange identity with another object"
  (Array with: self) elementsExchangeIdentityWith: (Array with: other)
]

doesNotUnderstand: aSel args: aArgs [
  thisContext backtrace.
  self error: (self printString + ' (class '+ (self class printString) + '): does not understand ' + aSel printString).
]

basicSize [
  <#ObjectSize self>
]

subclassResponsibility [
  self error: 'Subclass responsibility'
]

basicDo: aBlock [
  ^self do: aBlock
]

findMethod: aName [
  ^(self class findMethod: aName)
]

perform: aName withArguments: a [
  ^self class perform: aName withSelf: self withArguments: a
]

perform: aName withArgument: a [
  ^self perform: aName withArguments: (Array with: a).
]

perform: aName with: a [
  ^self perform: aName withArguments: (Array with: a).
]

perform: aName with: a with: b [
  ^self perform: aName withArguments: (Array with: a with: b).
]

perform: aName with: a with: b with: c [
  ^self perform: aName withArguments: (Array with: a with: b with: c).
]

perform: aName [
  ^self perform: aName withArguments: nil
]
!
