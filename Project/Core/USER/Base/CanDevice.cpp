#include "CanDevice.h"
//静态变量的初始化
uint8_t CanManager::RxData1[8]={0};
uint8_t CanManager::RxData2[8]={0};

CanDevice* CanDevice::Can1_Instances1[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can1_Instances2[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can2_Instances1[MAX_INSTANCES+1] = {nullptr};
CanDevice* CanDevice::Can2_Instances2[MAX_INSTANCES+1] = {nullptr};

CanDevice::CanDevice(CAN_HandleTypeDef *hcan_, uint8_t can_id_):hcan(hcan_), can_id(can_id_){
    if(hcan_ == &hcan1){          //根据挂载在CAN1上的设备的ID号，分配实例在数组的位置，数组的下标即是CAN设备的ID号，这也是MAX_INSTANCES+1的原因
        if(can_id_ <= 4){         //这样做是为了方便process_data()里配置下发报文中8字节数组里包含的电流值控制信息
            Can1_Instances1[can_id_] = this;
        }else if(can_id_ <= 8){
            Can1_Instances2[can_id_-4] = this;
        }
    }else if(hcan_ == &hcan2){
        if(can_id_ <= 4){
            Can2_Instances1[can_id_] = this;
        }else if(can_id_ <= 8){
            Can2_Instances2[can_id_-4] = this;
        }
    }

    CAN1_Filter_Init();
    CAN2_Filter_Init();
}

void CanDevice::CAN1_Filter_Init(){
    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;  //启用过滤器
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  //掩码模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //32位过滤器
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; //通过过滤器的报文放置FIFO0
    sFilterConfig.FilterBank = 0;  //使用0号过滤器
    sFilterConfig.FilterIdHigh = 0x00;
    sFilterConfig.FilterIdLow = 0x00;   //不过滤任何ID号，即接收所有ID号的报文
    sFilterConfig.FilterMaskIdHigh = 0x00;
    sFilterConfig.FilterMaskIdLow = 0x00;
    sFilterConfig.SlaveStartFilterBank = 14;  //can2的过滤器从14号开始,避免与can1的过滤器冲突
    if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig)!= HAL_OK){
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) //使能can1的FIFO0接收中断
    {
        /* Start Error */
        Error_Handler();
    }

     if (HAL_CAN_Start(&hcan1) != HAL_OK)  //启动CAN1
    {
        /* Start Error */
        Error_Handler();
    }
}

void CanDevice::CAN2_Filter_Init(){

    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterActivation = ENABLE;  //启用过滤器
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; //掩码模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //32位过滤器
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; //通过过滤器的报文放置FIFO0
    sFilterConfig.FilterBank = 14;  //使用14号过滤器
    sFilterConfig.FilterIdHigh = 0x00;
    sFilterConfig.FilterIdLow = 0x00; //不过滤任何ID号，即接收所有ID号的报文
    sFilterConfig.FilterMaskIdHigh = 0x00;
    sFilterConfig.FilterMaskIdLow = 0x00;
    
    if(HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK){
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) //使能can2的FIFO0接收中断
    {
        /* Start Error */
        Error_Handler();
    }

    if (HAL_CAN_Start(&hcan2) != HAL_OK)  //启动CAN2
    {
        /* Start Error */
        Error_Handler();
    }
}

CanManager::CanManager(){
    TxHeader1.StdId = 0x200;  //用于控制id为0x201~0x204的电机的can报文数据头
    TxHeader1.ExtId = 0x00;   //不使用扩展帧
    TxHeader1.RTR = CAN_RTR_DATA;  //使用数据帧
    TxHeader1.IDE = CAN_ID_STD;   //使用标准帧
    TxHeader1.DLC = 8;  //数据长度为8字节
    TxHeader1.TransmitGlobalTime = DISABLE;  //不使用全局时间戳

    TxHeader2.StdId = 0x1FF;  //用于控制id为0x205~0x208的电机的can报文数据头
    TxHeader2.ExtId = 0x00;   //不使用扩展帧
    TxHeader2.RTR = CAN_RTR_DATA;  //使用数据帧
    TxHeader2.IDE = CAN_ID_STD;   //使用标准帧
    TxHeader2.DLC = 8;  //数据长度为8字节
    TxHeader2.TransmitGlobalTime = DISABLE;  //不使用全局时间戳

}

