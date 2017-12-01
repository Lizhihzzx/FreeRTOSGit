#include "pid.h"

void PidInit(Pid *Pid,float Kp,float Ki,float Kd,float SatInt)
{
	Pid->SetValue=0.0;
	Pid->ActualVal=0.0;
	Pid->ErrVal=0.0;
	Pid->ErrValLast=0.0;
	Pid->Output=0.0;
	Pid->Ki=Ki;
	Pid->Kp=Kp;
	Pid->Kd=Kd;
	Pid->IntegralVal=0.0;
	Pid->SatIntegral=SatInt;
}

float PidCalc(Pid *Pid,float SetValue,float ActualVal,float OutMax)
{
	Pid->SetValue=SetValue;//赋目标值
	Pid->ActualVal=ActualVal;//赋实际值
	Pid->ErrVal=Pid->SetValue-Pid->ActualVal;//计算误差
	Pid->IntegralVal+=Pid->ErrVal;//积分值计算
	Pid->Output=Pid->ErrVal*Pid->Kp + Pid->IntegralVal*Pid->Ki + (Pid->ErrVal-Pid->ErrValLast)*Pid->Kd;//pid公式
	Pid->ErrValLast=Pid->ErrVal;
	if(Pid->IntegralVal>Pid->SatIntegral)
	{
		Pid->IntegralVal=Pid->SatIntegral;
	}
	else if(Pid->IntegralVal<-Pid->SatIntegral)
	{
			Pid->IntegralVal=-Pid->SatIntegral;
	}
	
	if(Pid->Output>OutMax)//限制输出值
	{
		Pid->Output=OutMax;
	}
	else if(Pid->Output<-OutMax)
	{
		Pid->Output=-OutMax;
	}
	return Pid->Output;//返回要设置的值
}




