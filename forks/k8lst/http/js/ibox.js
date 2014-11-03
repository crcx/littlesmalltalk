function showInfoBox (text, isErr) {
  var iname = "k8_info_box";
  var bg = "#333", clr = "#fff";
  if (isErr) {
    bg = "#f00"; clr = "#ff0";
  }
  var dd = document.getElementById(iname);
  if (!dd) {
    dd = document.createElement("div");
    dd.setAttribute("id", iname);
    dd.setAttribute("style",
      "background:"+bg+";color:"+clr+";"+
      "font-size:9pt;font-family:verdana, arial, helvetica, sans-serif;font-weight:normal;display:block;"+
      "text-align:center;white-space:nowrap;"+
      "width:auto;height:auto;"+
      "min-width:0;min-height:0;max-width:none;max-height:none;opacity:1;z-index:9995;float:none;clear:both;"+
      "padding:3px;border:#fff solid 1px;"+
      "position:fixed;left:0;top:0;"+
      ""
    );
    dd.appendChild(document.createTextNode(text||"loading next page"));
    if (isErr) {
      dd.onclick = function () {
        var p = dd.parentNode, me = dd;
        if (p) {
          dd.setAttribute("style", "display:none");
          setTimeout(function () { if (me.parentNode) me.parentNode.removeChild(me); }, 1);
        }
      };
    }
    document.body.appendChild(dd);
  } else {
    dd.firstChild.textValue = text||"...";
    dd.setAttribute("style", "display:block");
  }
}


function removeInfoBox () {
  var dd = document.getElementById("k8_info_box");
  if (dd) {
    dd.setAttribute("style", "display:none");
    setTimeout(function () { if (dd.parentNode) dd.parentNode.removeChild(dd); }, 1);
  }
}
