{
  "simple inject:into: to sum elements in an array"
  | array sum |
  array := #( 1 2 3 4 5 ).
  sum := array
    inject: 0
    into: [ :inj :ele | inj + ele ].
  "answer: 15"
  sum printNl.
}
