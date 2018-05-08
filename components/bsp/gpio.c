#include "bsp.h"


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<LED_R) | (1ULL<<LED_G)|(1ULL<<LED_B))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<KEY_1) | (1ULL<<KEY_2)|(1ULL<<KEY_3)|(1ULL<<SD_CD))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
}


void hal_gpio_init()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    //io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //start gpio task


    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(KEY_1, gpio_isr_handler, (void*) KEY_1);
    gpio_isr_handler_add(KEY_2, gpio_isr_handler, (void*) KEY_2);
    gpio_isr_handler_add(KEY_3, gpio_isr_handler, (void*) KEY_3);
    gpio_isr_handler_add(SD_CD, gpio_isr_handler, (void*) SD_CD);


    int cnt = 0;
    while(1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
    }
}
void hal_gpio_set(gpio_num_t gpio_num, uint32_t level)
{
    gpio_set_level(gpio_num, level);
}
int hal_gpio_get(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num_t gpio_num);
}