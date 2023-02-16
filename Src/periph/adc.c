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



	ADC1->CR |= ADC_CR_ADEN;
	//ADC2->CR |= ADC_CR_ADEN;

	ADC1->CFGR |= ADC_CFGR_JQM;



	// Аппаратный триггер по переднему фронту
	//ADC1->JSQR |= ADC_JSQR_JEXTEN_0;
	//ADC2->JSQR |= ADC_JSQR_JEXTEN_0;


	// Тип триггера Event 9 - HRTIM ADCTRIG2
	//ADC1->JSQR |= ADC_JSQR_JEXTSEL_0 | ADC_JSQR_JEXTSEL_3;
	//ADC2->JSQR |= ADC_JSQR_JEXTSEL_0 | ADC_JSQR_JEXTSEL_3;


	// Выбор канала первого преобразования
	ADC1->JSQR |= ADC_JSQR_JSQ1_2 ;                                   // CH IN4 (100) - Vout
	//ADC2->JSQR |= ADC_JSQR_JSQ1_0 | ADC_JSQR_JSQ1_2 | ADC_JSQR_JSQ1_3; // CH IN13 (1101) - Il

	// Выбор канала второго преобразования
	ADC1->JSQR |= ADC_JSQR_JSQ2_2 ;   // CH IN13 (1101) - Vin
	//ADC2->JSQR |= ADC_JSQR_JSQ2_0 | ADC_JSQR_JSQ2_2;				// CH IN5 (101) - Inj
	ADC1->JSQR |= ADC_JSQR_JSQ3_2 ;   // CH IN13 (1101) - Vin
	ADC1->JSQR |= ADC_JSQR_JSQ4_2 ;
	// Количество преобразований после получения триггера выборки
	ADC1->JSQR |= ADC_JSQR_JL_1|ADC_JSQR_JL_0; // 4 Преобразования
	//ADC2->JSQR |= ADC_JSQR_JL_0; // 2 Преобразования

	// Устанавливаем длительность выборки в тактах АЦП:
//	ADC1->SMPR1 |= (0x4 << ADC_SMPR1_SMP1_Pos) | (0x4 << ADC_SMPR1_SMP2_Pos); // 1е 2е преобразование 19.5 CLK
//	ADC2->SMPR1 |= (0x4 << ADC_SMPR1_SMP1_Pos) | (0x4 << ADC_SMPR1_SMP2_Pos); // 1е 2е преобразование 19.5 CLK


	// Включение внутреннего ИОН
	ADC12_COMMON->CCR |= ADC_CCR_VREFEN;


	// Подождать включения
	for (int i = 0; i < 1000; i++) asm("nop");

	// Включение прерывания после окончания группы преобразований
	ADC1->IER |= ADC_IER_JEOSIE;

	// Запуск АЦП
	//ADC1->CR |= ADC_CR_JADSTART;
	//ADC2->CR |= ADC_CR_JADSTART;

}

