// Here is where all of the front end stuff will happen
//- Create socket with UDP to talk to esp (recieve sensor data / send intensity data)
//- Create web page (can take code from prior skill, change port number)

// --------------- code from whizzer for UDP node server
// Required module
var dgram = require("dgram");

// Port and IP
var PORT = 3333;
var HOST = "192.168.1.105";

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

  // Send Ok acknowledgement
  server.send("Ok!", remote.port, remote.address, function (error) {
    if (error) {
      console.log("MEH!");
    } else {
      console.log("Sent: Ok!");
    }
  });
});

// Bind server to port and IP
server.bind(PORT, HOST);
