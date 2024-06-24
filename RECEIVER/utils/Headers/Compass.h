#pragma once

#include "esp_log.h"
#include "driver/i2c.h"
#include "math.h"

#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           15 
#define I2C_MASTER_SDA_IO           5                             
#define I2C_MASTER_FREQ_HZ          100000                   
#define I2C_MASTER_TX_BUF_DISABLE   0                        
#define I2C_MASTER_RX_BUF_DISABLE   0                      
#define I2C_MASTER_TIMEOUT_MS       1000
#define Sensor_Address              0x1E         

class compass
{
protected:
    
    esp_err_t write(uint8_t reg_addr, uint8_t data);
    esp_err_t read(uint8_t reg_addr, uint8_t *data, size_t len);
public:
    int readings[2];
    float heading;
    compass()
    {
    i2c_port_t i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed=I2C_MASTER_FREQ_HZ}
    };

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    };
    esp_err_t init();
    void update();
};