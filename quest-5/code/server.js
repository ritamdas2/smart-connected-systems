/* 
Contributors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber
Date: 12/06/2020
Quest 5 - Cruise Control
*/

// Here is code for the nodejs server:
// - create webclient
// - send and recieve messages from webclient (using socket.io)
// - Create Database
// - Add entries (votes) to database once recieved from ESP

//We're going to addapt code from an example that Matt gave:
/////////////////////////////////////////////////////////
//                      Modules                       //
/////////////////////////////////////////////////////////

var level = require("level");
var express = require("express");
var app = require("express")();
var http = require("http").Server(app);
var io = require("socket.io")(http);
var rimraf = require("rimraf");

/////////////////////////////////////////
// UDP comms with Leader ESP

var dgram = require("dgram");
// Port and IP
var PORT = 1131;
var HOST = "192.168.7.196"; //ip of my laptop

// Create socket
var server = dgram.createSocket("udp4");

// Create server
server.on("listening", function () {
  var address = server.address();
  console.log(
    "UDP Server listening on " + address.address + ":" + address.port
  );
});

// On connection, print out received message
server.on("message", function (message, remote) {
  console.log(remote.address + ":" + remote.port + " - " + message);

  // Get current time
  var date = Date.now();

  // Fill in data structure
  var value = [
    {
      id: String.fromCharCode(message[0]),
      vote: String.fromCharCode(message[2]),
    },
  ];
  
});

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

// Points to index.html to serve webpage
app.get("/", function (req, res) {
  res.sendFile(__dirname + "/index.html");
});

// When a new client connects

io.on("connection", function (socket) {
  console.log("a user connected");

  // socket.on("flag", function (data) {
  //   console.log("START");
  // });

  socket.on("disconnect", function () {
    console.log("user disconnected");
  });
});

//get message from front end
io.on("message", function (msg) {
  console.log("recieved msg: " + msg);
  //delete database here, then create new one
  rimraf("./mydb", function () {
    console.log("done");
  });


});

///////////////////////////////////////
//          MAIN                    //
///////////////////////////////////////
// Bind server to port and IP
server.bind(PORT, HOST);

// Listening on localhost:3000
http.listen(3000, function () {
  console.log("listening on *:3000");
});
