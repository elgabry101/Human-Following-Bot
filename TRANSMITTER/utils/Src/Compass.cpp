#include "Compass.h"


esp_err_t compass::init()
{
    
    esp_err_t ret=0;
    ret|=write(0x00, 0b01110000);
    ret|=write(0x01, 0b00100000);
    ret|=write(0x02, 0x00);
    return ret;
}

esp_err_t compass::write(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, Sensor_Address, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

esp_err_t compass::read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, Sensor_Address, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void compass::update()
{
        uint8_t data[6];
        read(0x03, data, 6);
        int x=((int8_t)data[0]<<8|data[1])+177.107914;
        int y=((int8_t)data[4]<<8|data[5])+42.218594;
        int x_final=0.223917*x+0.003387*y;
        int y_final=0.003387*x+0.221382*y;
        heading = (atan2(y_final,x_final)*180.0 /M_PI);
        if(heading < 0)
            heading += 360;
        
}