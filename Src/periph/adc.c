#include "stm32f3xx.h"
#include "adc.h"


void init_adc(void)
{
	// Включить тактирование АЦП
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;



	// -- Калибровка АЦП ---------------------------

	ADC1->CR |= ADC_CR_ADVREGEN_0;    // Vref enable
	// Задержка не менее 10 мкс
	for(uint32_t i = 0; i < 720; i++ ) asm("nop");





	// Запись 0 для калибровки в недифференциальном режиме
	ADC1->CR &= ~ADC_CR_ADCALDIF;

	// Старт калибровки
	ADC1->CR |= ADC_CR_ADCAL;

	// Ожидание окончания калибровки ( выход когда ADCAL = 0)
	while (ADC1->CR & ADC_CR_ADCAL);
	for(uint32_t i = 0; i < 720; i++ ) asm("nop");

	//Аппаратный триггер выборки по нарастающему фронту
	//ADC1->CFGR |= ADC_CFGR_EXTEN_0;

	// Event 7 (0111) - HRTIM_ADCTRG1 event
	//ADC1->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;
	ADC1->CR |= ADC_CR_ADVREGEN_0;    // Vref enable
	for(uint32_t i = 0; i < 720; i++ ) asm("nop");
	// Включить ADC1
	ADC1->CR = ADC_CR_ADEN;
	ADC1->CR |= ADC_CR_ADVREGEN_0;    // Vref enable

	// Select ADC1 channel IN4
	ADC1->SQR1 |= ADC_SQR1_SQ1_2;

    // Устанавливаем период выборки равным 480 циклов.
     ADC1->SMPR1 |= ADC_SMPR1_SMP4_0 | ADC_SMPR1_SMP4_1 | ADC_SMPR1_SMP4_2;

	//  ADC1->CFGR |= ADC_CFGR_CONT;
	// Length regular ADC channel = 1
	//ADC1->SQR1 &= ~ADC_SQR1_L;


//	ADC1->IER |= ADC_IER_ADRDYIE;

	//NVIC_SetPriorityGrouping(3);
	//NVIC_SetPriority(ADC1_2_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 1));
	// Enable interrupt ADC1 and ADC2
//	NVIC_EnableIRQ(ADC1_2_IRQn);



	// Включить ADC1
	//ADC1->CR |= ADC_CR_ADEN;

	// Задержка не менее 10 мкс
	//for(uint32_t i = 0; i < 720; i++) asm("nop");

	//  ADC start of regular conversion
	ADC1->CR |= ADC_CR_ADSTART;





}
