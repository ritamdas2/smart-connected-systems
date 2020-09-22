#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#define TIMER_DIVIDER 16                             //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // to seconds
#define TIMER_INTERVAL_SEC (1)                       // Sample test interval for the first timer
#define TEST_WITH_RELOAD 1                           // Testing will be done with auto reload

// A simple structure to pass "events" to main task
typedef struct
{
    int flag; // flag for enabling stuff in main code
} timer_event_t;

// Initialize queue handler for timer-based events
xQueueHandle timer_queue;

#define FEEDER_INTERVAL 120
int counter = FEEDER_INTERVAL; //time interval for fish feeder - 2 min for now

// ISR handler
void IRAM_ATTR timer_group0_isr(void *para)
{

    // Prepare basic event data, aka set flag
    timer_event_t evt;
    evt.flag = 1;

    // Clear the interrupt, Timer 0 in group 0
    TIMERG0.int_clr_timers.t0 = 1;

    // After the alarm triggers, we need to re-enable it to trigger it next time
    TIMERG0.hw_timer[TIMER_0].config.alarm_en = TIMER_ALARM_EN;

    // Send the event data back to the main program task
    xQueueSendFromISR(timer_queue, &evt, NULL);
}

// Initialize timer 0 in group 0 for 1 sec alarm interval and auto reload
static void alarm_init()
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = TEST_WITH_RELOAD;
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // Timer's counter will initially start from value below
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

    // Configure the alarm value and the interrupt on alarm
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL_SEC * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr,
                       (void *)TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    // Start timer
    timer_start(TIMER_GROUP_0, TIMER_0);
}

// The main task of this example program
static void timer_evt_task(void *arg)
{
    while (1)
    {
        // Create dummy structure to store structure from queue
        timer_event_t evt;

        // Transfer from queue
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);

        // Do something if triggered!
        if (evt.flag == 1)
        {
            if (counter > 0)
            {
                printf("counter is: %d\n", counter);
                counter--;
            }
            else
            {
                //reset counter back to top
                counter = FEEDER_INTERVAL;
                printf("servo moves\n");
            }
        }
    }
}

void init()
{
    // congifure init stuff here
}

void app_main(void)
{
    //Should use global variable as a counter just like in skill11

    // The overall idea can be divided into 3 tasks
    // - servo task
    // - timer task
    // - alphanumeric display

    init();

    // Create a FIFO queue for timer-based
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));

    //create tasks here for servo, timer and alphnumeric display
    // make timer task with highest priority?

    // Create task to handle timer-based events
    xTaskCreate(timer_evt_task, "timer_evt_task", 2048, NULL, configMAX_PRIORITIES - 5, NULL);

    //start 1 sec clock
    alarm_init();
}
