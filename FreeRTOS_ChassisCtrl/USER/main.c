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
TaskHandle_t StartTask_Handler;//������
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t IMUTask_Handler;//IMUģ��
TaskHandle_t DataTransTask_Handler;//�������ݽ���
TaskHandle_t ChassisCtrlTask_Handler;//�������

void vTaskStart(void *pvParameters);//������
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	TIM1_PWM_Init(0,0,0);
	Adc_Init();
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
	MPU_Init();
	while(mpu_dmp_init())
	{
		delay_ms(200);
	}
	
	//������ʼ����
	xTaskCreate(vTaskStart,"vTaskStart",50, NULL,1,&StartTask_Handler);
	vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void vTaskStart(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���
	SpeedQueue=xQueueCreate(5,sizeof(BaseSpeed));
	
	xTaskCreate(vTaskLed0,"vTaskLed0",30,NULL,1,&LED0Task_Handler);   
	xTaskCreate(vTaskLed1,"vTaskLed1",30,NULL,1,&LED1Task_Handler);     
	xTaskCreate(vTaskIMU,"vTaskIMU",150,NULL,1,&IMUTask_Handler);
	xTaskCreate(vTaskDataTrans,"vTaskDataTrans",100,NULL,1,&DataTransTask_Handler);
	xTaskCreate(vTaskChassisCtrl,"vTaskChassisCtrl",256,NULL,1,&ChassisCtrlTask_Handler);
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
	taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
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

//LED1������
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

void vTaskIMU(void *pvParameters)//IMU���ݽ�������
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
void vTaskDataTrans(void *pvParameters)//�������ݽ�������
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


void vTaskChassisCtrl(void *pvParameters)//���̿�������
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
