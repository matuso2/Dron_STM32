/*
 * assignment.h
 *
 *  Created on: Dec 1, 2022
 *      Author: jakub
 */

#include "stdio.h"

//STM32F328k8
#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

//TYPEDEF
typedef enum EDGE_TYPE{
	NONE = 0,
	RISING = 1,
	FALLING = 2
}EDGE_TYPE;

EDGE_TYPE edgeDetect(uint8_t pin_state, uint8_t samples);
void setRegisters(void);

/* General purpose input output port A macros */
//GPIOA peripheral base address
#define	GPIOA_BASE_ADDR			(uint32_t)(0x48000000U)
//MODER register
#define	GPIOA_MODER_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x00U)))
//OTYPER register
#define	GPIOA_OTYPER_REG		*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x04U)))
//OSPEEDER register
#define GPIOA_OSPEEDER_REG		*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x08U)))
//PUPDR register
#define GPIOA_PUPDR_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x0CU)))
//IDR register
#define GPIOA_IDR_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x10U)))
//ODR register
#define GPIOA_ODR_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x14U)))
//BSRR register
#define GPIOA_BSRR_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x18U)))
//BRR register
#define GPIOA_BRR_REG			*((volatile uint32_t *)((uint32_t)(GPIOA_BASE_ADDR + 0x28U)))

/*Reset clock control register macros */
//RCC base address
#define	RCC_BASE_ADDR			(uint32_t)(0x40021000)
//AHBEN register
#define	RCC_AHBENR_REG			*((volatile uint32_t *) (uint32_t)(0x40021000 + 0x00000014U))
//                              *((volatile uint32_t *)(uint32_t)(RCC_BASE_ADDR + 0x00000014U))

/* LED and button macros */
#define LED1_ON					*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x18U))) |= (1 << 4) // on
#define LED1_OFF					*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x28U))) |= (1 << 4) // reset
#define LED2_ON					*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x18U))) |= (1 << 1) // on
#define LED2_OFF					*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x28U))) |= (1 << 1) // reset

#define BUTTON1_GET_STATE		!(*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x10U))) & (1 << 3)) // 16b
#define BUTTON2_GET_STATE		!(*((volatile uint32_t *)((uint32_t)(0x48000000 + 0x10U))) & (1 << 0)) // 16b




#endif /* ASSIGNMENT_H_ */
