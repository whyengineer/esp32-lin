#ifndef BSP_H
#define BSP_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "board.h"


#define WM8978_RESET				0x00
#define WM8978_POWER_MANAGEMENT_1		0x01
#define WM8978_POWER_MANAGEMENT_2		0x02
#define WM8978_POWER_MANAGEMENT_3		0x03
#define WM8978_AUDIO_INTERFACE			0x04
#define WM8978_COMPANDING_CONTROL		0x05
#define WM8978_CLOCKING				0x06
#define WM8978_ADDITIONAL_CONTROL		0x07
#define WM8978_GPIO_CONTROL			0x08
#define WM8978_JACK_DETECT_CONTROL_1		0x09
#define WM8978_DAC_CONTROL			0x0A
#define WM8978_LEFT_DAC_DIGITAL_VOLUME		0x0B
#define WM8978_RIGHT_DAC_DIGITAL_VOLUME		0x0C
#define WM8978_JACK_DETECT_CONTROL_2		0x0D
#define WM8978_ADC_CONTROL			0x0E
#define WM8978_LEFT_ADC_DIGITAL_VOLUME		0x0F
#define WM8978_RIGHT_ADC_DIGITAL_VOLUME		0x10
#define WM8978_EQ1				0x12
#define WM8978_EQ2				0x13
#define WM8978_EQ3				0x14
#define WM8978_EQ4				0x15
#define WM8978_EQ5				0x16
#define WM8978_DAC_LIMITER_1			0x18
#define WM8978_DAC_LIMITER_2			0x19
#define WM8978_NOTCH_FILTER_1			0x1b
#define WM8978_NOTCH_FILTER_2			0x1c
#define WM8978_NOTCH_FILTER_3			0x1d
#define WM8978_NOTCH_FILTER_4			0x1e
#define WM8978_ALC_CONTROL_1			0x20
#define WM8978_ALC_CONTROL_2			0x21
#define WM8978_ALC_CONTROL_3			0x22
#define WM8978_NOISE_GATE			0x23
#define WM8978_PLL_N				0x24
#define WM8978_PLL_K1				0x25
#define WM8978_PLL_K2				0x26
#define WM8978_PLL_K3				0x27
#define WM8978_3D_CONTROL			0x29
#define WM8978_BEEP_CONTROL			0x2b
#define WM8978_INPUT_CONTROL			0x2c
#define WM8978_LEFT_INP_PGA_CONTROL		0x2d
#define WM8978_RIGHT_INP_PGA_CONTROL		0x2e
#define WM8978_LEFT_ADC_BOOST_CONTROL		0x2f
#define WM8978_RIGHT_ADC_BOOST_CONTROL		0x30
#define WM8978_OUTPUT_CONTROL			0x31
#define WM8978_LEFT_MIXER_CONTROL		0x32
#define WM8978_RIGHT_MIXER_CONTROL		0x33
#define WM8978_LOUT1_HP_CONTROL			0x34
#define WM8978_ROUT1_HP_CONTROL			0x35
#define WM8978_LOUT2_SPK_CONTROL		0x36
#define WM8978_ROUT2_SPK_CONTROL		0x37
#define WM8978_OUT3_MIXER_CONTROL		0x38
#define WM8978_OUT4_MIXER_CONTROL		0x39


typedef enum{
    KEY1_EVT=0U,
    KEY2_EVT,
    KEY3_EVT,
    SD_EVT
}BspEventList;

typedef struct{
    uint32_t tick;
    BspEventList event;
    int level;
}BspEventTypeDef;


void bsp_lin_init();

//gpio
void hal_gpio_init();
void hal_gpio_set(gpio_num_t gpio_num, uint32_t level);
int hal_gpio_get(gpio_num_t gpio_num);

void hal_adc_init();
uint32_t hal_adc_read();


//event 
void bsp_event_init();
void bsp_event_set_isr(BspEventTypeDef* event);
void bsp_event_set(BspEventTypeDef* event);


//sd card
void hal_sdcard_init();

//i2c 
void hal_i2c_init(uint8_t port ,uint8_t sda,uint8_t scl);
esp_err_t hal_i2c_master_mem_write(i2c_port_t i2c_num, uint8_t DevAddr,uint8_t MemAddr,uint8_t* data_wr, size_t size);
esp_err_t hal_i2c_master_mem_read(i2c_port_t i2c_num, uint8_t DevAddr,uint8_t MemAddr,uint8_t* data_rd, size_t size);

//i2s
void hal_i2s_init(uint8_t i2s_num,uint32_t rate,uint8_t bits,uint8_t ch);
int hal_i2s_read(uint8_t i2s_num,char* dest,size_t size,TickType_t timeout);
int hal_i2s_write(uint8_t i2s_num,char* dest,size_t size,TickType_t timeout);







#endif