#include "GPIO.h"



esp_err_t PIN::init(void)
{
    esp_err_t ret = gpio_config(&cfg);
    return ret;
}

esp_err_t PIN::set(bool level)
{
    esp_err_t ret= ESP_OK;
    if(GPIO_MODE_OUTPUT==pin_mode)
    {
        ret=gpio_set_level(pin_num,level);
        state=level;
    }
    else
    {
        ret= ESP_FAIL;
    }
    return ret;
}
esp_err_t PIN::toggle(void)
{
    esp_err_t ret= ESP_OK;
    if(GPIO_MODE_OUTPUT==pin_mode)
    {
        state=!state;
        ret=gpio_set_level(pin_num,state);
    }
    else
    {
        ret= ESP_FAIL;
    }
    return ret;
}
char PIN::read(void)
{
    char ret= 0;
    if(GPIO_MODE_INPUT==pin_mode)
    {
        ret=gpio_get_level(pin_num);
        state=ret;
    }
    else
    {
        ret= ESP_FAIL;
    }
    return ret;

}
void PIN::reset_pin(void)
{
    gpio_reset_pin(pin_num);
}