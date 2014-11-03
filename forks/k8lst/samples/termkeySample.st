{ System isWindows ifTrue: [ self error: 'windoze eats shit!' ]. }

Requires [ termkey ]


{[:c |
   'raw: ' print. System termRaw printNl.
   System termRaw: true.
   [ c == $q ] whileFalse: [
     System isKeyHit
       ifTrue: [
         (c := Char input)
           ifNil: [ c := $q ]
           ifNotNil: [
             (c value < 33 or: [ c value = 127]) ifTrue: [ '#' print. c value printNl ] ifFalse: [ '$' print. c printNl ].
           ].
       ] ifFalse: [
         System sleep: 0.1.
       ].
   ].
   System termRaw: false.
 ] value
}
