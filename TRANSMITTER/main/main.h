#include <stdio.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_rom_sys.h"
#include "nvs_flash.h"

#include "driver/mcpwm_cap.h"


#include "GPIO.h"
#include "Sensor.h"
#include "Timer.h"


typedef struct {
    int data;
} __attribute__((packed)) message_t;
static void send_data();
static void add_peer();
static void espnow_init();
static void wifi_init() ;
esp_timer_handle_t timer_init(void* argu);

static void IRAM_ATTR periodic_timer_callback(void* arg);