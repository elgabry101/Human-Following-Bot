#include "sensor.h"

esp_err_t PIN::init(void)
{
    return gpio_config(&cfg);
}

esp_err_t PIN::set(bool level)
{
    if (inverted_logic)
    {
        level = !level;
    }

    state = level;
    return gpio_set_level(pin_num, level);
}

esp_err_t PIN::toggle(void)
{
    state = !state;
    return gpio_set_level(pin_num, state);
}

void PIN::reset_pin(void)
{
    state = false;
    gpio_reset_pin(pin_num);
}

char PIN::read(void)
{
    return gpio_get_level(pin_num) ? '1' : '0';
}


