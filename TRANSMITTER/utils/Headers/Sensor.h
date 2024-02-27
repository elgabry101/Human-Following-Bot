#pragma once


#include "GPIO.h"




class ultrasonic_sender
{
    protected:
        PIN trig;
        PIN echo;
        long long start;
        long long end;
        int distance=0;
        
    public:
        char* id;
        ultrasonic_sender(){};
        ultrasonic_sender(gpio_num_t trig_num,gpio_num_t echo_num,char* identifier=NULL):
        id{identifier}
        {
            trig= PIN(OUTPUT,trig_num);
            echo= PIN(INPUT,echo_num,GPIO_INTR_ANYEDGE,GPIO_PULLDOWN_ENABLE);
        };
        esp_err_t trigger();
};

