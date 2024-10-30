#include "Start.h"

SerialDevice usart1 (&huart6, false);

extern "C" void Start_tasks(void)
{
    usart6.SendByte(0x01);
}