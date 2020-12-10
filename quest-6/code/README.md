# Code Readme

#### toastESP.c

#### secondaryESP.c

This code uses the garmin v4 LIDAR to check if there is "bread in the toaster". In actual terms, it checks to see if there is an object within
a certain distance. If it does detect something within the threshold, it will signal the servo motor to turn to "close the toaster door". I added a very long task delay to turn the other way to signal that the toast is ready. Otherwise, it will keep the door as is.

#### nodeserver.js

#### index.html
