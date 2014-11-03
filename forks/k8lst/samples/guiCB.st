Requires [ gui ]

Package [
  Browser
]


class: Browser [
  | dialog
    lbPackages
    lbClasses
    lbMethods
    lbVars
    edText
  |

  ^new [
    | obj |
    (obj := self basicNew) createDialog.
    ^obj
  ]

  createLBArea: aText lb: aLb [
    ^(GuiVBox new); expandable; attribute: 'SIZE' value: '0x0';
      add: (GuiLabel new: aText; expandableHorizontal; attribute: 'SIZE' value: '0x0');
      "add: (aLb expandable; attribute: 'SIZE' value: '100x90'; attribute: 'VISIBLECOLUMNS' value: '1');"
      add: (aLb expandable; attribute: 'SIZE' value: '0x90'; attribute: 'VISIBLECOLUMNS' value: '1');
      add: ((GuiHBox new; expandableHorizontal; attribute: 'SIZE' value: '0x0');
        add: (GuiButton new: 'Create'; expandableHorizontal; attribute: 'SIZE' value: '0x0');
        add: (GuiButton new: 'Remove'; expandableHorizontal; attribute: 'SIZE' value: '0x0'));
    .
  ]

  createDialog [
    | vb  hb  pa ca ma va split |

    lbPackages := GuiList new; action: [ self fillClasses ].
    lbClasses := GuiList new; action: [ self fillMethods. self fillVars ].
    lbMethods := GuiList new; action: [ self showMethod ].
    lbVars := GuiList new.

    edText := GuiMultiEdit new; expandable; attribute: 'SIZE' value: '0x120'.

    pa := self createLBArea: 'Packages' lb: lbPackages.
    ca := self createLBArea: 'Classes' lb: lbClasses.
    ma := self createLBArea: 'Methods' lb: lbMethods.
    va := self createLBArea: 'Variables' lb: lbVars.

    split := GuiVSplit new: ma and: va.
    split := GuiVSplit new: ca and: split.
    split := GuiVSplit new: pa and: split.

    (hb := GuiHBox new);
      add: (GuiButton new: 'Apply' action: [ self applyMethod ]; expandableHorizontal);
      add: (GuiButton new: 'GST-compile' action: [ self gstCompile ]; expandableHorizontal);
    .

    (vb := GuiVBox new);
      add: split;
      add: edText;
      add: hb;
    .

    dialog := GuiDialog new: 'LST Class Browser' widget: vb onClose: [ GuiSingleton setQuitFlag ].
    self show.

    self fillPackages.
  ]

  fillPackages [
    lbPackages empty.
    Package packages keysDo: [:p | lbPackages << p asString ].
  ]

  fillClasses [
    | si pkg |
    lbClasses empty.
    lbMethods empty.
    lbVars empty.
    (si := lbPackages selected) ifNil: [ ^self ].
    si := lbPackages at: si.
    pkg := Package find: si asSymbol.
    pkg ifNil: [ ^self ].
    pkg := pkg classes.
    pkg do: [:c | (c isKindOf: Class) ifTrue: [ c isMeta ifFalse: [ lbClasses << c asString ]]].
  ]

  fillMethods [
    | si cls |
    lbMethods empty.
    lbVars empty.
    (si := lbClasses selected) ifNil: [ ^self ].
    si := lbClasses at: si.
    cls := globals at: (si asSymbol) ifAbsent: [ ^self ].
    cls class methods do: [:mth | lbMethods << ('^' + mth name asString) ].
    cls methods do: [:mth | lbMethods << mth name asString ].
  ]

  fillVars [
    | si cls |
    lbVars empty.
    (si := lbClasses selected) ifNil: [ ^self ].
    si := lbClasses at: si.
    cls := globals at: (si asSymbol) ifAbsent: [ ^self ].
    cls class instanceVariables do: [:vn | lbVars << ('^' + vn asString) ].
    cls instanceVariables do: [:vn | lbVars << vn asString ].
  ]

  showMethod [
    | si cls mth isMeta m |
    edText empty.
    (si := lbClasses selected) ifNil: [ ^self ].
    si := lbClasses at: si.
    cls := globals at: (si asSymbol) ifAbsent: [ ^self ].
    (si := lbMethods selected) ifNil: [ ^self ].
    si := lbMethods at: si.
    si firstChar == $^ ifTrue: [ cls := cls class. mth := si from: 2. isMeta := '^' ] ifFalse: [ mth := si. isMeta := '' ].
    m := cls findMethodInAll: mth asSymbol ifAbsent: [ ^self ].
    edText value: isMeta + m text.
  ]

  dialog [
    ^dialog
  ]

  show [
    dialog show
  ]

  hide [
    dialog hide
  ]
]



{
  | browser |
  System newProcessGroupWith: (Process newWithMethod: #REPL class: REPL new).

  browser := Browser new.
  GuiSingleton add: browser dialog.
  browser show.
  GuiSingleton mainLoop.
}
