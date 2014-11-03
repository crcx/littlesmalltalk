"run this with -Z to write clean image"
"Requires [ disasm ]"



{| doclass totalwin file srcsize |
  srcsize := 0.
  doclass := [:cls |
    "cls printNl."
    cls methods do: [:m :mn :msgs :p |
      "cls print. '>>' print. m name printNl."
      srcsize := srcsize + m text size.
      msgs := List new.
      p := LstCompiler new.
      p warningBlock: [:msg :line |
        msgs addLast: 'WARNING near line ' + line asString + ': ' + msg.
      ].
      p errorBlock: [:msg :line |
        msgs addLast: 'ERROR near line ' + line asString + ': ' + msg.
      ].
      mn := cls compileMethod: m text withCompiler: p.
      msgs isEmpty ifTrue: [
        p := (m byteCodes size) - (mn byteCodes size).
        totalwin := totalwin + p.
        p > 0 ifTrue: [
          cls print. '>>' print. m name print. '  win: ' print. p printNl.
        ].
        cls replaceMethod: m name with: mn.
      ] ifFalse: [
        cls print. '>>' print. m name print. '  win: ' print.
        mn ifNotNil: [
            p := (m byteCodes size) - (mn byteCodes size).
            totalwin := totalwin + p.
            p printNl.
            cls replaceMethod: m name with: mn.
           ] ifNil: [ 'unknown' printNl. ].
        msgs reverseDo: [:i |
          System isStdOutTTY ifTrue: [
            (i firstChar = $W ifTrue: [ '\e[31m' ] ifFalse: [ '\e[41m' ]) print.
          ].
          '***' print. i print.
          System isStdOutTTY ifTrue: [ '\e[0m' printNl ] ifFalse: [ '\n' print ].
        ].
      ].
    ].
  ].

  totalwin := 0.

  globals keysAndValuesDo: [:name :value |
    ((value isKindOf: Class) and: [ (name asString from: 1 to: 4) ~= 'Meta' ]) ifTrue: [
      doclass value: (value class).
      doclass value: value.
    ].
  ].

  'total source code size: ' print. srcsize print. ' (' print. (srcsize / 1024) print. 'kb)' printNl.
  'total bytecode win: ' print. totalwin printNl.

  cliArgs size > 1 ifTrue: [
    doclass := cliArgs at: 2.
    'MESSAGE: writing image to ' print. doclass printNl.
    file := File openWrite: doclass.
    file opened ifFalse: [ ^self error: 'cannot open image file' ].
    file writeImage.
    file close.
  ].
}
