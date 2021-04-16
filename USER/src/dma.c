#include "dma.h"																	   	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DMA ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 u8 DMA_Rxbuf[RX_SIZE]; 

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:0~7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2ʱ��ʹ�� 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;						//���֮ǰ��stream�ϵ������жϱ�־
	
	DMA_Streamx->PAR=par;		//DMA�����ַ
	DMA_Streamx->M0AR=mar;		//DMA �洢��0��ַ
	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	//DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CR &=~(3<<6);		//���赽�洢��ģʽ
	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	DMA_Streamx->CR|=(u32)chx<<25;//ͨ��ѡ��
	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
} 
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0); 	//�ر�DMA���� 
	while(DMA_Streamx->CR&0X1);	//ȷ��DMA���Ա�����  
	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ 
	DMA_Streamx->CR|=1<<0;		//����DMA����
}	  

 

























