function fillSelector (aName, str) {
  var pl = document.getElementsByName(aName)[0];
  for (var f = pl.options.length; f > 0; --f) pl.remove(0);
  var sa = str.split("\n");
  for (var f = 0; f < sa.length; ++f) {
    if (!sa[f]) continue;
    var opt = document.createElement("option");
    opt.value = sa[f];
    opt.text = sa[f];
    pl.add(opt);
  }
}


function loadPackages () {
  showInfoBox("loading package list");
  ahahRequest({
    method: "GET",
    url: "/q/package",
    //data: formData+"message="+postContents,
    //headers: {"Content-Type": "application/x-www-form-urlencoded"},
    onError: function (resp) {
      removeInfoBox();
      showInfoBox("error loading package list", true);
    },
    onStart: function (resp) {
    },
    onDone: function (resp) {
      fillSelector("package", resp.text);
      removeInfoBox();
    }
  });
}


function updateSelector (aName, aParent) {
  var p = document.getElementsByName(aParent)[0];
  var q = p.selectedIndex;
  q = (q >= 0 && q < p.options.length) ? "?"+aParent+"="+escape(p.options[q].value) : "";
  showInfoBox("loading "+aName+" list");
  ahahRequest({
    method: "GET",
    url: "/q/"+aName+q,
    onError: function (resp) {
      removeInfoBox();
      showInfoBox("error loading "+aName+" list", true);
    },
    onStart: function (resp) {
    },
    onDone: function (resp) {
      fillSelector(aName, resp.text);
      removeInfoBox();
    }
  });
}


function updateClasses () { updateSelector("class", "package"); }
function updateMethods () { updateSelector("method", "class"); }


function updateSource () {
  var p, q, qq;
  p = document.getElementsByName("method")[0];
  qq = p.selectedIndex;
  q = (qq >= 0 && qq < p.options.length) ? "?method="+escape(p.options[qq].value) : "";
  p = document.getElementsByName("class")[0];
  qq = p.selectedIndex;
  q = q+((qq >= 0 && qq < p.options.length) ? "&class="+escape(p.options[qq].value) : "");
  showInfoBox("loading method source");
  ahahRequest({
    method: "GET",
    url: "/q/srctext"+q,
    onError: function (resp) {
      removeInfoBox();
      showInfoBox("error loading method source", true);
    },
    onStart: function (resp) {
    },
    onDone: function (resp) {
      document.getElementsByName("srctext")[0].value = resp.text;
      removeInfoBox();
    }
  });
}


function compileMethod () {
  var p, q, qq;
  p = document.getElementsByName("class")[0];
  qq = p.selectedIndex;
  q = (qq >= 0 && qq < p.options.length) ? "?class="+escape(p.options[qq].value) : "";
  p = document.getElementsByName("srctext")[0];
  showInfoBox("compiling method source");
  //opera.postError(escape("a + b"));
  ahahRequest({
    method: "POST",
    url: "/q/compile"+q,
    data: "srctext="+escape(p.value),
    headers: {"Content-Type": "application/x-www-form-urlencoded"},
    onError: function (resp) {
      removeInfoBox();
      showInfoBox("error compiling method source", true);
    },
    onStart: function (resp) {
    },
    onDone: function (resp) {
      document.getElementsByName("sterror")[0].value = resp.text;
      removeInfoBox();
    }
  });
}
