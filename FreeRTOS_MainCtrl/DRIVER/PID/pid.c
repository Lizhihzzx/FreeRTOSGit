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
	Pid->SetValue=SetValue;//��Ŀ��ֵ
	Pid->ActualVal=ActualVal;//��ʵ��ֵ
	Pid->ErrVal=Pid->SetValue-Pid->ActualVal;//�������
	Pid->IntegralVal+=Pid->ErrVal;//����ֵ����
	Pid->Output=Pid->ErrVal*Pid->Kp + Pid->IntegralVal*Pid->Ki + (Pid->ErrVal-Pid->ErrValLast)*Pid->Kd;//pid��ʽ
	Pid->ErrValLast=Pid->ErrVal;
	if(Pid->IntegralVal>Pid->SatIntegral)
	{
		Pid->IntegralVal=Pid->SatIntegral;
	}
	else if(Pid->IntegralVal<-Pid->SatIntegral)
	{
			Pid->IntegralVal=-Pid->SatIntegral;
	}
	
	if(Pid->Output>OutMax)//�������ֵ
	{
		Pid->Output=OutMax;
	}
	else if(Pid->Output<-OutMax)
	{
		Pid->Output=-OutMax;
	}
	return Pid->Output;//����Ҫ���õ�ֵ
}




