#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*在此处引用外部文件：       begin*/	
#include "stm32f4xx_hal.h"
#include "CanDevice.h"
#include <stdint.h>
#include "TaskManager.h"
/*引用外部文件end*/	

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class Motor : public CanDevice
{
public:
	Motor(CAN_HandleTypeDef *hcan_, uint8_t can_id_);
	int16_t motor_process();
	void Can_SendData();
};

#endif

#endif
