#include "timer.h"

//电机控制的PWM设置
void TIM1_PWM_Init(float duty_ratio_1,float duty_ratio_2,float duty_ratio_3)
{
	GPIO_InitTypeDef GPIO_InitStructure;//初始化GPIO
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//初始化时基
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	u16 CCR_Val1=duty_ratio_1*20000;//设置占空比
	u16 CCR_Val2=duty_ratio_2*20000;
	u16 CCR_Val3=duty_ratio_3*20000;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//PA8,9,10输出PWM,PA11控制3电机方向
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//PC8,9控制1,2电机方向

	//DIR控制配置
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	
	//PWM输出配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1);

	//PA引脚9和11为PWM，设置为复用
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	//时基初始化
	TIM_TimeBaseStructure.TIM_Period=1000-1;
	TIM_TimeBaseStructure.TIM_Prescaler=83;//168M/2000=36kHz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
	
	//PWM_CH1输出
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR_Val1;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	//PWM_CH2输出
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=CCR_Val2;
  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
//	//PWM_CH3输出
//	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse=CCR_Val3;
//  TIM_OC3Init(TIM1,&TIM_OCInitStructure);
//	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void MotorSet(float left_speed,float right_speed)
{
	if(left_speed>=0)
	{
		TIM1->CCR1=left_speed*1000;
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	else
	{
		TIM1->CCR1=-left_speed*1000;
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
	}
	
	if(right_speed>=0)
	{
		TIM1->CCR2=right_speed*1000;
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
	}
	else
	{
		TIM1->CCR2=-right_speed*1000;
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
}
