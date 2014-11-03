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

  set << (Association key: 'str0' value: 'value0').
  set << (Association key: 'str1' value: 'value1').
  set << (Association key: 'str2' value: 'value2').
  dumpSet value: set.

  (set at: 'str0') value printNl.

  set removeAll.
  dumpSet value: set.
}
