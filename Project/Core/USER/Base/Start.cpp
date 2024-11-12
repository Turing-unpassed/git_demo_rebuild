#include "Start.h"

//SerialDevice U3 (&huart6, false);


extern "C" void before_Start_tasks(void)
{
	
}

extern "C" void Start_tasks(void)
{
    Motor M3508(&hcan1,1);
	M3508.Can_SendData();
}