#include "dma.h"
#include "stm32f3xx.h"

uint32_t ADC_Buffer;
void init_dma(void)
{
	// Тактирование DMA 2
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// Настраиваем адрес источника данных
	DMA1_Channel2->CPAR = (unsigned int) &(ADC2->DR);

	// Настраиваем адрес приёмника данных
	DMA1_Channel2->CMAR = (unsigned int) &ADC_Buffer;

	// Настраиваем количество данных для передачи (0 - 216-1)
	DMA1_Channel2->CNDTR = 1;

	// Установить приоритет - Veri high
	DMA1_Channel2->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;

	// Направление передачи данных - 0 их переферии в память
	DMA1_Channel2->CCR &= ~DMA_CCR_DIR;

	// Круговой режим включён
	DMA1_Channel2->CCR |= DMA_CCR_CIRC;

	// Режим увеличения адреса памяти отключён
	DMA1_Channel2->CCR &= ~DMA_CCR_MINC;

	// Режим увеличения адреса переферии отключён
	DMA1_Channel2->CCR &= ~DMA_CCR_PINC;

	// Размер ячейки памяти - 32 бита
	DMA1_Channel2->CCR |= DMA_CCR_MSIZE_1;

	// Размер данных переферии - 32 бита
	DMA1_Channel2->CCR |= DMA_CCR_PSIZE_1;

	// Разрешение прерывания по окончанию передачи
	//DMA1_Channel2->CCR |= DMA_CCR_TCIE;

	DMA1_Channel2->CCR |= DMA_CCR_EN;

}
