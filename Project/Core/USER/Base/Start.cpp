#include "Start.h"

//SerialDevice U3 (&huart6, false);


extern "C" void before_Start_tasks(void)
{
	TaskManager task_core(1,10,"cantest",10,1024);
	CanManager can_core;
	Motor M3508(&hcan1,1);
	task_core.registerTask(1,&can_core);
}

extern "C" void Start_tasks(void)
{
    
}