void CanManager::process_data(){
    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can1_Instances1[i] != nullptr){    //如果id号为0x201~0x204的实例存在,can数据帧中对应id号的电流值由实例的motor_process()函数计算得到
            temp_vcurrent = CanDevice::Can1_Instances1[i]->motor_process();       //实例不存在则对应id号的电流值为0（任何值都可以，因为用不上）
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);   //控制电流值的高8位
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;      //控制电流值的低8位
    }
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader1, send_buf1, &msg_box1) == HAL_ERROR)  //将控制信息发给CAN1上id号为0x201~0x204的电机
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can1_Instances2[i] != nullptr){   //如果id号为0x205~0x208的实例存在,can数据帧中对应id号的电流值由实例的motor_process()函数计算得到
            temp_vcurrent = CanDevice::Can1_Instances2[i]->motor_process(); 
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);   //控制电流值的高8位
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;      //控制电流值的低8位
    }
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader2, send_buf1, &msg_box1) == HAL_ERROR)     //将控制信息发给CAN1上id号为0x205~0x208的电机
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    //CAN2的处理方式与CAN1相同
    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can2_Instances1[i] != nullptr){
            temp_vcurrent = CanDevice::Can2_Instances1[i]->motor_process();
        }
        send_buf2[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf2[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader1, send_buf2, &msg_box2) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }

    for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
        int16_t temp_vcurrent = 0;
        if(CanDevice::Can2_Instances2[i] != nullptr){
            temp_vcurrent = CanDevice::Can2_Instances2[i]->motor_process();
        }
        send_buf1[2 * i - 2] = (uint8_t)(temp_vcurrent >> 8);
        send_buf1[2 * i - 1] = (uint8_t)temp_vcurrent;
    }
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader2, send_buf2, &msg_box2) == HAL_ERROR)
    {
        Error_Handler();
        // Failed to add message to the transmit mailbox
    }
}

extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
  /*  if(hcan == &hcan1){

        CAN_RxHeaderTypeDef RxHeader1;
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader1, CanManager::RxData1);  //获取电调上传的电机信息,放在RxData1数组中

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){   //遍历所有实例
            if(CanDevice::Can1_Instances1[i]->can_id == RxHeader1.StdId - 0x200){    //如果收到的CAN帧的ID号与实例的ID号匹配，则调用实例的Can_update()函数更新实例的数据
                CanDevice::Can1_Instances1[i]->Can_update(CanManager::RxData1);
            }
        }

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){    //遍历所有实例
            if(CanDevice::Can1_Instances2[i]->can_id == RxHeader1.StdId - 0x200){    //如果收到的CAN帧的ID号与实例的ID号匹配，则调用实例的Can_update()函数更新实例的数据
                CanDevice::Can1_Instances2[i]->Can_update(CanManager::RxData1);
            }
        }

    //CAN2的处理方式与CAN1相同
    }else if(hcan == &hcan2){

        CAN_RxHeaderTypeDef RxHeader2;
        HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, CanManager::RxData2);

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can2_Instances1[i]->can_id == RxHeader2.StdId - 0x200){
                CanDevice::Can2_Instances1[i]->Can_update(CanManager::RxData2);
            }
        }

        for(uint8_t i=1; i<MAX_INSTANCES+1; i++){
            if(CanDevice::Can2_Instances2[i]->can_id == RxHeader2.StdId - 0x200){
                CanDevice::Can2_Instances2[i]->Can_update(CanManager::RxData2);
            }
        }
    }   */ 
}