#include "esp_timer.h"


#define PERIODIC 1
#define ONE_SHOT 0

class timer
{
    protected:
        esp_timer_handle_t timer_handle;
        uint64_t period;
        esp_timer_cb_t call_back;
        char * name;
        bool timer_kind;
    public:
        timer(){};
        timer(bool kind,uint64_t period,esp_timer_cb_t cb,void * args,char* id=NULL):
        period{period},
        call_back{cb},
        name{id},
        timer_kind{kind}
        {   
            const esp_timer_create_args_t periodic_timer_args = 
            {
            .callback = cb,
            .arg=args,
            .name = name
            };
            ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &timer_handle));

        };
        esp_err_t start_timer();
        esp_err_t stop_timer();
};
