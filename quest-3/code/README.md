# Code Readme

## sensorCollection.c - Ritam
The ESP32 reads from two different sensors (a thermistor and accelerometer) and outputs to an LED based on the led intensity inputted by the user on the web client. Code was adapted from skill 13 (thermistor), skill 23 (accelerometer), and skill 24 (PWM to control LED). The section of the file regarding the thermistor converted an adc voltage reading to temperature measured in celsius. The accelerometer invloved multiple functions: one to read from an 8-bit register, one to write to an 8-bit register, one to read 16 bits, two separate functions to calculate acceleration values in the X, Y, and Z directions as well as roll and pitch values. The LED used a PWM cycle which allows the user to set their desired LED intensity on a scale of 0-9 from the web client. The relevant outputs of each function were then printed in csv format from main.


## server.js - Brian

## index.html - Ram

## test_data.csv - ??
