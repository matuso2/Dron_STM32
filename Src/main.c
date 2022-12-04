/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "dma.h"

#include "lsm6dsl.h"
#include "computation.h"
#include "math.h"
#include "buttons_and_leds.h"

#define CHAR_BUFF_SIZE	30
uint8_t temp = 0;
float mag[3], acc[3], gyro[3];
int roll_speed, pitch_speed;
char formated_text[50];
uint8_t rc_control_state = 1;
char commandToPutty[] = "cmdToPutty";
void SystemClock_Config(void);
void stateButtonControl();

/*config, after testing set both speeds to 100*/
int max_roll_speed = 50;
int max_pitch_speed = 50;
int control_type = 2; //1 linear, 2 quadratic

int main(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  lsm6dsl_init();

  setRegisters();
  LED1_ON;

  while (1)
  {
	  stateButtonControl();
	  memset(formated_text, '\0', sizeof(formated_text));

	  // button 2 has to be pressed in order to control drone via STM
	  if (BUTTON2_GET_STATE)
	  {
		  // RC control section
		  if (rc_control_state)
		  {
			  lsm6dsl_get_acc(acc, (acc+1), (acc+2));
			  lsm6dsl_get_gyro(gyro,(gyro+1), (gyro+2));

			  roll_speed = compute_roll_speed(acc, max_roll_speed, control_type);
			  pitch_speed = compute_pitch_speed(acc, max_pitch_speed, control_type);

			  // format: LR,FB,UD,Y,command
			  sprintf(formated_text, "%d, %d, %d, %d, %s \r", roll_speed, pitch_speed, 0, 0, "rc" );
		  }
		  // OTHER control section (flips, land & take_off)
		  else
		  {
			  lsm6dsl_get_acc(acc, (acc+1), (acc+2));
			  lsm6dsl_get_gyro(gyro,(gyro+1), (gyro+2));

			  roll_speed = compute_roll_speed(acc, max_roll_speed, control_type);
			  pitch_speed = compute_pitch_speed(acc, max_pitch_speed, control_type);

			  // format: LR,FB,UD,Y,command
			  sprintf(formated_text, "%d, %d, %d, %d, %s \r", roll_speed, pitch_speed, 0, 0, commandToPutty);
		  }

		  LED2_ON;
	  }
	  // do nothing
	  else
	  {
		  sprintf(formated_text, "%d, %d, %d, %d, %s \r", 0, 0, 0, 0, "" );
		  LED2_OFF;
	  }

	  USART2_PutBuffer((uint8_t*)formated_text, strlen(formated_text));
	  LL_mDelay(10);
  }
}


void stateButtonControl()
{
	//BUTTON_GET_STATE - aktualny stav na pine
	//edge_state - globalna premenna co bolo na pine
	if(edgeDetect(BUTTON1_GET_STATE,5) == RISING)
	{
		if(rc_control_state == 0)
		{
			LED1_ON;
		  	rc_control_state = 1;
		}
		else
		{
			LED1_OFF;
		  	rc_control_state = 0;
		}
	}
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
