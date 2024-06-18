#pragma once

#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"
#include "string.h"
#include "esp_log.h"

typedef struct {
    int data;
} __attribute__((packed)) message_t;
const uint8_t broadcastAddress[] = {0xe8,0xdb,0x84,0x21,0x98,0x2c};

class comm
{

public:
    comm(){};

    esp_err_t init();
    void encode_message(const message_t *msg, uint8_t *buffer, size_t buffer_size);
    void decode_message(const uint8_t *buffer, size_t buffer_size, message_t *msg);
    esp_err_t send_data(int data);
};
