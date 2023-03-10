#include "stm32f3xx.h"
#include "adc.h"

/**
 *\brief Инициализация АЦП
 */
volatile void init_adc(void)
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

	// Автоматический режим инжектированной группы - преобразование начнётся после всех регулярных каналов
    ADC1->CFGR |= ADC_CFGR_JAUTO;
    ADC2->CFGR |= ADC_CFGR_JAUTO;

	// Выбор канала первого преобразования регулярной группы
	ADC1->SQR1 |= 4 << ADC_SQR1_SQ1_Pos;                                    // CH IN4 (100) - Vout
	ADC2->SQR1 |= 13 << ADC_SQR1_SQ1_Pos;  // CH IN13 (1101) - Il

	// Выбор канала первго преобразования инжектированной группы (преобразование после регулярной группы)
    ADC1->JSQR |= 13 << ADC_JSQR_JSQ1_Pos;                                   // CH IN13 - Vin
    ADC2->JSQR |= 5 << ADC_JSQR_JSQ1_Pos;                                   // CH IN5 - Inj

    // Устанавливаем длительность выборки в тактах АЦП:S
    ADC1->SMPR1 |= (3 << ADC_SMPR1_SMP4_Pos);//  7.5 CLK
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP13_Pos);//  7.5 CLK
    ADC2->SMPR1 |= (3 << ADC_SMPR1_SMP5_Pos); //  7.5 CLK
    ADC2->SMPR2 |= (3 << ADC_SMPR2_SMP13_Pos); //  7.5 CLK

    // Разрешение 10 бит
    ADC1->CFGR |= (1 << ADC_CFGR_RES_Pos);
    ADC2->CFGR |= (1 << ADC_CFGR_RES_Pos);

    // Включение прерывания после окончания группы преобразований
    ADC2->IER |= ADC_IER_JEOCIE;

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

