#include "PWM.h"



eesp_err_t PWM::INIT() {
    esp_err_t pwm_err = mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    if (pwm_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MCPWM");
        return pwm_err;
    }
    return ESP_OK;
}



esp_err_t PWM::setFrequencyAndResolution(uint16_t frequency, uint8_t resolution) {
    // Calculate PWM prescaler value based on desired frequency
    uint16_t prescaler;
    if (frequency == 0) {
        prescaler = 0; // Stop PWM
    } else {
        prescaler = 16000000 / (2 * frequency * (1 << resolution));
        if (prescaler < 1) prescaler = 1;
        else if (prescaler > 0xFF) prescaler = 0xFF;
    }

    // Set PWM frequency and resolution
    mcpwm_timer_t timer_num = MCPWM_TIMER_0;
    mcpwm_operator_t op_num = MCPWM_OPR_A;
    mcpwm_timer_init(MCPWM_UNIT_0, timer_num, &pwm_config);
    mcpwm_timer_set_frequency(MCPWM_UNIT_0, timer_num, frequency);
    mcpwm_set_duty(MCPWM_UNIT_0, timer_num, op_num, 0); // Set duty cycle to 0 initially
    mcpwm_start(MCPWM_UNIT_0, timer_num);

    return ESP_OK;
}


esp_err_t PWM::enableDeadtime(mcpwm_unit_t mcpwmNum, mcpwm_timer_t timerNum, uint32_t risingEdgeDeadtime, uint32_t fallingEdgeDeadtime) {
    // Configure dead time insertion for the specified MCPWM timer
    mcpwm_deadtime_config_t deadtimeConfig;
    deadtimeConfig.rise = risingEdgeDeadtime; // Rising edge dead time in nanoseconds
    deadtimeConfig.fall = fallingEdgeDeadtime; // Falling edge dead time in nanoseconds
    return mcpwm_deadtime_enable(mcpwmNum, timerNum, &deadtimeConfig);
}


esp_err_t PWM::setDutyCycle(uint8_t duty_cycle) {
    return mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
}


void PWM::start(){
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
}


void PWM::stop() { //used to disable the PWM, not to set the duty cycle to 0
        mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
}



// Initialize fault handling
esp_err_t PWM::initFaultHandling() {
    // Initialize fault detection and handling
    return mcpwm_fault_init(MCPWM_UNIT_0, MCPWM_TIMER_0);
}


// Set fault level for low-to-high transition on the PWM signal
esp_err_t PWM::setFaultLevelLowToHigh(mcpwm_fault_signal_t fault_signal, uint32_t threshold) {
    // Set the fault level for low-to-high transition on the PWM signal
    return mcpwm_fault_set_lh(MCPWM_UNIT_0, MCPWM_TIMER_0, fault_signal, threshold);
}


// Set fault level for low-to-low transition on the PWM signal
esp_err_t PWM::setFaultLevelLowToLow(mcpwm_fault_signal_t fault_signal, uint32_t threshold) {
    // Set the fault level for low-to-low transition on the PWM signal
    return mcpwm_fault_set_ll(MCPWM_UNIT_0, MCPWM_TIMER_0, fault_signal, threshold);
}


// Set fault level for high-to-low transition on the PWM signal
esp_err_t PWM::setFaultLevelHighToLow(mcpwm_fault_signal_t fault_signal, uint32_t threshold) {
    // Set the fault level for high-to-low transition on the PWM signal
    return mcpwm_fault_set_hl(MCPWM_UNIT_0, MCPWM_TIMER_0, fault_signal, threshold);
}


// Set fault level for high-to-high transition on the PWM signal
esp_err_t PWM::setFaultLevelHighToHigh(mcpwm_fault_signal_t fault_signal, uint32_t threshold) {
    // Set the fault level for high-to-high transition on the PWM signal
    return mcpwm_fault_set_hh(MCPWM_UNIT_0, MCPWM_TIMER_0, fault_signal, threshold);
}


// Set fault level based on the duty cycle
esp_err_t PWM::setFaultLevelByDutyCycle(mcpwm_fault_signal_t fault_signal, float duty_cycle) {
    // Set the fault level based on the duty cycle
    return mcpwm_fault_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, fault_signal, duty_cycle);
}