
#include "1602_iic_sw.h"
extern void delay_us(uint32_t nus);
const uint8_t Ainit[42] = {
	0x30, 0x34, 0x30,
	0x30, 0x34, 0x30,
	0x30, 0x34, 0x30,
	0x20, 0x24, 0x20,
	0x20, 0x24, 0x20,
	0x80, 0x84, 0x80,
	0x00, 0x04, 0x00,
	0xc0, 0xc4, 0xc0,
	0x00, 0x04, 0x00,
	0x10, 0x14, 0x10,
	0x00, 0x04, 0x00,
	0x60, 0x64, 0x60,
	0x00, 0x04, 0x00,
	0x20, 0x24, 0x20,
};

//set default status of backlight to ON
static uint8_t backLight = BACKLIGHT_ON;

/***
	initial 1602 pins and mode
***/
void init_1602(void)
{
	uint8_t i = 0;
	
	i2cAction(0x28);
	HAL_Delay(500);
	i2cAction(0X28);
	HAL_Delay(5);
	i2cAction(0X01);
	
	HAL_Delay(5);
	i2cAction(0X0f);
	
	HAL_Delay(2);


	for(i = 0; i < 42; ++i)
	{
		i2cAction(Ainit[i]);
		
		if((i == 2) || (i == 5))HAL_Delay(5);
		else if((i == 29) || (i == 41))HAL_Delay(2);
		else if(i == 8) delay_us(230);
		else delay_us(20);
	}
	
}

/***
	Output charactors on 1602
***/
void lcdPrint(const char* charactor)
{
	uint8_t c = 0;
	for(c = 0; c < (uint8_t)strlen(charactor); ++c)
	writeOneChar(DATA ,*(charactor + c));
}

void disOneChar(unsigned char X, unsigned char Y, char DData)
{
	Y &= 0x1;
	X &= 0xF;   //限制 X 不能大于 15，Y 不能大于 1
	if (Y) X |= 0x40;   //当要显示第二行时地址码+0x40;
	X |= 0x80;       //算出指令码
	writeOneChar(CMD, X);    //这里不检测忙信号，发送地址码
	writeOneChar(DATA, DData);
}

void writeOneChar(const uint8_t cmdOrData, const uint8_t Data)
{
	uint8_t LCM_Data = 0;
	LCM_Data = ((Data & 0xF0) | (cmdOrData) | (backLight));
	i2cAction(LCM_Data | 0x04);

	i2cAction(LCM_Data);
	delay_us(20);

	LCM_Data = (((Data & 0x0F)<<4) | (cmdOrData) | (backLight));
	i2cAction(LCM_Data | 0x04);

	i2cAction(LCM_Data);
	delay_us(20);
}

/***
	set position of cursor
***/
void setCursor(uint8_t colmn, uint8_t line)
{
	line &= 0x1;
	colmn &= 0xF;   //限制 X 不能大于 15，line 不能大于 1
	if (line) colmn |= 0x40;   //当要显示第二行时地址码+0x40;
	colmn |= 0x80;       //算出指令码
	writeOneChar(CMD, colmn);    //这里不检测忙信号，发送地址码
}

/***
	send one frame by iic
***/
void i2cAction(uint8_t status)
{
	IIC_Start();
	IIC_Send_Byte(ADDRWRITE);
	IIC_Wait_Ack();
	IIC_Send_Byte(status);
	IIC_Wait_Ack();
	IIC_Stop();
}
