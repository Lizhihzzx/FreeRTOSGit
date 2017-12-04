#include "chassis.h"

Motor MotorLeft=MOTOR_DEFAULTS;
Motor MotorRight=MOTOR_DEFAULTS;
void ChassisInit()
{
	PidInit(&MotorLeft.SpeedPid,1,0,0,20.0);
	PidInit(&MotorRight.SpeedPid,1,0,0,20.0);
}

void ChassisCtrl(float SpeedLeft,float SpeedRight)
{
	MotorLeft.SetSpeed=SpeedLeft;
	MotorRight.SetSpeed=SpeedRight;
	MotorCtrl(&MotorLeft);
	MotorCtrl(&MotorRight);
}
