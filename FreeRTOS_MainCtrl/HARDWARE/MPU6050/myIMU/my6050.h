#ifndef __MY6050_H
#define __MY6050_H
#include "i2c.h"   	
#include "mpu6050.h"
#include "sys.h"
#include "math.h"

typedef struct {
	float Pitch;
	float Yaw;
	float Roll;
}Angle;

void MPUCalc(void);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
#endif
