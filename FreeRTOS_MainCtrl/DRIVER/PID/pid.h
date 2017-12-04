#ifndef __PID_H
#define __PID_H
#include "sys.h"
typedef struct{
	float SetValue;
	float ActualVal;
	float ErrVal;
	float ErrValLast;
	float Output;
	float Ki;
	float Kp;
	float Kd;
	float IntegralVal;
	float SatIntegral;//»ý·Ö±¥ºÍ
}Pid;

void PidInit(Pid *Pid,float Kp,float Ki,float Kd,float SatInt);
float PidCalc(Pid *Pid,float SetValue,float ActualValue,float OutMax);
#define PID_DEFAULTS { 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f, \
											 0.0f}
#endif
