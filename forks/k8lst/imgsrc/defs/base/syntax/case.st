COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


COMMENT Case class; modelled after GNU Smalltalk one
COMMENT
COMMENT A nice class for switch-like tests. Slower than cascaded ifs but clearer.
COMMENT #case:do: uses identity for testing (useful since you'll likely use Case
COMMENT with symbols, integers, characters, and the like), #ifEqualTo:do: uses
COMMENT equality.
COMMENT
COMMENT e.g.
COMMENT
COMMENT Case new test: myObject;
COMMENT   case: 5                                do: [ 'got five'       printNl ];
COMMENT   same: 42                               do: [ 'got The Answer' printNl ];
COMMENT   when: [ :testVal | testVal isInteger ] do: [ 'an integer'
COMMENT   else:                                      [ :testVal | testVal printNl ].
COMMENT
COMMENT You can use (Case test: myObject) instead of the first line above. Which of
COMMENT the two possibilities is clearer, it is a matter of taste.
COMMENT
COMMENT Note, that LST compiler will optimize this to simple branches, whereas
COMMENT Image Builder will not.

CLASS Case  Object  condObj found res

METHODS FOR Case
^test: anObject [
  ^(super new) test: anObject
]

test: anObject [
  condObj := anObject.
  found := false.
]

reset [
  found := false
]

result [
  ^res
]

else: bAction [
  found ifTrue: [ ^res ].
  ^self internalDoBlock: bAction
]

case: anObject do: bAction [
  (found not and: [condObj = anObject]) ifTrue: [ ^self internalDoBlock: bAction ].
  ^res
]

same: anObject do: bAction [
  (found not and: [condObj == anObject]) ifTrue: [ ^self internalDoBlock: bAction ].
  ^res
]

when: bTest do: bAction [
  (found not and: [bTest value: condObj]) ifTrue: [ ^self internalDoBlock: bAction ].
  ^res
]

internalDoBlock: aBlock [
  found := true.
  ^res := aBlock value: condObj
]
!
