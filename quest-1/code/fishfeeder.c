#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void init()
{
    // congifure init stuff here
}

void app_main(void)
{

    init();
    //create tasks here for servo, timer and alphnumeric display
    // make timer task with highest priority?
}
