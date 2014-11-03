"
  simple HTTP server and class browser
  Copyright (C) 199? <unknown author>
  Changes by Ketmar // Vampire Avalon

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
"
Requires [
  socket
]


"******************************************************
   String class additions.  These help parsing and
   handling URLs and HTML.
 ******************************************************"
String extend [
htmlEscape [
  "encode '<>&'"
  | res |
  (self includes: '<>&') ifFalse: [ ^self ].
  res := StringBuffer new.
  self do: [:c |
    c == $< ifTrue: [ res << '&lt;' ]
    ifFalse: [
      c == $> ifTrue: [ res << '&gt;' ]
      ifFalse: [
        c == $& ifTrue: [ res << '&amp;' ]
        ifFalse: [ res << c ]]]].
  ^res asString
]
]


"****************************************"
"           HTTPRequest class            "
"****************************************"
Object subclass: HTTPRequest [
| sock reqPath reqAction reqArgs reqRawData reqPathAndArgs reqError reqLength |

read: aSock [
  sock := aSock.
   ^(self rawData size > 0)
]

response: aResp [
  | responseSize tmpResponse |
  tmpResponse := StringBuffer new.
  "get the response size"
  responseSize := aResp size.
  "make HTTP headers"
  tmpResponse <<
    'HTTP/1.0 200 LittleSmalltalk\r\n' <<
    'Content-Type: text/html\r\n' <<
    ('Content-Length: ' + (responseSize printString) + '\r\n') <<
    '\r\n'.
  "add the response text"
  tmpResponse << aResp.
  sock send: tmpResponse asString.
  "close the connection now"
  sock close.
  ^self
]

rawData [
  | i tempData contentLength |
  "read the request raw data; this does some parsing"
  reqRawData isNil ifFalse: [ ^reqRawData ].
  "is the socket nil? or not open?"
  sock isNil ifTrue: [ ^nil ].
  " get the data from the socket until we see the header/body delimiter"
  tempData := sock recv: 1.
  [ (tempData position: '\r\n\r\n') isNil]
    whileTrue: [ tempData := tempData + (sock recv: 1) ].
  "OK, we have all the headers, what kind of request is it?"
  reqRawData := tempData.
  reqLength := tempData size.
  "if this is a POST, we need to get the length and read the data"
  ((self action) = 'POST') ifTrue: [
    'Processing POST action.' printNl.
    i := tempData position: 'Content-Length:' .
    i isNil ifTrue: [ reqError := '400 POST without Content-Length header'. ^nil ].
    "find the first digit character"
    i := i + ('Content-Length:' size).
    [ (tempData at: i) isBlank ] whileTrue: [ i := i+1 ].
    contentLength := 0.
    "convert the size into an integer while reading it in"
    [ (tempData at: i) isDigit ] whileTrue:
      [ contentLength := (contentLength * 10) + (((tempData at: i) value) - ($0 value)). i := i+1 ].
    "store the length for later"
    reqLength := contentLength.
    "the total length is the length of the header plus separator plus body, -1 for zero start"
    contentLength := contentLength + (tempData position: '\r\n\r\n') + 3.
    "read until we have all the data"
    [ (tempData size) < contentLength ] whileTrue: [tempData := tempData + (sock recv: 1)].
   ]
   ifFalse: [ reqLength := 0 ].
  "we have all the raw data. We've set reqAction, reqLength already, so set reqRawData"
  reqRawData := tempData.
  ^reqRawData.
]

pathAndArgs [
  | lines firstLine fields |
  reqPathAndArgs isNil ifFalse: [ ^reqPathAndArgs ].
  "break raw data into lines"
  lines := (self rawData) break: '\r\n'.
  firstLine := lines first.
  "break on spaces"
  fields := firstLine break: ' '.
  "path plus arguments is second field"
  fields removeFirst.
  reqPathAndArgs := fields first.
  ^reqPathAndArgs.
]

action [
  "if it was set once, return it"
  reqAction isNil ifFalse: [ ^reqAction.].
  'reqAction before compileWithClass: ' print.
  reqAction printString printNl.
  ((self rawData) position: 'GET') = 1 ifTrue: [ reqAction := 'GET'. ].
  ((self rawData) position: 'POST') = 1 ifTrue: [ reqAction := 'POST'. ].
  reqAction isNil ifTrue: [ reqAction := 'UNKNOWN' ].
  'reqAction: ' print.
  reqAction printString printNl.
  ^reqAction.
]

path [
  | i pathArgField |
  reqPath isNil ifFalse: [ ^reqPath ].
  reqPath = '' ifTrue: [ ^nil ].
  pathArgField := self pathAndArgs.
  pathArgField isNil ifTrue: [ reqPath := ''. ^nil ].
  i := pathArgField position: '?'.
  i isNil ifTrue: [ reqPath := pathArgField. ^reqPath ].
  reqPath := pathArgField from: 1 to: (i - 1).
  ^reqPath.
]

args [
  | i pathArgField argsData keyValList key val argList |
  "get args for both URL and POST data"
  reqArgs isNil ifFalse: [ ^reqArgs ].
  reqArgs := Dictionary new.
  "concatenate args"
  pathArgField := self pathAndArgs.
  (pathArgField isNil) ifFalse: [
    i := pathArgField position: '?'.
    i isNil ifFalse: [
      "copy the data"
      argsData := pathArgField from: (i+1) to: (pathArgField size).
      "append a & to make sure that we break correctly"
      argsData := argsData + '&'.
    ]
  ].
  "copy data from the form data if this is a POST"
  (self action) = 'POST' ifTrue: [
    i := ((self rawData size) + 1) - reqLength.
    argsData := argsData + ((self rawData) from: i to: (self rawData size))
  ].
  "do a little error checking"
  argsData isNil ifTrue: [ ^reqArgs ].
  (argsData size) = 0 ifTrue: [ ^reqArgs ].
  "split up the key value pairs"
  keyValList := argsData break: '&'.
  keyValList do: [ :keyValField |
    argList := keyValField break: '='.
    key := argList first.
    argList removeFirst.
    " handle case where key indicates a flag "
    (argList size) = 0
      ifTrue: [ val := true asString ]
      ifFalse: [ val := argList first asString ].
    val isNil ifTrue: [ val := 'no value' ].
    reqArgs at: (key fromUrl asSymbol) put: (val fromUrl).
  ].
  ^reqArgs.
]

at: aSymbol [
  ^(self args) at: aSymbol ifAbsent: [ nil ].
]
]


