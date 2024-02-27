#include "main.h"

uint8_t broadcastAddress[] = {0x48,0xE7,0x29,0xC9,0x3D,0xB4};


extern "C" void app_main(void)
{
    wifi_init();
    espnow_init();
    add_peer();
    ultrasonic_sender transmitter= ultrasonic_sender((gpio_num_t)27,(gpio_num_t)26,"sndr");
    PIN button = PIN(INPUT,(gpio_num_t)32,GPIO_INTR_DISABLE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
    timer tmr= timer(PERIODIC,100000,periodic_timer_callback,&transmitter,"transmitter");
    while(0==gpio_get_level((gpio_num_t)32));
    send_data();
    esp_rom_delay_us(1500);
    tmr.start_timer();
    while(1)
    {
    }

}


void encode_message(const message_t *msg, uint8_t *buffer, size_t buffer_size) {
    if (buffer_size >= sizeof(message_t)) {
        memcpy(buffer, msg, sizeof(message_t));
    } else {
        ESP_LOGE("ESP-NOW Sender", "Buffer size too small for encoding");
    }
}

// Function to decode data from a byte array
void decode_message(const uint8_t *buffer, size_t buffer_size, message_t *msg) {
    if (buffer_size >= sizeof(message_t)) {
        memcpy(msg, buffer, sizeof(message_t));
    } else {
        ESP_LOGE("ESP-NOW Receiver", "Received invalid data size: %d", buffer_size);
    }
}

// Initialize Wi-Fi
static void wifi_init() {
    nvs_flash_init();
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
}

// Initialize ESP-NOW
static void espnow_init() {
    esp_now_peer_info_t peerInfo ={
        .peer_addr=*broadcastAddress,
        .lmk=NULL,
        .channel=0,
        .ifidx=WIFI_IF_STA,
        .encrypt=false,
    };
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE("ESP-NOW Sender", "Error initializing ESP-NOW: %d", ret);
        abort();
    }
}

// Add peer
static void add_peer() {
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_err_t ret = esp_now_add_peer(&peerInfo);
    if (ret != ESP_OK) {
        ESP_LOGE("ESP-NOW Sender", "Failed to add peer: %d", ret);
        abort();
    }
}

// Send data
static void send_data() {
    message_t myData;
    myData.data = 0b1111; // Generate random data
    uint8_t buffer[sizeof(message_t)];
    encode_message(&myData, buffer, sizeof(buffer)); // Encode data
    esp_err_t ret = esp_now_send(broadcastAddress, buffer, sizeof(buffer));
    if (ret == ESP_OK) {
        ESP_LOGI("espnow","Sent data: %d\n", myData.data);
    } else {
        ESP_LOGE("ESP-NOW Sender", "Error sending data: %d", ret);
    }
}

static void IRAM_ATTR periodic_timer_callback(void* arg)
{
    gpio_set_level(GPIO_NUM_27,0);
    gpio_set_level(GPIO_NUM_27,0);
    gpio_set_level(GPIO_NUM_27,0);
    gpio_set_level(GPIO_NUM_27,0);
    gpio_set_level(GPIO_NUM_27,1);
    gpio_set_level(GPIO_NUM_27,1);
}
