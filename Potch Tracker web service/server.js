var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');  
const { RSA_PKCS1_PADDING } = require('constants');
var urlencodedParser = bodyParser.urlencoded({ extended: false });

var app = express();

var readings = {};  // to store the readings by room

var cutoff = -110;  // value for which we assume the tag is out of reach

var closest = {};

app.set('port',process.env.PORT || 3000);

app.get('/room', function (req, res) {
    // set a default status of "nowhere"
    closest = {
        room: "Nowhere",
        rssi: cutoff
    };
    // loop through each reading in the rssi object
    Object.keys(readings).forEach(function(key) {
        var reading = readings[key];
        if(reading >= closest.rssi) {
            closest.room = key;
            closest.rssi = reading;
        }
    });
    res.end(closest.room);
});

app.post('/rssi', urlencodedParser, function(req, res) {
    readings[req.body.room] = req.body.rssi;
    res.end("saved for "+req.body.room);
});

app.get('/rssi/:id', function (req, res) {
    if(rssi[req.params.id]) {
        res.end(rssi[req.params.id]);
    } else {
        res.end("no such room");
    }
});

http.createServer(app).listen(app.get('port'), function(){
    console.log('Express server listening on port ' + app.get('port'));
});


