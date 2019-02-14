var socket = io.connect(),
    errorStatus = null,
    lastAcceptedResponse = null,
    ACCEPTED_RESPONSE_TIME_WINDOW = 2000;


function getUpdates() {
    if (errorStatus == 'SERVER') {
        showError('SERVER');
    } else if (errorStatus == 'BB_SERVER' && $('#selected-airplane')[0].attributes['selected'].value != '0') {
        showError('BB_SERVER');
    } else if (errorStatus == 'HARDWARE' && $('#selected-airplane')[0].attributes['selected'].value != '0') {
        showError('HARDWARE');
    } else if (errorStatus == null && (lastAcceptedResponse == null ||
            (((new Date).getTime()) - lastAcceptedResponse) > ACCEPTED_RESPONSE_TIME_WINDOW) &&
        $('#selected-airplane')[0].attributes['selected'].value != '0') {
    } else if (errorStatus == null && $('#selected-airplane')[0].attributes['selected'].value != '0') {
        showContent();
    } else {
        hideAll();
    }
    console.log((((new Date).getTime()) - lastAcceptedResponse));
    if ($('#selected-airplane')[0].attributes['selected'].value != '0') {
        socket.emit('GET_UPDATES', {
            name: $('#selected-airplane').html()
        });
    }
}

function onAirplaneSelect(airplaneName, idx) {
    $('#selected-airplane').html(airplaneName);
    $('#selected-airplane')[0].attributes['selected'].value = idx;
}

function setAirplanes(data) {
    var idx = 1;
    for (airplaneName in data) {
        $('#airplane-list-drop-down').append(
            '<li role="presentation"><a role="menuitem" tabindex="-1" href="javascript:void(0)" onclick="onAirplaneSelect(\'' + $('<div />').html(airplaneName).html() + '\', \'' + idx + '\')">' + $('<div />').html(airplaneName).html() + '</a></li>'
        );
    }
}

function setServerErrorText() {
    $('#error-title').html('Server Error');
    $('#error-subtitle').html('Server error detected. Please resolve error before continuing.');
    $('#error-text').html('SERVER ERROR: No response from the airplane application. Is it running?');
}

function setHardwareErrorText() {
    $('#error-title').html('Hardware Error');
    $('#error-subtitle').html('Hardware error detected. Please resolve error before continuing.');
    $('#error-text').html('HARDWARE ERROR: Cannot communicate with the hardware interface of the airplane on server.');
}

function setBeagleboneServerErrorText() {
    $('#error-title').html('Beaglebone Server Error');
    $('#error-subtitle').html('Beaglebone server error detected. Please resolve error before continuing.');
    $('#error-text').html('BEAGLEBONE SERVER ERROR: No response from the Beaglebone server. Is it running?');
}

function showUpdating() {
    $('#error-message').hide();
    $('#updating-message').show();
    $('#content').hide();
}

function showError(mode) {
    $('#error-message').hide();
    $('#updating-message').hide();
    $('#content').hide();

    if (mode == 'SERVER') {
        setServerErrorText();
    } else if (mode == 'HARDWARE') {
        setHardwareErrorText();
    } else if (mode == 'BB_SERVER') {
        setBeagleboneServerErrorText();
    }
    $('#error-message').show();
}

function showContent() {
    $('#error-message').hide();
    $('#updating-message').hide();
    $('#content').show();
}

function hideAll() {
    $('#error-message').hide();
    $('#updating-message').hide();
    $('#content').hide();
}

function onDisconnect() {
    errorStatus = 'SERVER';
}

function onConnect() {
    errorStatus = null;
}

function onHardwareError() {
    errorStatus = 'HARDWARE';
}

function onBBServerError() {
    errorStatus = 'BB_SERVER';
}

function onUpdate(data) {
    errorStatus = null;

    var direction;
    if (data['positionTrace'][data['positionTrace'].length - 1] == "1") {
        direction = "up";
    } else if (data['positionTrace'][data['positionTrace'].length - 1] == "0") {
        direction = "down";
    } else {
        direction = "stopped"
    }
    var doorState = data['doorStateTrace'][data['doorStateTrace'].length - 1] == '1' ? 'Closed' : 'Open';
    $('#temperature-val').html($('<div />').text(data['temperatureTrace'][data['temperatureTrace'].length - 1] + '°C').html());
    $('#humidity-val').html($('<div />').text(data['humidityTrace'][data['humidityTrace'].length - 1] + '%').html());
    $('#position-val').html($('<div />').text(data['positionTrace'][data['positionTrace'].length - 1] + 'm').html());
    $('#door-state-val').html($('<div />').text(doorState).html());
    $('#pressure-val').html($('<div />').text(data['pressureTrace'][data['pressureTrace'].length - 1] + 'J/(m^3)').html());
    $('#direction').html($('<div />').text(direction).html());
    $('#system-uptime-val').html($('<div />').text(data['systemUptimeTrace'][data['systemUptimeTrace'].length - 1] + '(H:M:S)').html());

    var temperatureTrace = {
        x: data['serverTimeMSTrace'],
        y: data['temperatureTrace'],
        type: 'scatter',
        name: 'Temperature Trace'
    };

    var humidityTrace = {
        x: data['serverTimeMSTrace'],
        y: data['humidityTrace'],
        type: 'scatter',
        name: 'Humidity Trace'
    };

    var positionTrace = {
        x: data['serverTimeMSTrace'],
        y: data['positionTrace'],
        type: 'scatter',
        name: 'Position Trace'
    };

    var pressureTrace = {
        x: data['serverTimeMSTrace'],
        y: data['pressureTrace'],
        type: 'scatter',
        name: 'Pressure Trace'
    };

    var plotData = [
        temperatureTrace,
        humidityTrace,
        positionTrace,
        pressureTrace
    ];

    Plotly.newPlot('plot', plotData);

    lastAcceptedResponse = (new Date).getTime();
}

$(document).ready(function () {
    socket.on('disconnect', onDisconnect);
    socket.on('GET_UPDATES_FAIL', onHardwareError);
    socket.on('BB_CONNECTION_FAIL', onBBServerError);
    socket.on('connect', onConnect);
    socket.on('AIRPLANES_LIST', setAirplanes);
    socket.emit('GET_AIRPLANES_LIST');
    socket.on('BB_CONNECTION_SUCCESS', onConnect);
    socket.on('GET_UPDATES_SUCCESS', onUpdate);
    setInterval(getUpdates, 500);
});