var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');
const { RSA_PKCS1_PADDING } = require('constants');
var urlencodedParser = bodyParser.urlencoded({ extended: false });
const fs = require('fs');

var app = express();

var readings = {};  // to store the readings (value) by MAC address (key) in a key/value pair dictionary
var cutoff = -99;  // value for which we assume the tag is out of reach
var ttl = 30000; // time-to-live of readings (in milliseconds)

// object will hold values for the closest reader to the bluetooth tag
var closest = {};

let rawdata = fs.readFileSync('config.json');
var config = JSON.parse(rawdata);
console.log(config);

app.set('port', config.defaultPort || process.env.PORT || 3000);

app.use('/ui', express.static('public'));

// HTTP GET /room function to get the name of the closest room
app.get('/room', function (req, res) {
    res.end(JSON.stringify(closest));
});

app.get('/tagAddress', function (req, res) {
    res.end(config.tagAddress);
});

function initClosest(now) {
    closest = {
        room: "Nowhere",
        mac: "",
        rssi: cutoff,
        timestamp: now
    }
}

async function findClosest() {
    var now = Date.now();
    var too_old = now - ttl;
    // set a default status of "nowhere"
    initClosest(now);
    // loop through each reading in the rssi object
    Object.keys(readings).forEach(function (key) {
        //console.log("check for " + key)
        if (readings[key].rssi < cutoff) {
            console.log(key + " is too far away");
            delete readings[key];
        } else if (readings[key].timestamp > too_old) {
            var reading = readings[key].rssi;

            // should both be numbers at this point
            //console.log("compare " + reading + " to " + closest.rssi);
            if (reading >= closest.rssi) {
                // console.log(key + " is closer");
                if(closest.hasOwnProperty('room') && closest.room.hasOwnProperty(key)) {
                    closest.room = config.rooms[key];
                } else {
                    closest.room = key;
                }
                closest.mac = key;
                closest.rssi = reading;
                closest.timestamp = readings[key].timestamp;
            }
        } else {
            console.log(key + " is too old");
            delete readings[key];
        }
    });
}

// HTTP POST /rssi for readers to post their signal strength to the BT tag
app.post('/rssi', urlencodedParser, function (req, res) {
    var rssi = Number(req.body.rssi);  // make sure saved as a number instead of a string

    if(!readings[req.body.mac]) {
        readings[req.body.mac] = {
            "max": rssi,
            "min": rssi
        }
    }

    readings[req.body.mac].rssi = rssi;
    readings[req.body.mac].timestamp = Date.now();
    if(config.hasOwnProperty('rooms') && config.rooms.hasOwnProperty(req.body.mac)) {
        readings[req.body.mac].room = config.rooms[req.body.mac];
    } else {
        readings[req.body.mac].room = String(req.body.mac);
    }

    if (rssi > readings[req.body.mac].max) {
        readings[req.body.mac].max = rssi;
    }
    if (rssi < readings[req.body.mac].min) {
        readings[req.body.mac].min = rssi;
    }

    findClosest();  // asynchronous call (like multitasking) to update closest room
    res.end("saved for " + req.body.mac);  // return while findClosest still running
});

// HTTP GET /rssi/{key}
// if the key is the MAC address, though, this will require the user to know the
// MAC address.  Another possibilitiy is to loop through the readings searching for
// the room name
app.get('/rssi/:id', function (req, res) {
    if (readings[req.params.id]) {
        res.end(readings[req.params.id].rssi);
    } else {
        res.end("no such room");
    }
});

app.get('/reading/:id', function (req, res) {
    if (readings[req.params.id]) {
        res.end(JSON.stringify(readings[req.params.id]));
    } else {
        res.end(JSON.stringify());
    }
});

app.get('/readings', function (req, res) {
    res.end(JSON.stringify(readings));
});

// start the HTTP server on the specified port
http.createServer(app).listen(app.get('port'), function () {
    console.log('Express server listening on port ' + app.get('port'));
});

// automatically run the findClosest function after the time-to-live
// in case no readers are finding the tag
setInterval(findClosest, ttl + 1);

initClosest(Date.now());