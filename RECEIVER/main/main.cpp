#include "main.h"

int f=0;
int b=0;
int l=0;
int r=0;
int h=0;


extern "C" void app_main(void)
{
    //timers for mcpwm capture and trigger timer
    cap_timer=cap_timer_init();


    //sensor objects and array
    ultrasonic snsr_l= ultrasonic(left_trigger,left_echo,cap_timer,"frst");
    ultrasonic snsr_m= ultrasonic(mid_trigger,mid_echo,cap_timer,"scnd");
    ultrasonic snsr_r= ultrasonic(right_trigger,right_echo,cap_timer,"thrd");
    snsrs[0] = &snsr_l;
    snsrs[1] = &snsr_m;
    snsrs[2] = &snsr_r;

    tmr= timer(PERIODIC,100000,timer_cb,NULL);
    //esp now init process
    now.init();
    now.register_cb(OnDataRecv);

    motor right=motor((gpio_num_t)22,(gpio_num_t)23,LEDC_CHANNEL_0);
    motor left=motor((gpio_num_t)18,(gpio_num_t)19,LEDC_CHANNEL_1);
    navigator controller = navigator(right,left);



    //tasks
    TaskHandle_t task1;
    TaskHandle_t task2;
    TaskHandle_t task3;
    xTaskCreate(measure_distance,"left ultrasonic",1024*3,(void*)&snsr_l,4,&task1);
    xTaskCreate(measure_distance,"middle ultrasonic",1024*3,(void*)&snsr_m,4,&task2);
    xTaskCreate(measure_distance,"right ultrasonic",1024*3,(void*)&snsr_r,4,&task3);
    xTaskCreate(navigate,"direction finding",1024*3,(void*)&controller,4,NULL);
    while(1)
    {
    }

}

void navigate(void * arg)
{
    navigator * controller = (navigator*)arg;
    while(1)
    {

        controller->readings[0]=snsrs[0]->reading;
        controller->readings[1]=snsrs[1]->reading;
        controller->readings[2]=snsrs[2]->reading;
        //printf("left sensor:%i cm   middle sensor:%i cm   rigt sensor:%i cm\n",snsrs[0]->reading,snsrs[1]->reading,snsrs[2]->reading);
        controller->user_heading=head-20;
        controller->main_lock=main_lock;
        controller->update_heading();
        controller->make_decision();
        controller->update_history();
        controller->move();
        vTaskDelay(50/portTICK_PERIOD_MS);
    }
    
}

void measure_distance(void * args)
{
    ultrasonic * snsr=(ultrasonic*)args;
    mcpwm_cap_channel_handle_t cap=snsr->mcpwm_channel_create(mcpwm_cb);
    uint32_t tof_ticks;
    int distance;
    float pulse_width_us;
    while(1)
    {
        if (xTaskNotifyWait(0x00, ULONG_MAX, &tof_ticks, pdMS_TO_TICKS(1000)) == pdTRUE) 
        {
            pulse_width_us = tof_ticks * (1000000.0 / esp_clk_apb_freq());
            if (pulse_width_us > 17640) 
            {

                distance =-1;
                snsr->reading=distance;
            }
            else
            {
                // convert the pulse width into measure distance
                distance = (int) pulse_width_us / 29.4;
                snsr->reading=distance;
            }

        }
        
    }

}



static bool IRAM_ATTR mcpwm_cb(mcpwm_cap_channel_handle_t cap_chan, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    static uint32_t cap_val_begin_of_sample = 0;
    static uint32_t cap_val_end_of_sample = 0;
    TaskHandle_t task_to_notify = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
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


mcpwm_cap_timer_handle_t cap_timer_init()
{
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_conf = 
    {
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    ESP_ERROR_CHECK(mcpwm_new_capture_timer(&cap_conf, &cap_timer));
    ESP_ERROR_CHECK(mcpwm_capture_timer_enable(cap_timer));
    return cap_timer;
}


static void IRAM_ATTR timer_cb(void* arg)
{   
    if(main_lock)
    {
        gpio_set_level(mid_trigger,0);
        gpio_set_level(right_trigger,0);
        gpio_set_level(left_trigger,0);
        gpio_set_level(mid_trigger,1);
        gpio_set_level(right_trigger,1);
        gpio_set_level(left_trigger,1);
    }

}


void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int data_len) {
    if(!tmr.is_started())
    {
        tmr.start_timer();
        ESP_ERROR_CHECK(mcpwm_capture_timer_start(cap_timer));
    }
    message_t received_data;
    now.decode_message(data, data_len, &received_data); // Decode received data
    if(received_data.data&0x01)
    {
        main_lock=~main_lock;
        printf("Received data: %d\n", received_data.data);
    }
    else
    {   
        head=received_data.data>>2;
        int web_data=received_data.data>>11;
        if(web_data>0)
        {
            main_lock=0;
            if(web_data==1)
            {
                f=1;
                b=0;
                r=0;
                l=0;
                h=0;
            }
            else if(web_data==2)
            {
                b=1;
                f=0;
                r=0;
                l=0;
                h=0;
            }
            else if(web_data==4)
            {
                r=1;
                b=0;
                f=0;
                l=0;
                h=0;
            }
            else if(web_data==8)
            {
                l=1;
                r=0;
                b=0;
                f=0;
                h=0;
            }
            else if(web_data==15)
            {
                h=1;
                l=0;
                r=0;
                b=0;
                f=0;
            }
        }
        else
        {
                l=0;
                r=0;
                b=0;
                f=0;
        }
    }
    
}