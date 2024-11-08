#include "Comm.h"

int f=0;
int b=0;
int r=0;
int l=0;
int h=0;

esp_err_t comm::init() {
    esp_err_t ret= ESP_OK;
    ret|=nvs_flash_init();
    ret|=esp_netif_init();
    ret|=esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret|=esp_wifi_init(&cfg);
    //ret|= esp_wifi_set_storage(WIFI_STORAGE_RAM) ;
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_netif_t* netif = esp_netif_create_default_wifi_ap();

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config((wifi_interface_t) 1, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

    // Start the HTTP server
    start_webserver();
    esp_now_peer_info_t peerInfo ={
        .lmk=NULL,
        .channel=0,
        .ifidx=WIFI_IF_AP,
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

esp_err_t comm::register_cb(esp_now_recv_cb_t cb)
{
    return esp_now_register_recv_cb(cb);
}



void comm::wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{

}

// HTTP GET handler for "/hello" URI
esp_err_t comm::control_handler(httpd_req_t *req)
{
    httpd_resp_send(req, webpage, strlen(webpage));
    return ESP_OK;
}

esp_err_t comm::empty(httpd_req_t *req)
{
    httpd_resp_send(req, "1", strlen("1"));
    if(strcmp("/motorRight",req->uri)==0)
    {
        r=1;
    }
    else if(strcmp("/motorBackward",req->uri)==0)
    {
        b=1;
    }
    else if(strcmp("/motorLeft",req->uri)==0)
    {
        l=1;
    }
    else if(strcmp("/motorForward",req->uri)==0)
    {
        f=1;
    }
    else if(strcmp("/history",req->uri)==0)
    {
        h=1;
    }
    return ESP_OK;
}

const httpd_uri_t comm::motorRight = {
    .uri       = "/motorRight",
    .method    = HTTP_GET,
    .handler   = empty,
    .user_ctx  = NULL
};
const httpd_uri_t comm::motorBackward = {
    .uri       = "/motorBackward",
    .method    = HTTP_GET,
    .handler   = empty,
    .user_ctx  = NULL
};
const httpd_uri_t comm::motorLeft = {
    .uri       = "/motorLeft",
    .method    = HTTP_GET,
    .handler   = empty,
    .user_ctx  = NULL
};
const httpd_uri_t comm::motorForward = {
    .uri       = "/motorForward",
    .method    = HTTP_GET,
    .handler   = empty,
    .user_ctx  = NULL
};

const httpd_uri_t comm::history = {
    .uri       = "/history",
    .method    = HTTP_GET,
    .handler   = empty,
    .user_ctx  = NULL
};

const httpd_uri_t comm::control = {
    .uri       = "/control",
    .method    = HTTP_GET,
    .handler   = control_handler,
    .user_ctx  = NULL
};

// Function to start the web server
httpd_handle_t comm::start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &control);
        httpd_register_uri_handler(server, &motorForward);
        httpd_register_uri_handler(server, &motorLeft);
        httpd_register_uri_handler(server, &motorBackward);
        httpd_register_uri_handler(server, &motorRight);
        httpd_register_uri_handler(server, &history);
    }

    return server;
}