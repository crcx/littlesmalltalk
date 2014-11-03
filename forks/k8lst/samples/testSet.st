Requires [ collections ]


{
  | set dumpSet |

  dumpSet := [:s |
    'set:' print.
    s do: [:e | ' <' print. e print. '>' print. ].
    ';' printNl.
  ].

  set := Set new.
  dumpSet value: set.

  set << 10.
  set << 13.
  set << 42.
  set << 56.
  dumpSet value: set.

  (set at: 42) printNl.

  set << 42.
  set << 56.
  dumpSet value: set.

  set remove: 13.
  dumpSet value: set.

  set remove: 128 ifAbsent: [ 'no element!' printNl ].
  dumpSet value: set.

  "and now for something big"
  1 to: 32 do: [:i | set << i ].
  dumpSet value: set.

  1 to: 32 by: 2 do: [:i | set remove: i ].
  dumpSet value: set.

  (set includes: 5) ifTrue: [ 'FAIL!' printNl ].
  (set includes: 6) ifFalse: [ 'FAIL!' printNl ].

  set removeAll.
  dumpSet value: set.
}
