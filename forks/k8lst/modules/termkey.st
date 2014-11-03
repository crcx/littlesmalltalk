"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
"
Package [
  TermIO
]
"


System extend [
  ^isKeyHit [
    <#SystemIsKeyHit>.
    self primitiveFailed
  ]

  ^termCanonical [
    <#SystemTermMode 0>.
    self primitiveFailed
  ]

  ^termCanonical: aFlag [
    <#SystemTermMode 0 aFlag>.
    self primitiveFailed
  ]

  ^termEcho [
    <#SystemTermMode 1>.
    self primitiveFailed
  ]

  ^termEcho: aFlag [
    <#SystemTermMode 1 aFlag>.
    self primitiveFailed
  ]

  ^termRaw [
    <#SystemTermMode 2>.
    self primitiveFailed
  ]

  ^termRaw: aFlag [
    <#SystemTermMode 2 aFlag>.
    self primitiveFailed
  ]
]
