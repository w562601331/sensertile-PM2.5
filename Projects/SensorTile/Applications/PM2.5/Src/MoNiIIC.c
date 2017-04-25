#include "moniiic.h"
extern void delay_us(uint32_t nus);

//产生IIC起始信号
void IIC_Start(void)
{
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);



	//sda线输出
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	  	  
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号
void IIC_Stop(void)
{
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda线输出
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);      //SDA设置为输入 


	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);;delay_us(1);	   
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);delay_us(1);	 
	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))
	{
		ucErrTime ++;
		if(ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//时钟输出0 	   
	return 0;  
}

//产生ACK应答
void IIC_Ack(void)
{
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	
	
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda线输出
	
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
}

//不产生ACK应答		    
void IIC_NAck(void)
{
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda线输出	
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda线输出


	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//拉低时钟开始数据传输
    for(t = 0;t < 8;t ++)
    {              
		if((txd&0x80) >> 7)
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
		else
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		txd <<= 1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		delay_us(2); 
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);	
		delay_us(2);
    }	 
}


//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive;

	GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);      //SDA设置为输入
	
	
    for(i = 0;i < 8;i ++ )
	{
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
        delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
        receive <<= 1;
        if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))    receive ++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}






