#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
//#include <at.h>
//#include <hi_at.h>

#include "wifi_device.h"

#include "lwip/api_shell.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_i2c.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_adc.h"
#include "wifiiot_errno.h"
#include "sht40.h"
#include "oled_ssd1306.h"
#include "wifi_utils.h"
#include "mqtt_utils.h"

#define SSID "IYanyaning"
#define PASSWD "0000000oo"


#define FAN_PIN WIFI_IOT_IO_NAME_GPIO_8          // GPIO8控制散热风扇开关
#define FAN_PIN_MODE WIFI_IOT_GPIO_DIR_OUT       // GPIO8输出状态
#define FAN_POWER_PIN WIFI_IOT_IO_NAME_GPIO_12   // GPIO12控制散热风扇电源开关
#define FAN_POWER_PIN_MODE WIFI_IOT_GPIO_DIR_OUT // GPIO12输出状态

#define WATER_PUMP_PIN WIFI_IOT_IO_NAME_GPIO_10         // GPIO10控制电磁阀开关
#define WATER_PUMP_PIN_MODE WIFI_IOT_GPIO_DIR_OUT       // GPIO10输出状态、水泵开关
#define WATER_PUMP_POWER_PIN WIFI_IOT_IO_NAME_GPIO_5    // GPIO5控制电磁阀电源
#define WATER_PUMP_POWER_PIN_MODE WIFI_IOT_GPIO_DIR_OUT // GPIO5输出状态
#define WATER_PUMP_ON 1                                 // 水泵开
#define WATER_PUMP_OFF 0                                // 水泵关
#define WATER_PUMP_POWER_ON 1                           // 水泵电源开
#define WATER_PUMP_POWER_OFF 0                          // 
int ARTIFICIAL_FAN = -1;                                //人为控制风扇
int ARTIFICIAL_WATER = -1;                              //人为控制水泵
int Fan_State = -1;                                     //风扇初始状态
int Water_State = -1;                                   //水泵初始状态

#define GAS_SENSOR_CHAN_NAME WIFI_IOT_ADC_CHANNEL_5
#define SAND_SENSOR_CHAN_NAME WIFI_IOT_ADC_CHANNEL_4 // 土壤湿度
#define SHT40_BAUDRATE (400 * 1000)
#define SHT40_I2C_IDX 0
#define ADC_RESOLUTION 2048
#define TASK_STACK_SIZE 4096

typedef struct
{
    uint8_t Temp;  //温度
    uint8_t TempThreshold;  //风扇开启对应温度阈值
}TS_TEMP_PARAM;


typedef struct
{
    uint8_t Humidity;      // 土壤湿度值
    uint8_t HumiThreshold; // 水泵开启对应湿度阈值
} TS_SOIL_PARAM;

static TS_TEMP_PARAM t_Temp = {0};
static TS_SOIL_PARAM s_Soil = {0};

uint32_t retval = 0;
float humidity = 0.0f;
float temperature = 0.0f;
unsigned short gasdata = 0;
unsigned short sandhum = 0;

static char line[32] = {0};



/*
* 函数名称 : Set_Water_Pump_Onoff_Threshold
* 功能描述 : 设置水阀开发湿度阈值
* 参    数 : HumiThreshold - 水泵开启对应湿度阈值
* 返回值   : 0 - 设置成功
            -1 - 设置失败
* 示    例 : result = Set_Water_Pump_Onoff_Threshold(HumiThreshold);
*/
/******************************************************************************/
char Set_Water_Pump_Onoff_Threshold(uint8_t HumiThreshold)
/******************************************************************************/
{
    s_Soil.HumiThreshold = HumiThreshold;
    return 0;
}

/*
 * 函数名称 : Get_Water_Pump_Onoff_Threshold
 * 功能描述 : 获取水阀开发湿度阈值
 * 参    数 : 空
 * 返回值   : 湿度阈值
 * 示    例 : HumiThreshold = Set_Water_Pump_Onoff_Threshold();
 */
/******************************************************************************/
uint8_t Get_Water_Pump_Onoff_Threshold(void)
/******************************************************************************/
{

    return s_Soil.HumiThreshold; // 水泵开启对应湿度阈值
}

/*
 * 函数名称 : Get_Soil_Humi
 * 功能描述 : 获取土壤湿度
 * 参    数 : 空
 * 返回值   : 土壤湿度
 * 示    例 : Humidity = Get_Soil_Humi();
 */
/******************************************************************************/
uint8_t Get_Soil_Humi(void)
/******************************************************************************/
{

    return s_Soil.Humidity; // 传递湿度值
}

void Api_Init(void)
{

    GpioInit();                        // GPIO初始化
    OledInit();                        // OLED初始化
    OledFillScreen(0);                 // LOED显示屏清屏
    GpioSetDir(FAN_PIN, FAN_PIN_MODE); // 设置GPIO工作状态
    GpioSetOutputVal(FAN_PIN, 0);      // 关闭风扇

    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_FUNC_GPIO_12_GPIO);

    GpioSetDir(FAN_POWER_PIN, FAN_POWER_PIN_MODE); // 设置状态
    GpioSetOutputVal(FAN_POWER_PIN, 0);

    GpioSetDir(WATER_PUMP_PIN, WATER_PUMP_PIN_MODE);
    GpioSetOutputVal(WATER_PUMP_PIN, 0); // 关闭水泵

    GpioSetDir(WATER_PUMP_POWER_PIN, WATER_PUMP_POWER_PIN_MODE);
    GpioSetOutputVal(WATER_PUMP_POWER_PIN, 0); // 关闭水泵电源
    IoSetFunc(WATER_PUMP_POWER_PIN, WIFI_IOT_IO_FUNC_GPIO_5_GPIO);
    t_Temp.TempThreshold = 25;//温度阈值默认25
    s_Soil.HumiThreshold = 1; //阈值默认50

    I2cInit(SHT40_I2C_IDX, SHT40_BAUDRATE); // 配置I2C的设备和时钟频率
}

