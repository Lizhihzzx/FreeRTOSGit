#include "my6050.h"

#define PI (3.141592653)
void MPUCalc(void)
{
	//IMU数据
//	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temp;					//温度
	
	float gx=0,gy=0,gz=0;
	float ax=0,ay=0,az=0;

//	mpu_dmp_get_data(&pitch,&roll,&yaw);
	temp=MPU_Get_Temperature();	//得到温度值
	MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
	MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据

	gx=gyrox/32768.0*2000/180.0*PI;
	gy=gyroy/32768.0*2000/180.0*PI;
	gz=gyroz/32768.0*2000/180.0*PI;

	ax=aacx/32768.0*20;
	ay=aacy/32768.0*20;
	az=aacz/32768.0*20;
	IMUupdate(gx,gy,gz,ax,ay,az);
}

Angle ANGLE;

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az) 
{
	static float q0=1.0,q1=0.0,q2=0.0,q3=0.0;
	static float exInt=0,eyInt=0,ezInt=0;
	
	float Ki=0.001;
	float Kp=2;
	
	float norm; 
	float vx, vy, vz;
	float ex, ey, ez; 
	float qnorm;
	norm = sqrt(ax*ax + ay*ay + az*az);       

	ax = ax / norm; 
	ay = ay / norm; 
	az = az / norm; 

	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3); 
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3; 
	
	ex = (ay*vz - az*vy) ; 
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;
		
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki; 
	ezInt = ezInt + ez*Ki; 

	gx = gx + Kp*ex + exInt; 
	gy = gy + Kp*ey + eyInt; 
	gz = gz + Kp*ez + ezInt; 
	
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*0.001f*0.5f; 
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*0.001f*0.5f; 
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*0.001f*0.5f; 
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*0.001f*0.5f;   
	qnorm=sqrt(q0*q0+q1*q1+q2*q2+q3*q3);
	q0=q0/qnorm;
	q1=q1/qnorm;
	q2=q2/qnorm;
	q3=q3/qnorm;

	ANGLE.Yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; // yaw 
	ANGLE.Pitch= asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch 
	ANGLE.Roll= atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
}
