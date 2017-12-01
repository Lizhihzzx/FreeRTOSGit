#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"
#include "pid.h"

typedef struct{
	float MaxSpeed;
	float SetSpeed;
	float MeasureSpeed;
	float MeasureSpeedUniform;
	Pid SpeedPid;
	
	float MaxCurrent;
	float SetCurrent;
	float MeasureCurrent;
	float MeasureCurrentUniform;
	Pid CurrentPid;

	float SetDutyRatio;
}Motor;

#define MOTOR_DEFAULTS     {0.0, \
                           0.0f, \
                           0.0f, \
	                         0.0f, \
                           {0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
													 0.0f},\
                           0.0, \
                           0.0f, \
                           0.0f, \
	                         0.0f, \
													 {0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
													 0.0f}, \
													 0.0f}

void MotorInit(Motor *Motor);
void MotorCtrl(Motor *Motor);

#endif