static void MQTT_Task(void *arg)
{
    (void)arg;
    connect_wifi(SSID, PASSWD);

    mqtt_connect();
}

static void Environment_Task(void *arg)
{
    (void)arg;
    Api_Init();

    while (1)
    {

        // 发送命令，开始测量
        if (SHT40_StartMeasure() != WIFI_IOT_SUCCESS)
        {
            printf("trigger measure failed!\r\n");
        }
        usleep(20 * 1000);
        // 获取温湿度结果
        if (SHT40_GetMeasureResult(&temperature, &humidity) != WIFI_IOT_SUCCESS)
        {
            printf("get humidity data failed!\r\n");
        }
        // 获取MQ2传感器结果

        AdcRead(SAND_SENSOR_CHAN_NAME, &sandhum, WIFI_IOT_ADC_EQU_MODEL_8,
                WIFI_IOT_ADC_CUR_BAIS_DEFAULT, 0);

        float voltage = sandhum / (0xFF + 1) * 3.3;
        if (voltage > 3.3)
        {
            voltage = 3.3;
        }
        sandhum = (100 - (unsigned short)((voltage / 3.3) * 100));

        // OLED显示
        OledShowString(0, 0, "Sensor values:", 1);
        snprintf(line, sizeof(line), "temp: %.2f", temperature);
        OledShowString(0, 1, line, 1);
        snprintf(line, sizeof(line), "humi: %.2f", humidity);
        OledShowString(0, 2, line, 1);
        // snprintf(line, sizeof(line), "g:%d, s:%d", gasdata,sandhum);
        snprintf(line, sizeof(line), "soilhumi: %d%%", sandhum);
        OledShowString(0, 3, line, 1);
        printf("temp:%.2f, humi:%.2f, soilhumi:%d\n", temperature, humidity, sandhum);
        get_f(temperature, humidity, sandhum);

        if (change_fan() == 0 || ARTIFICIAL_FAN == 0)
        {
            if (fan_switch() == 0)
            { // 开启风扇和风扇继电器
                GpioSetOutputVal(FAN_POWER_PIN, 1);
                GpioSetOutputVal(FAN_PIN, 1);
                ARTIFICIAL_FAN = 0;
                Fan_State = 0;
            }
            else
            {
                GpioSetOutputVal(FAN_POWER_PIN, 0);
                GpioSetOutputVal(FAN_PIN, 0);
                ARTIFICIAL_FAN = -1;
                Fan_State = -1;
            }
            printf("come in change\n");
        }else if(temperature >=change_tempThreshold() ){
                GpioSetOutputVal(FAN_POWER_PIN, 1);
                GpioSetOutputVal(FAN_PIN, 1);
                ARTIFICIAL_FAN = -1;
                printf("come in >\n");
                Fan_State = 0;
        }else{
                GpioSetOutputVal(FAN_POWER_PIN, 0);
                GpioSetOutputVal(FAN_PIN, 0);
                ARTIFICIAL_FAN = -1;
                printf("come in <\n");
                Fan_State = -1;
        }
        /*if(sandhum <= s_Soil.HumiThreshold){
            GpioSetOutputVal(WATER_PUMP_PIN,1);
            GpioSetOutputVal(WATER_PUMP_POWER_PIN,1);
        }else{
            GpioSetOutputVal(WATER_PUMP_POWER_PIN,0);
            GpioSetOutputVal(WATER_PUMP_PIN,0);
        }*/

        if (change_water() == 0 || ARTIFICIAL_WATER == 0)
        {
            if (water_switch() == 0)
            { // 开启水泵和水泵继电器
                GpioSetOutputVal(WATER_PUMP_POWER_PIN, 1);
                GpioSetOutputVal(WATER_PUMP_PIN, 1);
                ARTIFICIAL_WATER = 0;
                Water_State = 0;
            }
            else
            {
                GpioSetOutputVal(WATER_PUMP_POWER_PIN, 0);
                GpioSetOutputVal(WATER_PUMP_PIN, 0);
                ARTIFICIAL_WATER = -1;
                Water_State = -1;
                
            }
        }else if(sandhum <= change_humiThreshold()){
            GpioSetOutputVal(WATER_PUMP_PIN,1);
            GpioSetOutputVal(WATER_PUMP_POWER_PIN,1);
            ARTIFICIAL_WATER = -1;
            Water_State = 0;
        }else {
            GpioSetOutputVal(WATER_PUMP_POWER_PIN,0);
            GpioSetOutputVal(WATER_PUMP_PIN,0);
            ARTIFICIAL_WATER = -1;
            Water_State = -1;
        }

        get_state(Fan_State,Water_State);        

        sleep(3);
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
    attr.priority = osPriorityNormal1;

    osThreadAttr_t attr1;
    attr1.name = "MQTT_Task";
    attr1.attr_bits = 0U;
    attr1.cb_mem = NULL;
    attr1.cb_size = 0U;
    attr1.stack_mem = NULL;
    attr1.stack_size = TASK_STACK_SIZE;
    attr1.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Environment_Task, NULL, &attr) == NULL)
    {
        printf("[EnvironmentDemo] Falied to create EnvironmentTask!\n");
    }
    if (osThreadNew((osThreadFunc_t)MQTT_Task, NULL, &attr1) == NULL)
    {
        printf("[EnvironmentDemo] Falied to create EnvironmentTask!\n");
    }
}
APP_FEATURE_INIT(Environment_MainLoop);