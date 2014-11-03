function ahahRequest (argList) {
  var req;
  if (window.XMLHttpRequest) req = new XMLHttpRequest();
  if (!req) return false;
  var args = argList;

  function ahahNewState () {
    var st = req.readyState;
    var res = { state: st, XML: null, text: "", headers: "", status: 0, statusText: "" };
    res.abort = function () { req.abort(); };
    var hname = "!";
    switch (st) {
      case 1: hname = "onStart"; break;
      case 2: hname = "onHeaders"; break;
      case 3: hname = "onLoading"; break;
      case 4:
        res.status = req.status;
        res.statusText = ""+req.statusText;
        if (req.status >= 200 && req.status < 300) {
          hname = "onDone";
          res.XML = req.responseXML;
          res.text = ""+req.responseText;
          res.headers = ""+req.getAllResponseHeaders();
        } else hname = "onError";
        break;
    }
    if (args[hname]) setTimeout(function () { args[hname](res); }, 1);
  }

  req.onreadystatechange = ahahNewState;
  var m = (args.method||"GET")+"";
  try { req.open(m.toUpperCase(), args.url); }
  catch (e) {
    if (typeof(args.OnError) != "function") return false;
    setTimeout(function () {
      args.onError({XML:"", text:"", state:4, headers:"", status:403, statusText:"forbidden"});
    }, 1);
    return false;
  }
  if (args.headers) for (var n in args.headers) if (typeof(n) == "string") req.setRequestHeader(n, args.headers[n]);

  try { req.send(typeof(args.data)!="undefined"?args.data+"":null); }
  catch (e) {
    if (typeof(args.OnError) != "function") return false;
    setTimeout(function () {
      args.onError({XML:"", text:"", state:4, headers:"", status:403, statusText:"forbidden"});
    }, 1);
    return false;
  }
  return true;
}


/*
function AppendPagePosts (pageURL) {
  ahahRequest({
    method: "GET",
    url: pageURL+"&noscript",
    //data: formData+"message="+postContents,
    //headers: {"Content-Type": "application/x-www-form-urlencoded"},
    onError: function (resp) {
      RemoveInfoBox();
      requestIsRunning = false;
      requestQueue = [];
      ShowInfoBox("error loading page", true);
    },
    onStart: function (resp) {
    },
    onDone: function (resp) {
    }
  });
};
*/
