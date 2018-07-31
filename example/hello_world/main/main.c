/* Play mp3 file by audio pipeline

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "bsp.h"

#define TAG "main"

void app_main(void)
{
    bsp_init();
    bsp_event_t bsp_e;
    while(1){

        bsp_event_get(&bsp_e,portMAX_DELAY);
        ESP_LOGI(TAG,"key_event:%d,vol:%d,sd:%d",bsp_e.key_e,bsp_e.vol,bsp_e.sd);
    }
}
