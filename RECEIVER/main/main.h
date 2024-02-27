#pragma once

#include <stdio.h>
#include <utility>
#include <iostream>
#include <queue>
#include "string.h"
#include "math.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "esp_timer.h"
#include "nvs_flash.h"

#include "GPIO.h"
#include "sensor.h"
#include "Timer.h"
using namespace std;


esp_timer_handle_t timer_init(void* argu);
mcpwm_cap_timer_handle_t cap_timer_init();
static bool IRAM_ATTR mcpwm_cb(mcpwm_cap_channel_handle_t cap_chan,
                                        const mcpwm_capture_event_data_t *edata,
                                        void *user_data);
static void IRAM_ATTR timer_cb(void* arg);
void measure_distance(void * args);
void direction_calc(void * arg);