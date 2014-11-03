"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Requires [ httpsvx ahah ]

Package [
  HttpClassBrowser
]


HttpHandler subclass: HttpHandlerFiles
| d2t |
[
  ^initialize [
    (d2t := Dictionary new);
      at: '/js/' put: 'text/javascript';
      at: '/css/' put: 'text/css';
      .
  ]

  ^d2t: aReq [
    ^d2t at: aReq path ifAbsent: [ nil ]
  ]

  openFile [
    | fl pt |
    (fl := req file) isEmpty ifTrue: [ ^nil ].
    (pt := self class d2t: req) ifNil: [ ^nil ].
    [ 'WANT FILE: ' print. req path print. fl printNl. ' type: ' print. pt printNl. ] runLocked.
    (fl := File openRead: (req path from: 2) + fl) opened ifFalse: [ ^nil ].
    self set2xx.
    self fileType: pt.
    ^fl
  ]

  emitBody [
    self set4xx.
    self
      emit: '<html><body>file not found: <i>';
      emit: (req path htmlEscape);
      emit: (req file htmlEscape);
      emit: '</i></body></html>'.
  ]
]

HttpHandler subclass: HttpHandlerMain [
  openFile [
    | fl |
    (fl := req file) isEmpty ifTrue: [ fl := 'index.html' ].
    fl = 'cbr.html' ifTrue: [ ^nil ].
    [ 'WANT FILE: ' print. fl printNl ] runLocked.
    (fl := File openRead: 'html/' + fl) opened ifFalse: [ ^nil ].
    self set2xx.
    self fileType: 'text/html'.
    ^fl
  ]

  emitVar: aName skip: aSkipName [
    | v |
    (aSkipName = 'method' and: [ aName = 'srctext' ]) ifTrue: [ ^self ].
    aName = aSkipName ifFalse: [
      (v := req var: aName) ifNotNil: [
        (aName = 'srctext' or: [ aName = 'sterror' ]) ifTrue: [ v := v toUrl ].
        self
          emit: '<input type="hidden" name="';
          emit: aName htmlEscape;
          emit: '" value="';
          emit: v htmlEscape;
          emit: '" />'.
      ]
    ]
  ]

  emitVars: aSkipName [
    self
      emitVar: 'package' skip: aSkipName;
      emitVar: 'class' skip: aSkipName;
      emitVar: 'method' skip: aSkipName;
      emitVar: 'srctext' skip: aSkipName;
      emitVar: 'sterror' skip: aSkipName.
  ]

  selection: aName collection: aCollection active: aItem [
    | is |
    self emit: '<form accept-charset="utf-8" method="post" action="" class="column"><div>'.
    self emitVars: aName.
    self emit: '<select size="10" onchange="submit()" name="'; emit: aName htmlEscape; emit: '">'.
    aCollection ifNotNil: [
      aCollection do: [:item |
        is := item asString htmlEscape.
        self emit: '<option value="'; emit: is.
        aItem ifNotNil: [ aItem = item ifTrue: [ self emit: '" selected="selected' ]].
        self emit: '">'; emit: is; emit: '</option>'.
      ].
    ].
    self emit: '</select></div></form>'.
  ]

  emitPackages [
    | pkgs |
    pkgs := List new.
    Package packages keysDo: [:obj | pkgs add: obj asString ].
    self selection: 'package' collection: (pkgs reverse) active: (req var: #package).
  ]

  emitClasses [
    | pkg clist |
    (pkg := req var: #package) ifNotNil: [ pkg := Package find: pkg asSymbol ].
    clist := List new.
    pkg
      ifNil: [
        globals do: [:obj | (obj isKindOf: Class) ifTrue: [ obj isMeta ifFalse: [ clist add: obj asString ]]].
      ] ifNotNil: [
        pkg classes do: [:obj | (obj isKindOf: Class) ifTrue: [ obj isMeta ifFalse: [ clist add: obj asString ]]].
      ].
    self selection: 'class' collection: (clist reverse) active: (req var: #class).
  ]

  emitMethods [
    | cls mlist |
    (cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]].
    mlist := List new.
    cls ifNotNil: [
      cls class methods do: [:mth | mlist add: '^' + (mth name asString) ].
      cls methods do: [:mth | mlist add: mth name asString ].
    ].
    self selection: 'method' collection: (mlist reverse) active: (req var: #method).
  ]

  emitSource [
    | cls mth isMeta |
    self
      emit: '<form accept-charset="utf-8" method="post" action="" class="definition"><div>';
      emitVars: 'srctext';
      emit: '<input type="hidden" name="fromtext" value="tan" />';
      emit: '<textarea rows="auto" cols="auto" name="srctext" wrap="soft">'.
    (mth := req var: #srctext ifAbsent: ['']) = '' ifTrue: [
      ((cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]]) ifNotNil: [
        (mth := req var: #method) ifNotNil: [
          mth firstChar == $^ ifTrue: [ mth := mth from: 2. cls := cls class. isMeta := '^' ] ifFalse: [ isMeta := '' ].
          (mth := cls findMethodInAll: mth asSymbol ifAbsent: [ nil ]) ifNotNil: [
            self
              emit: isMeta;
              emit: mth text htmlEscape.
          ]
        ]
      ]
    ] ifFalse: [
      (req var: #fromtext) ifNil: [ mth := mth fromUrl ].
      self emit: mth htmlEscape
    ].
    self emit: '</textarea><br /><input value="Accept" name="mthaccept" type="submit" class="submit"></div></form>'.
  ]

  emitError [
    | et |
    self
      emit: '<form accept-charset="utf-8" method="post" action="" class="definition"><div>';
      emitVars: 'sterror';
      emit: '<input type="hidden" name="fromerror" value="tan" />';
      emit: '<textarea rows="auto" cols="auto" name="sterror" wrap="soft" readonly="yes">'.
    (et := req var: #sterror) ifNotNil: [
      (req var: #fromerror) ifNil: [ et := et fromUrl ].
      self emit: et htmlEscape.
    ].
    self emit: '</textarea></div></form>'.
  ]

  compileMethod [
    | err mth cls p |
    req var: #sterror put: ''; var: #fromerror put: true.
    (mth := (req var: #srctext ifAbsent: ['']) removeTrailingBlanks) = '' ifTrue: [
      req var: #sterror put: 'nothing to accept!'.
      ^self
    ].
    ((cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]]) ifNil: [
      req var: #sterror put: 'no class selected!'.
      ^self
    ].
    mth := mth reject: [ :c | c isCR ].
    "compile and add method"
    err := ''.
    p := LstCompiler new.
    p errorBlock: [ :msg :lineNum |
      err := err + 'ERROR near line ' + lineNum asString + ': ' + msg htmlEscape + '\n'.
      req var: #sterror put: err.
      ^self
    ].
    p warningBlock: [ :msg :lineNum |
      err := err + 'WARNING near line ' + lineNum asString + ': ' + msg htmlEscape + '\n'.
    ].
    (mth := (cls addMethod: mth withCompiler: p)) ifNotNil: [
      err := err + 'method succcesfully compiled.\n'
    ].
    req var: #sterror put: err.
  ]

  emitCBR [
    self
      set2xx;
      emit: '<html><head><link rel="stylesheet" type="text/css" href="css/classbrowser.css" /></head><body>';
      emitPackages;
      emitClasses;
      emitMethods;
      emitSource.
    (req var: #mthaccept) ifNotNil: [ self compileMethod ].
    self
      emitError;
      emit: '</body></html>'.
  ]

  emitBody [
    req file = 'cbr.html' ifTrue: [ ^self emitCBR ].
    ^super emitBody
  ]
]


{
  HttpHandlerFiles initialize.
  HttpDispatcher addHandler: '/' handler: HttpHandlerMain.
  HttpDispatcher addHandler: '/js/' handler: HttpHandlerFiles.
  HttpDispatcher addHandler: '/css/' handler: HttpHandlerFiles.
  HttpDispatcher addHandler: '/q/' handler: HttpHandlerAhah.
  (HttpSheduler new) startOn: 6789.
}
