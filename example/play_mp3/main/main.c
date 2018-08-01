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
#include "mp3dec.h"


#define TAG "main"

static void play_task(){
    while(1){
        HMP3Decoder hMP3Decoder;
		MP3FrameInfo mp3FrameInfo;
		unsigned char *readBuf=malloc(MAINBUF_SIZE);
		if(readBuf==NULL){
			ESP_LOGE(TAG,"readBuf malloc failed");
			return;
		}
		short *output=malloc(1153*4);
		if(output==NULL){
			free(readBuf);
			ESP_LOGE(TAG,"outBuf malloc failed");
		}
		hMP3Decoder = MP3InitDecoder();
		if (hMP3Decoder == 0){
			free(readBuf);
			free(output);
			ESP_LOGE(TAG,"memory is not enough..");
		}

		int samplerate=0;
		i2s_zero_dma_buffer(0);
		FILE *mp3File=fopen("/sdcard/music1.mp3","rb");
		if(mp3File==NULL){
			MP3FreeDecoder(hMP3Decoder);
			free(readBuf);
			free(output);
			ESP_LOGE(TAG,"open file failed");
		}
		char tag[10];
		int tag_len = 0;
		int read_bytes = fread(tag, 1, 10, mp3File);
		if(read_bytes == 10) 
			 {
				if (memcmp(tag,"ID3",3) == 0) 
				 {
					tag_len = ((tag[6] & 0x7F)<< 21)|((tag[7] & 0x7F) << 14) | ((tag[8] & 0x7F) << 7) | (tag[9] & 0x7F);
						// ESP_LOGI(TAG,"tag_len: %d %x %x %x %x", tag_len,tag[6],tag[7],tag[8],tag[9]);
					fseek(mp3File, tag_len - 10, SEEK_SET);
				 }
				else 
				 {
						fseek(mp3File, 0, SEEK_SET);
				 }
			 }
			 unsigned char* input = &readBuf[0];
			 int bytesLeft = 0;
			 int outOfData = 0;
			 unsigned char* readPtr = readBuf;
			 while (1)
			 {    
	
				 if (bytesLeft < MAINBUF_SIZE)
						{
								memmove(readBuf, readPtr, bytesLeft);
								int br = fread(readBuf + bytesLeft, 1, MAINBUF_SIZE - bytesLeft, mp3File);
								if ((br == 0)&&(bytesLeft==0)) break;
 
								bytesLeft = bytesLeft + br;
								readPtr = readBuf;
						}
				int offset = MP3FindSyncWord(readPtr, bytesLeft);
				if (offset < 0)
				{  
						 ESP_LOGE(TAG,"MP3FindSyncWord not find");
						 bytesLeft=0;
						 continue;
				}
				else
				{
					readPtr += offset;                         //data start point
					bytesLeft -= offset;                 //in buffer
					int errs = MP3Decode(hMP3Decoder, &readPtr, &bytesLeft, output, 0);
					if (errs != 0)
					{
							ESP_LOGE(TAG,"MP3Decode failed ,code is %d ",errs);
							break;
					}
					MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);   
					if(samplerate!=mp3FrameInfo.samprate)
					{
							samplerate=mp3FrameInfo.samprate;
							//hal_i2s_init(0,samplerate,16,mp3FrameInfo.nChans);
							//i2s_set_clk(0,samplerate,16,mp3FrameInfo.nChans);
                            hal_i2s_deinit(0);
                            hal_i2s_init(0,samplerate,mp3FrameInfo.bitsPerSample,mp3FrameInfo.nChans);
							//wm8978_samplerate_set(samplerate);
							ESP_LOGI(TAG,"mp3file info---bitrate=%d,layer=%d,nChans=%d,samprate=%d,outputSamps=%d",mp3FrameInfo.bitrate,mp3FrameInfo.layer,mp3FrameInfo.nChans,mp3FrameInfo.samprate,mp3FrameInfo.outputSamps);
					}   
					hal_i2s_write(0,(char*)output,mp3FrameInfo.outputSamps*2, 1000 / portTICK_RATE_MS);
				}
			
			}
		i2s_zero_dma_buffer(0);
		//i2s_driver_uninstall(0);
		MP3FreeDecoder(hMP3Decoder);
		free(readBuf);
		free(output);  
		fclose(mp3File);
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
