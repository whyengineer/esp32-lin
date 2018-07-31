#ifndef BSP_H
#define BSP_H


#include "hal_i2c.h"
#include "hal_i2s.h"
#include "wm8978.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"

#define LED_R    5
#define LED_G    21
#define LED_B    22

#define KEY_1    34
#define KEY_2    35
#define KEY_3    32

#define SD_CD    23



typedef enum{
    KEY_1_PUSH=0U,
    KEY_1_RELEASE,
    KEY_2_PUSH,
    KEY_2_RELEASE,
    KEY_3_PUSH,
    KEY_3_RELEASE,
    KEY_NULL,
}key_event_t;

typedef struct{
    uint32_t vol;
    int sd;
    key_event_t key_e;
}bsp_event_t;

void bsp_init();
BaseType_t bsp_event_get(void* buf,TickType_t timeout);


#endif