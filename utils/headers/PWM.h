#pragma once

#include "driver/ledc.h"
#include "esp_log.h"

class PWM
{
protected:
    ledc_timer_config_t timer_conf;
    ledc_channel_config_t channel_conf;

public:
    PWM(uint8_t channel,
        gpio_num_t gpio_num,
        uint32_t frequency = 5000,
        uint8_t resolution = 10) :
        timer_conf({
            .duty_resolution = resolution,
            .freq_hz = frequency,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_num = LEDC_TIMER_0
        }),
        channel_conf({
            .channel = LEDC_CHANNEL_0,
            .duty = 0,
            .gpio_num = gpio_num,
            .intr_type = LEDC_INTR_DISABLE,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_sel = LEDC_TIMER_0
        }) {}

    esp_err_t INIT(void);
    esp_err_t setDutyCycle(uint8_t duty_cycle);
    esp_err_t updateDuty();
    void stop();
    void setFrequency(uint32_t frequency);
    void setResolution(uint8_t resolution);
    void setPhase(uint16_t phase);
    void fade(uint8_t target_duty, uint32_t duration_ms);
};