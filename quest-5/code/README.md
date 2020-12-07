# Code Readme

Please describe what is in your code folder and subfolders. Make it
easy for us to navigate this space.

Also

- Please provide your name and date in any code submitted
- Indicate attributrion for any code you have adopted from elsewhere

# index.html

For index.html, we went with a very basic layout that displays our frontend properly. It showcases our name, the quest, and two buttons to indicate START and STOP. I use socket.io in order to communicate from the client to the server. With each button, there are two onclick events that are triggered. On pressing "START", the START() function will be called which sets the flag to 1, and emits a "START" message via socket.emit. Likewise, "STOP" will emit a "STOP" message via socket.emit to the console log on the server.js file. This successfully proves web client to server communication to control the car.

# server.js

This file creates and hosts a server through UDP connection and node.js. This was taken from our code from last quest and altered in order to fit the requirements. Upon establishing a successful connection, the server will receive any messages from the client. In this case, it will receive a START or STOP message that will be outputted to the console.log. Server.js will also be responsible for using the console.log output after receiving the message from the client to communicate with the crawler device to start and stop moving.
