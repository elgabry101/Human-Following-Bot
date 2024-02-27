#pragma once

#include "driver/gpio.h"


#define INPUT false
#define OUTPUT true

class PIN
{
protected:
    gpio_num_t pin_num; 
    gpio_config_t cfg;
    bool state=0;
    gpio_mode_t pin_mode;
    gpio_pullup_t pull_up;
    gpio_pulldown_t pull_down;
    gpio_int_type_t pin_inter;
public:
    PIN(){};

    PIN(bool mod,gpio_num_t _pin,gpio_int_type_t inter=GPIO_INTR_DISABLE,
    gpio_pulldown_t down=GPIO_PULLDOWN_DISABLE,gpio_pullup_t up=GPIO_PULLUP_DISABLE):
    pin_num{_pin},
    pull_up{up},
    pull_down{down},
    pin_inter{inter}
    {
        if(mod)
            pin_mode=GPIO_MODE_OUTPUT;
        else
            pin_mode=GPIO_MODE_INPUT;
        cfg=gpio_config_t
        {
            .pin_bit_mask=(1ULL)<<_pin,
            .mode=pin_mode,
            .pull_up_en=up,
            .pull_down_en=down,
            .intr_type=inter
        };
        gpio_config(&cfg);
    };

    esp_err_t attach_int(gpio_isr_t func);
    esp_err_t set(bool level);
    esp_err_t toggle(void);
    [[nodiscard]] char read(void);
    void reset_pin(void);
};