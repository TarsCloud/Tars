var EchoHandle = function () {

}

EchoHandle.prototype.initialize = function () {
    console.log("EchoHandle.prototype.initialize");
}

EchoHandle.prototype.echo = function (current, appBuffer) {
    current.sendResponse("Response","TX", "TX-MIG");
}

EchoHandle.prototype.doRequest = function ($current, $originRequest) {
    console.log("EchoHandle.doRequest::", $originRequest, arguments.length);

    $current.sendResponse("Response","TX", "TX-MIG");
}

module.exports.EchoHandle = EchoHandle;