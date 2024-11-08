#pragma once

#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"
#include "string.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include <string.h>

const char webpage[] = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Robot Car Control</title>
    <style>
        .button {
            width: 100px;
            height: 100px;
            font-size: 24px;
            margin: 10px;
            background-color: #f0f0f0;
            border: 1px solid #ccc;
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            transition: background-color 0.2s ease;
        }
        .button.active {
            background-color: #ccc; /* Darker color when active */
        }
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }
        .row {
            display: flex;
        }
        .top-right {
            position: absolute;
            top: 10px;
            right: 10px;
        }
    </style>
    <script>
        function sendRequest(url) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", url, true);
            xhr.send();
        }

        function setupButton(id, url, isToggle) {
            var button = document.getElementById(id);
            var active = false;
            button.addEventListener('mousedown', function() {
                if (isToggle) {
                    sendRequest(url); // Send request one time for history button
                } else {
                    button.classList.add('active'); // Add active class
                    this.interval = setInterval(function() {
                        sendRequest(url);
                    }, 100); // Adjust the interval as needed
                }
            });
            button.addEventListener('mouseup', function() {
                if (!isToggle) {
                    button.classList.remove('active'); // Remove active class
                    clearInterval(this.interval);
                }
            });
            button.addEventListener('mouseleave', function() {
                if (!isToggle) {
                    button.classList.remove('active'); // Remove active class
                    clearInterval(this.interval);
                }
            });
        }

        window.onload = function() {
            setupButton('up', 'http://192.168.4.1/motorForward', false);
            setupButton('left', 'http://192.168.4.1/motorLeft', false);
            setupButton('right', 'http://192.168.4.1/motorRight', false);
            setupButton('down', 'http://192.168.4.1/motorBackward', false);
            setupButton('history', 'http://192.168.4.1/history', true);

            var activeKeys = {};
            var intervals = {};

            document.addEventListener('keydown', function(event) {
                if (event.defaultPrevented) {
                    return; // Do nothing if the event was already processed
                }
                var key = event.key.toLowerCase();
                if (!activeKeys[key]) {
                    activeKeys[key] = true;
                    // Stop history requests if any other key is pressed
                    clearInterval(document.getElementById('history').interval);
                    document.getElementById('history').classList.remove('active');
                    switch(key) {
                        case 'w':
                            intervals['w'] = setInterval(function() { sendRequest('http://192.168.4.1/motorForward'); }, 100);
                            document.getElementById('up').classList.add('active'); // Add active class to 'up' button
                            break;
                        case 's':
                            intervals['s'] = setInterval(function() { sendRequest('http://192.168.4.1/motorBackward'); }, 100);
                            document.getElementById('down').classList.add('active'); // Add active class to 'down' button
                            break;
                        case 'a':
                            intervals['a'] = setInterval(function() { sendRequest('http://192.168.4.1/motorLeft'); }, 100);
                            document.getElementById('left').classList.add('active'); // Add active class to 'left' button
                            break;
                        case 'd':
                            intervals['d'] = setInterval(function() { sendRequest('http://192.168.4.1/motorRight'); }, 100);
                            document.getElementById('right').classList.add('active'); // Add active class to 'right' button
                            break;
                    }
                }
                event.preventDefault();
            }, true);

            document.addEventListener('keyup', function(event) {
                var key = event.key.toLowerCase();
                if (activeKeys[key]) {
                    clearInterval(intervals[key]);
                    activeKeys[key] = false;
                    switch(key) {
                        case 'w':
                            document.getElementById('up').classList.remove('active'); // Remove active class from 'up' button
                            break;
                        case 's':
                            document.getElementById('down').classList.remove('active'); // Remove active class from 'down' button
                            break;
                        case 'a':
                            document.getElementById('left').classList.remove('active'); // Remove active class from 'left' button
                            break;
                        case 'd':
                            document.getElementById('right').classList.remove('active'); // Remove active class from 'right' button
                            break;
                    }
                }
            }, true);
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="row">
            <div id="up" class="button">&#8593;</div>
        </div>
        <div class="row">
            <div id="left" class="button">&#8592;</div>
            <div id="right" class="button">&#8594;</div>
        </div>
        <div class="row">
            <div id="down" class="button">&#8595;</div>
        </div>
    </div>
    <div id="history" class="button top-right">History</div>
</body>
</html>
)";



#define EXAMPLE_ESP_WIFI_SSID      "ESP32-AP"
#define EXAMPLE_ESP_WIFI_PASS      "123456789"
#define EXAMPLE_MAX_STA_CONN       4

static const char *TAG = "wifi_softAP";

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
    esp_err_t register_cb(esp_now_recv_cb_t cb);
    static const httpd_uri_t motorRight;
    static const httpd_uri_t motorBackward;
    static const httpd_uri_t motorLeft;
    static const httpd_uri_t motorForward;
    static const httpd_uri_t history;
    static const httpd_uri_t control;
    static esp_err_t control_handler(httpd_req_t *req);
    static esp_err_t empty(httpd_req_t *req);
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
    static httpd_handle_t start_webserver(void);
};
