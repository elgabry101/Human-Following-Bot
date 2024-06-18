#include "Navigator.h"



void navigator::calculate(int distance_l,int distance_m,int distance_r)
{
    float theta_l=0,theta_r=0;
    //ESP_LOGI("distance","left    %i   mid   %i   right %i ",distance_l,distance_m,distance_r);
    if(distance_l>0 && distance_m>0 && distance_r>0)
    {
        
        distance=(distance_l+distance_r+distance_m)/3;
        theta_l= (acos(inrange((left_right_pow+pow(distance_l,2)-pow(distance_r,2))/(2*left_right*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((left_right_pow+pow(distance_r,2)-pow(distance_l,2))/(2*left_right*distance_r)))*180/3.141593)-90;
        
    }
    else if(distance_l>0 && distance_m>0)
    {
        
        distance=(distance_l+distance_m)/2;
        theta_l= (acos(inrange((mid_pow+pow(distance_l,2)-pow(distance_m,2))/(2*mid*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((mid_pow+pow(distance_m,2)-pow(distance_l,2))/(2*mid*distance_m)))*180/3.141593)-90;

    }
    else if(distance_r>0 && distance_m>0)
    {
        
        distance=(distance_r+distance_m)/2;
        theta_l= (acos(inrange((mid_pow+pow(distance_m,2)-pow(distance_r,2))/(2*mid*distance_m)))*180/3.141593)-90;
        theta_r= (acos(inrange((mid_pow+pow(distance_r,2)-pow(distance_m,2))/(2*mid*distance_r)))*180/3.141593)-90;

    }
    else if(distance_r>0 && distance_l>0)
    {
        distance=(distance_l+distance_r)/2;
        theta_l= (acos(inrange((left_right_pow+pow(distance_l,2)-pow(distance_r,2))/(2*left_right*distance_l)))*180/3.141593)-90;
        theta_r= (acos(inrange((left_right_pow+pow(distance_r,2)-pow(distance_l,2))/(2*left_right*distance_r)))*180/3.141593)-90;

    }

    else if(distance_l>0)
    {
        distance=distance_l;
        theta_l= 90;
        theta_r= -90;
    }
    else if(distance_r>0)
    {
        distance=distance_r;
        theta_l= -90;
        theta_r= 90;
    }
    else if(distance_m>0)
    {
        distance=distance_m;
        theta_l= 0;
        theta_r= 0;
    }
    else if(stop_time<esp_timer_get_time())
    {
        distance=-1;
        theta_l= 0;
        theta_r= 0;
        stop_time=esp_timer_get_time()+2000000;
        augment_data();
        return;
    }


    if(theta_l>2&&theta_l<91)
    {
        angle=-theta_l;
    }
    else if(theta_r>2&&theta_r<91)
    {
        angle=theta_r;
    }
    else
    {
        angle=0;
    }

    if(distance>0&&stop_time<esp_timer_get_time())
    {
        entry s={distance,angle};
        history.push_back(s);
        if(history.size()>200)
        {
            history.erase(history.begin());
        }
            
    }
    if(lock==0)
    {
        left.stop();
        right.stop();
        for(int i=0;i<history.size();i++)
        {
            history.pop_back();
        }
    }
}



void navigator::move()
{


    float speed= 8 + 0.08 * (distance-140);
    float diffrence= angle*0.05;

    //ESP_LOGI("calculate","distance    %i     angle   %f ",distance,angle);
    if(diffrence<0)
    {
        diffrence=floor(diffrence);
    }
    else if(diffrence>0)
    {
        diffrence=ceil(diffrence);
    }
    if(distance==-1)
    {
        right.stop();
        left.stop();
    }
    else if(distance<110)
    {
        left.move_backward(10+diffrence);
        right.move_backward(10-diffrence);
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


float navigator::inrange(float num)
{
    if(num>1)
        return 1;
    else if(num<-1)
        return -1;
    else
        return num;
}


void navigator::augment_data()
{
    if(!history.empty())
    {
        int weight_window_size=100;
        entry weighted_avg;
        ESP_LOGI("history","             distance %i          angle %f",history.front().distance,history.front().angle);
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
        double next_distance = weighted_avg.distance + avg_velocity_distance;
        double next_angle = weighted_avg.angle + avg_velocity_angle;
        distance=next_distance;
        angle=next_angle;
        ESP_LOGI("predict","                                                       distance= %lf   angle=%lf",next_distance,next_angle);
        history.clear();
    }

}


void navigator::update_heading()
{
    dir.update();
    car_heading=dir.heading;
}
