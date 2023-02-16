#include "stm32f3xx.h"
#include "interrupt.h"

/**
 * \brief Инициализация прерываний
 */
void init_interrupt(void)
{

	// Установка приоритет прерыванийя DMA1_Channel2_IRQn: приоритет 2
	//NVIC_SetPriority(DMA1_Channel1_IRQn, 2);

	// Разрешаем прерывание DMA канал 2

	//NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	// Установка приоритет прерыванийя HRTIM1_TIME_IRQn: приоритет 1
	//NVIC_SetPriority(HRTIM1_TIME_IRQn, 1);

	// Разрешаем прерывание TE
	//NVIC_EnableIRQ(HRTIM1_TIME_IRQn);

	// Разрешаем прерывание TE
	NVIC_EnableIRQ(ADC1_2_IRQn);

	//	NVIC_EnableIRQ(COMP2_IRQn);
	//NVIC_EnableIRQ(COMP4_6_IRQn);
	//NVIC_EnableIRQ(HRTIM1_FLT_IRQn);
}
