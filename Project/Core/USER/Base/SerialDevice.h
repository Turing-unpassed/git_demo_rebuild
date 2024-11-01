#ifndef __SERIALDEVICE_H
#define __SERIALDEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*在此处进行宏定义：         begin*/	
#define RX_BUFFER_SIZE 1
#define Max_Package_Length 64
#define MAX_INSTANCES 8

/*宏定义end*/	


/*在此处进枚举类型定义：         begin*/	
#define Error_huart_init 0
/*枚举定义end*/	


/*在此处进行类和结构体的定义：begin*/
	
typedef struct _Package
{
    uint8_t  Head_0;
    uint8_t  Head_1;    
	  uint8_t  End_0;
    uint8_t  End_1;
    uint8_t  frame_id;
    uint8_t  length;
    uint8_t  rxIndex=0;

    union _PK_Data
    {
        uint8_t   u8_PK_data   [Max_Package_Length];
        int32_t   int32_PK_data[Max_Package_Length/4];
        int16_t   int16_PK_data[Max_Package_Length/2];
        uint32_t  u32_PK_data  [Max_Package_Length/4];
        uint16_t  u16_PK_data  [Max_Package_Length/2];
        float     float_PK_data[Max_Package_Length/4];
    }PK_Data;//同个实例的包只有可能接收一种类型的数据，所以用union成员是共享一块空间，节省空间，同时易于类型转换
    union _check_code
    {
        uint16_t crc_code;
        uint8_t  crc_buff[2]; // CRC 校验的字节形式
    }check_code;   //虽然不知道是要干什么的，但是先复制过来
    uint16_t crc_calculated = 0;

}Package;

class SerialDevice
{
    public:

    uint8_t rxBuffer_[RX_BUFFER_SIZE];
    UART_HandleTypeDef *huart_; // 保存 UART 句柄
    
    bool init_status = false;
    bool enableCrcCheck_; // 是否启用 CRC 校验

    Package rx_frame_mat;
    uint8_t receive_ok_flag = 0;
		uint8_t RxPK_ok_flag = 0;
    static SerialDevice *instances_[MAX_INSTANCES]; // 保存所有实例(最多八个)
    static int instanceCount_;                      // 记录保存实例个数
	
    enum rxState
    {
        WAITING_FOR_HEADER_0,
        WAITING_FOR_HEADER_1,
        WAITING_FOR_ID,
        WAITING_FOR_LENGTH,
        WAITING_FOR_DATA,
        WAITING_FOR_CRC_0,
        WAITING_FOR_CRC_1,
        WAITING_FOR_END_0,
        WAITING_FOR_END_1
    } state_;

    SerialDevice(UART_HandleTypeDef *huartx,bool enableCrcCheck);   
    void SetRxPackage_identity(Package PKD);
    bool SendByte   (uint8_t  data);
    bool SendString (char    *data);
    bool SendArray  (uint8_t *data, uint8_t data_len);
    bool SendFloat  (float    data);
    bool SendInt32  (int32_t  data);
    bool SendInt16  (int16_t  data);
    bool SendPackage(Package    *package);
    void startUartReceiveIT();
    virtual void handleReceiveData(uint8_t byte) = 0;//串口接收数据处理函数

    //static void registerInstance(SerialDevice *instance);
};
/*类和结构体定义end*/	


/*在此处进行函数定义：       begin*/	
uint16_t CRC16_Table(uint8_t *p, uint8_t counter);
uint8_t  CRC8_Table (uint8_t *p, uint8_t counter);
/*函数定义end*/	

#endif

#endif