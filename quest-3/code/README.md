# Code Readme

## sensorCollection.c - Ritam

The ESP32 reads from two different sensors (a thermistor and accelerometer) and outputs to an LED based on the led intensity inputted by the user on the web client. Code was adapted from skill 13 (thermistor), skill 23 (accelerometer), and skill 24 (PWM to control LED). The section of the file regarding the thermistor converted an adc voltage reading to temperature measured in celsius. The accelerometer invloved multiple functions: one to read from an 8-bit register, one to write to an 8-bit register, one to read 16 bits, two separate functions to calculate acceleration values in the X, Y, and Z directions as well as roll and pitch values. The LED used a PWM cycle which allows the user to set their desired LED intensity on a scale of 0-9 from the web client. The relevant outputs of each function were then printed in csv format from main.

## server.js - Brian

## index.html - Ram

The index.html file deals with the front-end aspect of the hurricane box. Using a similar outline and model as the previous quest with minor changes to accommodate the necessary requirements from this quest. Firstly, in the front-end portion, I adapted similar graphing methods as from our previous quest. I just had to overlay two different sensors on top of the thermistor over time since previous skill was also based in real-time. I included code in order to generate two more graphs from the accelerometer for better viewing. After this aspect, the next step was to ensure that there was a way to select the intensity of the LED. This also required having to deal with some backend aspect since we incorporated a form that just uses text input in order to communicate a numeric value. When selected, it would dim or brighten the led. The front-end portion also dealt with displaying the raspberry pi webcam video. We were not able to properly get webcam video and the graphs to display on one site since the graph would be "refreshed" in real time while the webcam video would be displaying a live video of the led.

## test_data.csv - ??
