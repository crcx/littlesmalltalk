Requires [ gui ]















{
  | box dlg l0 l1 spl rb |
  System newProcessGroupWith: (Process newWithMethod: #REPL class: REPL new).

  (box := GuiVBox new);
    add: (GuiToggle new: 'radio 0' action: [ 'radio 0 toggled' printNl ]; checked: true);
    add: (GuiToggle new: 'radio 1' action: [ 'radio 1 toggled' printNl ]; checked: true);
    add: (GuiToggle new: 'radio 2' action: [ 'radio 2 toggled' printNl ]; checked: true);
  .
  rb := GuiRadioBox new: box.

  l0 := GuiList new; << 'list 0 item 0'; << 'list 0 item 1'.
  l1 := GuiList new; << 'list 1 item 0'; << 'list 1 item 1'.
  l1 enabled: false.
  (spl := GuiSplit new); add: l0; add: l1.
  (box := GuiVBox new);
    add: (GuiLabel new: 'label');
    add: (GuiButton new: 'test &button' action: [ 'test button pressed' printNl. GuiSingleton setQuitFlag ]);
    add: (GuiToggle new: 'tog&gle 0' action: [ 'toggle 0 toggled' printNl ]; checked: true);
    add: (GuiProgressBar new);
    add: (GuiHTrackBar new: [:v | 'trackbar value changed: ' print. v printNl ]; max: 666; value: 42);
    add: (GuiEdit new: [:t | 'text: ' print. t printNl ]);
    add: (GuiList new: [:i :sel | 'item #' print. i print. ' is ' print. sel ifFalse: ['un' print]. 'selected' printNl];
      << 'item 0';
      << 'item 1';
      );
    add: rb;
    add: spl;
    .
  dlg := GuiDialog new: 'First Dialog Window' widget: box onClose: [ GuiSingleton setQuitFlag ].
  dlg attribute: 'RESIZE' value: 'NO'.
  "dlg attribute: 'TRAY' value: 'YES'."
  dlg show.
  "GuiSingleton message: 'starting GUI loop...'."
  GuiSingleton add: dlg.
  GuiSingleton mainLoop.
}
