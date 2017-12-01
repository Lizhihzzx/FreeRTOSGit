#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void TIM1_PWM_Init(float duty_ratio_1,float duty_ratio_2,float duty_ratio_3);
void MotorSet(float left_speed,float right_speed);

#endif
