{
  | d i l s rand |
  i := 20.
  d := '0123456789abcdefghijklmnopqrstuvwxyz'.
  l := d size.
  s := String new: i.
  rand := Random new.
  1 to: i do: [ :x |
    s at: x put: (d at: (rand between: 1 and: l))
  ].
  s printNl.
}