"*******************************"
"     HTTPDispatcher class      "
"*******************************"
Object subclass: HTTPDispatcher [
| map env runFlag sock request errorHandler |

register: aBlock at: aPath [
  map isNil ifTrue: [ map := Dictionary new ].
  map at: aPath put: aBlock.
  ^self.
]

registerErrorHandler: anObj [
  errorHandler := anObj.
  ^self.
]

startOn: aSock [
  | tmpRequest aBlock clientSock |
  runFlag := true.
  env := Dictionary new.
  aSock listen < 0 ifTrue: [
    aSock close.
    self error: 'can''t listen'
  ].
  [ runFlag = true ] whileTrue: [
    "get a request from the socket and dispatch it"
    clientSock := aSock accept.
    tmpRequest := HTTPRequest new.
    (tmpRequest read: clientSock) ifTrue: [
      aBlock := map at: (tmpRequest path) ifAbsent: [ nil ].
      ( aBlock isNil )
        ifTrue: [ errorHandler value: tmpRequest value: env]
        ifFalse: [ aBlock value: tmpRequest value: env ].
    ].
    clientSock close.
  ].
]

stop [
  runFlag := false.
]
]


"*********************************"
"     HTTPClassBrowser class     "
"*********************************"
Object subclass: HTTPClassBrowser [

listPackagesOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.
  outBuf << '<HTML><BODY>'.
  Package packages keysDo: [ :obj |
    obj := obj asString.
    outBuf <<
      '<A HREF="/class_list_frame?package=' <<
      (obj toUrl) <<
      '" target="class_list_frame">' <<
      obj <<
      '</A><BR>'
  ].
  outBuf << '</BODY></HTML>'.
  ^aReq response: outBuf
]

addClassName: outBuf class: obj [
  | name |
  obj isMeta ifFalse: [
    name := obj printString.
    outBuf <<
      '<A HREF="/method_list_frame?class=' <<
      (name toUrl) <<
      '" target="method_list_frame">' <<
      name <<
      '</A><BR>'
  ].
]

