#include "main.h"


typedef pair <int,int> trajectory;

uint8_t broadcastAddress[] = {0xA0,0xB7,0x65,0x61,0xC8,0x24};
timer tmr;
mcpwm_cap_timer_handle_t cap_timer;
QueueHandle_t q1;
QueueHandle_t q2;
QueueHandle_t q3;

// Structure to hold received data
typedef struct {
    int data;
} __attribute__((packed)) message_t;

// Function to encode data into a byte array
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

// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int data_len) {
    tmr.start_timer();
    ESP_ERROR_CHECK(mcpwm_capture_timer_start(cap_timer));
    message_t received_data;
    decode_message(data, data_len, &received_data); // Decode received data
    printf("Received data: %d\n", received_data.data);
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
        ESP_LOGE("ESP-NOW Receiver", "Error initializing ESP-NOW: %d", ret);
        abort();
    }
}

// Register callback function for receiving data
static void register_recv_cb() {
    esp_now_register_recv_cb(OnDataRecv);
}

extern "C" void app_main(void)
{
    //timers for mcpwm capture and trigger timer
    cap_timer=cap_timer_init();
    

    
    q1=xQueueCreate(2,sizeof(float));
    q2=xQueueCreate(2,sizeof(float));
    q3=xQueueCreate(2,sizeof(float));
    //sensor objects and array
    ultrasonic snsr1= ultrasonic((gpio_num_t)33,(gpio_num_t)32,cap_timer,q1,"frst");
    ultrasonic snsr2= ultrasonic((gpio_num_t)26,(gpio_num_t)25,cap_timer,q2,"scnd");
    ultrasonic snsr3= ultrasonic((gpio_num_t)14,(gpio_num_t)27,cap_timer,q3,"thrd");
    ultrasonic snsrs[]={snsr1,snsr2,snsr3}; 

    tmr= timer(PERIODIC,100000,timer_cb,&snsrs);
    //esp now init process
    wifi_init();
    espnow_init();
    register_recv_cb();



    //tasks
    TaskHandle_t task1;
    TaskHandle_t task2;
    TaskHandle_t task3;
    xTaskCreatePinnedToCore(measure_distance,"first ultrasonic",3000,&snsr1,4,&task1,1);
    xTaskCreatePinnedToCore(measure_distance,"second ultrasonic",3000,&snsr2,4,&task2,1);
    xTaskCreatePinnedToCore(measure_distance,"third ultrasonic",3000,&snsr3,4,&task3,1);
    xTaskCreate(direction_calc,"direction finding",3000,snsrs,4,NULL);
    while(1)
    {
    }

}

void direction_calc(void * arg)
{
    static ultrasonic * snsrs=(ultrasonic*)arg;
    float reading1;
    float reading2;
    float reading3;
    float mean;
    static trajectory history; 
    trajectory curr;
    while(1)
    {
        xQueueReceive(snsrs[0].get_queue(),&reading1,10/portTICK_PERIOD_MS);
        xQueueReceive(snsrs[1].get_queue(),&reading2,10/portTICK_PERIOD_MS);
        xQueueReceive(snsrs[2].get_queue(),&reading3,10/portTICK_PERIOD_MS);
        //ESP_LOGI("direction"," %.2fcm      %.2fcm      %.2fcm",reading1,reading2,reading3);
        mean= (reading1+reading2+reading3)/3;

        if(-1==mean)
        {
            ESP_LOGI("direction","OUT OF RANGEEE");
        }
        else if(reading2>0||(reading1>0&&reading3>0))
        {
            if(abs(reading1-reading3)<3)
            {
                curr.first= reading2-30;
                curr.second=0;
                ESP_LOGI("direction"," forward ");
            }
            else if(abs(reading1-reading3)>3)
            {
                if(reading1<reading3)
                {
                    curr.first= reading2-30;
                    int theta= (acos((pow(13,2)+pow(reading1,2)-pow(reading3,2))/(26*reading1))*180/3.141593)-90;
                    ESP_LOGI("direction"," RIGHT %i",theta);
                    curr.second=theta;
                }
                else
                {
                    curr.first= reading2-30;
                    int theta= (acos((pow(13,2)+pow(reading3,2)-pow(reading1,2))/(26*reading3))*180/3.141593)-90;
                    ESP_LOGI("direction"," LEFT %i",theta);
                    curr.second=-theta;
                }
            }
        }
        else
        {
            ESP_LOGI("direction"," msh mthndle");
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    



}


void measure_distance(void * args)
{
    ultrasonic snsr=*(ultrasonic*)args;
    mcpwm_cap_channel_handle_t cap=snsr.mcpwm_channel_create(mcpwm_cb);
    uint32_t tof_ticks;
    while(1)
    {
        if (xTaskNotifyWait(0x00, ULONG_MAX, &tof_ticks, pdMS_TO_TICKS(1000)) == pdTRUE) {
            float pulse_width_us = tof_ticks * (1000000.0 / esp_clk_apb_freq());
            if (pulse_width_us > 17640) {
                float distance =-1;
                xQueueSendToBack(snsr.get_queue(),(void *)&distance,10/portTICK_PERIOD_MS);
                continue;
            }
            // convert the pulse width into measure distance
            float distance = (float) pulse_width_us / 29.4;
            //ESP_LOGI(snsr.id, "distance: %.2fcm", distance);
            xQueueSendToBack(snsr.get_queue(),(void *)&distance,10/portTICK_PERIOD_MS);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}



mcpwm_cap_timer_handle_t cap_timer_init()
{
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_conf = {
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    ESP_ERROR_CHECK(mcpwm_new_capture_timer(&cap_conf, &cap_timer));
    ESP_ERROR_CHECK(mcpwm_capture_timer_enable(cap_timer));
    return cap_timer;
}


static bool IRAM_ATTR mcpwm_cb(mcpwm_cap_channel_handle_t cap_chan, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    static uint32_t cap_val_begin_of_sample = 0;
    static uint32_t cap_val_end_of_sample = 0;
    TaskHandle_t task_to_notify = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    //calculate the interval in the ISR,
    //so that the interval will be always correct even when capture_queue is not handled in time and overflow.
    if (edata->cap_edge == MCPWM_CAP_EDGE_POS) {
        // store the timestamp when pos edge is detected
        cap_val_begin_of_sample = edata->cap_value;
        cap_val_end_of_sample = cap_val_begin_of_sample;
    } else {
        cap_val_end_of_sample = edata->cap_value;
        uint32_t tof_ticks = cap_val_end_of_sample - cap_val_begin_of_sample;

        // notify the task to calculate the distance
        xTaskNotifyFromISR(task_to_notify, tof_ticks, eSetValueWithOverwrite, &high_task_wakeup);
    }

    return high_task_wakeup == pdTRUE;
}


static void IRAM_ATTR timer_cb(void* arg)
{
    gpio_set_level(GPIO_NUM_14,0);
    gpio_set_level(GPIO_NUM_33,0);
    gpio_set_level(GPIO_NUM_26,0);
    gpio_set_level(GPIO_NUM_14,1);
    gpio_set_level(GPIO_NUM_26,1);
    gpio_set_level(GPIO_NUM_33,1);
}
