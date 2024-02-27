#include "Sensor.h"

esp_err_t ultrasonic_sender::trigger()
{
    esp_err_t ret=ESP_OK;
    ret|=trig.set(0);
    ret|=trig.set(1);
    return ret;
}


