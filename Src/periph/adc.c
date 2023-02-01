#include "stm32f3xx.h"
#include "adc.h"


void init_adc(void)
{
	// Включить тактирование АЦП
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;



    // Очистить биты источника опорного
	ADC1->CR &= ~ADC_CR_ADVREGEN;
	ADC1->CR &= ~ADC_CR_ADVREGEN;

	// Включить опорное напряжение
	ADC1->CR |= ADC_CR_ADVREGEN_0;
	ADC2->CR |= ADC_CR_ADVREGEN_0;
	// Дождаться не менее 10 мкс
	for(int i = 0; i< 1000; i++) asm("nop");

	// Калибровка для недифференциального режима
	ADC1->CR &= ~ADC_CR_ADCALDIF;
	ADC2->CR &= ~ADC_CR_ADCALDIF;

	// Начать калибровку и дождаться её окончания
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);
	ADC2->CR |= ADC_CR_ADCAL;
	while (ADC2->CR & ADC_CR_ADCAL);



	// Выбор для старта преобразования внешний триггер
	ADC1->CFGR |= ADC_CFGR_EXTEN_0;
	ADC2->CFGR |= ADC_CFGR_EXTEN_0;

	// Тип триггера Event 7 - HRTIM
	ADC2->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;

	// Select ADC2 channel IN13 (1101) - Vout
	ADC2->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3;

	// Length regular ADC channel = 1
	ADC2->SQR1 &= ~ADC_SQR1_L;

	//ADC2->IER |= ADC_IER_EOCIE;    // Interrupt enable
	//NVIC_EnableIRQ(ADC1_2_IRQn);    // enable interrupt ADC1 and ADC2


	// DMA Circular Mode selected
	ADC2->CFGR |= ADC_CFGR_DMACFG;

	// Direct memory access enable
	ADC2->CFGR |= ADC_CFGR_DMAEN;


	// Включить АЦП2
	ADC2->CR |= ADC_CR_ADEN;

	// Подождать включения
	for(int i = 0; i< 1000; i++) asm("nop");

	// Запуск АЦП2
	ADC2->CR |= ADC_CR_ADSTART;
	}

