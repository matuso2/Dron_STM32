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
uint8_t rc_control_state = 1;
char commandToPutty[] = "cmdToPutty";
uint32_t edge_state = 0; // previous impulse value
uint32_t count_i = 1;
void SystemClock_Config(void);
void setRegisters();
void stateButtonControl();
EDGE_TYPE edgeDetect(uint8_t pin_state, uint8_t samples);
int roll_speed, pitch_speed, yaw_speed, vertical_speed, last_button_state, zero_speed_iterations=0;
char formated_text[50];

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
	  if (zero_speed_iterations!=0){
	  sprintf(formated_text, "\\%d, %d, %d, %d, %s \n\r", 0, 0, 0, 0, "rc" );
	 				  zero_speed_iterations=zero_speed_iterations-1;}

	  if (BUTTON2_GET_STATE)
	  {
		  // RC control section
		  if (rc_control_state)
		  {
			  lsm6dsl_get_acc(acc, (acc+1), (acc+2));
			  lsm6dsl_get_gyro(gyro,(gyro+1), (gyro+2));

			  yaw_speed = compute_yaw_speed(gyro);
			  vertical_speed = compute_vertical_speed(acc);

			  roll_speed = compute_roll_speed(acc, max_roll_speed, control_type);
			  pitch_speed = compute_pitch_speed(acc, max_pitch_speed, control_type);

			  // format: LR,FB,UD,Y,command
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s \n\r", -roll_speed, pitch_speed, vertical_speed*20, yaw_speed*20, "rc" );
		  }
		  // OTHER control section (flips, land & take_off)
		  else
		  {
			  if(zero_speed_iterations==0){
				  lsm6dsl_get_acc(acc, (acc+1), (acc+2));
				  lsm6dsl_get_gyro(gyro,(gyro+1), (gyro+2));

				  int takeoff_land = compute_vertical_speed(acc);
				  if (takeoff_land<0){
					  memset(commandToPutty,'\0', sizeof(commandToPutty));
					  strcpy(commandToPutty,"LAND");
				  }
				  else if (takeoff_land>0){
					  memset(commandToPutty,'\0', sizeof(commandToPutty));
					  strcpy(commandToPutty,"TAKEOFF");
				  }
				  else{
					  memset(commandToPutty,'\0', sizeof(commandToPutty));
				  	  strcpy(commandToPutty,"donothing");
				  }
				  //roll_speed = compute_roll_speed(acc, max_roll_speed, control_type);
				  //pitch_speed = compute_pitch_speed(acc, max_pitch_speed, control_type);

				  // format: LR,FB,UD,Y,command
				  sprintf(formated_text, "\\%d, %d, %d, %d, %s \n\r", 1, 2, 3, 4, commandToPutty);
			  }

		  }

		  LED2_ON;
	  }
	  // do nothing
	  else
		  //
	  {
		  if(zero_speed_iterations==0){
		  sprintf(formated_text, "\\%d, %d, %d, %d, %s \n\r", 0, 0, 0, 0, "donothing" );
		  LED2_OFF;}
	  }

	  if(BUTTON2_GET_STATE==0&&rc_control_state==1){
		  sprintf(formated_text, "\\%d, %d, %d, %d, %s \n\r", 0, 0, 0, 0, "rc" );
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
		last_button_state=rc_control_state;
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
		if(last_button_state==1&&rc_control_state==0){
			zero_speed_iterations=5;
		}
	}
}

// pin_state = aktualny stav vstupneho pinu
// samples  = kolkokrat musi byt detegovany stav vstupneho pinu
EDGE_TYPE edgeDetect(uint8_t pin_state, uint8_t samples)
{
	if (edge_state != pin_state) //prve volanie funkcie
	{
		count_i = 1;
	}
	else //ostatne volania fukncie kym sa pin_state nemeni
	{
		count_i ++;
	}

	if (count_i == samples) //zapnutie led len ak sa rovnaju premenne
	{
		if (pin_state) //ak je stlacene tlacidlo
		{
			return RISING;
		}
	}

	edge_state = pin_state;
	return NONE;
}

void setRegisters()
{
	RCC_AHBENR_REG |= (uint32_t)(1 << 17);

	  /*OUTPUT*/
	  /*A4(p4) A2(p1) */
	  GPIOA_MODER_REG &= ~(uint32_t)(0x3 << 8);  //reset
	  GPIOA_MODER_REG |= (uint32_t)(1 << 8);	//output - 01
	  GPIOA_OTYPER_REG &= ~(0x1 << 4); //OTYPER - 16b - output push pull / reset
	  GPIOA_OSPEEDER_REG &= ~(0x3 << 8); //OSPEEDR - 32b - x0=low speed
	  GPIOA_PUPDR_REG &= ~(0x3 << 8); // PUPDR - 32b - 00=no pull, no pull down

	  GPIOA_MODER_REG &= ~(uint32_t)(0x3 << 2);  //reset
	  GPIOA_MODER_REG |= (uint32_t)(1 << 2);	//output - 01
	  GPIOA_OTYPER_REG &= ~(0x1 << 1); //OTYPER - 16b - output push pull / reset
	  GPIOA_OSPEEDER_REG &= ~(0x3 << 2); //OSPEEDR - 32b - x0=low speed
	  GPIOA_PUPDR_REG &= ~(0x3 << 2); // PUPDR - 32b - 00=no pull, no pull down

	  /*INPUT*/
	  /*A2(p3) A0(p0)*/
	  GPIOA_MODER_REG &= ~(uint32_t)(0x3 << 6); // bazova addr - 00=input
	  GPIOA_PUPDR_REG &= ~(0x3 << 6); // PUPDR - 32b - reset
	  GPIOA_PUPDR_REG |= (0x1 << 6); //01=pull up - ked stalcim tlacidlo = logicka 0

	  GPIOA_MODER_REG &= ~(uint32_t)(0x3 << 0); // bazova addr - 00=input
	  GPIOA_PUPDR_REG &= ~(0x3 << 0); // PUPDR - 32b - reset
	  GPIOA_PUPDR_REG |= (0x1 << 0); //01=pull up - ked stalcim tlacidlo = logicka 0
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
  SysTick_Config(SystemCoreClock/1000);
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
