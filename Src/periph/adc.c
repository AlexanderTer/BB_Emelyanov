#include "stm32f3xx.h"
#include "adc.h"

/**
 *\brief Инициализация АЦП
 */
void init_adc(void)
{
	// Включить тактирование АЦП
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	// Очистить биты источника опорного
	ADC1->CR &= ~ADC_CR_ADVREGEN;
	ADC2->CR &= ~ADC_CR_ADVREGEN;

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
	uint32_t i = 0;
	while ((ADC1->CR & ADC_CR_ADCAL) || i > 10000 )  i++;
	ADC2->CR |= ADC_CR_ADCAL;
	i = 0;
	while ((ADC2->CR & ADC_CR_ADCAL) || i > 10000 )  i++;

	// Выбор для старта преобразования внешний триггер
	ADC1->CFGR |= ADC_CFGR_EXTEN_0;
	ADC2->CFGR |= ADC_CFGR_EXTEN_0;

	// Тип триггера Event 7 - HRTIM
	ADC1->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;
	ADC2->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;


	// Выбор канала первого преобразования
	ADC1->SQR1 |= ADC_SQR1_SQ1_2;                                   // CH IN4 (100) - Vout
	ADC2->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_3; // CH IN13 (1101) - Il

	// Выбор канала второго преобразования
	ADC1->SQR1 |= ADC_SQR1_SQ2_0 | ADC_SQR1_SQ2_2 | ADC_SQR1_SQ2_3; // CH IN13 (1101) - Vin
	ADC2->SQR1 |= ADC_SQR1_SQ2_0 | ADC_SQR1_SQ2_2; 					// CH IN5 (101) - Inj



	// Количество преобразований после получения триггера выборки
	ADC1->SQR1 |= ADC_SQR1_L_0; // 2 Преобразования
	ADC2->SQR1 |= ADC_SQR1_L_0; // 2 Преобразования

	// Устанавливаем длительность выборки в тактах АЦП:
	ADC1->SMPR1 |= (0x4 << ADC_SMPR1_SMP1_Pos) | (0x4 << ADC_SMPR1_SMP2_Pos); // 1е 2е преобразование 19.5 CLK
	ADC2->SMPR1 |= (0x4 << ADC_SMPR1_SMP1_Pos) | (0x4 << ADC_SMPR1_SMP2_Pos); // 1е 2е преобразование 19.5 CLK

	// Выбор кругового режима ДМА
	ADC1->CFGR |= ADC_CFGR_DMACFG;
	ADC2->CFGR |= ADC_CFGR_DMACFG;

	// Включение прямого доступа к памяти
	ADC1->CFGR |= ADC_CFGR_DMAEN;
	ADC2->CFGR |= ADC_CFGR_DMAEN;

	// Включение внутреннего ИОН
	ADC12_COMMON->CCR |= ADC_CCR_VREFEN;

	// Включить АЦП
	ADC1->CR |= ADC_CR_ADEN;
	ADC2->CR |= ADC_CR_ADEN;

	// Подождать включения
	for (int i = 0; i < 1000; i++) asm("nop");

	// Запуск АЦП
	ADC1->CR |= ADC_CR_ADSTART;
	ADC2->CR |= ADC_CR_ADSTART;
}

