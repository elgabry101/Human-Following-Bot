#pragma once

#include "driver/mcpwm.h"
#include "esp_log.h"

class PWM
{
protected:
    mcpwm_config_t pwm_config;

public:
    PWM(uint8_t channel,
        gpio_num_t gpio_num,
        uint32_t frequency = 5000,
        uint8_t resolution = 10) :
        pwm_config({
            .frequency = frequency,
            .cmpr_a = 0,
            .cmpr_b = 0,
            .counter_mode = MCPWM_UP_COUNTER,
            .duty_mode = MCPWM_DUTY_MODE_0
        }) {}

    esp_err_t INIT(void);
    esp_err_t setDutyCycle(uint8_t duty_cycle);
    esp_err_t setFrequencyAndResolution(uint16_t frequency, uint8_t resolution);
    esp_err_t enableDeadtime(mcpwm_unit_t mcpwmNum, mcpwm_timer_t timerNum, uint32_t risingEdgeDeadtime, uint32_t fallingEdgeDeadtime);
    esp_err_t initFaultHandling();
    esp_err_t setFaultLevelLowToHigh(mcpwm_fault_signal_t fault_signal, uint32_t threshold);
    esp_err_t setFaultLevelLowToLow(mcpwm_fault_signal_t fault_signal, uint32_t threshold);
    esp_err_t setFaultLevelHighToLow(mcpwm_fault_signal_t fault_signal, uint32_t threshold);
    esp_err_t setFaultLevelHighToHigh(mcpwm_fault_signal_t fault_signal, uint32_t threshold);
    esp_err_t setFaultLevelByDutyCycle(mcpwm_fault_signal_t fault_signal, float duty_cycle);
    void start();
    void stop();
};