listClassesOn: aReq [
  | outBuf pkg |
  pkg := aReq at: #package.
  pkg ifNotNil: [ pkg := Package find: pkg ].
  outBuf := StringBuffer new.
  outBuf << '<HTML><BODY>'.
  pkg
    ifNil: [
      globals do: [:obj | (obj isKindOf: Class) ifTrue: [ self addClassName: outBuf class: obj ]].
    ] ifNotNil: [
      pkg classes do: [:obj | (obj isKindOf: Class) ifTrue: [ self addClassName: outBuf class: obj ]].
    ].
  outBuf << '</BODY></HTML>'.
  ^aReq response: outBuf
]

addMethodToBuf: outBuf class: classStr name: name asMeta: asMeta [
  | n |
  n := name asString.
  asMeta ifTrue: [ n := '^' + n ].
  outBuf <<
    '<A HREF="/edit_frame?class=' <<
    classStr <<
    '&method=' <<
    (n toUrl) <<
    '" target="edit_frame">' <<
    (n htmlEscape) <<
    '</A><BR>'.
]

addClassMethodsToBuf: outBuf class: class classStr: classStr asMeta: asMeta [
  (class methods size) = 0 ifTrue: [ ^false ].
  class methods keysAndValuesDo: [ :name :meth |
    "HTML doesn't like < signs"
    self addMethodToBuf: outBuf class: classStr name: name asMeta: asMeta.
  ].
  ^true
]

listMethodsOn: aReq [
  | outBuf classStr class cc |
  outBuf := StringBuffer new.
  "header for page"
  outBuf << '<HTML><BODY>'.
  classStr := aReq at: #class.
  "if there isn't a class string chosen"
  classStr isNil ifTrue: [ outBuf << '<B>No class chosen.</B></BODY></HTML>'. ^aReq response: outBuf ].
  class := globals at: (classStr asSymbol) ifAbsent: [ nil ].
  class isNil ifTrue: [ outBuf << '<B>No such class!</B></BODY></HTML>'. ^aReq response: outBuf ].
  "some classes have no methods"
  (cc := class class) isMeta ifTrue: [
    self addClassMethodsToBuf: outBuf class: cc classStr: classStr asMeta: true.
    (cc methods size) = 0 ifFalse: [
      (class methods size) = 0 ifFalse: [ outBuf << '<hr>' ]
    ].
  ].
  self addClassMethodsToBuf: outBuf class: class classStr: classStr asMeta: false.
  "outBuf << '<B>No methods in class</B>'"
  outBuf << '</BODY></HTML>'.
  ^aReq response: outBuf.
]

frameSubclass: aReq [
  | outBuf classStr newClassStr |
  outBuf := StringBuffer new.
  outBuf << '<html><body>'.
  aReq action = 'POST' ifFalse: [
    outBuf << '<b>POST form submission required.</b>'
  ] ifTrue: [
    classStr := aReq at: #class.
    newClassStr := aReq at: #subclassname.
    classStr printNl.
    newClassStr printNl.
    (globals at: (classStr asSymbol) ifAbsent: [ nil ]) ifNil: [
      outBuf << '<b>ERROR: can''t subclass non-existing class.</b>'
    ] ifNotNil: [
      aReq at: #class put: newClassStr.
      ^self listMethodsOn: aReq.
    ].
  ].
  outBuf << '</body></html>'.
  ^aReq response: outBuf.
]

