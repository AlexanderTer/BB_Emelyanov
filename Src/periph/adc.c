#include "stm32f3xx.h"
#include "adc.h"


void init_adc(void)
{
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;
	/************************************************
	*                          Calibration
	***********************************************/

	ADC2->CR &= ~ADC_CR_ADVREGEN;
	ADC2->CR |= ADC_CR_ADVREGEN_0;    // Vref enable
	for(int i = 0; i< 10000; i++) asm("nop");
	ADC2->CR &= ~ADC_CR_ADCALDIF;

	ADC2->CR |= ADC_CR_ADCAL;    // Start calibration
	while (ADC2->CR & ADC_CR_ADCAL);    // Wait end calibration

	/************************************************
	*              Select event trigger and channel
	***********************************************/

	// Enable start conversion external trigger
	ADC2->CFGR |= ADC_CFGR_EXTEN_0;

	// Event 7 - HRTIM
	ADC2->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;

	// Select ADC2 channel IN5
	ADC2->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3;

	// Length regular ADC channel = 1
	ADC2->SQR1 &= ~ADC_SQR1_L;

	ADC2->IER |= ADC_IER_EOCIE;    // Interrupt enable
	NVIC_EnableIRQ(ADC1_2_IRQn);    // enable interrupt ADC1 and ADC2

	/************************************************
	*                           Start ADC
	***********************************************/

	ADC2->CR |= ADC_CR_ADEN;    // Enable ADC2
	for(int i = 0; i< 10000; i++) asm("nop");
	ADC2->CR |= ADC_CR_ADSTART;
	}

