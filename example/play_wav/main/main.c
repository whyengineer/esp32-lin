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
#include "util.h"

#define TAG "main"

static void play_task(){
    while(1){
        wav_header_t wav_head;
        FILE *f= fopen("/sdcard/music1.wav", "r");
        if (f == NULL) {
                ESP_LOGE(TAG,"Failed to open file");
                return;
        }
        int rlen=fread(&wav_head,1,sizeof(wav_head),f);
        if(rlen!=sizeof(wav_head)){
                ESP_LOGE(TAG,"read faliled");
                return;
        }
        int channels = wav_head.wChannels;
        int frequency = wav_head.nSamplesPersec;
        int bit = wav_head.wBitsPerSample;
        int datalen= wav_head.wSampleLength;
        (void)datalen;
        ESP_LOGI(TAG,"channels:%d,frequency:%d,bit:%d\n",channels,frequency,bit);
        hal_i2s_deinit(0);
        hal_i2s_init(0,frequency,bit,channels);
        char* samples_data = malloc(1024);
        do{
            rlen=fread(samples_data,1,1024,f);
            //datalen-=rlen;
            hal_i2s_write(0,samples_data,rlen,5000);
        }while(rlen>0);
        fclose(f);
        free(samples_data);
        f=NULL;
    }
}
void app_main(void)
{
    bsp_init();
    bsp_event_t bsp_e;
    xTaskCreate(play_task, "play_task", 4096, NULL, 9, NULL);//low priority
    while(1){

        bsp_event_get(&bsp_e,portMAX_DELAY);
        ESP_LOGI(TAG,"key_event:%d,vol:%d,sd:%d",bsp_e.key_e,bsp_e.vol,bsp_e.sd);
    }
}
