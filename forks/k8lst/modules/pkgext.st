"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package extend [
^showAll [
  packages keysDo: [:k | k asString printNl ]
]

showClasses [
  'Package: ' print. self name printNl.
  classes keysDo: [:k | ' ' print. k asString printNl ]
]
]
