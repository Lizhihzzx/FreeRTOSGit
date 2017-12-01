#include "motor.h"
void MotorInit(Motor *Motor)
{
	PidInit(&Motor->SpeedPid,0,0,0,0);
	
	PidInit(&Motor->CurrentPid,1,0,0,0);
}

void MotorCtrl(Motor *Motor)
{
//	if(Motor->SetSpeed>Motor->MaxSpeed)
//	{
//		Motor->SetSpeed=Motor->MaxSpeed;
//	}
//	else if(Motor->SetSpeed<-Motor->MaxSpeed)
//	{
//		Motor->SetSpeed=-Motor->MaxSpeed;
//	}
//	Motor->SetCurrent=PidCalc(&Motor->SpeedPid,Motor->SetSpeed,Motor->MeasureSpeed,1.0);
	Motor->SetDutyRatio=1-PidCalc(&Motor->CurrentPid,Motor->SetCurrent,Motor->MeasureCurrentUniform,1.0);
	if(Motor->SetDutyRatio<0.01f)
	{
		Motor->SetDutyRatio=0.01f;
	}
}
