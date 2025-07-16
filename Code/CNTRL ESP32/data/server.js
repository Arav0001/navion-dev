function sendCommand(cmd) {
    fetch(`/${cmd}`)
    .then(res => res.text())
    .then(msg => alert(msg))
    .catch(err => alert("Error: " + err));
}