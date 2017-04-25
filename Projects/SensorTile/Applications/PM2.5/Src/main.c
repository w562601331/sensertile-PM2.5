/**
******************************************************************************
* @file    DataLog/Src/main.c
* @author  Central Labs
* @version V1.1.0
* @date    27-Sept-2016
* @brief   Main program body
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include <string.h> /* strlen */
#include <stdio.h>  /* sprintf */
#include <math.h>   /* trunc */
#include "main.h"

#include "datalog_application.h"
#include "usbd_cdc_interface.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

/* Private typedef -----------------------------------------------------------*/


#include "string.h"
#include "SensorTile.h"

#include "1602_iic_sw.h"
#include "moniiic.h"
#include "AM2320.h"

/* Private define ------------------------------------------------------------*/

/* Data acquisition period [ms] */
#define DATA_PERIOD_MS (100)
//#define NOT_DEBUGGING

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* SendOverUSB = 0  --> Save sensors data on SDCard (enable with double click) */
/* SendOverUSB = 1  --> Send sensors data via USB */
uint8_t SendOverUSB=1;
UART_HandleTypeDef huart5;
uint8_t pRxData[1];

	uint8_t flag=0;
	uint8_t start=0;
	uint8_t i=0;
 	uint8_t pmbuff[9];
	uint8_t air[]="";
	uint8_t j=0;
	uint8_t onerun=1;
	uint8_t sdcount=0;
	uint8_t itflag=1;
	uint8_t count=0;
	uint16_t pressfloat;




static volatile uint8_t MEMSInterrupt = 0;
static volatile uint8_t acquire_data_enable_request  = 1;
static volatile uint8_t acquire_data_disable_request = 0;
static volatile uint8_t no_H_HTS221 = 0;
static volatile uint8_t no_T_HTS221 = 0;
static volatile uint8_t no_GG = 0;

static RTC_HandleTypeDef RtcHandle;
static void *LSM6DSM_X_0_handle = NULL;
static void *LSM6DSM_G_0_handle = NULL;
static void *LSM303AGR_X_0_handle = NULL;
static void *LSM303AGR_M_0_handle = NULL;
static void *LPS22HB_P_0_handle = NULL;
static void *LPS22HB_T_0_handle = NULL; 
static void *HTS221_H_0_handle = NULL; 
static void *HTS221_T_0_handle = NULL;


/* Private function prototypes -----------------------------------------------*/

static void Error_Handler( void );
static void RTC_Config( void );
static void RTC_TimeStampConfig( void );
static void initializeAllSensors( void );
static void MX_GPIO_Init(void);
static void MX_UART5_Init(void);
extern char dataOut[256];
extern float pressure;
static uint32_t fac_us=0;
extern FRESULT res;
extern FIL MyFile; 
extern uint32_t byteswritten;

/* Private functions ---------------------------------------------------------*/

extern void PM(void);	


void wenshi(void)
{
	
	
//	printf("\r\ntemperature:%d.%02d",(wendubuff[0]*256+wendubuff[1])/10,(wendubuff[0]*256+wendubuff[1])%10);	
	
}


int fputc(int ch,FILE *f)
{
    uint8_t temp[1]={ch};
    HAL_UART_Transmit(&huart5,temp,1,2);
    return 1;
}





 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
				uint8_t rec;

				rec=*((huart->pRxBuffPtr)-1);	
				
				if(rec==0xff) start=1;
				
				
				if(start==1) 
				{i++;

				pmbuff[i-1]=rec;
		
				if(i==9)
							{
									i=0;
							start=0;
								
								
			SendOverUSB=1;
			SD_Log_Enabled=0;

			RTC_Handler( &RtcHandle );
		
			PM();
	//		wenshi();
      
      Accelero_Sensor_Handler( LSM6DSM_X_0_handle );
      
      Gyro_Sensor_Handler( LSM6DSM_G_0_handle );
      
     Magneto_Sensor_Handler( LSM303AGR_M_0_handle );
      
      Pressure_Sensor_Handler( LPS22HB_P_0_handle );
			
						sdcount++;		

								
								}
								

							
				}
				
				if(itflag==1)
				{
	
HAL_UART_Receive_IT(&huart5, pRxData, sizeof(pRxData));//使能接收中断
				}

}


void UART5_IRQHandler(void)
{

  HAL_UART_IRQHandler(&huart5);


}


