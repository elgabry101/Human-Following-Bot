#include <iostream>
#include "PWM.h"

using namespace std;

class Motor {
private:
    PWM pwm_pin1;
    PWM pwm_pin2;

public:
    Motor(PWM pwm_pin1, PWM pwm_pin2) : pwm_pin1(pwm_pin1), pwm_pin2(pwm_pin2) {}

    void move(const string& direction, int speed) {
        if (direction == "forward") {
            pwm_pin1.set_duty_cycle(speed);
            pwm_pin2.set_duty_cycle(0);
        } else if (direction == "backward") {
            pwm_pin1.set_duty_cycle(0);
            pwm_pin2.set_duty_cycle(speed);
        } else {
            cout << "Invalid direction" << endl;
        }
    }
};

int main() {
    
    PWM pwm_pin1;
    PWM pwm_pin2;
    Motor motor(pwm_pin1, pwm_pin2);
    
    motor.move("forward", 50); 
    motor.move("backward", 75);
    
    return 0;
}