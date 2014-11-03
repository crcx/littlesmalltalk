COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


EXTPRIM SystemGetOSName
EXTPRIM SystemIsATTY
EXTPRIM SystemUnixTime
EXTPRIM SystemLocalTime
EXTPRIM SystemBinPath
EXTPRIM SystemSleep
EXTPRIM SystemTickCountMS


COMMENT Object subclass: System  packages modules
RAWCLASS MetaSystem  Class       MetaObject  packages modules verbose classmap
RAWCLASS System      MetaSystem  Object

METHOD FOR GSTParser
internalloadModule: aName [
  ^System loadModule: aName
]

COMMENT ---------System----------
METHODS FOR System
^initialize [
  packages ifNil: [ packages := Dictionary new ].
  modules ifNil: [ modules := OrderedArray new: 0 ].
  classmap ifNil: [ classmap := Dictionary new ].
  verbose := true.
]

^modules [
  ^modules
]

^packages [
  ^packages
]

^verbose [
  ^verbose
]

^verbose: aFlag [
  verbose := aFlag
]

^reloadModule: aName [
  | ns fname |
  (File exists: (fname := aName + '.st')) ifFalse: [
    (File exists: (fname := System modulesPath + aName + '.st'))
      ifFalse: [ self error: 'module not found: ' + aName ].
  ].
  verbose ifTrue: [ ('loading module: ' + aName) printNl ].
  File fileIn: fname.
  ns := aName asSymbol.
  (modules includes: ns) ifFalse: [ modules := modules add: ns. ].
  ^true
]

^loadModule: aName [
  (modules includes: aName asSymbol) ifFalse: [ self reloadModule: aName. ^true ].
  ^false
]

^classModule: aClassName [
  ^classmap at: aClassName asSymbol ifAbsent: [ ^nil ].
]

^class: aClassName inModule: aModuleName [
  classmap at: aClassName asSymbol put: aModuleName
]

^osName [
  <#SystemGetOSName>
]

^isWindows [
  ^self osName = 'Windows'
]

^isUnix [
  "Simplified: All non Windows systems are Unix systems ;)"
  ^self isWindows not
]

^suffixForDLL [
  "FIXME: For HP/UX we should use the suffix .sl"
  ^self isWindows ifTrue: [ '.dll' ] ifFalse: [ '.so' ]
]

^isStdInTTY [
  <#SystemIsATTY 0>
]

^isStdOutTTY [
  <#SystemIsATTY 1>
]

^isStdErrTTY [
  <#SystemIsATTY 2>
]

^indexOfPrimitive: aName [
  <#PrimIdxName 0 aName>.
  self primitiveFailed
]

^nameOfPrimitive: idx [
  <#PrimIdxName 1 idx>.
  self primitiveFailed
]

^unixTime [
  <#SystemUnixTime>.
  self primitiveFailed
]

^tickCountMS [
  <#SystemTickCountMS>.
  self primitiveFailed
]

^localTime [
  <#SystemLocalTime>.
  self primitiveFailed
]

^localTimeFor: unixTime [
  <#SystemLocalTime unixTime>.
  self primitiveFailed
]

^binPath [
  <#SystemBinPath>.
  self primitiveFailed
]

^modulesPath [
  ^self binPath + 'modules' + self pathSeparatorString
]

^sleep: secs [
  "can accept floats"
  <#SystemSleep secs>.
  self primitiveFailed
]

^pathSeparatorChar [
  ^System isWindows ifTrue: [ $\ ] ifFalse: [ $/ ]
]

^pathSeparatorString [
  ^System isWindows ifTrue: [ '\\' ] ifFalse: [ '/' ]
]

^gc [
  <#RunGC>
]

^error: str [
  str printNl.
  <#ErrorOrYield>
]

^yield: res [
  <#ErrorOrYield res>
]

^currentProcess [
  <#GetCurrentProcess>.
  self primitiveFailed
]

^eventWaitFor: aId [
  <#EventHandlerCtl aId true>.
  self primitiveFailed
]

^newProcessGroupWith: aProcess ticks: aTicks [
  <#ProcessGroupCtl 0 aProcess aTicks>.
  self primitiveFailed
]

^newProcessGroupWith: aProcess [
  <#ProcessGroupCtl 0 aProcess>.
  self primitiveFailed
]

^readStdInCharAsSmallInt [
  <#ReadCharacter>.
  self primitiveFailed
]

^printBaseObject: aObj [
  <#PrintObject aObj>.
  self primitiveFailed
]

^printBaseObjectNl: aObj [
  <#PrintObject aObj true>.
  self primitiveFailed
]

^flushStdOut [
  <#PrintObject>.
  self primitiveFailed
]

^userBreakSignal [
  <#UserBreakSignal>.
  self primitiveFailed
]
!


METHODS FOR SmallInt [
^getInitialSeed [
  ^((System tickCountMS) % 1073741823) asSmallInt
]
!
