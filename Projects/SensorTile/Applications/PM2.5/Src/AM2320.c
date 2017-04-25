#include "moniiic.h"
#include "AM2320.h"

volatile uint8_t AMbuff[6]="";


void readAM2320(void)
{
	
	  IIC_Start();
    IIC_Send_Byte(0xb8);
    IIC_Wait_Ack();
    HAL_Delay(2);
    IIC_Stop();//唤醒AM2320
	
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
	AMbuff[0]=	IIC_Read_Byte(1);//读取第一个字节

	AMbuff[1]=	IIC_Read_Byte(1);

	AMbuff[2]=	IIC_Read_Byte(1);//湿度高
	AMbuff[3]=	IIC_Read_Byte(1);//湿度低
	AMbuff[4]=	IIC_Read_Byte(1)&0x7f;//温度高
	AMbuff[5]=	IIC_Read_Byte(1);//温度低
	IIC_Stop();	
		
//	HAL_Delay(2000);

}

