"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Requires [ socket httpreq ]

Package [
  HttpServer
]


class: HttpHandler [
  | obuf code msg ftype postData req ctype |

  postData: pd [
    postData := pd
  ]

  emit: aStr [
    obuf << aStr
  ]

  set2xx [
    code := 200.
    msg := 'OK'.
  ]

  set4xx [
    code := 404.
    msg := 'Not Found'.
  ]

  emitAll: aEmit [
    aEmit value: 'HTTP/1.0 '.
    aEmit value: (code printWidth: 3).
    aEmit value: ' '.
    aEmit value: msg asString.
    aEmit value: '\r\n'.
    aEmit value: 'Expires: Thu, 01 Dec 1994 16:00:00 GMT\r\n'.
    aEmit value: 'Cache-Control: no-cache, no-store, must-revalidate\r\n'.
    aEmit value: 'Connection: close\r\n'.
    aEmit value: 'Content-Type: '.
    aEmit value: ctype.
    aEmit value: 'text/html\r\n'.
    aEmit value: 'Content-Length: '.
    aEmit value: (obuf size asString).
    aEmit value: '\r\n\r\n'.
    aEmit value: obuf.
  ]

  emitBody [
    ctype := 'text/plain'.
    self set4xx.
    self emit: '<html><body>nothing to see here</body></html>'.
  ]

  fileType: ft [
    ftype := ft
  ]

  openFile [
    ^nil
  ]

  sendFile: fl emit: aEmit [
    | buf left rd |
    left := fl size.
    [ 'sending ' print. left print. ' bytes...' printNl ] runLocked.
    aEmit value: 'HTTP/1.0 '.
    aEmit value: (code printWidth: 3).
    aEmit value: ' '.
    aEmit value: msg asString.
    aEmit value: '\r\n'.
    aEmit value: 'Connection: close\r\n'.
    aEmit value: 'Content-Type: '.
    aEmit value: ftype.
    aEmit value: '\r\n'.
    aEmit value: 'Content-Length: '.
    aEmit value: (left asString).
    aEmit value: '\r\n\r\n'.
    buf := String new: 4096.
    [ left > 0 ] whileTrue: [
      fl read: buf size: (rd := left min: (buf size)).
      "FIXME: any errors?"
      rd < buf size ifTrue: [ aEmit value: (buf from: 1 to: rd) ] ifFalse: [ aEmit value: buf ].
      left := left - rd.
    ].
    fl close.
  ]

  process: aReq emit: aEmit [
    | fl |
    req := aReq.
    ctype := 'text/html'.
    (fl := self openFile)
      ifNotNil: [
        self sendFile: fl emit: aEmit
      ] ifNil: [
        obuf := StringBuffer new.
        self set4xx;
          emitBody;
          emitAll: aEmit.
      ]
  ]
]


class: HttpDispatcher
| pd |
[
  ^initialize [
    pd := Dictionary new.
  ]

  ^addHandler: aPath handler: aHandler [
    pd ifNil: [ self initialize ].
    pd at: aPath put: aHandler
  ]

  ^findHandler: aPath [
    | h |
    pd ifNil: [ self initialize ].
    h := pd at: aPath ifAbsent: [ HttpHandler ].
    ^h new.
  ]
]


