#ifndef AHT20_H
#define AHT20_H

#include <stdint.h>

#define AHT20_I2C_IDX 0
#define AHT20_STARTUP_TIME 20 * 1000 // 上电启动时间
#define AHT20_CALIBRATION_TIME 40 * 1000 // 初始化（校准）时间
#define AHT20_MEASURE_TIME 75 * 1000 // 测量时间
#define AHT20_DEVICE_ADDR 0x44  //38  1110 0000
//#define AHT20_DEVICE_ADDR 0x38     //1110 110 
#define AHT20_READ_ADDR ((0x44<<1)|0x1)      //110 000  0000 0001
#define AHT20_WRITE_ADDR ((0x44<<1)|0x0)  //11100 001  00000001


#define AHT20_CMD_TRIGGER 0xFD // 触发测量命令
#define SHT40_STATUS_RESPONSE_MAX   6                   //读取传感器数据长度



// 发送 触发测量 命令，开始测量
uint32_t AHT20_StartMeasure(void);
// 接收测量结果，拼接转换为标准值
uint32_t AHT20_GetMeasureResult(float* temp, float* humi);
#endif // AHT20_H
