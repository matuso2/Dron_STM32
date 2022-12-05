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

/*variables & defines*/
#define CHAR_BUFF_SIZE	30
char formated_text[50];
char commandToPutty[] = "cmdToPutty";
float  acc[3], gyro[3];
int roll_speed, pitch_speed, yaw_speed, vertical_speed, last_button_state, zero_speed_iterations=0;

/*config*/
int max_roll_speed = 50;
int max_pitch_speed = 50;
int control_type = 2; //1 linear, 2 quadratic

/*declaration of functions*/
void SystemClock_Config(void);

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
		  if (getRcControlState())
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


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
