/* 
Contributors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber
Date: 11/13/2020
Quest 4 - E-Voting v2
*/

// Here is code for the nodejs server:
// - create webclient
// - send and recieve messages from webclient (using socket.io)
// - Create Database
// - Add entries (votes) to database once recieved from ESP

//We're going to addapt code from an example that Matt gave:
// Modules
var level = require("level");
var express = require("express");
var app = require("express")();
var http = require("http").Server(app);
var io = require("socket.io")(http);

/////////////////////////////////////////
// UDP comms with Leader ESP
/////////////////////////////////////////
var dgram = require("dgram");
// Port and IP
var PORT = 1131;
var HOST = "192.168.4.25"; //ip of my laptop

// Create socket
var server = dgram.createSocket("udp4");

// Create server
server.on("listening", function () {
  var address = server.address();
  console.log(
    "UDP Server listening on " + address.address + ":" + address.port
  );
});

// Create or open the underlying LevelDB store
var db = level("./mydb", { valueEncoding: "json" });

// On connection, print out received message
server.on("message", function (message, remote) {
  console.log(remote.address + ":" + remote.port + " - " + message);

  // Get current time
  var date = Date.now();

  // Fill in data structure
  var value = [{ id: message[0], vote: message[3] }];

  // Here we want to add the message (vote) to the database
  db.put([date], value, function (err) {
    if (err) return console.log("Ooops!", err); // some kind of I/O error
  });
      // Parse data to send to client
      var msg = { [date]: value };

      // Send to client
      io.emit("message", msg);
  
      // Log to console
      console.log(Object.keys(msg));


  // Send Ok acknowledgement
  server.send("got it", remote.port, remote.address, function (error) {
    if (error) {
      console.log("MEH!");
    } else {
      console.log("Sent: Ok!");
    }
  });
});

// Bind server to port and IP
server.bind(PORT, HOST);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// Points to index.html to serve webpage
app.get("/", function (req, res) {
  res.sendFile(__dirname + "/index.html");
});

// Function to stream from database
function readDB(arg) {
  db.createReadStream()
    .on("data", function (data) {
      console.log(data.key, "=", data.value);
      // Parsed the data into a structure but don't have to ...
      var dataIn = { [data.key]: data.value };
      // Stream data to client
      io.emit("message", dataIn);
    })
    .on("error", function (err) {
      console.log("Oh my!", err);
    })
    .on("close", function () {
      console.log("Stream closed");
    })
    .on("end", function () {
      console.log("Stream ended");
    });
}

// When a new client connects
var clientConnected = 0; // this is just to ensure no new data is recorded during streaming
io.on("connection", function (socket) {
  console.log("a user connected");
  clientConnected = 0;

  // Call function to stream database data
  readDB();
  clientConnected = 1;
  socket.on("disconnect", function () {
    console.log("user disconnected");
  });
});

// Listening on localhost:3000
http.listen(3000, function () {
  console.log("listening on *:3000");
});

// *********************************** //
// We probably won't use this
// *********************************** //

// Every 15 seconds, write random information
function intervalFunc() {
  if (clientConnected == 1) {
    // Get current time
    var date = Date.now();

    // Fill in data structure
    var value = [{ id: 1, temp: getRndInteger(30, 80) }];

    // Put structure into database based on key == date, and value
    db.put([date], value, function (err) {
      if (err) return console.log("Ooops!", err); // some kind of I/O error
    });

    // Parse data to send to client
    var msg = { [date]: value };

    // Send to client
    io.emit("message", msg);

    // Log to console
    console.log(Object.keys(msg));
  }
}
// Do every 1500 ms
setInterval(intervalFunc, 1500);
