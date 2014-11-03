"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Requires [ rex ]


SimpleRegExp extend [
showCaptures [
  (rexStr, ' | ', string, ' |') printNl.
  ('start=', matchStart, '; end=', matchEnd) printNl.
  1 to: captures size do: [:m |
    ' cpt #' print. m print. '=' print. (self captureAt: m) printNl.
  ].
  '----' printNl.
]
]


{[:rm |
   rm := SimpleRegExp new: '([a-b])+(c).*'.
   (rm matchFor: 'abcd') printNl. rm showCaptures.
   (rm matchFor: 'dabcd') printNl. rm showCaptures.
 ] value.
 [:rm |
   rm := SimpleRegExp new: '^([a-b])+(c).*$'.
   (rm matchFor: 'abcd') printNl. rm showCaptures.
   (rm matchFor: 'dabcd') printNl. rm showCaptures.
 ] value.
 [:rm |
   rm := SimpleRegExp new: '^([a-b])+(c)$'.
   (rm matchFor: 'abc') printNl. rm showCaptures.
   (rm matchFor: 'abcd') printNl. rm showCaptures.
   (rm matchFor: 'dabcd') printNl. rm showCaptures.
 ] value.
 [:rm |
   rm := SimpleRegExp new: '([a-b])+?.'.
   (rm matchFor: 'abcd') printNl. rm showCaptures.
   (rm matchFor: 'dabcd') printNl. rm showCaptures.
 ] value.
 [:rm |
   rm := SimpleRegExp new: '([a-b])+.'.
   (rm matchFor: 'abcd') printNl. rm showCaptures.
   (rm matchFor: 'dabcd') printNl. rm showCaptures.
 ] value.
}
