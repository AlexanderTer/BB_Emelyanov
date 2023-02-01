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
	for (int i = 0; i < 1000; i++) asm("nop");

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
	ADC1->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;
	ADC2->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;



	// Выбор канала первого преобразования
	ADC1->SQR1 |= ADC_SQR1_SQ1_2;                                   // CH IN4 (100) - Il
	ADC2->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3; // CH IN13 (1101) - Vout

	// Выбор канала второго преобразования
	ADC1->SQR1 |= ADC_SQR1_SQ2_0 | ADC_SQR1_SQ2_1; // CH IN3 (11) - Inj

	// Выбор канала третьего преобразования
	ADC1->SQR1 |= ADC_SQR1_SQ3_0 | ADC_SQR1_SQ3_2 | ADC_SQR1_SQ3_3; // CH IN13 (1101) - Vin


	// Количество преобразований после получения триггера выборки
	ADC1->SQR1 |= ADC_SQR1_L_1; // 3 Преобразования
	ADC2->SQR1 &= ~ADC_SQR1_L;  // 1 Преобразование

	// Устанавливаем длительность преобразования в тактах АЦП: 19.5 CLK
	ADC1->SMPR1 |= ADC_SMPR1_SMP1_2 | ADC_SMPR1_SMP2_2| ADC_SMPR1_SMP3_2; // 1е 2е 3е преобразование
	ADC2->SMPR1 |= ADC_SMPR1_SMP1_2;// 1е преобразование

	// DMA Circular Mode selected
	ADC1->CFGR |= ADC_CFGR_DMACFG;
	ADC2->CFGR |= ADC_CFGR_DMACFG;

	// Direct memory access enable
	ADC1->CFGR |= ADC_CFGR_DMAEN;
	ADC2->CFGR |= ADC_CFGR_DMAEN;


	// Включить АЦП
	ADC1->CR |= ADC_CR_ADEN;
	ADC2->CR |= ADC_CR_ADEN;

	// Подождать включения
	for (int i = 0; i < 1000; i++) asm("nop");

	// Запуск АЦП
	ADC1->CR |= ADC_CR_ADSTART;
	ADC2->CR |= ADC_CR_ADSTART;
}

