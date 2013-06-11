self.onmessage = function(event) {
    self.postMessage(self.foo());
    self.close();
    return;
};
