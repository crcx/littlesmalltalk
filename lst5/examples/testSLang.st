+FFI subclass: #SLang variables: #(so) classVariables: #(SLangSingleton).
=SLang
singleton
  SLangSingleton isNil ifTrue: [SLangSingleton <- self basicNew open].
  ^SLangSingleton.
!

!SLang
open
  so <- FFI new: 'libslang'.
  #(
    'SLang_getkey_intr_hook'
    'SLang_getkey'
    'SLang_init_tty'
    'SLang_input_pending'
    'SLang_reset_tty'
    'SLang_ungetkey'

    'SLkp_init'
    'SLkp_getkey'

    'SLsignal_intr'

    'SLsmg_cls'
    'SLsmg_draw_box'
    'SLsmg_erase_eos'
    'SLsmg_fill_region'
    'SLsmg_gotorc'
    'SLsmg_init_smg'
    'SLsmg_read_raw'
    'SLsmg_refresh'
    'SLsmg_reinit_smg'
    'SLsmg_reset_smg'
    'SLsmg_resume_smg'
    'SLsmg_set_char_set'
    'SLsmg_set_color'
    'SLsmg_suspend_smg'
    'SLsmg_touch_lines'
    'SLsmg_write_char'
    'SLsmg_write_nstring'
    'SLsmg_write_raw'
    'SLsmg_write_string'

    'SLtt_beep'
    'SLtt_get_screen_size'
    'SLtt_get_terminfo'
    'SLtt_set_color'
    'SLtt_set_cursor_visibility'
    'SLtt_set_mono'
    'SLtt_tgetstr'

    'SLtt_Screen_Cols'
    'SLtt_Screen_Rows'
    'SLtt_Use_Ansi_Colors'
  ) do: [ :elem | so add: elem asSymbol ].
!
!SLang
init
  so call: ('SLang_init_tty' asSymbol) args: #(-1 0 0).
  so call: ('SLtt_get_terminfo' asSymbol).
  so call: ('SLkp_init' asSymbol).
  so call: ('SLsmg_init_smg' asSymbol).
  so call: ('SLtt_set_cursor_visibility' asSymbol) arg: 0.
!
!SLang
reset
  so call: ('SLtt_set_cursor_visibility' asSymbol) arg: 1.
  so call: ('SLsmg_reset_smg' asSymbol).
  so call: ('SLang_reset_tty' asSymbol).
!
!SLang
drawBox: box
  so call: ('SLsmg_draw_box' asSymbol) args: box.
!
!SLang
refresh
  so call: ('SLsmg_refresh' asSymbol) arg: 1.
!
!SLang
screenCols
  ^so getInt: ('SLtt_Screen_Cols' asSymbol).
!
!SLang
screenRows
  ^so getInt: ('SLtt_Screen_Rows' asSymbol).
!
!SLang
pendingInput: t
  ^so callInt: ('SLang_input_pending' asSymbol) arg: t.
!
!SLang
flushInput
  [(self pendingInput: 0) ~= 0] whileTrue: [self getKey].
!
!SLang
getKey
  ^so callInt: ('SLkp_getkey' asSymbol).
!
!SLang
goto: rowCol
  so call: ('SLsmg_gotorc' asSymbol) args: rowCol.
!
!SLang
gotoRow: r col: c 
  |rowCol|
  rowCol <- Array new: 2.
  rowCol at: 1 put: r.
  rowCol at: 2 put: c.
  so call: ('SLsmg_gotorc' asSymbol) args: rowCol.
!
!SLang
writeString: s
  so call: ('SLsmg_write_string' asSymbol) arg: s.
!
!SLang
writeString: s at: rowCol
  self goto: rowCol. self writeString: s.
!
!SLang
normalCharSet
  so call: 'SLsmg_set_char_set' asSymbol arg: 0.
!
!SLang
lineCharSet
  so call: 'SLsmg_set_char_set' asSymbol arg: 1.
!
!SLang
writeChar: c
  so call: ('SLsmg_write_char' asSymbol) arg: c.
!
!SLang
writeChar: c at: rowCol
  self goto: rowCol.  self writeChar: c.
!
!SLang
clearScreen
  so call: ('SLsmg_cls' asSymbol).
!
!SLang
close
  so close.
!

+Object subclass: #SLComponent variables: #(sl top left height width)
=SLComponent
new
  ^self basicNew initialize.
!
  
!SLComponent
initialize
  sl <- SLang singleton.
!

=SLang
test
  | s k |
  s <- SLang singleton.
  s init.
  s writeString: 'Enter ctrl-g to quit' at: #(1 0).
  s writeString: (s screenRows asString + ',' + s screenCols asString)
    at: #(3 0).

  s drawBox: #(7 5 10 15).

  s gotoRow: 7 col: 20. s writeChar: $^ value.
  s lineCharSet.
  8 to: 15 do: [ :r |
      s gotoRow: r col: 20.
      s writeChar: $a value].
  s normalCharSet.
  s gotoRow: 16 col: 20. s writeChar: $v value.

  s refresh.
  [ s flushInput. k <- s getKey. 7 = k ] whileFalse:
    [s writeString: ('you pressed ' + k asString + '    ') at: #(30 0).
     s refresh ].
  s clearScreen.
  s refresh.
  s reset.
  s close.
!
