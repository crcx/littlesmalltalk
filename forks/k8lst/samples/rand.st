{ | r |
  r := Random new.
  1 to: 10 do: [ (r between: 1 and: 42) printNl ].
}
