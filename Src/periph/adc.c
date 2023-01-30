#include "stm32f3xx.h"
#include "adc.h"


void init_ADC(void)
{
	// Включить тактирование АЦП
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;


	// -- Калибровка АЦП ---------------------------
	ADC1->CR |= ADC_CR_ADVREGEN_0;    // Vref enable

	// Задержка не менее 10 мкс
	for(uint32_t i = 0; i < 720; i++ ) asm("nop");



}
