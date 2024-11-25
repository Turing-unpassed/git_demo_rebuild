#include "Action.h"

Action::Action(UART_HandleTypeDef *huart,bool enableCrcCheck, int8_t install_position_x_ , int8_t install_position_y_)
        :SerialDevice(huart,enableCrcCheck),state(WAITING_FOR_HEADER_0),
        install_position_x(install_position_x_),install_position_y(install_position_y_)
{
    
}

void Action::Action_Info_Update(){
    action_info.Angle_Yaw = rx_data.data[0];
    action_info.Angle_Pitch = rx_data.data[1];
    action_info.Angle_Roll = rx_data.data[2];
    action_info.Position_X = rx_data.data[3];
    action_info.Position_Y = rx_data.data[4];
    action_info.W = rx_data.data[5];
}

void Action::handleReceiveData(uint8_t byte){
    switch (state)
    {
    case WAITING_FOR_HEADER_0:
        if (byte == ACTION_FRAME_HEAD_0) // 检查包头的第一个字节
        {
            state = WAITING_FOR_HEADER_1;
        }
        break;

    case WAITING_FOR_HEADER_1:
        if (byte == ACTION_FRAME_HEAD_1) // 检查包头的第二个字节
        {
            state = WAITING_FOR_DATA;
            rxIndex = 0; // 重置接收索引
        }
        else
        {
            state = WAITING_FOR_HEADER_0; // 不是正确的包头，重新开始
        }
        break;

    case WAITING_FOR_DATA:
        rx_data.byte[rxIndex++]; // 保存接收到的字节
        if (rxIndex >= ACTION_DATA_LENGTH)           // 接收到24字节数据
        {
            state = WAITING_FOR_TAIL_0;
        }
        break;

    case WAITING_FOR_TAIL_0:
        if (byte == ACTION_FRAME_TAIL_0) // 检查包尾的第一个字节
        {
            state = WAITING_FOR_TAIL_1;
        }
        else
        {
            state = WAITING_FOR_HEADER_0; // 包尾不匹配，重新开始
        }
        break;

    case WAITING_FOR_TAIL_1:
        if (byte == ACTION_FRAME_TAIL_1) // 检查包尾的第二个字节
        {
            Action_Info_Update();    // 处理接收到的数据
        }
        state = WAITING_FOR_HEADER_0; // 处理完毕后重新等待新包
        break;

    default:
        state = WAITING_FOR_HEADER_0;
        break;
    }
}