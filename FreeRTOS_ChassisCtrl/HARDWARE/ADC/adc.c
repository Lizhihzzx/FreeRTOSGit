/****************************************************
*			Title:		ADC配置
*			Version:	1.0.0
*			Data:			2017.01.21
*												LD.
*****************************************************/
#include "adc.h"
#include "stm32f4xx.h" 
#include "math.h"
#define I_PU	(3.3f/(4096))

u16 Adc_Value[30];

void	Adc_Init(void)
{    
  GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	
  //先初始化ADC1通道5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,1, ADC_SampleTime_480Cycles);

	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	
	
	ADC_SoftwareStartConv(ADC1);	
	
	DMA_DeInit(DMA2_Stream0);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&Adc_Value;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = 0X1E;//数据传输量
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//初始化DMA Stream
	
	//DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);	
	DMA_Cmd(DMA2_Stream0, ENABLE); 
	
  ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); 
  ADC_DMACmd(ADC1, ENABLE); 
}

float Getcurrent(void)
{
	u8 i=0,j=0;
	u32 ADC_ConvertedValue1=0;
	float adc_current;		//电机电流
	for(i=0;i<30;i++)
	{
		ADC_ConvertedValue1=ADC_ConvertedValue1+Adc_Value[i];
	}
//  for(i=0;i<19;i++)
//	{
//		for(j=0;j<19-i;j++)
//		{
//			if(Adc_Value[j] > Adc_Value[j+1])
//			{
//					u32 temp=Adc_Value[j];
//					Adc_Value[j]=Adc_Value[j+1];
//					Adc_Value[j+1]=temp;
//			}
//		}
//	}
	

//		ADC_ConvertedValue1=ADC_ConvertedValue1-Adc_Value[0]-Adc_Value[19];

	ADC_ConvertedValue1=ADC_ConvertedValue1/30;
//	adc_current =I_PU*ADC_ConvertedValue1;
	adc_current =fabs(I_PU*ADC_ConvertedValue1-2.51);
	
	return adc_current;//偏置
}

