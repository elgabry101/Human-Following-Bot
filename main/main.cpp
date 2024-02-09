#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "GPIO.h"

extern "C" void app_main(void)
{
    PIN s=PIN(OUTPUT,(gpio_num_t)2);
    s.init();
    while(1)
    {
        s.toggle();
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}
