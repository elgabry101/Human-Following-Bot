#include "Timer.h"



esp_err_t timer::start_timer()
{
    if(timer_kind)
    {
        return esp_timer_start_periodic(timer_handle,period);
    }
    else
    {
        return esp_timer_start_once(timer_handle,period);
    }
    
}

esp_err_t timer::stop_timer()
{
    return esp_timer_stop(timer_handle);
}

char timer::is_started()
{
    return esp_timer_is_active(timer_handle);
}