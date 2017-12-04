#include "datatransfer.h"

extern u8 USART_RX_BUF[];

BaseSpeed USARTDataGet(void)
{
	BaseSpeed Speed;
	u8 first_bit_loc=0;
	u8 bit_offset=0;
	int left_speed_usart=0;
	int right_speed_usart=0;
	float left_speed=0;
	float right_speed=0;
	if(USART_RX_BUF[0]=='s')
	{
		first_bit_loc=0;
		if(USART_RX_BUF[first_bit_loc+1]==' ')
		{
			bit_offset=2;
			if(USART_RX_BUF[first_bit_loc+2]=='-')
			{
				bit_offset++;
				left_speed_usart=0;
				while(USART_RX_BUF[first_bit_loc+bit_offset]!=' ')
				{
					left_speed_usart=left_speed_usart*10-(USART_RX_BUF[first_bit_loc+bit_offset]-0x30);
					bit_offset++;
				}
				left_speed=left_speed_usart/1000.0f;
				bit_offset++;
			}
			else if(USART_RX_BUF[first_bit_loc+2]>='0'&&USART_RX_BUF[first_bit_loc+2]<='9')
			{
				left_speed_usart=0;
				while(USART_RX_BUF[first_bit_loc+bit_offset]!=' ')
				{
					left_speed_usart=left_speed_usart*10+(USART_RX_BUF[first_bit_loc+bit_offset]-0x30);
					bit_offset++;
				}
				left_speed=left_speed_usart/1000.0f;
				bit_offset++;
			}
			else
			{
				left_speed=0;
				right_speed=0;
			}
			
			if(USART_RX_BUF[first_bit_loc+bit_offset]=='-')
			{
				right_speed_usart=0;
				bit_offset++;
				while(USART_RX_BUF[first_bit_loc+bit_offset]!=0x0d)
				{
					right_speed_usart=right_speed_usart*10-(USART_RX_BUF[first_bit_loc+bit_offset]-0x30);
					bit_offset++;
				}
				right_speed=right_speed_usart/1000.0f;
				bit_offset=0;
			}
			else if(USART_RX_BUF[first_bit_loc+bit_offset]>='0'&&USART_RX_BUF[first_bit_loc+bit_offset]<='9')
			{
				right_speed_usart=0;
				while(USART_RX_BUF[first_bit_loc+bit_offset]!=0x0d)
				{
					right_speed_usart=right_speed_usart*10+(USART_RX_BUF[first_bit_loc+bit_offset]-0x30);
					bit_offset++;
				}
				right_speed=right_speed_usart/1000.0f;
				bit_offset=0;
			}
			else
			{
				left_speed=0;
				right_speed=0;
			}
		}
		else
		{
			left_speed=0;
			right_speed=0;
		}
	}
	else if(USART_RX_BUF[0]=='r')//¸´Î»
	{
		left_speed=0;
		right_speed=0;
//		Robo_Reset();
	}
	else
	{
		left_speed=0;
		right_speed=0;
	}
	Speed.LeftSpeed=left_speed;
	Speed.RightSpeed=right_speed;
	return Speed;
}
