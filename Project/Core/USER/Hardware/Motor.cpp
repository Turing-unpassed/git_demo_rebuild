#include "Motor.h"

Motor::Motor(CAN_HandleTypeDef *hcan_, uint8_t can_id_):CanDevice(hcan_,can_id_){

}

int16_t Motor::motor_process(){
	return 2000;
}