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
#include "xtensa_math.h"

static const char *TAG = "DSP";

static float cof[38]={-0.00028361,-0.00075393,-0.00109268,-0.0006769,0.00102914,0.00391172,\
  0.00678037,0.0074884,0.00382395,-0.00503141,-0.0171363,-0.02737452,\
 -0.02859017,-0.01418401,0.01873238,0.0670341,0.12139937,0.16869521,\
  0.19622892,0.19622892,0.16869521,0.12139937,0.0670341,0.01873238,\
 -0.01418401,-0.02859017,-0.0273745,-0.0171363,-0.00503141,0.00382395,\
  0.0074884,0.00678037,0.00391172,0.00102914,-0.0006769,-0.00109268,\
 -0.00075393,-0.00028361};

static float x[64]={1.1016856340840009,1.3819116564898124,1.2902960763808005,1.0566626343732346,
    0.9442254510513154,0.9579868821695956,1.046306253339488,1.2410366852938386,
    1.4436282176750759,1.3874515506551908,1.0378805602935988,0.7756155443385301,
    0.9178050511383941,1.2537702552749763,1.3426204627941956,1.117610065929717,
    0.8800604905211858,0.8012517778630224,0.7821389128510301,0.7711632039505969,
    0.8501510692896972,0.951564312477108,0.8436661347816815,0.5268377902750552,
    0.33441324855963317,0.4608704807237677,0.660838911589431,0.6315802275119023,
    0.4503064294285132,0.3690546542314113,0.3830365542353578,0.32286699253190193,
    0.23477111445056748,0.30990336758188164,0.4885556520187167,0.49487230610275945,
    0.29017088466245566,0.15479252877059746,0.24169920785325388,0.3670607943691402,
    0.36701157780354643,0.3520189111198753,0.42271529556318366,0.4225052367712395,
    0.23092439222513883,0.06926710273608785,0.1994021343386783,0.4723172130423151,
    0.5111771910438687,0.2622959422441363,0.03179207010391415,0.00435686084990973,
    0.060746245121383116,0.09208186182560787,0.14769880330463428,0.18278609616995745,
    0.004175541124054452,-0.36016119316009115,-0.5430364808518742,-0.33032288155563755,
    -0.031070289589745434,-0.0789812885502211,-0.444599870929681,-0.7399276026332652};


void calculate(){
    float res[64]={0.0};
    float state[64+37]={0.0};
    int i;
    /****** fir *******/
    xtensa_fir_instance_f32 fir;
    //float* state=malloc(sizeof(float32_t)*(38+64-1));
    xtensa_fir_init_f32(&fir,38,cof,state,64);
    xtensa_fir_f32(&fir,x,res,64);
    ESP_LOGI(TAG,"FIR Result:")
    for(i=0;i<64;i=i+4){
        ESP_LOGI(TAG,"[%f,%f,%f,%f]",res[i],res[i+1],res[i+2],res[i+3]);
    }
    /****** rfft ******/
    xtensa_rfft_fast_instance_f32 rfft;
    xtensa_rfft_fast_init_f32(&rfft,64);
    xtensa_rfft_fast_f32(&rfft,x,res,0);
    ESP_LOGI(TAG,"RFFT Result:")
    for(i=0;i<64;i=i+4){
        ESP_LOGI(TAG,"[%f,%f,%f,%f]",res[i],res[i+1],res[i+2],res[i+3]);
    }
#if 1
    float mag[33]={0};
    configASSERT(mag!=NULL);
    xtensa_cmplx_mag_f32(res,mag,64);
    mag[0]=res[0];
    mag[32]=res[1];
    ESP_LOGI(TAG,"RFFT Magnitude Result:")
    for(int i=0;i<32;i=i+4){
        ESP_LOGI(TAG,"[%f,%f,%f,%f]",mag[i],mag[i+1],mag[i+2],mag[i+3]);
    }
    ESP_LOGI(TAG,"[%f]",mag[32]);
#endif
    xtensa_rfft_fast_instance_f32 rifft;
    xtensa_rfft_fast_init_f32(&rifft,64);
    float ifft_res[64];
    xtensa_rfft_fast_f32(&rifft,res,ifft_res,1);
    ESP_LOGI(TAG,"IRFFT Result:")
    for(i=0;i<64;i=i+4){
        ESP_LOGI(TAG,"[%f,%f,%f,%f]",ifft_res[i],ifft_res[i+1],ifft_res[i+2],ifft_res[i+3]);
    }
    vTaskSuspend(NULL);
}
void app_main(void)
{
    //add and mul
    float x[2]={0.333,1.2};
    float y[2]={32.1,1000.2};
    float res[2]={0};
    xtensa_add_f32(x,y,res,2);
    ESP_LOGI(TAG,"%f + %f =%f",x[0],y[0],res[0]);
    ESP_LOGI(TAG,"%f + %f =%f",x[1],y[1],res[1]);
    //sin and cos
    res[0]=xtensa_sin_f32(0.1);
    ESP_LOGI(TAG,"sin(%f)=%f",0.1,res[0]);
    res[0]=xtensa_cos_f32(2.0);
    ESP_LOGI(TAG,"cos(%f)=%f",2.0,res[0]);
    //fir 

    
    xTaskCreate(calculate,"cal",1024*3,NULL,3,NULL);
    vTaskSuspend(NULL);

}
