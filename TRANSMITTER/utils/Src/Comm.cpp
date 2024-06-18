#include "Comm.h"

esp_err_t comm::init() {
    esp_err_t ret= ESP_OK;
    ret|=nvs_flash_init();
    ret|=esp_netif_init();
    ret|=esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret|=esp_wifi_init(&cfg);
    ret|= esp_wifi_set_storage(WIFI_STORAGE_RAM) ;
    ret|=esp_wifi_set_mode(WIFI_MODE_STA);
    ret|=esp_wifi_start();
    ret|=esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_now_peer_info_t peerInfo ={
        .lmk=NULL,
        .channel=0,
        .ifidx=WIFI_IF_STA,
        .encrypt=false,
    };
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    ret|= esp_now_init();
    ret |= esp_now_add_peer(&peerInfo);
    return ret;
}

void comm::encode_message(const message_t *msg, uint8_t *buffer, size_t buffer_size) {
    if (buffer_size >= sizeof(message_t)) {
        memcpy(buffer, msg, sizeof(message_t));
    } else {
        ESP_LOGE("ESP-NOW Sender", "Buffer size too small for encoding");
    }
}

void comm::decode_message(const uint8_t *buffer, size_t buffer_size, message_t *msg) {
    if (buffer_size >= sizeof(message_t)) {
        memcpy(msg, buffer, sizeof(message_t));
    } else {
        ESP_LOGE("ESP-NOW Receiver", "Received invalid data size: %d", buffer_size);
    }
}



 esp_err_t comm::send_data(int data) {
    message_t myData;
    myData.data = data;
    uint8_t buffer[sizeof(message_t)];
    encode_message(&myData, buffer, sizeof(buffer)); 
    return esp_now_send(broadcastAddress, buffer, sizeof(buffer));
}

