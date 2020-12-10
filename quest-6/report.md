# Quest 6: Smart Toaster

Authors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber

## Date: 2020-12-04

## Summary

For this quest, we had the option to create our own project. We decided to do a smart toaster as our smart and connected system. Due to the limited electronics available, our smart toaster will only mimic the functionality of one. For the actuators, we aim to use the servo to operate toaster door and the i2c to display the time left to toast. Regarding the sensors, we aim to use a thermistor to collect the temperature data of the toast, LIDAR to detect if "bread is in the toaster", and buttons to start/stop the timer in person. On the web client, hosted by the node server, there will be the option to remotely start/stop the toast timer and show the data of temperature v. time while also periodically showing images of the toast to show "progression".

One main ESP-32 (toast ESP) will be used to show/start/stop the toast timer and output temperature readings. It will also check for button presses by which it will start/stop toasting. The secondary ESP-32 will receive a flag from the main ESP32 and check if there is bread in the toaster. Based on these two chonditions, the toaster may start/stop. Relevant info will be relayed back and forth amongst the ESP32s and node server for frontend visualization.

Requirements: 
- Something meaningful: Smart Toaster
- 2 Actuators: servo, alphanumeric display
- 3 Sensors: thermistor, LIDAR, button
- 1 Camera: pi webcam
- Remote Control & Data presentation: temperature, image (toast progression) v. time
- Time based function: Timer needed for toast
- Multiple ESPs: toast ESP & start ESP (check for button thru remote control & check for bread to start toasting)

## Self-Assessment

### Objective Criteria

|       Objective Criterion       | Rating | Max Value |
| ------------------------------- | :----: | :-------: |
| 2 actuators                     |    1   |     1     |
| 3 sensors                       |    1   |     1     |
| 1 camera                        |    1   |     1     |
| Remote control & Data pres.     |    1   |     1     |
| Timer based function            |    1   |     1     |
| Multiple ESP32s                 |    1   |     1     |
| Objective Seven                 |    1   |     1     |

### Qualitative Criteria

| Qualitative Criterion                          | Rating | Max Value |
| ---------------------------------------------- | :----: | :-------: |
| Quality of solution                            |        |     5     |
| Quality of report.md including use of graphics |        |     3     |
| Quality of code reporting                      |        |     3     |
| Quality of video presentation                  |        |     3     |

## Solution Design

### Embedded System
There are two ESP32s. One main ESP32 waiting on user input through Stop/Start buttons will start a toast timer, which is displayed on an alphanumeric display over i2c. The main ESP32 will also read temperature every second upon start. There is also a secondary ESP32 waiting on a flag from the main ESP32 and checking to see if bread is in the toaster with a LIDAR. Based on these two conditions, door will close by way of the servo and toasting will begin. The wiring is as shown in figure 2. Relevant skills with necessary adaptations included thermistor,i2c_display, stopwatch, LIDAR, and servo. 

### UDP Communication
BRIAN

### Node Server & Web Client
The node server creates the web client locally on port 3000. On here it creates the '/' endpoint where it renders index.html, the '/data' endpoint where it streams data from the test_data.csv file. In addition to this, it creates a socket on local port 1131 to talk to the ESP-32s through UDP. It receives the formatted temperature data. This temperature data is displayed on the web client as a function of time. The web client also displays video footage from the Pi webcam to show toast progression. 


## Sketches and Photos

<center><img src="./images/ece444.png" width="25%" /></center>  
<center> </center>
Figure 1:

![Screen Shot 2020-12-08 at 2 58 18 PM](https://user-images.githubusercontent.com/37518854/101534954-e4eab800-3965-11eb-8372-56eaa3a5ecab.png)

Figure 2:

BRIAN INSERT FINAL HARDWARE PIC

Figure 3: 

RITAM INSERT GRAPHIC 

## Supporting Artifacts

- [Link to video demo](). Not to exceed 120s

## Modules, Tools, Source Used Including Attribution

## References

---