editMethodOn: aReq [
  | outBuf classStr class methStr method body |
  outBuf := StringBuffer new.
  outBuf << '<HTML><BODY>'.
  classStr := aReq at: #class.
  "if there isn't a class string chosen"
  classStr isNil ifTrue: [ outBuf << '<B>No class chosen.</B></BODY></HTML>'. ^aReq response: outBuf ].
  class := globals at: (classStr asSymbol) ifAbsent: [ nil ].
  class isNil ifTrue: [ outBuf << '<B>No such class!</B></BODY></HTML>'. ^aReq response: outBuf ].
  "if there isn't a method string chosen"
  methStr := aReq at: #method.
  methStr printNl.
  "debugging (aReq args) keysAndValuesDo: [ :key :val | outBuf << ((key printString) + ' = ' + (val printString) + '<BR>') ]."
  methStr = 'no value' ifTrue: [ outBuf + '<B>No Value!</B></BODY></HTML>'. ^aReq response: outBuf ].
  ((methStr isNil) or: [ methStr isEmpty ]) ifTrue: [ outBuf << '<B>No method chosen.</B></BODY></HTML>'. ^aReq response: outBuf ].
  methStr firstChar == $^
    ifTrue: [
      method := (class class methods) at: ((methStr from: 2) asSymbol) ifAbsent: [ nil ].
    ] ifFalse: [
      method := (class methods) at: (methStr asSymbol) ifAbsent: [ nil ].
    ].
  method isNil ifTrue: [ outBuf << '<B>No such method!</B></BODY></HTML>'. ^aReq response: outBuf ].
  (body := method text) ifNil: [ body := '"no source"' ].
  methStr firstChar == $^ ifTrue: [ body := '^' + body ].
  outBuf <<
    '<FORM ACTION="/compile_method?class=' <<
    classStr <<
    '&method=' <<
    (method name printString toUrl) <<
    '" ENCTYPE="application/x-www-form-urlencoded" METHOD="POST">' <<
    '<TEXTAREA COLS=60 ROWS=40 NAME="methsrc" WRAP="OFF">' <<
    (body htmlEscape) <<
    '</TEXTAREA>' <<
    '<BR><INPUT TYPE=SUBMIT NAME=compile VALUE="Compile">' <<
    '<br><br><input type="submit" name=dogst value="Execute GST-style code">' <<
     '</FORM>' <<
     '</BODY></HTML>'.
  ^aReq response: outBuf.
]

compileMethodOn: aReq [
  | outBuf classStr class methSrc action meth p warnings |
  outBuf := StringBuffer new.
  outBuf << '<HTML><BODY>'.
  "check to make sure this is a POST"
  action := aReq action.
  action = 'POST' ifFalse: [
    outBuf << '<B>POST form submission required.</B></BODY></HTML> '.
    ^aReq response: outBuf.
  ].
  methSrc := aReq at: #methsrc.
  (aReq at: #compile) ~= 'Compile' ifTrue: [
    methSrc isNil ifTrue: [ outBuf << '<B>No source!</B></BODY></HTML>'. ^aReq response: outBuf ].
    "filter out carriage returns"
    methSrc := (methSrc printString) reject: [ :c | c isCR ].
    methSrc := StringStream newWith: (methSrc removeTrailingBlanks).
    warnings := ''.
    p := GSTParser newWith: methSrc.
    p errorBlock: [ :msg |
      outBuf << '<b>COMPILATION ERROR: ' << (msg htmlEscape) << '</b>'.
      outBuf << '</body></html>'.
      ^aReq response: outBuf.
    ].
    p warningBlock: [ :msg |
      outBuf << '<b>COMPILATION WARNING: ' << (msg htmlEscape) << '</b><br />'.
    ].
    p parse.
    outBuf << warnings << '</body></html>'.
    ^aReq response: outBuf.
  ].
  "if there isn't a class string chosen"
  classStr := aReq at: #class.
  classStr isNil ifTrue: [ outBuf << '<B>No class chosen.</B></BODY></HTML>'. ^aReq response: outBuf ].
  class := globals at: (classStr asSymbol) ifAbsent: [ nil ].
  class isNil ifTrue: [ outBuf << '<B>No such class!</B></BODY></HTML>'. ^aReq response: outBuf ].
  "get the method source."
  methSrc isNil ifTrue: [ outBuf << '<B>No method source!</B></BODY></HTML>'. ^aReq response: outBuf ].
  methSrc isEmpty ifTrue: [ outBuf << '<B>No method source!</B></BODY></HTML>'. ^aReq response: outBuf ].
  "compile and add method"
  p := LstCompiler new.
  p errorBlock: [ :msg :lineNum |
    outBuf << '<b>COMPILATION ERROR: near line ' << (lineNum asString) << ': ' << (msg htmlEscape) << '</b>'.
    outBuf << '</body></html>'.
    ^aReq response: outBuf.
  ].
  p warningBlock: [ :msg :lineNum |
    outBuf << '<b>COMPILATION WARNING: near line' << (lineNum asString) << ': ' << (msg htmlEscape) << '</b><br />'.
  ].
  (meth := (class addMethod: methSrc withCompiler: p)) ifNotNil: [
    outBuf << (meth name printString) << ' added.'.
  ].
  outBuf << '</body></html>'.
  ^aReq response: outBuf.
]

showBaseFrameOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.

  outBuf <<
    '<HTML><FRAMESET COLS="50%,50%" FRAMEBORDER="YES">' <<
    '<FRAME SRC="/control_list_frame" NAME="control_list_frame">' <<
    '<FRAME SRC="/edit_frame" NAME="edit_frame">' <<
    '</FRAMESET></HTML>'.

  ^aReq response: outBuf.
]

showControlListFrameOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.

  outBuf << '<HTML><FRAMESET ROWS="80%,20%" FRAMEBORDER="YES">' <<
    '<FRAME SRC="/list_frame" NAME="list_frame">' <<
    '<FRAME SRC="/control_frame" NAME="control_frame">' <<
    '</FRAMESET></HTML>'.

  ^aReq response: outBuf.
]

showListFrameOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.

  outBuf << '<HTML><FRAMESET COLS="20%,40%,40%" FRAMEBORDER="YES">' <<
    '<FRAME SRC="/package_list_frame" NAME="package_list_frame">' <<
    '<FRAME SRC="/class_list_frame" NAME="class_list_frame">' <<
    '<FRAME SRC="/method_list_frame" NAME="method_list_frame">' <<
    '</FRAMESET></HTML>'.

  ^aReq response: outBuf.
]

showControlFrameOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.

  outBuf addLast: '<HTML><BODY><FORM METHOD="GET" ACTION="/stop" TARGET="_top">'.
  outBuf addLast: '<INPUT TYPE=SUBMIT NAME=stop VALUE="Stop Browser">'.
  outBuf addLast: '</FORM></BODY></HTML>'.

  ^aReq response: outBuf.
]

showErrorOn: aReq [
  | outBuf |
  outBuf := StringBuffer new.

  outBuf addLast: '<HTML><BODY><B>Path not recognized!</B><BR>'.
  outBuf addLast: '<PRE>'.
  outBuf addLast: ('path: ' + (aReq path) + '\n').
  aReq args isNil ifFalse: [ (aReq args) keysAndValuesDo:
          [ :key :val | outBuf addLast: ((key printString) + '=' + (val printString) + '\n') ] ].

  outBuf addLast: '</PRE></BODY></HTML>'.

  ^aReq response: outBuf.
]

startOn: aSock [
  | dispatcher |
  dispatcher := HTTPDispatcher new.

  dispatcher register: [:aReq :anEnv | self showBaseFrameOn: aReq. nil]
      at: '/'.
  dispatcher register: [:aReq :anEnv | self showControlListFrameOn: aReq. nil]
      at: '/control_list_frame'.
  dispatcher register: [:aReq :anEnv | self showListFrameOn: aReq. nil]
      at: '/list_frame'.
  dispatcher register: [:aReq :anEnv | self showControlFrameOn: aReq. nil]
      at: '/control_frame'.
  dispatcher register: [:aReq :anEnv | self listPackagesOn: aReq. nil]
      at: '/package_list_frame'.
  dispatcher register: [:aReq :anEnv | self listClassesOn: aReq. nil]
      at: '/class_list_frame'.
  dispatcher register: [:aReq :anEnv | self listMethodsOn: aReq. nil]
      at: '/method_list_frame'.
  dispatcher register: [:aReq :anEnv | self editMethodOn: aReq. nil]
      at: '/edit_frame'.
  dispatcher register: [:aReq :anEnv | self compileMethodOn: aReq. nil]
      at: '/compile_method'.
  dispatcher register: [:aReq :anEnv | self frameSubclass: aReq. nil]
      at: '/subclass'.
  dispatcher register: [:aReq :anEnv | aReq response: '<HTML><BODY><B>Class browser stopped.</B></BODY></HTML>'. dispatcher stop.  aSock close. nil]
      at: '/stop'.

  dispatcher registerErrorHandler: [ :aReq :anEnv | self showErrorOn: aReq. nil].
  dispatcher startOn: aSock.
  ^nil.
]

^start [
  | sock obj cnt |
  "create a default socket on which to listen"
  cnt := 20.
  [ cnt > 0 ] whileTrue: [
    sock := TCPSocket new.
    (sock bind: '127.0.0.1' port: 6789) < 0 ifFalse: [
      'starting...' printNl.
      obj := self new.
      ^obj startOn: sock.
    ].
    sock close.
    'binding failed.' printNl.
    cnt := cnt - 1.
    System sleep: 3.
  ].
  self error: 'can''t start browser: binding error'.
]
]


{ HTTPClassBrowser start }
