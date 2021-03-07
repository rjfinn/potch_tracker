var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');  
const { RSA_PKCS1_PADDING } = require('constants');
var urlencodedParser = bodyParser.urlencoded({ extended: false });
var nconf = require('nconf');

var app = express();

var readings = {};  // to store the readings (value) by MAC address (key) in a key/value pair dictionary
var cutoff = -110;  // value for which we assume the tag is out of reach

// object will hold values for the closest reader to the bluetooth tag
var closest = {};

// read in our config file
nconf.file({ file: 'config.json' });

app.set('port', nconf.get('defaultPort') || process.env.PORT || 3000);

// HTTP GET /room function to get the name of the closest room
app.get('/room', function (req, res) {
    // set a default status of "nowhere"
    closest = {
        room: "Nowhere",
        rssi: cutoff
    };
    // loop through each reading in the rssi object
    Object.keys(readings).forEach(function(key) {
        console.log("check for "+key)
        var reading = readings[key];
        console.log("compare "+reading+" to "+closest.rssi);
        if(Number(reading) >= Number(closest.rssi)) {
            console.log(key+" is closer");
            closest.room = key;
            closest.rssi = reading;
        }
    });
    res.end(closest.room);
});

// HTTP POST /rssi for readers to post their signal strength to the BT tag
app.post('/rssi', urlencodedParser, function(req, res) {
    readings[req.body.mac] = req.body.rssi;
    //console.log("mac: "+req.body.mac+", rssi: "+req.body.rssi);
    res.end("saved for "+req.body.mac);
});

// HTTP GET /rssi/{key}
// if the key is the MAC address, though, this will require the user to know the
// MAC address.  Another possibilitiy is to loop through the readings searching for
// the room name
app.get('/rssi/:id', function (req, res) {
    if(readings[req.params.id]) {
        res.end(rssi[req.params.id]);
    } else {
        res.end("no such room");
    }
});

// start the HTTP server on the specified port
http.createServer(app).listen(app.get('port'), function(){
    console.log('Express server listening on port ' + app.get('port'));
});


