#include "Start.h"

//SerialDevice U3 (&huart6, false);

Motor M3508(&hcan1,1);
extern "C" void before_Start_tasks(void)
{
   CanDevice::Can_Init();
   
 
}

extern "C" void Start_tasks(void)
{ 
	M3508.Can_SendData();
}