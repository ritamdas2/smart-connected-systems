/*
Contributors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber
Date: 12/10/2020
Quest 6 - Smart Toaster
*/


/////////////////////////////////////////////////////////
//                      Modules                       //
/////////////////////////////////////////////////////////

var express = require("express");
var app = require("express")();
var http = require("http").Server(app);
var io = require("socket.io")(http);
var dgram = require("dgram");

/////////////////////////////////////////
// UDP comms with Toast ESP

// Port and IP
var PORT = 1131;
var HOST = "192.168.7.196"; //ip of my laptop

var startFlag = false;
var resetFlag = false;

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

    // once message recieved from client, send back appropriate message
    if (startFlag) {
        server.send("Start", remote.port, remote.address, function (error) {
            if (error) {
                console.log("error");
            } else {
                console.log("Start message sent!");
            }
        })
        startFlag = false;
    } else {
        if (resetFlag) {
            server.send("Reset", remote.port, remote.address, function (error) {
                if (error) {
                    console.log("error");
                } else {
                    console.log("Reset message sent!");
                }
            })
            resetFlag = false;
        } else {
            server.send("NULL", remote.port, remote.address, function (error) {
                if (error) {
                    console.log("error");
                } else {
                    console.log("NULL message sent!");
                }
            })
        }
    }
});

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

// Points to index.html to serve webpage
app.get("/", function (req, res) {
    res.sendFile(__dirname + "/index.html");
});

// When a new client connects
//get message from front end
io.on("connection", (socket) => {
    console.log(socket.id);
    console.log("a user connected");

    socket.on("message", (arg) => { // recieved something from frontend
        console.log(arg);
        // set some flag to Start or Reset based on what the button pushed was
        if (arg === "Start") {
            startFlag = true;
        } else {
            if (arg === "Reset") {
                resetFlag = true;
            } else {
                // do nothing
            }
        }
    });

    socket.on("disconnect", function () {
        console.log("user disconnected");
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
