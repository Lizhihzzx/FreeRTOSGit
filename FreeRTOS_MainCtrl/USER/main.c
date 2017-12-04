#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#include "my6050.h"
#include "timer.h"
#include "datatransfer.h"
#include "adc.h"
#include "Chassis.h"
#include "kalman.h"
TaskHandle_t StartTask_Handler;//任务句柄
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t IMUTask_Handler;//IMU模块
TaskHandle_t DataTransTask_Handler;//串口数据解析
TaskHandle_t ChassisCtrlTask_Handler;//电机控制

void vTaskStart(void *pvParameters);//任务函数
void vTaskLed0(void *pvParameters);
void vTaskLed1(void *pvParameters);
void vTaskIMU(void *pvParameters);
void vTaskDataTrans(void *pvParameters);
void vTaskChassisCtrl(void *pvParameters);

QueueHandle_t SpeedQueue;


KalmanInfo info;

int main(void)
{ 
	Init_KalmanInfo(&info,0.1,0.1);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	TIM1_PWM_Init(0,0,0);
	Adc_Init();
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	MPU_Init();
	while(mpu_dmp_init())
	{
		delay_ms(200);
	}
	
	//创建开始任务
	xTaskCreate(vTaskStart,"vTaskStart",50, NULL,1,&StartTask_Handler);
	vTaskStartScheduler();          //开启任务调度
}
 
//开始任务任务函数
void vTaskStart(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	SpeedQueue=xQueueCreate(5,sizeof(BaseSpeed));
	
	xTaskCreate(vTaskLed0,"vTaskLed0",30,NULL,1,&LED0Task_Handler);   
	xTaskCreate(vTaskLed1,"vTaskLed1",30,NULL,1,&LED1Task_Handler);     
	xTaskCreate(vTaskIMU,"vTaskIMU",150,NULL,1,&IMUTask_Handler);
	xTaskCreate(vTaskDataTrans,"vTaskDataTrans",100,NULL,1,&DataTransTask_Handler);
	xTaskCreate(vTaskChassisCtrl,"vTaskChassisCtrl",256,NULL,1,&ChassisCtrlTask_Handler);
	vTaskDelete(StartTask_Handler); //删除开始任务
	taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void vTaskLed0(void *pvParameters)
{
	while(1)
	{
		LED0=0;
		vTaskDelay(1000);
		LED0=1;
		vTaskDelay(1000);
	}
}   

//LED1任务函数
void vTaskLed1(void *pvParameters)
{
	while(1)
	{
		LED1=0;
		vTaskDelay(300);
		LED1=1;
		vTaskDelay(1700);
	}
}

void vTaskIMU(void *pvParameters)//IMU数据解算任务
{
	portTickType xLastWakeTime;
	xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		MPUCalc();
//		vTaskDelay(1);
		vTaskDelayUntil(&xLastWakeTime,(1/portTICK_RATE_MS));
	}
}

void SpeedSend(BaseSpeed Speed)
{
	BaseType_t err;
	if(SpeedQueue!=0)
	{
		err=xQueueSend(SpeedQueue,&Speed,5);
		if(err==errQUEUE_FULL)
		{
		}
	}
}
void vTaskDataTrans(void *pvParameters)//串口数据解析任务
{
	BaseSpeed Speed;
	while(1)
	{
		Speed=USARTDataGet();
		SpeedSend(Speed);
		vTaskDelay(5);
	}
}
float Ia;
float IaK;
int IaInt;
int IaKInt;
extern Motor MotorLeft;


void vTaskChassisCtrl(void *pvParameters)//底盘控制任务
{
	BaseSpeed Speed;
	
	while(1)
	{
		if(SpeedQueue!=0)
		{
			if(xQueueReceive(SpeedQueue,&Speed,1))
			{

//				printf("%f\r\n",Speed.LeftSpeed);
//				printf("%f\r\n",Speed.RightSpeed);
			}
		}
		Ia=Getcurrent();
		IaK=KalmanFilter(&info,Ia);
		IaInt=1000*Ia;
		IaKInt=1000*IaK;
		printf("%f\n",Ia);
		MotorLeft.MeasureCurrentUniform=Ia;
		MotorCtrl(&MotorLeft);
		MotorSet(MotorLeft.SetDutyRatio,Speed.RightSpeed);
		vTaskDelay(1);
	}
}
