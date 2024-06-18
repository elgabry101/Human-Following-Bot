#pragma once

#include "driver/ledc.h"
#include "GPIO.h"
class motor {


public:
    motor(){}; 
    PIN dir;
    ledc_channel_t channel;
    motor(gpio_num_t direction_control,gpio_num_t pwm,ledc_channel_t ch):channel{ch} {
        
        
            ledc_timer_config_t s1={
            .speed_mode=LEDC_HIGH_SPEED_MODE,
            .duty_resolution=LEDC_TIMER_4_BIT,
            .timer_num=LEDC_TIMER_0,
            .freq_hz=1000,
            .clk_cfg= LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&s1));
 
        ledc_channel_config_t ledc_channel0 = {

        .gpio_num       = pwm,
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = channel,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel0));

        dir= PIN(direction_control,OUTPUT);
        dir.set(0);
    };

    void move_forward(int speed);
    void move_backward(int speed);
    void stop();
};