class: HttpShedulerProcList [
  | procs curProc count |

  ^new [
    | obj |
    obj := super new.
    self in: obj var: #count put: 0.
    ^obj
  ]

  isEmpty [
    ^procs isNil
  ]

  current [
    curProc ifNil: [ ^nil ].
    ^curProc value
  ]

  add: aProc [
    aProc ifNotNil: [
      procs := Link value: aProc next: procs.
      curProc ifNil: [ curProc := procs ].
      count := count + 1.
    ].
  ]

  includes: aProc [
    | link |
    link := procs.
    [ link isNil ] whileFalse: [
      link value == aProc ifTrue: [ ^true ].
      link := link next
    ].
    ^false
  ]

  remove: aProc [
    | link prev |
    link := procs.
    prev := nil.
    [ link isNil ] whileFalse: [
      link value == aProc ifTrue: [
        prev ifNil: [ procs := procs next ] ifNotNil: [ prev next: link next ].
        curProc ifNotNil: [ curProc value == aProc ifTrue: [ curProc := nil ] ].
        count := count - 1.
        ^true
      ].
      prev := link.
      link := link next
    ].
    ^false
  ]

  sheduleNext [
    | prc flg |
    "next to end"
    (prc := curProc) ifNil: [ prc := procs ] ifNotNil: [ prc := prc next ].
    [ prc ] whileNotNil: [
      prc value isWaiting ifFalse: [ ^(curProc := prc) value ].
      prc := prc next.
    ].
    "start to current (inclusive)"
    curProc ifNotNil: [
      prc := procs.
      [ prc value isWaiting ifFalse: [ ^(curProc := prc) value ].
        flg := prc == curProc.
        prc := prc next.
        flg ] whileFalse: [ ].
    ].
    ^curProc := nil.
  ]

  do: aBlock [
    | link |
    link := procs.
    [ link isNil ] whileFalse: [
      aBlock value: link value.
      link := link next
    ]
  ]

  size [
    ^count
  ]
]


