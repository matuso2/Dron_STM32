/*
 * buttons_and_leds.c
 *
 *  Created on: Dec 4, 2022
 *      Author: admin
 */

#include "buttons_and_leds.h"

uint32_t edge_state = 0; // previous impulse value
uint32_t count_i = 1;
uint8_t rc_control_state = 1;
int last_button_state, state_changed_from_rc=0;




int checkIfStateChangedFromRc()
{
	if (state_changed_from_rc)
	{
		state_changed_from_rc = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t getRcControlState()
{
	return rc_control_state;
}

EDGE_TYPE edgeDetect(uint8_t pin_state, uint8_t samples)
{
	/* pin_state = aktualny stav vstupneho pinu
	* samples  = kolkokrat musi byt detegovany stav vstupneho pin
	*/
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

void checkControlStateButton()
{
	//BUTTON_GET_STATE - aktualny stav na pine
	//edge_state - globalna premenna co bolo na pine
	if(edgeDetect(BUTTON1_GET_STATE,5) == RISING)
	{
		last_button_state = rc_control_state;

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

		// bol RC a teraz je INE
		if(last_button_state && !rc_control_state){
			state_changed_from_rc = 1;
		}
	}
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
