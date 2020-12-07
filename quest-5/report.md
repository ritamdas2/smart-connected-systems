# Quest Cruise Control

Authors: Ritam Das, Brian Macomber, and Raghurama Bukkarayasamudram
Date: 2020-12-6

---

## Summary

This quest tasked us with adding autonomous capabilties to a crawler vehicle. We also implemented steering control and speed control. We used one lidar to attain distance and a pulse counter to track wheel speed. There is also an alphanumeric display to show realtime speed of the crawler in m/s. The crawler can also be stopped and started from a web client that uses socket.io to send data from the client to the server which will signal the crawler.

### Objective Criteria

| Objective Criterion                 | Rating | Max Value |
| ------------------------------------| :----: | :-------: |
| Remote Start/Stop                   |   1    |     1     |
| Stops before collision              |   1    |     1     |
| PID for speed control               |   0    |     1     |
| Measures wheel speed                |   1    |     1     |
| Displays speed on the alpha display |   1    |     1     |
| Sucessfully traverse A to B         |   1    |     1     |
| Controls Steering                   |   0    |     1     |

### Qualitative Criteria

| Qualitative Criterion                          | Rating | Max Value |
| ---------------------------------------------- | :----: | :-------: |
| Quality of solution                            |    3   |     5     |
| Quality of report.md including use of graphics |    3   |     3     |
| Quality of code reporting                      |    3   |     3     |
| Quality of video presentation                  |    3   |     3     |

## Solution Design

An optical encoder and black and white pinwheel was mounted to the vehicle for the pulse counter. It was mounted at the rear of the vehicle. The Lidar and display used i2c. The main components to this vehicle were the ESP32, ESC, Lidar, alphanumeric display, steering servo, motors, 7.2V battery for the car, and 5V batteery for the ESP32. This can be seen in Figure 1 with a diagram and figure 2 with the actual crawler.

BRIAN INSERT SENSOR/SPEED/STEERING stuff

A node server was connected to an ESP32 over UDP socket communication on our local network. The web client allowed for start and stop functionality of the crawler. A boolean variable is set to either true or false permitting the car to either drive or stop.

## Investigative Question

### How would you change your solution if you were asked to provide ‘adaptive’ cruise control?

We would have the lidar mounted on the front to read distance and judge the speed and acceleration relative to the crawler through a series of calculations and mathematical manipulations. The crawler would then slow down to match or go just below that measured speed. We could also use multiple lidars and use a decision algorithm to determine good lidar readings, which would result in better speed and acceleration readings for adaptive capabilities.

## Sketches and Photos

<center><img src="./images/ece444.png" width="25%" /></center>  
<center> </center>
Figure 1:

![Screen Shot 2020-12-07 at 1 54 36 PM](https://user-images.githubusercontent.com/37518854/101392436-c61bf100-3893-11eb-80da-895cb1f70fde.png)


BRIAN INSERT HARDWARE/CRAWLER PICS FIGURE 2 HARDWARE

## Supporting Artifacts

- [Link to video demo](). Not to exceed 120s

## Modules, Tools, Source Used Including Attribution

## References

- Socket.io: https://socket.io/docs/v3/index.html
- Level & Nodejs: https://www.npmjs.com/package/node-leveldb
- Level: https://github.com/google/leveldb/blob/master/doc/index.md
- HTML Button: https://www.w3schools.com/tags/tryit.asp?filename=tryhtml_button_css
- Crawler speed:

---
