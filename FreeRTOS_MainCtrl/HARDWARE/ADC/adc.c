/****************************************************
*			Title:		ADC����
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
	
  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,1, ADC_SampleTime_480Cycles);

	ADC_Cmd(ADC1, ENABLE);//����ADת����	
	
	ADC_SoftwareStartConv(ADC1);	
	
	DMA_DeInit(DMA2_Stream0);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&Adc_Value;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = 0X1E;//���ݴ�����
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�ߵ����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);//��ʼ��DMA Stream
	
	//DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);	
	DMA_Cmd(DMA2_Stream0, ENABLE); 
	
  ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); 
  ADC_DMACmd(ADC1, ENABLE); 
}

float Getcurrent(void)
{
	u8 i=0,j=0;
	u32 ADC_ConvertedValue1=0;
	float adc_current;		//�������
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
	
	return adc_current;//ƫ��
}

