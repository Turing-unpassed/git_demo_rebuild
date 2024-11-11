#ifndef CAN_DEVICE_H
#define CAN_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "can.h"
#include <stdint.h>
#include "TaskManager.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#define MAX_INSTANCES 4 // 一条can上最多挂四个3508

enum Can_id  // 定义M3508和GM6020的can设备id
{
    m3508_id_1 = 0x201,
    m3508_id_2 = 0x202,
    m3508_id_3 = 0x203,
    m3508_id_4 = 0x204,

    gm6020_id_1 = 0x205,
    gm6020_id_2 = 0x206,
    gm6020_id_3 = 0x207,
    gm6020_id_4 = 0x208

};

class CanDevice
{
public:
    CAN_HandleTypeDef *hcan = nullptr;  // CAN 句柄
    uint8_t can_id = 0;  //保存每个实例的can_id
    static CanDevice *Can1_Instances1[MAX_INSTANCES+1]; // 保存can1上id为1-4的实例
    static CanDevice *Can1_Instances2[MAX_INSTANCES+1]; // 保存can1上id为5-8的实例
    static CanDevice *Can2_Instances1[MAX_INSTANCES+1]; // 保存can2上id为1-4的实例
    static CanDevice *Can2_Instances2[MAX_INSTANCES+1]; // 保存can2上id为5-8的实例
    uint8_t Can1_instance_index1 = 0; // 实例索引
    uint8_t Can1_instance_index2 = 0;
    uint8_t Can2_instance_index1 = 0;
    uint8_t Can2_instance_index2 = 0;

    virtual int16_t motor_process() = 0; // 给m3508用的接口，其他电机不要管
    virtual void Can_update(uint8_t can_RxData[8]) = 0;  //用于更新电机上传的数据
    void CAN1_Filter_Init(void); // 初始化CAN1过滤器
    void CAN2_Filter_Init(void);

    CanDevice(CAN_HandleTypeDef *hcan_, uint8_t can_id_);
};

class CanManager : public ITaskProcessor
{
private:
    
    CAN_TxHeaderTypeDef TxHeader1,TxHeader2;  // 定义CAN下发报文的数据头
    uint8_t send_buf1[8] = {0};  // 定义CAN下发报文用于控制电机的数据
    uint8_t send_buf2[8] = {0};
    uint32_t msg_box1 = 0;  //保存下发报文使用的邮箱
    uint32_t msg_box2 = 0;

public:
    void process_data();  //在freertos中调用的函数，用于发送CAN下发报文
    CanManager();

    static uint8_t RxData1[8];  //存储CAN1接收到的数据
    static uint8_t RxData2[8];  //存储CAN2接收到的数据
};

#endif

#endif
