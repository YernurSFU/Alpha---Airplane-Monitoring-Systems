"use strict";

var PORT_NUMBER = 8181;


var http = require('http');
var fs   = require('fs');
var path = require('path');
var mime = require('mime');
var sh   = require("shelljs");

// Get the original path.
exports.callerDir = sh.pwd().stdout;

// Change to the main server directory.
process.chdir(path.dirname(__filename));

var server = http.createServer(function(request, response) {
    var filePath = false;

    if (request.url == '/') {
        filePath = 'public/index.html';
    } else {
        filePath = 'public' + request.url;
    }
    
    var absPath = './' + filePath;

    console.log(absPath);
    serveStatic(response, absPath);
});

server.listen(PORT_NUMBER, function() {
    console.log("Server listeneing on port " + PORT_NUMBER);
});

function serveStatic(response, absPath) {
    fs.exists(absPath, function(exists) {
        if (exists) {
            fs.readFile(absPath, function(err, data) {
                if (err) {
                    send404(response);
                } else {
                    sendFile(response, absPath, data);
                }
            });
        } else {
            send404(response);
        }
    });
}

function send404(response) {
    response.writeHead(404, {'Content-Type': 'text/plain'});
    response.write('Error 404: resource not found.');
    response.end();
}

function sendFile(response, filePath, fileContents) {
    response.writeHead(
        200,
        {"content-type": mime.lookup(path.basename(filePath))}
    );
    response.end(fileContents);
}


var airplaneserver = require('./lib/airplaneserver.js');
airplaneserver.listen(server);
