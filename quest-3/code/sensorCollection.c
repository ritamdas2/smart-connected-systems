
/*
Here is where all of the esp modules should be:
- i2c accelerometer module to get accel data (ROLL and PITCH)
- Thermistor module to read temperature
- Update LED intensity
- UDP module that sends data over a socket to the Node.js server
 - send roll, pitch, temperature
 - recieve LED intensity
*/

// ------------------ LED STUFF --------------
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO (12)
#define LEDC_HS_CH0_CHANNEL LEDC_CHANNEL_0

#define LEDC_LS_TIMER LEDC_TIMER_1
#define LEDC_LS_MODE LEDC_LOW_SPEED_MODE

#define LEDC_TEST_CH_NUM (1)
#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (3000)

#define LEDC_STEP_SIZE (250)

int intensity = 0;
// ----------------------------------------------

void LED_task()
{
    int ch = 0;
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_LS_MODE,           // timer mode
        .timer_num = LEDC_LS_TIMER,           // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_HS_MODE;
    ledc_timer.timer_num = LEDC_HS_TIMER;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {.channel = LEDC_HS_CH0_CHANNEL,
         .duty = 0,
         .gpio_num = LEDC_HS_CH0_GPIO,
         .speed_mode = LEDC_HS_MODE,
         .hpoint = 0,
         .timer_sel = LEDC_HS_TIMER},
    };

    // Set LED Controller with previously prepared configuration
    ledc_channel_config(&ledc_channel[ch]);

    // Initialize fade service.
    ledc_fade_func_install(0);

    while (1)
    {
        // here is where we can set the intensity
        // - note the `intensity` global variable that will be assigned from server response
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, LEDC_TEST_DUTY * intensity * 0.1);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
// led task, thermistor task, udp task, accelerometer task
void MAIN()
{

    while (1)
    {
        LED_task();
    }
}

void app_main(void)
{
    xTaskCreate(MAIN, "MAIN", 4096, NULL, 5, NULL);
}
