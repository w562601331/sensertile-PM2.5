#include "moniiic.h"
#include "AM2320.h"

volatile uint8_t AMbuff[6]="";


void readAM2320(void)
{
	
	  IIC_Start();
    IIC_Send_Byte(0xb8);
    IIC_Wait_Ack();
    HAL_Delay(2);
    IIC_Stop();//����AM2320
	
	HAL_Delay(2);
	
	
	IIC_Start();
	IIC_Send_Byte(0xb8);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x03);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x04);
	IIC_Wait_Ack();
	IIC_Stop();
	
	HAL_Delay(2);
	
	IIC_Start();
	IIC_Send_Byte(0xb8|0x01);
	IIC_Wait_Ack();	
	HAL_Delay(1);
	AMbuff[0]=	IIC_Read_Byte(1);//��ȡ��һ���ֽ�

	AMbuff[1]=	IIC_Read_Byte(1);

	AMbuff[2]=	IIC_Read_Byte(1);//ʪ�ȸ�
	AMbuff[3]=	IIC_Read_Byte(1);//ʪ�ȵ�
	AMbuff[4]=	IIC_Read_Byte(1)&0x7f;//�¶ȸ�
	AMbuff[5]=	IIC_Read_Byte(1);//�¶ȵ�
	IIC_Stop();	
		
//	HAL_Delay(2000);

}

