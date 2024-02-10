#include "PWM.h"



esp_err_t PWM::INIT() {
    esp_err_t timer_err = ledc_timer_config(&timer_conf);
    if (timer_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure PWM timer");
        return timer_err;
    }
    
    esp_err_t channel_err = ledc_channel_config(&channel_conf);
    if (channel_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure PWM channel");
        return channel_err;
    }

    return ESP_OK; // Both timer and channel configurations were successful
}


esp_err_t PWM::setDutyCycle(uint8_t duty_cycle) {
        uint32_t duty = ((uint32_t)duty_cycle * ((1 << timer_conf.duty_resolution) - 1)) / 100;
        return ledc_set_duty(timer_conf.speed_mode, channel_conf.channel, duty);
    }


esp_err_t PWM::updateDuty() {
        return ledc_update_duty(timer_conf.speed_mode, channel_conf.channel);
    }


void PWM::stop() { //used to disable the PWM, not to set the duty cycle to 0
        ledc_stop(timer_conf.speed_mode, channel_conf.channel, 0);
    }


void PWM::setFrequency(uint32_t frequency) {
        timer_conf.freq_hz = frequency;
        ledc_timer_config(&timer_conf);
    }


void PWM::setResolution(uint8_t resolution) {
        timer_conf.duty_resolution = resolution;
        ledc_timer_config(&timer_conf);
    }


void PWM::setPhase(uint16_t phase) {
    // Check if phase control is supported by your hardware
    // This implementation assumes phase control is supported
    ledc_set_phase(timer_conf.speed_mode, channel_conf.channel, phase);
}


void PWM::fade(uint8_t target_duty, uint32_t duration_ms) {
    // Calculate the number of steps for the fade
    const uint32_t num_steps = 100;
    const uint32_t step_delay = duration_ms / num_steps;

    // Calculate the change in duty cycle for each step
    const uint8_t current_duty = channel_conf.duty;
    const int16_t duty_change = target_duty - current_duty;

    // Perform the fade
    for (uint32_t step = 0; step < num_steps; ++step) {
        uint8_t new_duty = current_duty + (duty_change * step) / num_steps;
        setDutyCycle(new_duty);
        updateDuty();
        vTaskDelay(step_delay / portTICK_PERIOD_MS);
    }

    // Ensure the final duty cycle is set correctly
    setDutyCycle(target_duty);
    updateDuty();
}




/* notes about fade and set pase functions

1- Phase Control: The setPhase() method sets the phase of the PWM signal.
This can be useful for controlling the timing of the PWM signal relative to another event, 
such as the position of a motor rotor.

2- Fade Functionality: The fade() method gradually changes the duty cycle of the PWM signal from its current value to a target value over a specified duration.
It calculates the number of steps needed for the fade and the change in duty cycle for each step.
Then, it iterates through the steps, updating the duty cycle and delaying for a short period between each step to create a smooth transition.
Finally, it ensures that the final duty cycle is set to the target value.



overall You can use these methods to control the speed of a DC motor by adjusting the duty cycle of the PWM signal.
For example, you can gradually increase or decrease the duty cycle to accelerate or decelerate the motor smoothly,
and you can adjust the phase of the PWM signal to synchronize it with the position of the motor rotor if necessary.

*/