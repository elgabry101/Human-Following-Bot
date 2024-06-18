#pragma once


#include "GPIO.h"
#include "driver/mcpwm_cap.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


class ultrasonic
{
    protected:
        PIN trig;
        PIN echo;
        mcpwm_cap_timer_handle_t cap_timer;
        
    public:
        char* id;
        int reading=-1;
        ultrasonic(){};
        ultrasonic(gpio_num_t trig_num,gpio_num_t echo_num,mcpwm_cap_timer_handle_t cap_timer=NULL,char* identifier=NULL):
        cap_timer{cap_timer},
        id{identifier}
        {
            trig= PIN(trig_num,OUTPUT);
            echo= PIN(echo_num,INPUT,GPIO_INTR_ANYEDGE,GPIO_PULLDOWN_ENABLE);

        };
        void trigger();
        mcpwm_cap_channel_handle_t mcpwm_channel_create(
                    bool (*func)(mcpwm_cap_channel_handle_t,
                    const mcpwm_capture_event_data_t *, void*));
};

