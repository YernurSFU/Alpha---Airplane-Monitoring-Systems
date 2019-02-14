"use strict";

var socketio = require('socket.io'),
    dgram = require('dgram'),
    app = require('express')(),
    http = require('http').Server(app),
    io = require('socket.io')(http),
    HARDWARE_SERVER_PORT_NUMBER = 12345,
    SERVER_PORT_NUMBER = 9191,
    HARDWARE_SERVER_HOST_ADDR = '127.0.0.1',
    hardwareServer = dgram.createSocket('udp4'),
    serverTimeout,
    isTimeOutSet = false,
    globalSocket,
    globalValues = {
        'systemUptimeTrace': [],
        'serverTimeMSTrace': [],
        'humidityTrace': [],
        'temperatureTrace': [],
        'positionTrace': [],
        'pressureTrace': [],
        'doorStateTrace': []
    },
    lastReplyTime = null,
    MAX_WINDOW_LENGTH = 100,
    ACCEPTED_MS_TIME = 5000,
    UDP_MS_TIMEOUT = 5000;

function secondsToHms(d) {
    d = Number(d);
    var h = Math.floor(d / 3600);
    var m = Math.floor(d % 3600 / 60);
    var s = Math.floor(d % 3600 % 60);
    return ('0' + h).slice(-2) + ":" + ('0' + m).slice(-2) + ":" + ('0' + s).slice(-2);
}

function onTimeOut() {
    clearTimeout(serverTimeout);
    isTimeOutSet = false;
}

hardwareServer.on("message", function (msg, rinfo) {
    clearTimeout(serverTimeout);
    isTimeOutSet = false;
    var msgData = msg.toString('ascii', 0, rinfo.size);
    var jsonData = JSON.parse(msgData);
    console.log('Timeout cleared (success).');
    if (globalValues['systemUptimeTrace'].length == MAX_WINDOW_LENGTH) {
        globalValues['systemUptimeTrace'].shift();
        globalValues['serverTimeMSTrace'].shift();
        globalValues['humidityTrace'].shift();
        globalValues['temperatureTrace'].shift();
        globalValues['positionTrace'].shift();
        globalValues['pressureTrace'].shift();
        globalValues['doorStateTrace'].shift();
    }
    var replyTime = new Date();
    lastReplyTime = replyTime.getTime();
    globalValues['systemUptimeTrace'].push(secondsToHms(process.uptime()));
    globalValues['serverTimeMSTrace'].push(replyTime.getHours() + ':' + replyTime.getMinutes() + ':' + replyTime.getSeconds() + ':' + replyTime.getMilliseconds());
    globalValues['humidityTrace'].push(jsonData['humidityVal']);
    globalValues['temperatureTrace'].push(jsonData['temperatureVal']);
    globalValues['positionTrace'].push(jsonData['positionVal']);
    globalValues['pressureTrace'].push(jsonData['pressureVal']);
    globalValues['doorStateTrace'].push(jsonData['doorStateVal']);
});

function handleCommand(socket) {
    socket.on('GET_UPDATES', function () {
        if (lastReplyTime == null || (((new Date).getTime()) - lastReplyTime) > ACCEPTED_MS_TIME) {
            globalSocket.emit('GET_UPDATES_FAIL');
        } else {
            globalSocket.emit('GET_UPDATES_SUCCESS', globalValues);
        }
    });
};

io.set('log level', 0);
io.on('connection', function (socket) {
    console.log('Connected with: "' + socket.handshake.address + '".');
    globalSocket = socket;
    handleCommand(socket);
});

http.listen(SERVER_PORT_NUMBER, function () {
    console.log('Listening on: "*:' + SERVER_PORT_NUMBER + '"...');
});

setInterval(function () {
    if (!isTimeOutSet) {
        var message = "GET_UPDATES";
        hardwareServer.send(message, 0, message.length, HARDWARE_SERVER_PORT_NUMBER, HARDWARE_SERVER_HOST_ADDR);

        // Set UDP_MS_TIMEOUT timeout.
        serverTimeout = setTimeout(onTimeOut, UDP_MS_TIMEOUT);
        isTimeOutSet = true;
        console.log('Timeout set.');
    }
}, 1);