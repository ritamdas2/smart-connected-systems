#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

//You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 mocrosecond
#define SERVO_MIN_PULSEWIDTH 450  //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2100 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 180      //Maximum angle in degree upto which servo can rotate

#define SERVO_GPIO 25

int counter = 60 //1 min for now

    static void
    mcpwm_example_gpio_initialize(void)
{
    printf("initializing mcpwm servo control gpio......\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_GPIO);
}

/**
 * @brief Use this function to calcute pulse width for per degree rotation
 *
 * @param  degree_of_rotation the angle in degree to which servo has to rotate
 *
 * @return
 *     - calculated pulse width
 */
static uint32_t servo_per_degree_init(uint32_t degree_of_rotation) {
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

// Want servo to rotate back and forth 180 degrees

static void rotate_servo() {
    uint32_t angle, count;
    while (1) {
        if (counter == 0) {
            for (int i = 0; i < 3; i++) {
                //servo at degree 0
                count = 0;
                printf("Angle of rotation: %d\n", count);
                angle = servo_per_degree_init(count);
                printf("pulse width: %dus\n", angle);
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
                vTaskDelay(100); //Add delay

                //servo at degree 180
                count = SERVO_MAX_DEGREE;
                printf("Angle of rotation: %d\n", count);
                angle = servo_per_degree_init(count);
                printf("pulse width: %dus\n", angle);
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
                vTaskDelay(100); //Add delay
            }
            //servo at degree 0
            count = 0;
            printf("Angle of rotation: %d\n", count);
            angle = servo_per_degree_init(count);
            printf("pulse width: %dus\n", angle);
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
            vTaskDelay(100); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }
    }
}

void init()
{
    //initialize servo gpio
    mcpwm_example_gpio_initialize();

    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50; //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;     //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;     //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); //Configure PWM0A & PWM0B with above settings
}

void app_main(void)
{
    init();

    //create task for servo
    xTaskCreate(rotate_servo, "rotate_servo", 4096, NULL, configMAX_PRIORITIES - 10, NULL);
}
