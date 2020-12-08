# Quest 6: Toaster

Authors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber

## Date: 2020-12-04

## Summary

For this quest, we had the option to create our own project. We decided to do a smart toaster as our smart and connected system. Due to the limited electronics available, our smart toaster will only mimic the functionality of one. For the actuators, we aim to use the servo as a toaster door and the i2c to display the time left. Regarding the sensors, we aim to use a thermistor to collect the temperature data of the toast, IR to detect if "bread is on the toaster", and a button to start the timer in person. On the web client, there will be the option to remotely start the toast timer and show the data of temperature v. time while also periodically showing images of the toast to show "progression".

One main ESP-32 (toast ESP) will be used to host the node server, "toast", show the toast timer, and control the servo. The second ESP-32 (start ESP) will be used exclusively to check for physical button input by which the data will be sent to toast ESP through WiFi to start the "toasting process".

Requirements: 
- Something meaningful: Smart Toaster
- 2 Actuators: button, servo, or I2c
- 3 Sensors: thermistor, IR, button
- 1 Camera: pi webcam
- Remote Control & Data presentation: temperature, image (toast progression) v. time
- Time based function: Timer needed for toast
- Multiple ESPs: toast ESP & start ESP (check for button thru remote control to start toasting)

## Self-Assessment

### Objective Criteria

| Objective Criterion | Rating | Max Value |
| ------------------- | :----: | :-------: |
| Objective One       |        |     1     |
| Objective Two       |        |     1     |
| Objective Three     |        |     1     |
| Objective Four      |        |     1     |
| Objective Five      |        |     1     |
| Objective Six       |        |     1     |
| Objective Seven     |        |     1     |

### Qualitative Criteria

| Qualitative Criterion                          | Rating | Max Value |
| ---------------------------------------------- | :----: | :-------: |
| Quality of solution                            |        |     5     |
| Quality of report.md including use of graphics |        |     3     |
| Quality of code reporting                      |        |     3     |
| Quality of video presentation                  |        |     3     |

## Solution Design

## Sketches and Photos

<center><img src="./images/ece444.png" width="25%" /></center>  
<center> </center>

## Supporting Artifacts

- [Link to video demo](). Not to exceed 120s

## Modules, Tools, Source Used Including Attribution

## References

---
