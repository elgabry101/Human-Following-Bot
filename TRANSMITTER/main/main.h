#include <stdio.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_rom_sys.h"

#include "driver/mcpwm_cap.h"


#include "GPIO.h"
#include "Sensor.h"
#include "Timer.h"
#include "Comm.h"
#include "Compass.h"



static void IRAM_ATTR periodic_timer_callback(void* arg);
static void button_int(void* arg);
void timer_start(void* t);
void update_heading(void * args);
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int data_len);

comm now;
timer tmr;
timer single;
unsigned long debounceTimeout=0;