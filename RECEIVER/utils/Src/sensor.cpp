#include "sensor.h"

void ultrasonic::trigger()
{
    trig.set(0);
    trig.set(1);
}

QueueHandle_t ultrasonic::get_queue()
{
    return q;
}

mcpwm_cap_channel_handle_t ultrasonic::mcpwm_channel_create(bool (*func)(mcpwm_cap_channel_handle_t, const mcpwm_capture_event_data_t *, void *))
{

    mcpwm_cap_channel_handle_t cap_chan = NULL;
    mcpwm_capture_channel_config_t cap_ch_conf = {
        .gpio_num = echo.get_pin(),
        .intr_priority = 0,
        .prescale = 1,
    // capture on both edge
        .flags = {
            .pos_edge = true,
            .neg_edge = true,
            // pull up internally
            .pull_up = true,
        },
};
ESP_ERROR_CHECK(mcpwm_new_capture_channel(cap_timer, &cap_ch_conf, &cap_chan));

TaskHandle_t cur_task = xTaskGetCurrentTaskHandle();
mcpwm_capture_event_callbacks_t cbs = {
    .on_cap = func,
};
ESP_ERROR_CHECK(mcpwm_capture_channel_register_event_callbacks(cap_chan, &cbs, cur_task));

ESP_ERROR_CHECK(mcpwm_capture_channel_enable(cap_chan));

gpio_config_t io_conf = {
    .pin_bit_mask = 1ULL << (uint64_t)trig.get_pin(),
    .mode = GPIO_MODE_OUTPUT,
};
ESP_ERROR_CHECK(gpio_config(&io_conf));
// drive low by default
ESP_ERROR_CHECK(gpio_set_level(trig.get_pin(), 0));
return cap_chan;
}