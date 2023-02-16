#include <stdint.h>
#include "stm32f3xx.h"

#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "control.h"
#include "comp.h"
#include "interrupt.h"

uint32_t TEMPERATURE;



int main(void)
{
	init_RCC();
	init_GPIO();
	init_spi();
	//init_comp();
	init_timer();
	init_dma();
	init_adc();
	init_dac();
	init_interrupt();



	//  Записываем конфигурацию датчика
	// -Режим непрерывного измерения
	// -Точность 8 бит

	// Сконфигурировать датчик
	//write_DS1722(0x80, 0xE0);

	while (1)
	{
        ADC1->CR |= ADC_CR_JADSTART; // запуск АЦП
   //     while(!(ADC1->ISR & ADC_ISR_JEOS)); // ожидание завершения преобразования
		// Получаем температуру с датчика
	//	TEMPERATURE = read_DS1722(0x02);
	//	for (int i = 0; i < 10000; i++){}

		//DAC2->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
	//	if (TEMPERATURE < 30) GPIOB->ODR &= ~(1 << 7);
	//	else GPIOB->ODR |= (1 << 7);

		//setDuty (0.5);

		GPIOA->ODR ^= (1 << 15);
//		GPIOC->ODR ^= (1 << 10) | (1 << 11) | (1 << 12);

		// Проверяем PB1 (SW1) на ноль.
		if (!(GPIOD->IDR & (1 << 2)))
			{
				BB_Measure.count = SET_SHIFTS_MAX_COUNT;
			}


	}
}

void ADC1_2_IRQHandler (void)
{

	ADC1->ISR |= ADC_ISR_JEOS;
    // res= (float)ADC2->DR * 3.3 / 4096. ; // пересчет в напряжение
}




