#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "aht20.h"
#include "wifiiot_i2c.h"
#include "wifiiot_errno.h"

static uint32_t AHT20_Read(uint8_t* buffer, uint32_t buffLen) {
    WifiIotI2cData data = { 0 };
    data.receiveBuf = buffer;
    data.receiveLen = buffLen;
    uint32_t retval = I2cRead(AHT20_I2C_IDX, AHT20_READ_ADDR, &data);
    if (retval != WIFI_IOT_SUCCESS)
    {
        printf("I2cRead() failed, %0X!\n", retval);
        return retval;
    }
    return WIFI_IOT_SUCCESS;
}

static uint32_t AHT20_Write(uint8_t* buffer, uint32_t buffLen) {
    WifiIotI2cData data = { 0 };
    data.sendBuf = buffer;
    data.sendLen = buffLen;
    uint32_t retval = I2cWrite(AHT20_I2C_IDX, AHT20_WRITE_ADDR, &data);
    if (retval != WIFI_IOT_SUCCESS)
    {
        printf("I2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }
    return WIFI_IOT_SUCCESS;
}

// 发送 触发测量 命令，开始测量
uint32_t AHT20_StartMeasure(void) {
    uint8_t triggerCmd[] = {AHT20_CMD_TRIGGER};
    return AHT20_Write(triggerCmd, sizeof(triggerCmd));
}

// 接收测量结果，拼接转换为标准值
uint32_t AHT20_GetMeasureResult(float* temp, float* humi) {
    uint32_t retval = 0;

    float t_degC=0;
    float rh_pRH=0;
    float t_ticks=0.0;
    float rh_ticks=0.0;
    
    if (temp == NULL || humi == NULL) 
    {
        return -1;
    }

    uint8_t buffer[SHT40_STATUS_RESPONSE_MAX] = { 0 };
    memset(&buffer, 0x0, sizeof(buffer));
    retval = AHT20_Read(buffer, sizeof(buffer));  // recv status command result
    if (retval != 0) 
    {
        return retval;
    }

    t_ticks=buffer[0]*256+buffer[1];
    rh_ticks=buffer[3]*256+buffer[4];
    t_degC=-45+175*t_ticks/65535;
    rh_pRH=-6+125*rh_ticks/65535; 
    if(rh_pRH >= 100)
    {
        rh_pRH = 100;
    }
    if(rh_pRH < 0)
    {
        rh_pRH = 0;
    }
    *humi = (uint8_t)rh_pRH;

    *temp = (uint8_t)t_degC;

    return retval;
}
