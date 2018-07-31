#include "bsp.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<LED_R) | (1ULL<<LED_B)|(1ULL<<LED_G))

#define GPIO_INPUT_PIN_SEL  ((1ULL<<KEY_1) | (1ULL<<KEY_2)| (1ULL<<KEY_3))
#define ESP_INTR_FLAG_DEFAULT 0


#define DEFAULT_VREF    1100


#define TAG "bsp"


static xQueueHandle gpio_evt_queue = NULL;
static xQueueHandle bsp_evt_queue = NULL;
static esp_adc_cal_characteristics_t *adc_chars;
static bsp_event_t bsp_e;

static void bsp_sdcard_init(){
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%d). "
                "Make sure SD card lines have pull-up resistors in place.", ret);
        }
    }else{
        sdmmc_card_print_info(stdout, card);
    }
}
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
BaseType_t bsp_event_get(void* buf,TickType_t timeout){
    return xQueueReceive(bsp_evt_queue,buf, timeout);
}
static void bsp_poll_task(void* arg){
    uint32_t adc;
    uint32_t sd_det;
    uint32_t adc_reading;
    while(1){
        sd_det=gpio_get_level(SD_CD);
        if(sd_det!=bsp_e.sd){
            bsp_e.sd=sd_det;
            bsp_e.key_e=KEY_NULL;
            xQueueSend(bsp_evt_queue,&bsp_e,portMAX_DELAY);
            if(bsp_e.sd==0){
                bsp_sdcard_init();
            }else{
                esp_vfs_fat_sdmmc_unmount();
            }
        }
        adc_reading = adc1_get_raw((adc1_channel_t)ADC_CHANNEL_0);
        adc = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        if((adc>(bsp_e.vol+10))||(adc<(bsp_e.vol-10))){
            bsp_e.vol=adc;
            bsp_e.key_e=KEY_NULL;
            xQueueSend(bsp_evt_queue,&bsp_e,portMAX_DELAY);
        }
        //printf("Raw: %d\tVoltage: %dmV\n", adc_reading, adc);
        vTaskDelay(500);

    }
}
static void bsp_event_task(void* arg)
{
    uint32_t io_num;
    int l;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            l=gpio_get_level(io_num);
            if(io_num==KEY_1){
                if(l)
                    bsp_e.key_e=KEY_1_RELEASE;
                else    
                   bsp_e.key_e=KEY_1_PUSH; 
            }else if(io_num==KEY_2){
                if(l)
                    bsp_e.key_e=KEY_2_RELEASE;
                else    
                   bsp_e.key_e=KEY_2_PUSH; 
            }else if(io_num==KEY_3){
                if(l)
                    bsp_e.key_e=KEY_3_RELEASE;
                else    
                   bsp_e.key_e=KEY_3_PUSH; 
            }else{
                continue;
            }
            //printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
            xQueueSend(bsp_evt_queue,&bsp_e,portMAX_DELAY);
        }
    }
}
static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}
/*
    init button,led,sd card,codec,adc and so on
*/
void bsp_init(){
    /* codec*/
    hal_i2c_init(0,19,18);
    hal_i2s_init(0,48000,16,2);
    WM8978_Init();
    WM8978_ADDA_Cfg(1,1);
    WM8978_Input_Cfg(1,0,0);
    WM8978_Output_Cfg(1,0);
    WM8978_MIC_Gain(25);
    WM8978_AUX_Gain(0);
    WM8978_LINEIN_Gain(0);
    WM8978_SPKvol_Set(0);
    WM8978_HPvol_Set(15,15);
    WM8978_EQ_3D_Dir(0);
    /*gpio output*/
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    gpio_set_level(LED_R,0);
    gpio_set_level(LED_G,1);
    gpio_set_level(LED_B,1);
    /* gpio input */
    io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;   
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);
    io_conf.intr_type=GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<SD_CD); 
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));

    bsp_evt_queue = xQueueCreate(5, sizeof(bsp_event_t));
  
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(KEY_1, gpio_isr_handler, (void*) KEY_1);
    gpio_isr_handler_add(KEY_2, gpio_isr_handler, (void*) KEY_2);
    gpio_isr_handler_add(KEY_3, gpio_isr_handler, (void*) KEY_3);

    /*  adc1 config */
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL_0, ADC_ATTEN_DB_0);

    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    

    bsp_e.sd=gpio_get_level(SD_CD);

    uint32_t adc_reading = adc1_get_raw((adc1_channel_t)ADC_CHANNEL_0);
    bsp_e.vol=esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);


    if(bsp_e.sd==0){
        bsp_sdcard_init();
    }

    xTaskCreate(bsp_event_task, "bsp_event_task", 2048, NULL, 2, NULL);//high priority
    xTaskCreate(bsp_poll_task, "bsp_poll_task", 2048, NULL, 10, NULL);//low priority


    
}


