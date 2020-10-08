var fs = require("fs");

const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const port = new SerialPort("/COM5", { baudRate: 115200 });
const parse = port.pipe(new Readline({ delimiter: "\n" }));
// Read the port data
port.on("open", () => {
  console.log("serial port open");
});
parse.on("data", (data) => {
  console.log("output from esp:", data);
  fs.appendFile("test_data.csv", data, function (err) {
    if (err) throw err;
    console.log("Saved!");
  });
});
