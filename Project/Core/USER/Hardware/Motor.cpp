#include "Motor.h"

Motor::Motor(CAN_HandleTypeDef *hcan_, uint8_t can_id_):CanDevice(hcan_,can_id_){

}

int16_t Motor::motor_process(){
	return 2000;
}

void Motor::Can_SendData(){
	TxHeader1.StdId = 0x200;  //用于控制id为0x201~0x204的电机的can报文数据头
    TxHeader1.ExtId = 0x00;   //不使用扩展帧
    TxHeader1.RTR = CAN_RTR_DATA;  //使用数据帧
    TxHeader1.IDE = CAN_ID_STD;   //使用标准帧
    TxHeader1.DLC = 8;  //数据长度为8字节
    TxHeader1.TransmitGlobalTime = DISABLE;  //不使用全局时间戳
	
	uint16_t ctemp = motor_process();
	send_buf1[0] = (uint8_t)(ctemp>>8);
	send_buf1[1] = (uint8_t)ctemp;
	
	HAL_CAN_AddTxMessage(hcan,&TxHeader1,send_buf1,&msg_box1);
}