void delay_init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main( void )
{
//	HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	  	  
//	HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
//	HAL_Delay(500);//选择AM2320工作模式
	
  uint32_t msTick, msTickPrev = 0;
  
  /* STM32L4xx HAL library initialization:
  - Configure the Flash prefetch, instruction and Data caches
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 4
  - Global MSP (MCU Support Package) initialization
  */
  HAL_Init();
	
  
  /* Configure the system clock */
  SystemClock_Config();
  
	//串口设置
	MX_GPIO_Init();
  MX_UART5_Init();
	
	
	//LCD1602
	
	delay_init(64);
	HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);
	init_1602();

	
	
  /* Initialize LED */
    BSP_LED_Init(LED1);
    


  
  /* Initialize RTC */
  RTC_Config();
  RTC_TimeStampConfig();
  
  DATALOG_SD_Init();//SD卡初始化

  
  /* Initialize and Enable the available sensors */
		initializeAllSensors();
		enableAllSensors();
			
		BSP_LED_On(LED1);	 
	
	
		//	HAL_UART_Receive_IT(&huart5, pRxData, sizeof(pRxData));//使能接收中断
		while(HAL_UART_Receive_IT(&huart5, pRxData, sizeof(pRxData))==HAL_OK);
		while (1)
  {
		
		count++;
		
    /* Get sysTick value and check if it's time to execute the task */
    msTick = HAL_GetTick();
    if(msTick % DATA_PERIOD_MS == 0 && msTickPrev != msTick)
    {
			msTickPrev = msTick;
		}
		

		
//		readAM2320();

		
		
		if(count==255)
			{
					count=0;
		
					setCursor(0,0);

					lcdPrint("PM2.5: .  ");

						setCursor(6,0);

					writeOneChar(DATA, pmbuff[3]+0x30);

						setCursor(8,0);

					writeOneChar(DATA, pmbuff[4]/10+0x30);
						setCursor(9,0);
					writeOneChar(DATA, pmbuff[4]%10+0x30);
	

					setCursor(0,1);
					lcdPrint("Pressure:   .  P");

						setCursor(9,1);

					writeOneChar(DATA, (uint16_t)(pressure)/100+0x30);

						setCursor(10,1);

					writeOneChar(DATA, (uint16_t)(pressure)%100/10+0x30);
						setCursor(11,1);
					writeOneChar(DATA, (uint16_t)(pressure)%10+0x30);
					
					pressfloat=(pressure-(uint16_t)(pressure))*100;
						setCursor(13,1);
						
					writeOneChar(DATA, pressfloat/10+0x30);
					setCursor(14,1);
					writeOneChar(DATA, pressfloat%10+0x30);
					
		
			}
		
		
		
			
			if(sdcount==10)
			{
				itflag=0;
				sdcount=0;
			
			HAL_Delay(10);//延时使中断结束
			
			
			SendOverUSB=0;
			SD_Log_Enabled=1;
			
			

			DATALOG_SD_Log_Enable();

			

			RTC_Handler( &RtcHandle );
		

		//	wenshi();
      
      Accelero_Sensor_Handler( LSM6DSM_X_0_handle );
      
      Gyro_Sensor_Handler( LSM6DSM_G_0_handle );
      
			Magneto_Sensor_Handler( LSM303AGR_M_0_handle );
      
      Pressure_Sensor_Handler( LPS22HB_P_0_handle );

			PM();
			
			DATALOG_SD_Log_Disable();
			itflag=1;
			
				
			
			HAL_UART_Receive_IT(&huart5, pRxData, sizeof(pRxData));//使能接收中断
			}
			
	
			BSP_LED_Off(LED1);
 

	}
}


/**
* @brief  Initialize all sensors
* @param  None
* @retval None
*/
static void initializeAllSensors( void )
	
{

  if (BSP_ACCELERO_Init( LSM6DSM_X_0, &LSM6DSM_X_0_handle ) != COMPONENT_OK)
  {
  //  while(1);
		HAL_Delay(100);
  }
  
  if (BSP_GYRO_Init( LSM6DSM_G_0, &LSM6DSM_G_0_handle ) != COMPONENT_OK)
  {
  //  while(1);
		HAL_Delay(100);
  }
  
  if (BSP_ACCELERO_Init( LSM303AGR_X_0, &LSM303AGR_X_0_handle ) != COMPONENT_OK)
  {
  //  while(1);
		HAL_Delay(100);
  }
  
  if (BSP_MAGNETO_Init( LSM303AGR_M_0, &LSM303AGR_M_0_handle ) != COMPONENT_OK)
  {
  //  while(1);
		HAL_Delay(100);
  }
  
  if (BSP_PRESSURE_Init( LPS22HB_P_0, &LPS22HB_P_0_handle ) != COMPONENT_OK)
  {
  //  while(1);
		HAL_Delay(100);
  }
  
 
  
  

  
  
}

