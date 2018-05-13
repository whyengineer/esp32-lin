#include "bsp.h"


#define TAG "BSP_EVENT"

static xQueueHandle bsp_evt_queue = NULL;

static void bsp_event_task(){
    BspEventTypeDef a;
    for(;;) {
        if(xQueueReceive(bsp_evt_queue, &a, portMAX_DELAY)) {
            if(a.event==KEY1_EVT){
                if(a.level==0){
                    hal_gpio_set(LED_R,0);
                    ESP_LOGI(TAG,"key1 press");
                }else{
                    hal_gpio_set(LED_R,1);
                     ESP_LOGI(TAG,"key1 release");
                }
            }else if(a.event==KEY2_EVT){
               if(a.level==0){
                    hal_gpio_set(LED_G,0);
                    ESP_LOGI(TAG,"key2 press");
                }else{
                    hal_gpio_set(LED_G,1);
                    ESP_LOGI(TAG,"key2 release");
                }
            }else if(a.event==KEY3_EVT){
                if(a.level==0){
                    hal_gpio_set(LED_B,0);
                    ESP_LOGI(TAG,"key3 press");
                }else{
                    hal_gpio_set(LED_B,1);
                     ESP_LOGI(TAG,"key3 release");
                }
            }else if(a.event==SD_EVT){
                if(a.level==0){
                    ESP_LOGI(TAG,"sd plug in");
                    hal_sdcard_init(); 
                }else{
                    ESP_LOGI(TAG,"sd plug out");
                    esp_vfs_fat_sdmmc_unmount();
                }
            }
        }
    }
        
    

}
void bsp_event_init(){
    bsp_evt_queue = xQueueCreate(10, sizeof(BspEventTypeDef));
    xTaskCreate(bsp_event_task, "event task", 2048, NULL, 10, NULL);
}
void bsp_event_set(BspEventTypeDef* event){
    xQueueSend(bsp_evt_queue,event , NULL);
}
void bsp_event_set_isr(BspEventTypeDef* event){
    xQueueSendFromISR(bsp_evt_queue,event , NULL);
}

