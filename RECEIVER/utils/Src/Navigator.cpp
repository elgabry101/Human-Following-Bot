#include "Navigator.h"



extern int f;
extern int b;
extern int l;
extern int r;
extern int h;

entry navigator::calculate()
{
    int distance_l=readings[0];
    int distance_m=readings[1];
    int distance_r=readings[2];
    float theta_l=0,theta_r=0;
    int ret_distance=-1;
    float ret_angle=0;
    if(distance_l>0 && distance_m>0 && distance_r>0)
    {
        
        ret_distance=(distance_l+distance_r+distance_m)/3;
        theta_l= (acos(inrange((left_right_pow+pow(distance_l,2)-pow(distance_r,2))/(2*left_right*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((left_right_pow+pow(distance_r,2)-pow(distance_l,2))/(2*left_right*distance_r)))*180/3.141593)-90;
        
    }
    else if(distance_l>0 && distance_m>0)
    {
        
        ret_distance=(distance_l+distance_m)/2;
        theta_l= (acos(inrange((mid_pow+pow(distance_l,2)-pow(distance_m,2))/(2*mid*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((mid_pow+pow(distance_m,2)-pow(distance_l,2))/(2*mid*distance_m)))*180/3.141593)-90;

    }
    else if(distance_r>0 && distance_m>0)
    {
        
        ret_distance=(distance_r+distance_m)/2;
        theta_l= (acos(inrange((mid_pow+pow(distance_m,2)-pow(distance_r,2))/(2*mid*distance_m)))*180/3.141593)-90;
        theta_r= (acos(inrange((mid_pow+pow(distance_r,2)-pow(distance_m,2))/(2*mid*distance_r)))*180/3.141593)-90;

    }
    else if(distance_r>0 && distance_l>0)
    {
        ret_distance=(distance_l+distance_r)/2;
        theta_l= (acos(inrange((left_right_pow+pow(distance_l,2)-pow(distance_r,2))/(2*left_right*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((left_right_pow+pow(distance_r,2)-pow(distance_l,2))/(2*left_right*distance_r)))*180/3.141593)-90;

    }

    else if(distance_l>0)
    {
        ret_distance=distance_l;
        theta_l= 90;
        theta_r= -90;
    }
    else if(distance_r>0)
    {
        ret_distance=distance_r;
        theta_l= -90;
        theta_r= 90;
    }
    else if(distance_m>0)
    {
        ret_distance=distance_m;
        theta_l= 0;
        theta_r= 0;
    }
    else
    {
        ret_distance=-1;
        theta_l= 0;
        theta_r= 0;
    }


    if(theta_l>2&&theta_l<=90)
    {
        ret_angle=-theta_l;
    }
    else if(theta_r>2&&theta_r<=90)
    {
        ret_angle=theta_r;
    }
    else
    {
        ret_angle=0;
    }
    entry s={ret_distance,ret_angle};
    return s;
}


void navigator::update_history()
{
    if(distance>0&&distance<600&&stop_time<esp_timer_get_time())
    {
        entry s={distance,angle};
        history.push_back(s);

        if(history.size()>100)
        {
            history.erase(history.begin());
        }
    }
    if(main_lock==0)
    {
        history.clear();
    }
    if(web_history.size()>1800)
    {
        web_history.clear();
    }
    else if(web_history.empty())
    {
        web_history.push_back(0);
    }
}


void navigator::move()
{
    if(main_lock)
    {
        float speed= 8 + 0.08 * (distance-140);
        float diffrence= angle*0.05;
        if(speed>12&&(diffrence>3||diffrence<-3))
        {
            speed=12;
        }
        

        if(diffrence<0)
        {
            diffrence=floor(diffrence);
        }
        else if(diffrence>0)
        {
            diffrence=ceil(diffrence);
        }


        if(distance<0)
        {
            if(angle==0)
            {
                right.stop();
                left.stop();
            }
            else if(angle>0&&abs(angle)>30)
            {
                left.move_forward(10);
                right.move_backward(10);
            }
            else if(angle<0&&abs(angle)>30)
            {
                right.move_forward(10);
                left.move_backward(10);
            }

        }
        else if(distance<110)
        {
            left.move_backward(8-(diffrence/2));
            right.move_backward(8+(diffrence/2));
        }
        else if(distance<140)
        {
            right.stop();
            left.stop();
        }
        else
        {
            left.move_forward(speed+diffrence);
            right.move_forward(speed-diffrence);
        }
    }
    else
    {
        if(f==1||(h==1&&web_history.at(web_history.size()-1)==1))
        {
            left.move_forward(8);
            right.move_forward(8);
            if(!h)
            {
                web_history.push_back(2);
            }
            else
            {
                web_history.pop_back();
            }
            
        }
        else if(b==1||(h==1&&web_history.at(web_history.size()-1)==2))
        {
            right.move_backward(8);
            left.move_backward(8);
            if(!h)
            {
                web_history.push_back(1);
            }
            else
            {
                web_history.pop_back();
            }
        }
        else if(r==1||(h==1&&web_history.at(web_history.size()-1)==2))
        {
            left.move_forward(10);
            right.move_backward(10);
            if(!h)
            {
                web_history.push_back(8);
            }
            else
            {
                web_history.pop_back();
            }
        }
        else if(l==1||(h==1&&web_history.at(web_history.size()-1)==2))
        {
            right.move_forward(10);
            left.move_backward(10);
            if(!h)
            {
                web_history.push_back(4);
            }
            else
            {
                web_history.pop_back();
            }
        }
        else
        {
            right.stop();
            left.stop();
            if(!h)
            {
                web_history.push_back(0);
            }
            else
            {
                web_history.pop_back();
            }
        }
        
    }

}


float navigator::inrange(float num)
{
    if(num>1)
        return 1;
    else if(num<-1)
        return -1;
    else
        return num;
}


entry navigator::augment_data()
{
    if(!history.empty())
    {
        int ret_distance=-1;
        float ret_angle=0;
        int weight_window_size=20;
        entry weighted_avg;
            int n = history.size();
        if (n < weight_window_size) {
            weight_window_size = n; // Adjust if history is smaller than the window
        }

        double weighted_sum_distance = 0.0;
        double weighted_sum_angle = 0.0;
        double weight_sum = 0.0;

        for (int i = n - weight_window_size; i < n; ++i) {
            double weight = (i - (n - weight_window_size) + 1); // Increasing weight for more recent readings
            weighted_sum_distance += history[i].distance * weight;
            weighted_sum_angle += history[i].angle * weight;
            weight_sum += weight;
            ESP_LOGI("first loop","     %i         distance %i          angle %f",i,history[i].distance,history[i].angle);
        }

        weighted_avg.distance= weighted_sum_distance / weight_sum;
        weighted_avg.angle= weighted_sum_angle / weight_sum;

        ////////////////////////////////////////////////////////////////////////////////
        double total_distance_diff = 0.0;
        double total_angle_diff = 0.0;
        weight_sum = 0.0;

        for (int i = n - weight_window_size + 1; i < n; ++i) {
            double weight = (i - (n - weight_window_size) + 1); // Increasing weight
            total_distance_diff += (history[i].distance - history[i - 1].distance) * weight;
            total_angle_diff += (history[i].angle - history[i - 1].angle) * weight;
            weight_sum += weight;
        }

        double avg_velocity_distance = total_distance_diff / weight_sum;
        double avg_velocity_angle = total_angle_diff / weight_sum;

        // Predict the next position
        float next_distance = weighted_avg.distance + avg_velocity_distance;
        float next_angle = weighted_avg.angle + avg_velocity_angle;
        ret_distance=next_distance;
        ret_angle=next_angle;
        history.clear();
        entry ret={ret_distance,ret_angle};
        return ret;
    }
    else
    {
        entry ret={-1,0};
        return ret;
    }

}


void navigator::update_heading()
{
    dir.update();
    car_heading=dir.heading;
    int tmp=user_heading-car_heading;
    tmp=(tmp+180)%360;
    if(tmp<0)
    {
        tmp+=360;
    }
    align=tmp-180;
    //ESP_LOGI("heading","car=%f  user=%i   align=%i",dir.heading,user_heading,align);
}


void navigator::make_decision()
{
    entry calc=calculate();

    if(calc.distance==-1)
    {
        distance=-1;
        angle=align;
        //ESP_LOGI("first","distance=%i angle =%f", distance, angle);
    }
    else if(calc.distance>0&&abs(align)<80)
    {
        distance=calc.distance;
        angle=calc.angle;
        count=0;
        //ESP_LOGI("second","distance=%i angle =%f", distance, angle);
    }
    else if(calc.distance>0&&((calc.angle>20&&align<-20)||(calc.angle<-20&&align>20)||abs(calc.angle-align)>=80))
    {

        distance=calc.distance;
        angle=align;
        count=0;
        if(abs(align)>120)
        {
            distance=-1;
        }
        //ESP_LOGI("third","distance= %i angle =%i", distance, align);
    }
    
}