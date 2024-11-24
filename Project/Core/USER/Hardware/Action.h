#ifndef ACTION_H
#define ACTION_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "SerialDevice.h"
#include <stdint.h>

/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#define ACTION_FRAME_HEAD_0 0x0D
#define ACTION_FRAME_HEAD_1 0x0A
#define ACTION_FRAME_TAIL_0 0x0A
#define ACTION_FRAME_TAIL_1 0x0D
#define ACTION_DATA_LENGTH 24  // 数据段长度（24字节）
#define ACTION_FRAME_LENGTH 28 // 整个包的长度（头+数据+尾）

/*机器人坐标和全局坐标定义:机器人前方为y轴正方向，机器人左侧为x轴正方向，角速度顺时针为正，角度范围为顺时针0到180，逆时针0到-180*/

typedef struct {
    float Position_X; // 机器人x坐标
    float Position_Y; // 机器人y坐标
    float W;          // 机器人角速度
    float Angle_Pitch;    // 机器人x轴角度(俯仰角)
    float Angle_Roll;    // 机器人y轴角度（横滚角）
    float Angle_Yaw;    // 机器人z轴角度 (偏航角)
}Action_Info;

typedef union{
    float data[ACTION_DATA_LENGTH/4];
    uint8_t byte[ACTION_DATA_LENGTH];
}Action_RxData;

class Action public:SerialDevice
{
public:
    Action(UART_HandleTypeDef *huart, bool enableCrcCheck =true, int8_t install_position_x_ = 0, int8_t install_position_y_ = 0);
    void Action_Info_Update();
    void handleReceiveData(uint8_t byte);
private:
    uint8_t rxIndex = 0;
    Action_Info action_info;
    Action_RxData rx_data;
    int8_t install_position_x;
    int8_t install_position_y;
    enum RxState
    {
        WAITING_FOR_HEADER_0,
        WAITING_FOR_HEADER_1,
        WAITING_FOR_DATA,
        WAITING_FOR_TAIL_0,
        WAITING_FOR_TAIL_1
    } state;
    
};

#endif

#endif