#include <stdint.h>
#include "stm32f3xx.h"

#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "adc.h"
#include "dma.h"
uint32_t TEMPERATURE;



int main(void)
{
	init_RCC();
	init_GPIO();
	init_spi();
	init_timer();
	setDuty (0.9);
	init_dma();
	init_adc();


	//  Записываем конфигурацию датчика
	// -Режим непрерывного измерения
	// -Точность 8 бит

	// Сконфигурировать датчик
	write_DS1722(0x80, 0xE0);

	while (1)
	{
		// ADC1->CR |= ADC_CR_ADSTART; // запуск АЦП
		// while(!(ADC1->ISR & ADC_ISR_EOC)); // ожидание завершения преобразования

		// res= (float)ADC1->DR * 3.3 / 4096. ; // пересчет в напряжение


		// Получаем температуру с датчика
		TEMPERATURE = read_DS1722(0x02);


		for (int i = 0; i < 10000; i++)
	{
	}


		if (TEMPERATURE < 30) GPIOB->ODR &= ~(1 << 7);
		else GPIOB->ODR |= (1 << 7);

		setDuty (0.9);
		//GPIOB->ODR ^= (1 << 7);
//		GPIOA->ODR ^= (1 << 15);
//		GPIOC->ODR ^= (1 << 10) | (1 << 11) | (1 << 12);

	}
}

void ADC1_2_IRQHandler (void)
{

	ADC2->ISR |= ADC_ISR_EOC;
    // res= (float)ADC2->DR * 3.3 / 4096. ; // пересчет в напряжение
}


