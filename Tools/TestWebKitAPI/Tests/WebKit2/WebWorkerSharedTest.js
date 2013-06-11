var ports = [];

onconnect = function(event) {
    var port = event.ports[0];
    ports.push(port);
    port.start();
    port.addEventListener("message", function(event) { listenForMessage(event, port); } );
}

listenForMessage = function (event, port) {
    port.postMessage(self.bar());
    self.close();
}
