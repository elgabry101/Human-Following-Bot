#include "GPIO.h"



esp_err_t PIN::attach_int(gpio_isr_t func)
{
    esp_err_t ret=ESP_OK;
    ret|=gpio_install_isr_service(0);
    ret|= gpio_isr_handler_add(pin_num,func,(void *) pin_num);
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
gpio_num_t PIN::get_pin(void)
{
    return pin_num;
}
void PIN::reset_pin(void)
{
    gpio_reset_pin(pin_num);
}