Process subclass: HttpProcess [
  | worker socket inbuf outbuf waiting hdr pdleft pdata |

  ^new: sk [
    | obj ctx args |
    obj := super new.
    "worker := HttpWorker new."
    self in: obj var: #socket put: sk.
    self in: obj var: #waiting put: true.
    (args := Array new: 1) at: 1 put: obj.
    ctx := Context new.
    ctx setup: (obj findMethod: #run) withArguments: args.
    obj context: ctx.
    ^obj
  ]

  id [
    ^socket fd
  ]

  socket [
    ^socket
  ]

  isWaiting [
    ^waiting
  ]

  waiting: aFlag [
    waiting := aFlag
  ]

  wantWrite [
    outbuf ifNil: [ ^false ].
    ^true
  ]

  doWrite [
    | sent s |
    [
      "'doWrite:' printNl. outbuf asString printNl."
      outbuf ifNotNil: [
        socket canWrite ifTrue: [
          s := outbuf asString.
          s isEmpty ifFalse: [
            sent := socket send: s.
            "'sent: ' print. sent printNl."
            sent < 0 ifTrue: [ sent := s size ].
            s := s from: sent+1.
          ].
          s isEmpty ifTrue: [
            context ifNil: [
              "'NO MORE!' printNl."
              outbuf := nil.
              socket close.
            ] ifNotNil: [ outbuf clear ].
          ] ifFalse: [ outbuf clear; addLast: s ].
        ] ifFalse: [
          (socket selectFor: 2 timeout: -1) < 0 ifTrue: [ socket close. outbuf := nil ].
        ]
      ]
    ] runLocked.
  ]

  run [
    | s hdrDone p req |
    hdr := ''.
    hdrDone := false.
    [ hdrDone ] whileFalse: [
      "[ 'rcv; id=' print. self id printNl. ] runLocked."
      (s := socket recv: 2048) ifNil: [ ^false ].  "error"
      "[ 'GOT; id=' print. self id print. '; s=' print. s printNl. ] runLocked."
      s isEmpty ifFalse: [
        hdr := hdr + s. s := nil.
        "[ 'got: ' printNl. s printNl. ] runLocked."
        p := hdr position: '\r\n\r\n'.
        "[ '***p=' print. p printNl. ] runLocked."
        p ifNotNil: [
          inbuf := StringBuffer new.
          inbuf << (hdr from: p+1).
          hdr := hdr from: 1 to: p-1.
          "[ 'headers:' printNl. hdr print. ] runLocked."
          req := HTTPRequest new: hdr abortBlock: [:err |
            outbuf := StringBuffer new.
            [ outbuf <<
                'HTTP/1.0 500 Internal Error\r\n' <<
                'Connection: close\r\n' <<
                'Content-type: text/plain\r\n' <<
                '\r\n' <<
                'internal error\n\n' <<
                err ] runLocked.
            ^false
          ].
          hdr := nil.
          [ 'path: ' print. req path printNl ] runLocked.
          req method = 'POST' ifTrue: [
            (pdleft := req field: 'content-length') ifNotNil: [
              (pdleft := pdleft asNumber) ifNotNil: [
                [ 'post data size: ' print. pdleft printNl. ] runLocked.
                pdleft < 65536 ifTrue: [
                  pdata := StringBuffer new.
                  [ pdleft > 0 ] whileTrue: [
                    [ waiting := true ] runLocked. System yield: true.
                    (s := socket recv: (pdleft min: 2048)) ifNil: [ ^false ].  "error"
                    "[ 's: ' print. s printNl. ] runLocked."
                    s isEmpty ifFalse: [ pdata << s. pdleft := pdleft - s size ].
                    s := nil.
                  ].
                  [ 'got ' print. pdata size print. ' post data bytes' printNl. ] runLocked.
                  "
                  pdata := pdata asString fromUrl.
                  [ 'pd: ' print. pdata printNl ] runLocked.
                  "
                  req parseVars: pdata asString.
                  pdata := nil.
                ]
              ]
            ]
          ].
          "[ req debugDump ] runLocked."
          p := HttpDispatcher findHandler: (req path).
          outbuf := StringBuffer new.
          p ifNil: [
            [ outbuf <<
              'HTTP/1.0 404 Not Found\r\n' <<
              'Connection: close\r\n' <<
              'Content-type: text/plain\r\n' <<
              '\r\n' <<
              'void runner!' ] runLocked.
            ^true
          ].
          p process: req emit: [:txt | [ outbuf << txt ] runLocked ].
        ] ifNil: [
          hdr size > 32768 ifTrue: [ ^false ].  "headers too big"
        ].
      ].
      [ waiting := true ] runLocked. System yield: true.
    ].
  ]
]

class: HttpSheduler [
  | procs abort slice lsk tokill |

  ^new: aSlice [
    | obj |
    obj := super new.
    self in: obj var: #procs put: (HttpShedulerProcList new).
    self in: obj var: #abort put: false.
    self in: obj var: #tokill put: (List new).
    obj slice: aSlice.
    ^obj
  ]

  ^new [
    ^self new: 10000
  ]

  slice [
    ^slice
  ]

  slice: aTicks [
    aTicks < 0 ifTrue: [ slice := 1 ] ifFalse: [ slice := aTicks + 1 ]
  ]

  newProcess: sock [
    | proc |
    proc := HttpProcess new: sock.
    procs add: proc.
    ^true
  ]

  abortAll [
    abort := true
  ]

  buildReadArray [
    | wa idx |
    wa := Array new: (procs size) + 1.
    wa at: 1 put: lsk socket.
    idx := 2.
    procs do: [:prc |
      prc isWaiting ifTrue: [ wa at: idx put: prc socket socket ].
      idx := idx + 1.
    ].
    ^wa
  ]

  buildWriteArray [
    | wa idx |
    wa := Array new: (procs size).
    idx := 1.
    procs do: [:prc |
      prc wantWrite ifTrue: [ wa at: idx put: prc socket socket ].
      idx := idx + 1.
    ].
    ^wa
  ]

  runOne: prc [
    | res |
    prc wantWrite ifTrue: [ prc doWrite ].
    prc context ifNil: [
      prc wantWrite ifFalse: [ tokill add: prc ].
      ^self
    ].
    "'running: ' print. prc id print."
    prc waiting: false.
    res := prc doExecute: slice.
    "'; res=' print. res printNl."
    Case test: res;
      case: 4 do: [  "process complete"
        "'  WW: ' print. prc wantWrite printNl."
        prc wantWrite
          ifTrue: [ prc waiting: false ]
          ifFalse: [ tokill add: prc ].
      ];
      case: 5 do: [ prc waiting: false ];  "time quantum expired"
      case: 7 do: [ ];  "yielded; do nothing, prc takes care about it's 'waiting' state"
      else: [ tokill add: prc. prc errorReport: res ].
  ]

  runSR: anArray sidx: idx [
    | aa |
    procs do: [:prc |
      aa := anArray at: idx.
      idx := idx + 1.
      aa ifTrue: [ self runOne: prc ].
    ].
  ]

  killUndead [
    tokill do: [:prc | prc socket close. procs remove: prc ].
    tokill := List new.
  ]

  procDump [
    procs ifNil: [ 'no processes' printNl ].
    procs do: [:prc |
      (procs current == prc ifTrue: [ '*' ] ifFalse: [ ' ' ]) print.
      'process id=' print. prc id print.
      '; waiting=' print. prc isWaiting print.
      '; wantWrite=' print. prc wantWrite print.
      '' printNl.
    ]
  ]

  run [
    | tout sres csock ra wa |
    [ abort ] whileFalse: [
      procs isEmpty
        ifTrue: [
          "wait for client"
          'no active processes...' printNl.
          sres := lsk selectFor: 1 timeout: -1.
          sres <= 0 ifTrue: [ ^false ].
          'client comes!' printNl.
          csock := lsk accept.
          self newProcess: csock.
        ] ifFalse: [
          procs sheduleNext ifNotNil: [
            "has something to do"
            tout := 0.
          ] ifNil: [
            "nothing to do"
            tout := -1.
          ].
          "self procDump."
          tokill := List new.
          "'building read array...' printNl."
          ra := self buildReadArray.
          "'building write array...' printNl."
          wa := self buildWriteArray.
          "'tout: ' print. tout print. '; ra=' print. ra print. '; wa=' print. wa printNl."
          sres := Socket selectRead: ra write: wa timeout: tout.
          "' res: ' print. sres print. '; ra=' print. ra print. '; wa=' print. wa printNl."
          sres
            ifTrue: [
              "have something"
              "'trying RA...' printNl."
              self runSR: ra sidx: 2.
              "'trying WA...' printNl."
              self runSR: wa sidx: 1.
              "'removing zombies...' printNl."
              self killUndead.
              "'checking for the new client...' printNl."
              (ra at: 1) ifTrue: [
                'client comes!' printNl.
                csock := lsk accept.
                self newProcess: csock.
              ].
            ].
          "'procs current = ' print. procs current printNl."
          procs current ifNil: [
            procs sheduleNext.
            "'new procs current = ' print. procs current printNl."
          ].
          procs current ifNotNil: [ self runOne: procs current. self killUndead. ].
        ].
    ].
  ]

  startOn: aPort [
    "create a default socket on which to listen"
    | cnt |
    cnt := 20.
    [ cnt > 0 ] whileTrue: [
      lsk := TCPSocket new.
      (lsk bind: '127.0.0.1' port: aPort) < 0 ifFalse: [
        'HTTP server is ready at port ' print. aPort printNl.
        lsk listen < 0 ifTrue: [ lsk close. self error: 'can''t listen'. ].
        self run.
        lsk close.
        ^true
      ].
      lsk close.
      'binding failed.' printNl.
      cnt := cnt - 1.
      System sleep: 6.
    ].
    self error: 'can''t start sheduler: binding error'.
  ]
]


"
{
  HttpDispatcher addHandler: '/' handler: (HttpHandlerMain new).
  HttpDispatcher addHandler: '/js/' handler: (HttpHandlerJS new).
  (HttpSheduler new) startOn: 6789.
}
"