/**
* @brief  Enable all sensors
* @param  None
* @retval None
*/
void enableAllSensors( void )
{
  BSP_ACCELERO_Sensor_Enable( LSM6DSM_X_0_handle );
  BSP_GYRO_Sensor_Enable( LSM6DSM_G_0_handle );
  BSP_ACCELERO_Sensor_Enable( LSM303AGR_X_0_handle );
  BSP_MAGNETO_Sensor_Enable( LSM303AGR_M_0_handle );
  BSP_PRESSURE_Sensor_Enable( LPS22HB_P_0_handle );
 
}



/**
* @brief  Disable all sensors
* @param  None
* @retval None
*/
void disableAllSensors( void )
{
  BSP_ACCELERO_Sensor_Disable( LSM6DSM_X_0_handle );
  BSP_ACCELERO_Sensor_Disable( LSM303AGR_X_0_handle );
  BSP_GYRO_Sensor_Disable( LSM6DSM_G_0_handle );
  BSP_MAGNETO_Sensor_Disable( LSM303AGR_M_0_handle );
  BSP_HUMIDITY_Sensor_Disable( HTS221_H_0_handle );
  BSP_TEMPERATURE_Sensor_Disable( HTS221_T_0_handle );
  BSP_TEMPERATURE_Sensor_Disable( LPS22HB_T_0_handle );
  BSP_PRESSURE_Sensor_Disable( LPS22HB_P_0_handle );
}



/**
* @brief  Configures the RTC
* @param  None
* @retval None
*/
static void RTC_Config( void )
{
  /*##-1- Configure the RTC peripheral #######################################*/
  RtcHandle.Instance = RTC;
  
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
  - Hour Format    = Format 12
  - Asynch Prediv  = Value according to source clock
  - Synch Prediv   = Value according to source clock
  - OutPut         = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType     = Open Drain */
  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if ( HAL_RTC_Init( &RtcHandle ) != HAL_OK )
  {
    
    /* Initialization Error */
    Error_Handler();
  }
}

/**
* @brief  Configures the current time and date
* @param  None
* @retval None
*/
static void RTC_TimeStampConfig( void )
{
  
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
  
  /*##-3- Configure the Date using BCD format ################################*/
  /* Set Date: Monday January 1st 2000 */
  sdatestructure.Year    = 0x00;
  sdatestructure.Month   = RTC_MONTH_JANUARY;
  sdatestructure.Date    = 0x01;
  sdatestructure.WeekDay = RTC_WEEKDAY_MONDAY;
  
  if ( HAL_RTC_SetDate( &RtcHandle, &sdatestructure, FORMAT_BCD ) != HAL_OK )
  {
    
    /* Initialization Error */
    Error_Handler();
  }
  
  /*##-4- Configure the Time using BCD format#################################*/
  /* Set Time: 00:00:00 */
  stimestructure.Hours          = 0x00;
  stimestructure.Minutes        = 0x00;
  stimestructure.Seconds        = 0x00;
  stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if ( HAL_RTC_SetTime( &RtcHandle, &stimestructure, FORMAT_BCD ) != HAL_OK )
  {   
    /* Initialization Error */
    Error_Handler();
  }
}

/**
* @brief  Configures the current time and date
* @param  hh the hour value to be set
* @param  mm the minute value to be set
* @param  ss the second value to be set
* @retval None
*/
void RTC_TimeRegulate( uint8_t hh, uint8_t mm, uint8_t ss )
{
  
  RTC_TimeTypeDef stimestructure;
  
  stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM;
  stimestructure.Hours          = hh;
  stimestructure.Minutes        = mm;
  stimestructure.Seconds        = ss;
  stimestructure.SubSeconds     = 0;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if ( HAL_RTC_SetTime( &RtcHandle, &stimestructure, FORMAT_BIN ) != HAL_OK )
  {
    /* Initialization Error */
    Error_Handler();
  }
}



/**
* @brief  EXTI line detection callbacks
* @param  GPIO_Pin: Specifies the pins connected EXTI line
* @retval None
*/
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
  MEMSInterrupt=1;
}



/**
* @brief  This function is executed in case of error occurrence
* @param  None
* @retval None
*/
static void Error_Handler( void )
{
  
  while (1)
  {}
}


static void MX_GPIO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();



  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pins : PC1 PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

static void MX_UART5_Init(void)
{

  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }

}


void delay_init(uint8_t SYSCLK)
{

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	fac_us=SYSCLK;					

}	 

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			    	 
	ticks=nus*fac_us; 						

	told=SysTick->VAL;        		
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;		
		}  
	};
										    
}  

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed( uint8_t *file, uint32_t line )
{
  
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  while (1)
  {}
}



#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
