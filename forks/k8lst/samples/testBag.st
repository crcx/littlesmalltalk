Requires [ collections ]


{
  | bag dumpBag dumpBagX |

  dumpBag := [:s |
    'bag:(' print. s size print. ')' print.
    s do: [:e | ' ' print. e print. ].
    ';' printNl.
  ].

  dumpBagX := [:s |
    'bag:(' print. s size print. ')' print.
    s itemsDo: [:e | ' ' print. e print. ].
    ';' printNl.
  ].

  bag := Bag new.
  dumpBag value: bag.

  bag << 10.
  bag << 13.
  bag << 42.
  bag << 56.
  dumpBag value: bag.

  bag << 42.
  bag << 56.
  dumpBag value: bag.

  bag remove: 13.
  dumpBag value: bag.

  bag remove: 128 ifAbsent: [ 'no element!' printNl ].
  dumpBag value: bag.

  "and now for something big"
  1 to: 32 do: [:i | bag << i ].
  dumpBag value: bag.

  2 to: 32 by: 2 do: [:i | bag remove: i ].
  dumpBag value: bag.

  (bag includes: 5) ifFalse: [ 'FAIL!' printNl ].
  (bag includes: 6) ifTrue: [ 'FAIL!' printNl ].

  dumpBagX value: bag.

  bag removeAll.
  dumpBag value: bag.
}
