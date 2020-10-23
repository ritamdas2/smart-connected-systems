# Code Readme

## sensorCollection.c - Ritam Das
The ESP32 reads from two different sensors (the thermistor and accelerometer) and outputs to one LED based on led intensity inputted by user on the node server. Code was adapted from skill 13 (thermistor), skill 23 (accelerometer), and skill 24 (PWM to control LED). The section of the file regarding the thermistor converted an adc voltage reading to temperature measured in celsius. The accelerometer invloved multiple functions: one to read from an 8-bit register, one to write to an 8-bit register, one to read 16 bits, two separate functions to calculate acceleration values in the X, Y, and Z directions as well as roll and pitch values. The LED used a PWM cycle which allows the user to set their desired LED intensity on a scale of 0-9 from the web client. The relevant outputs of each function were then printed in csv format from main.



