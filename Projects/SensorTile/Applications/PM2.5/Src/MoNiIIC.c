#include "moniiic.h"
extern void delay_us(uint32_t nus);

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);



	//sda�����
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	  	  
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda�����
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);      //SDA����Ϊ���� 


	
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
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//ʱ�����0 	   
	return 0;  
}

//����ACKӦ��
void IIC_Ack(void)
{
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	
	
	
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda�����
	
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
}

//������ACKӦ��		    
void IIC_NAck(void)
{
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda�����	
	
	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(2);
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//sda�����


	
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);//����ʱ�ӿ�ʼ���ݴ���
    for(t = 0;t < 8;t ++)
    {              
		if((txd&0x80) >> 7)
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
		else
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		txd <<= 1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		delay_us(2); 
HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);	
		delay_us(2);
    }	 
}


//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive;

	GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_1;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);      //SDA����Ϊ����
	
	
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}






