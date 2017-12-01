#ifndef __DATATRANSFER_H
#define __DATATRANSFER_H
#include "sys.h"
#include "FreeRTOS.h"
#include "queue.h"
typedef struct {
	float LeftSpeed;
	float RightSpeed;
}BaseSpeed;
BaseSpeed USARTDataGet(void);

#endif
