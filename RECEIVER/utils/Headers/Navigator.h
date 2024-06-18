#pragma once

#include "motor.h"
#include "Compass.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include "esp_log.h"
#include <vector>
#include "esp_timer.h"

using namespace std;

#define left_right 26
#define mid 13
#define left_right_pow 676
#define mid_pow 169

struct entry{
    int distance;
    float angle;
};

class navigator {
    
protected:
    motor left;
    motor right;
    compass dir=compass();
    int distance=0;
    float angle=0;
    vector<entry> history;
public:
    int lock=0;
    int user_heading=0;
    int car_heading=0;
    navigator(){};
    unsigned long stop_time=0;
    navigator(motor right,motor left):left{left},right{right} {dir.init();};
    void calculate(int distance_l,int distance_m,int distance_r);
    float inrange(float);
    void move();
    void augment_data();
    void update_heading();

};
