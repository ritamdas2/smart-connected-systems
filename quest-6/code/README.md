# Code Readme

#### toastESP.c

#### secondaryESP.c

This code uses the garmin v4 LIDAR to check if there is "bread in the toaster". In actual terms, it checks to see if there is an object within a certain distance (20 cm). If it does detect something within the threshold, it will first print that "toast is detected", and then, signal the servo motor to turn to "close the toaster door". The LIDAR distance readings will also be printed out to the console for the user to see. I added a very long task delay to turn the servo the other way to signal that the toast is ready. Otherwise, it will keep the door as is, and just print to console that there is "no toast detected".

#### nodeserver.js

#### index.html

This file is the front end of our smart toaster. Its functionalities include starting and stopping/resetting the toaster timer, displaying real-time webcam feed of the toaster, and graphing temperature v. time (of the toast). The design includes two buttons- Start and Stop, and when pressed, the start button will send a signal to the node-server which will send a signal to the ESP to start the "toasting process" as indicated by the timer. A similar functionality exists for the stop button which will reset the timer to signal the esp.

Underneath the buttons, there is the live video feed of the raspberry pi which was taken from a previous skill.

Lastly, there is one graph that displays the toast temperature v. time (s). This reads a csv file that was generated from the thermistor readings from the node server. Our graph was done using canvas js with the code adopted from the Fish Feeder quest.
