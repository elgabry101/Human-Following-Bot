#include "main.h"


extern "C" void app_main(void)
{

    ultrasonic_sender transmitter= ultrasonic_sender((gpio_num_t)27,(gpio_num_t)26,"sndr");
    PIN button = PIN(INPUT,(gpio_num_t)32,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
    tmr= timer(PERIODIC,100000,periodic_timer_callback,&transmitter,"transmitter");
    single=timer(ONE_SHOT,1800,timer_start,&tmr);
    now=comm();
    esp_err_t s=now.init();

    button.attach_int(button_int,&now);
    xTaskCreate(update_heading,"heading",1024*3,(void*)&now,4,NULL);
    while(1)
    {
    }

}


void update_heading(void * args)
{
    comm * now=(comm*)args;
    compass comp=compass();
    comp.init();
    while(1)
    {
        comp.update();
        if(tmr.is_started())
        {
            now->send_data(comp.heading<<2);
            ESP_LOGI("sent","%i",comp.heading);
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
        
    }

}

void IRAM_ATTR button_int(void * arg)
{
    if(debounceTimeout<esp_timer_get_time())
    {
        if(!tmr.is_started())
        {
            single.start_timer();
        }
        comm* tmp = (comm *)arg;
        debounceTimeout = esp_timer_get_time()+500000;
        tmp->send_data(0x01);
    }

}
void IRAM_ATTR timer_start(void* s)
{
    timer* tmp=(timer*)s;
    tmp->start_timer();
}

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int data_len) {
    message_t received_data;
    now.decode_message(data, data_len, &received_data); // Decode received data
    if(received_data.data==0)
    {
        printf("%i\n", received_data.data);
    }
    else
    {   
        printf("%i ", received_data.data);
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
