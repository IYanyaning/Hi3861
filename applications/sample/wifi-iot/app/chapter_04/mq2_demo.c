#include <stdio.h>
 #include <stdint.h>
 #include <string.h>
 #include <unistd.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"
 #include "wifiiot_gpio.h"
 #include "wifiiot_gpio_ex.h"
 #include "wifiiot_adc.h"
 #include "wifiiot_errno.h"
 #define ADC_TASK_STACK_SIZE 4096
 #define GAS_SENSOR_CHAN_NAME WIFI_IOT_ADC_CHANNEL_5

 typedef  unsigned  char       u8;
 typedef  unsigned  short int  u16;
 typedef  unsigned  long  int  u32;
 typedef  signed    char       s8;
 typedef  signed    short int  s16;
 typedef  signed    long  int  s32;
 static void ADC_Task(void *arg)
 {
    (void)arg;
     GpioInit();
     IoSetPull(WIFI_IOT_IO_NAME_GPIO_8, WIFI_IOT_IO_PULL_UP);
    
    while (1)
    {
        unsigned short data = 0;
        // 该函数通过使用AdcRead()函数来读取 ADC_CHANNEL_5 的数值存储在data中， 
    // WIFI_IOT_ADC_EQU_MODEL_8 表示8次平均算法模式，
    // WIFI_IOT_ADC_CUR_BAIS_DEFAULT 表示默认的自动识别模式，

        
        if (AdcRead(GAS_SENSOR_CHAN_NAME, &data, WIFI_IOT_ADC_EQU_MODEL_8, 
WIFI_IOT_ADC_CUR_BAIS_DEFAULT, 0) == WIFI_IOT_SUCCESS)
        {
            printf("gas:%d ppm \n", data);
        }
        sleep(1);
    }
 }
 static void ADC_MainLoop(void)
 {
    osThreadAttr_t attr;
    attr.name = "ADC_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = ADC_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)ADC_Task, NULL, &attr) == NULL)
    {
        printf("[EnvironmentDemo] Falied to create EnvironmentTask!\n");
    }
 }
 // APP_FEATURE_INIT(ADC_MainLoop);
 SYS_RUN(ADC_MainLoop);