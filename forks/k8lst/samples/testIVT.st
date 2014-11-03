class: TestBase [
  | iv0 iv1 |

  ^new [
    | obj |
    obj := self basicNew.
    self;
      in: obj var: #iv0 put: 0;
      in: obj var: #iv1 put: 1.
    ^obj
  ]

  dump [
    'iv0=' print. iv0 printNl.
    'iv1=' print. iv1 printNl.
  ]
]


TestBase subclass: Test [
  | iv2 iv3 |

  ^new [
    | obj |
    obj := super new.
    obj dump.
    self;
      in: obj var: #iv0 put: 10;
      in: obj var: #iv1 put: 11;
      in: obj var: #iv2 put: 12;
      in: obj var: #iv3 put: 13.
    ^obj
  ]

  dump [
    super dump.
    'iv2=' print. iv2 printNl.
    'iv3=' print. iv3 printNl.
  ]
]


{
  | tb t1 |
  tb := TestBase new. tb dump.
  t1 := Test new. t1 dump.
}
