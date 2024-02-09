#pragma once

#include "GPIO.h"
#include "esp_log.h"



class sensor
{
protected:
    PIN trig;
    PIN echo;
    long long time;
    
public:
    esp_err_t init(gpio_num_t trig,gpio_num_t echo  );

};