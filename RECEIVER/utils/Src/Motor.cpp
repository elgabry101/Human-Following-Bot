#include "Motor.h"


void motor::move_forward(int speed){
    if(speed>16)
        speed=16;
    if(speed<0)
        speed=0;
    speed=abs(speed-16);
    dir.set(1);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,channel,speed);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE,channel);
}
void motor::move_backward(int speed){
    if(speed>16)
        speed=16;
    if(speed<0)
        speed=0;
    dir.set(0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,channel,speed);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE,channel);
}
void motor::stop(){
    dir.set(0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,channel,0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE,channel);

}