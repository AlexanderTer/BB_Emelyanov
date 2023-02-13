#include "stm32f3xx.h"
#include "interrupt.h"

/**
 * \brief Инициализация прерываний
 */
void init_interrupt(void)
{

	// Разрешаем прерывание DMA канал 2
	NVIC_EnableIRQ(DMA1_Channel2_IRQn); //разрешаем прерывания

	//	NVIC_EnableIRQ(COMP2_IRQn);
	//NVIC_EnableIRQ(COMP4_6_IRQn);
	//NVIC_EnableIRQ(HRTIM1_FLT_IRQn);
}
