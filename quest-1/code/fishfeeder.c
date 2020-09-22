#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int counter;

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
    //create tasks here for servo, timer and alphnumeric display
    // make timer task with highest priority?
}
