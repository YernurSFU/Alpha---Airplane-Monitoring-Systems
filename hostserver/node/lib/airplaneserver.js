var socketio = require('socket.io'),
    io,
    io2 = require('socket.io-client'),
    fs = require("fs"),
    path = require('path'),
    jsonContent;


exports.listen = function (server) {
    var originalPath = process.cwd(),
        dirString = path.dirname(fs.realpathSync(__filename));
    process.chdir(dirString);
    var contents = fs.readFileSync("locations.json");
    jsonContent = JSON.parse(contents);
    process.chdir(originalPath);

    io = socketio.listen(server);
    io.set('log level', 0);

    io.sockets.on('connection', function (socket) {
        handleCommand(socket);
    });
};

function handleCommand(socket) {
    socket.on('GET_UPDATES', function (data) {
        var addr = jsonContent[data['name']],
            socket2 = io2.connect(addr);

        socket2.on('connect', function () {
            console.log('Connected with Beaglebone server on: "' + addr + '".');
            socket.emit('BB_CONNECTION_SUCCESS');
            socket2.on('GET_UPDATES_SUCCESS', function (data) {
                socket.emit('GET_UPDATES_SUCCESS', data);
            });
            socket2.on('GET_UPDATES_FAIL', function () {
                socket.emit('GET_UPDATES_FAIL');
            });
            socket2.emit('GET_UPDATES');
        });
        socket2.on('disconnect', function () {
            console.log('Cannot connect to Beaglebone server on: "' + addr + '"!');
            socket.emit('BB_CONNECTION_FAIL');
        });
        socket2.on('reconnect_error', function () {
            console.log('Cannot connect to Beaglebone server on: "' + addr + '"!');
            socket.emit('BB_CONNECTION_FAIL');
        });
        socket2.on('reconnect', function () {
            console.log('Connected with Beaglebone server on: "' + addr + '".');
            socket.emit('BB_CONNECTION_SUCCESS');
        });
    });

    socket.on('GET_AIRPLANES_LIST', function () {
        socket.emit('AIRPLANES_LIST', jsonContent);
    });
};