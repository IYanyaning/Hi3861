#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_i2c.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_adc.h"
#include "wifiiot_errno.h"
#include "aht20.h"
#include "oled_ssd1306.h"


#define FAN_PIN                     WIFI_IOT_IO_NAME_GPIO_8                 //GPIO8控制散热风扇开关
#define FAN_PIN_MODE                WIFI_IOT_GPIO_DIR_OUT    //GPIO8输出状态
#define FAN_POWER_PIN               WIFI_IOT_IO_NAME_GPIO_12                //GPIO12控制散热风扇电源开关
#define FAN_POWER_PIN_MODE          WIFI_IOT_GPIO_DIR_OUT    //GPIO12输出状态

#define GAS_SENSOR_CHAN_NAME WIFI_IOT_ADC_CHANNEL_5
#define SAND_SENSOR_CHAN_NAME WIFI_IOT_ADC_CHANNEL_4//土壤湿度
#define AHT20_BAUDRATE (400 * 1000)
#define AHT20_I2C_IDX 0
#define ADC_RESOLUTION 2048
#define TASK_STACK_SIZE 4096


uint32_t retval = 0;
float humidity = 0.0f;
float temperature = 0.0f;
unsigned short gasdata = 0;
unsigned short sandhum = 0;

static char line[32] = {0};

void Api_Init(void)
{
    GpioInit();                             // GPIO初始化
    OledInit();                             // OLED初始化
    OledFillScreen(0);                      // LOED显示屏清屏
    GpioSetDir(FAN_PIN, FAN_PIN_MODE);//设置GPIO工作状态
    GpioSetOutputVal(FAN_PIN, 0);//关闭风扇

    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_FUNC_GPIO_12_GPIO);

    GpioSetDir(FAN_POWER_PIN, FAN_POWER_PIN_MODE);//设置状态
    GpioSetOutputVal(FAN_POWER_PIN,0);

    //IoSetFunc(WIFI_IOT_IO_NAME_GPIO_13, WIFI_IOT_IO_FUNC_GPIO_13_I2C0_SDA);
    //IoSetFunc(WIFI_IOT_IO_NAME_GPIO_14, WIFI_IOT_IO_FUNC_GPIO_14_I2C0_SCL);
    
    I2cInit(AHT20_I2C_IDX, AHT20_BAUDRATE); // 配置I2C的设备和时钟频率
    
    // 发送初始化校准命令
    //uint32_t ret  = AHT20_Calibrate();
    /*while (WIFI_IOT_SUCCESS != AHT20_Calibrate())
    {
        printf("AHT20 sensor init failed!\r\n");
        break;
        usleep(1000);
    }*/
}

static void Environment_Task(void *arg)
{
    (void)arg;
    Api_Init();
    while (1)
    {
        // 发送命令，开始测量
        if (AHT20_StartMeasure() != WIFI_IOT_SUCCESS)
        {
            printf("trigger measure failed!\r\n");
        }
        usleep(20*1000);
        // 获取温湿度结果                                                                                                                                                                                                                    
        if (AHT20_GetMeasureResult(&temperature, &humidity) != WIFI_IOT_SUCCESS)
        {
            printf("get humidity data failed!\r\n");
        }
        // 获取MQ2传感器结果

        AdcRead(SAND_SENSOR_CHAN_NAME, &sandhum, WIFI_IOT_ADC_EQU_MODEL_8,
        WIFI_IOT_ADC_CUR_BAIS_DEFAULT, 0);
        
        float voltage = sandhum/(0xFF + 1) * 3.3;
        if(voltage > 3.3){
            voltage = 3.3;
        }
        sandhum =  (100-(unsigned short)((voltage/3.3)*100));
        
        // OLED显示
        OledShowString(0, 0, "Sensor values:", 1);
        snprintf(line, sizeof(line), "temp: %.2f", temperature);
        OledShowString(0, 1, line, 1);
        snprintf(line, sizeof(line), "humi: %.2f", humidity);
        OledShowString(0, 2, line, 1);
        //snprintf(line, sizeof(line), "g:%d, s:%d", gasdata,sandhum);
        snprintf(line, sizeof(line), "soilhuni: %d", sandhum);
        OledShowString(0, 3, line, 1);
        if(temperature > 20 ){
            GpioSetOutputVal(FAN_PIN,1);
            GpioSetOutputVal(FAN_POWER_PIN,1);
        }else{
            GpioSetOutputVal(FAN_PIN,0);
            GpioSetOutputVal(FAN_POWER_PIN,0);
        }
        sleep(1);
    }
}

static void Environment_MainLoop(void)
{
    osThreadAttr_t attr;
    attr.name = "Environment_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)Environment_Task, NULL, &attr) == NULL)
    {
        printf("[EnvironmentDemo] Falied to create EnvironmentTask!\n");
    }
}

APP_FEATURE_INIT(Environment_MainLoop);
