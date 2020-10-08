# Quest 2: Tactile Internet (sensors)

Authors: Brian Macomber, Ritam Das, Raghurama Bukkarayasamudram

## Date: 2020-10-08

## Summary

We wired three different sensors to the ESP32: a thermistor, an ultrasonic sensor, and an IR rangefinder. Using separate ADC (analog-to-digital converter) channels, the ESP32 was able to read the voltage. This voltage reading was converted to the appropriate engineering units for each sensor. These readings were written to the serial port and saved into a CSV file. We created a localhost endpoint, where the latest readings from the CSV file were converted to JSON and plotted using canvasjs and nodejs. 

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

### Wiring
The thermistor was wired through voltage divider with a 10kohm resistor to step down the voltage from the USB 5V input. The ultrasonic and IR rangefinder sensors were wired directly to 3V and 5V and outputs were sent to adc pins.

        //thermistor monitor adc
        static const adc_channel_t channel2 = ADC_CHANNEL_4; //GPIO32
        //ultrasonic monitor adc
        static const adc_channel_t channel3 = ADC_CHANNEL_5; //GPIO33
        //rangefinder monitor adc
        static const adc_channel_t channel4 = ADC_CHANNEL_6; //GPIO34

### Sensor Code
We defined four functions: one for each sensor and a display_console function to call each sensor function and display the latest readings in an easy to read format. 

In the thermistor function, we solved for the thermistor resistance using the voltage divider formula and plugging in our respective values. We then solved for the temperature using the previously calculated values from voltage and resistance. Lastly, the tempKelvin was converted to Celsius for final output.

In the ultrasonic and IR rangefinder functions we converted the voltage to distance in cm using a formula generated from linear fitting the provided data sheets. The IR rangefinder had a functional range of 20cm to 150cm, so we capped it using that range. 

display_console called each sensor function and printed outputs in CSV format. 

### Node.js & Canvas.js

### Investigative Question

## Sketches and Photos

<center><img src="./images/ece444.png" width="25%" /></center>  
<center> </center>

## Supporting Artifacts

- [Link to video demo](). Not to exceed 120s

## Modules, Tools, Source Used Including Attribution

## References

esp-idf/examples/peripherals/adc

https://canvasjs.com/javascript-charts/

https://www.w3schools.com/nodejs/default.asp

---
