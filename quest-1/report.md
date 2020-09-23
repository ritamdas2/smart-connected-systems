# Quest Name

Authors: Brian Macomber, Raghurama Bukaarayasamudram, Ritam Das

## Date: 2020-09-21

## Summary

#### Investigative Question

What approach can you use to make setting the time interval dynamic (not hard coded)? Elaborate.

One approach to setting the time interval dynamically would be to allow the user to enter the length of the timer
every time the timer finishes. This way there is no hard coded value for the timer, and the user has freedom to start the new fish food timer whenever they want.
Futhermore, the system could also somehow monitor how much food is exactly dropped in at the last alarm, and calculate the timer until the next food drop based on how much food the fish just recieved.

## Self-Assessment

### Objective Criteria

| Objective Criterion                  | Rating | Max Value |
| ------------------------------------ | :----: | :-------: |
| 1. Must keep track of time           |   1    |     1     |
| 2. Must report time for every second |   1    |     1     |
| 3. Must use a servo                  |   1    |     1     |

### Qualitative Criteria

| Qualitative Criterion                          | Rating | Max Value |
| ---------------------------------------------- | :----: | :-------: |
| Quality of solution                            |        |     5     |
| Quality of report.md including use of graphics |        |     3     |
| Quality of code reporting                      |        |     3     |
| Quality of video presentation                  |        |     3     |

## Solution Design

## Sketches and Photos

## Supporting Artifacts

Video Demonstration:
[![](http://img.youtube.com/vi/HcJgBgHboic/0.jpg)](http://www.youtube.com/watch?v=HcJgBgHboic "Team 3: Fish Feeder")

## Modules, Tools, Source Used Including Attribution

## References

- Timer: https://github.com/espressif/esp-idf/tree/17ac4ba/examples/peripherals/timer_group
- Servo Control: https://github.com/espressif/esp-idf/tree/master/examples/peripherals/mcpwm/mcpwm_servo_control
- I2C Displau: https://github.com/BU-EC444/code-examples/tree/master/i2c-display

---
