/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "dma.h"
#include <stdlib.h>

#include "lsm6dsl.h"
#include "computation.h"
#include "math.h"
#include "buttons_and_leds.h"

/*variables & defines*/
 // format: LR,FB,UD,Y,command
#define CHAR_BUFF_SIZE	30
char formated_text[50];
char commandToPutty[] = "cmdToPutty";
float  acc[3], gyro[3];
int roll_speed, pitch_speed, yaw_speed, vertical_speed, speed_reset_delay=0;
int takeoff_land; //>0 = takeoff, <0 = land
/*config*/
int max_roll_speed = 50;
int max_pitch_speed = 50;
int control_type = 2; //1 linear, 2 quadratic
int counter = 0;
int flipSpeedThreshold = 48;
int flipLock = 0;
/*declaration of functions*/
void SystemClock_Config(void);
void setCommandToPutty(char cmd[50]);
void checkIfTakeOffOrLand();
int countUp(int c);
void checkForFlip();

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
	  counter = countUp(counter);
	  memset(formated_text, '\0', sizeof(formated_text));

	  checkControlStateButton();
	  if(checkIfStateChangedFromRc())
	  {
		  speed_reset_delay = 5;
	  }

	  if (speed_reset_delay)
	  {
		  setCommandToPutty("rc");
		  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", 0, 0, 0, 0, commandToPutty, counter);
		  speed_reset_delay -= 1;
	  }

	  // BUTTON2 PRESSED
	  if (BUTTON2_GET_STATE && !speed_reset_delay)
	  {

		  lsm6dsl_get_acc(acc, (acc+1), (acc+2));
		  lsm6dsl_get_gyro(gyro,(gyro+1), (gyro+2));

		  // RC control section
		  if (getRcControlState())
		  {

			  yaw_speed = -20*compute_yaw_speed(gyro);
			  vertical_speed = 20*compute_vertical_speed(acc);

			  roll_speed = compute_roll_speed(acc, max_roll_speed, control_type);
			  pitch_speed = -compute_pitch_speed(acc, max_pitch_speed, control_type);

			  setCommandToPutty("rc");
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", roll_speed, pitch_speed, vertical_speed, yaw_speed, commandToPutty, counter);
		  }
		  // OTHER control section (flips, land & take_off)
		  else
		  {
			  //compute_roll_speed(acc, max_roll_speed, control_type);
			  //-compute_pitch_speed(acc, max_pitch_speed, control_type);
			  // todo vytvorit funkciu ktora detekuje flip na zaklade roll a pitch.

			  setCommandToPutty("doNothing");
			  checkForFlip();
			  takeoff_land = compute_vertical_speed(acc); // nemalo by byt toto dane do funkcie v riadku 95???
			  checkIfTakeOffOrLand(compute_vertical_speed(acc)); // nechapem ako do tejto funkcie mozes dat premennu ked ta funkcia nema definovane ziadne parametre

			  // pridat flipy
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", 1, 2, 3, 4, commandToPutty, counter);
		  }

		  LED2_ON;
	  }
	  // BUTTON2 NOT PRESSED
	  else
	  {
		  // RC control section
		  if (getRcControlState())
		  {
			  setCommandToPutty("rc");
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", 0, 0, 0, 0, commandToPutty, counter);
		  }
		  // if sending 5times RC null speeds
		  else if(speed_reset_delay)
		  {
			  setCommandToPutty("rc");
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", 0, 0, 0, 0, commandToPutty ,counter);
		  }
		  // OTHER control section (flips, land & take_off)
		  else
		  {
			  setCommandToPutty("doNothing");
			  sprintf(formated_text, "\\%d, %d, %d, %d, %s, %d \n\r", 1, 2, 3, 4, commandToPutty ,counter);
		  }

		  LED2_OFF;
	  }

	  USART2_PutBuffer((uint8_t*)formated_text, strlen(formated_text));
	  LL_mDelay(10);
  }
}
int countUp(int c){
	 int maxCount = 1000;
	 if(c < maxCount){
		 c++;
	 }
	 else{
		 c = 0;
	 }
	 return c;
}
void checkForFlip(){
	 int r_speed = compute_roll_speed(acc, max_roll_speed, control_type);
	 int p_speed = -compute_pitch_speed(acc, max_pitch_speed, control_type);
	 if(abs(r_speed) < 10 && abs(p_speed) < 10){
			 flipLock = 0;
		 }
	 if(flipLock == 0){
	 if (abs(r_speed) > flipSpeedThreshold && abs(p_speed) < flipSpeedThreshold){
		 //r_flip
		 if(r_speed > 0 ){
			 setCommandToPutty("rFlip");
		 }
		 else{
			 setCommandToPutty("lFlip");

		 }
		 flipLock = 1;
	 }
	 if(abs(p_speed) > flipSpeedThreshold && abs(r_speed) < flipSpeedThreshold){
		 //p_flip
		if(p_speed > 0 ){
			setCommandToPutty("fFlip");
		}
		else{
			setCommandToPutty("bFlip");
		}
		flipLock = 1;
	 }
	 }

}

void setCommandToPutty(char cmd[50])
{
	memset(commandToPutty,'\0', sizeof(commandToPutty));
	strcpy(commandToPutty, cmd);
}

void checkIfTakeOffOrLand()
{
	if (takeoff_land<0){
		setCommandToPutty("LAND");
	}
	else if (takeoff_land>0){
		setCommandToPutty("TAKEOFF");
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
