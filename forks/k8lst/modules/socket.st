"
 Little Smalltalk, Version 5

 Copyright (C) 1987-2005 by Timothy A. Budd
 Copyright (C) 2007 by Charles R. Childers
 Copyright (C) 2005-2007 by Danny Reinhold
 Copyright (C) 2010 by Ketmar // Vampire Avalon

 ============================================================================
 This license applies to the virtual machine and to the initial image of
 the Little Smalltalk system and to all files in the Little Smalltalk
 packages except the files explicitly licensed with another license(s).
 ============================================================================
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the 'Software'), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
"
Package [
  Socket
]

Object subclass: Inet [
^firstIpStringFromHostname: nm [
  <#SocketInet 0 nm>
]

^firstIpAddressFromHostname: nm [
  <#SocketInet 1 nm>
]

^newIpAddressFromIpString: s [
  <#SocketInet 2 s>
]

^newSocketAddressFromIpAddress: addr port: p [
  <#SocketInet 3 addr p>
]

^newSocketAddressFromIpString: s port: p [
  <#SocketInet 4 s p>
]

^newSocketAddressFromHostname: nm port: p [
  <#SocketInet 5 nm p>
]
]

Object subclass: Socket [
| sockfd |

^createTCP [
  <#SocketSocket 11>
]

^createUDP [
  <#SocketSocket 12>
]

^newWithFD: fd [
  | obj |
  obj := self new.
  obj socket: (self createTCP).
  obj fd: fd.
  ^obj
]

"
  /* args: rArray wArray eArray [timeoutMs] */
  /* returns: patched arrays; for each item: boolean */
  /* return nil on error */
  /* return false if there is timeout */
  /* return true if anything happens */
"

^selectRead: rArray write: wArray exception: eArray timeout: ms [
  <#SocketSocket 95 rArray wArray eArray ms>.
  self primitiveFailed
]

^selectRead: rArray write: wArray timeout: ms [
  ^self selectRead: rArray write: wArray exception: nil timeout: ms
]

^selectRead: rArray timeout: ms [
  ^self selectRead: rArray write: nil exception: nil timeout: ms
]

^selectWrite: wArray timeout: ms [
  ^self selectRead: nil write: wArray exception: nil timeout: ms
]

socket [
  ^sockfd.
]

socket: fd [
  sockfd := fd.
]

fd [
  "return old fd"
  <#SocketSocket 28 sockfd>.
  self primitiveFailed
]

fd: fd [
  self fd < 0 ifFalse: [ self basicClose ].
  <#SocketSocket 29 sockfd fd>.
  self primitiveFailed
]

shutdown [
  <#SocketSocket 21 sockfd 2>.
  self primitiveFailed
]

basicClose [
  self isClosed ifFalse: [
    self shutdown.
    <#SocketSocket 20 sockfd>.
    self primitiveFailed
  ].
]

close [
  self basicClose.
  sockfd := nil.
]

isClosed [
  sockfd ifNil: [ ^true ].
  self fd < 0 ifTrue: [ ^true ].
  ^false
]

connect: socketAddr [
  <#SocketSocket 60 sockfd socketAddr>.
  self primitiveFailed
]

connect: iporaddr port: aPort [
  | addr |
  addr := Inet newSocketAddressFromHostname: iporaddr port: aPort.
  ^self connect: addr
]

internalSelectFor: cond timeout: ms [
  "return number (0: timeout; 1: can read; 2: can write; 3: both; <0: error)"
  <#SocketSocket 92 sockfd cond ms>.
  self primitiveFailed
]

canRead [
  ^(self internalSelectFor: 1 timeout: -1) = 1
]

canReadWithTimeout: milliseconds [
  ^(self internalSelectFor: 1 timeout: milliseconds) = 1
]

canWrite [
  ^(self internalSelectFor: 2 timeout: -1) = 2
]

canWriteWithTimeout: milliseconds [
  ^(self internalSelectFor: 2 timeout: milliseconds) = 1
]

selectFor: cond timeout: msec [
  ^self internalSelectFor: cond timeout: msec
]

selectFor: cond [
  ^self selectFor: cond timeout: -1
]

basicRecv: aBuffer size: n [
  <#SocketSocket 80 sockfd aBuffer n>.
  self primitiveFailed
]

basicRecv: aBuffer [
  ^self basicRecv: aBuffer size: (aBuffer size).
]

recv: n [
  | aBuffer got |
  aBuffer := String new: n.
  got := self basicRecv: aBuffer size: n.
  got > 0
    ifTrue: [ ^aBuffer from: 1 to: got ]
    ifFalse: [ ^nil ].
]

recvAll [
  | buf total rc |
  total := ''.
  buf := String new: 2048.
  rc := self basicRecv: buf.
  [ rc > 0 ] whileTrue: [
    rc printNl.
    total := total + (buf from: 1 to: rc).
    (self canReadWithTimeout: 2500) ifFalse: [ ^total ].
    "'next...' printNl."
    rc := self basicRecv: buf.
    "'got: ' print. rc printNl."
    rc ifNil: [ ^total ].
  ].
  ^total
]

basicSend: bytes size: n [
  <#SocketSocket 70 sockfd bytes n>.
  self primitiveFailed
]

send: aString [
  ^self basicSend: aString size: (aString size).
]

bind: socketAddr [
  <#SocketSocket 30 sockfd socketAddr>.
  self primitiveFailed
]

bind: iporaddr port: aPort [
  | addr |
  addr := Inet newSocketAddressFromHostname: iporaddr port: aPort.
  ^self bind: addr
]

listen [
  <#SocketSocket 40 sockfd 128>.
  self primitiveFailed
]

internalAccept [
  <#SocketSocket 50 sockfd>.
  self primitiveFailed
]

accept [
  | ns |
  ns := self internalAccept.
  ns < 0 ifTrue: [ ^nil ].
  ^Socket newWithFD: ns
]
]


Socket subclass: TCPSocket [
^newWith: fd [
  | obj |
  obj := super new.
  obj socket: fd.
  ^obj
]

^new [
  ^self newWith: (super createTCP)
]
]
