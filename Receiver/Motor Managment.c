#include <stdio.h>
#include "driver/ledc.h"

// PWM settings
#define PWM_FREQUENCY   5000    // PWM frequency in Hz
#define PWM_RESOLUTION  10      // PWM resolution

// Initialize PWM
void pwm_init() {
    ledc_timer_config_t timer_conf;
    timer_conf.duty_resolution = PWM_RESOLUTION;
    timer_conf.freq_hz = PWM_FREQUENCY;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf;
    channel_conf.channel = LEDC_CHANNEL_0;
    channel_conf.duty = 0;
    channel_conf.gpio_num = GPIO_NUM_4; // Change to your desired GPIO pin
    channel_conf.intr_type = LEDC_INTR_DISABLE;
    channel_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    channel_conf.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&channel_conf);
}

// Set PWM duty cycle (0-100)
void pwm_set_duty_cycle(uint8_t duty_cycle) {
    uint32_t duty = ((uint32_t)duty_cycle * ((1 << PWM_RESOLUTION) - 1)) / 100;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

// Example usage
void app_main() {
    // Initialize PWM
    pwm_init();
    // Set PWM duty cycle (0-100)
    pwm_set_duty_cycle(50); // Set duty cycle to 50%
}