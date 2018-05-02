var unirest = require("unirest");

var SerialPort = require('serialport')
var serialPort = new SerialPort('COM4', {
    baudRate: 115200
})

var Readline = SerialPort.parsers.Readline
var cuerpo = {};
var parser = new Readline()
serialPort.pipe(parser)
parser.on('data', function (data) {
    cuerpo = data;
    var req = unirest("POST", "http://104.196.17.88:3000/savedata");

    req.headers({
        "postman-token": "fd841325-8442-6ec1-dbd2-15bab99970b4",
        "cache-control": "no-cache",
        "content-type": "application/json"
    });

    req.type("json");
    req.send(cuerpo);

    req.end(function (res) {
        try {
            console.log(res.body);
        }
        catch (e) {
            console.log(e);
        }
    });

})

serialPort.on('open', function () {
    console.log('Communication is on!')
})
