#pragma once

#include <stdio.h>
#include <utility>
#include <iostream>
#include <queue>
#include "string.h"


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
#include "Sensor.h"
#include "Timer.h"
#include "Motor.h"
#include "Navigator.h"
#include "Compass.h"
#include "comm.h"
using namespace std;


esp_timer_handle_t timer_init(void* argu);
mcpwm_cap_timer_handle_t cap_timer_init();
static bool IRAM_ATTR mcpwm_cb(mcpwm_cap_channel_handle_t cap_chan,
                                        const mcpwm_capture_event_data_t *edata,
                                        void *user_data);
static void IRAM_ATTR timer_cb(void* arg);
void measure_distance(void * args);
void navigate(void * arg);
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int data_len);




#define left_trigger GPIO_NUM_27
#define left_echo GPIO_NUM_14
#define mid_trigger GPIO_NUM_25
#define mid_echo GPIO_NUM_26
#define right_trigger GPIO_NUM_32 
#define right_echo GPIO_NUM_33

comm now;
timer tmr;
mcpwm_cap_timer_handle_t cap_timer;
int lock=0;
int head=0;