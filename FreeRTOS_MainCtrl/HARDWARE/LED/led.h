#ifndef __LED_H
#define __LED_H
#include "sys.h"
//LED端口定义
#define LED0 PCout(3)	// DS0
#define LED1 PCout(2)	// DS1	 
void Laser_Init(void);
void LED_Init(void);//初始化		 				    
#endif
