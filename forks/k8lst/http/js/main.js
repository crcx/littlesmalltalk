function mainOnLoad () {
  setTimeout(function () {
    var div = document.createElement("div");
    div.innerHTML = 'TEST';
    document.body.appendChild(div);
  }, 4000);
}
