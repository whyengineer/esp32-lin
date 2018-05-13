#include "bsp.h"

void bsp_lin_init()
{
    bsp_event_init();

    hal_gpio_init();
    hal_adc_init();
    hal_sdcard_init();
    hal_i2c_init(0,I2C_SDA,I2C_SCL);
    hal_i2s_init(0,48000,16,2);

}