#include "dma.h"
#include "stm32f3xx.h"

uint32_t Il;
uint32_t Uout[3];

uint32_t ADC_BUFFER[4]; // 0 - Il 1 - Uout 2 - Inj 3 - Uin
void init_dma(void)
{
	// Тактирование DMA
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// Настраиваем адрес источника данных
	DMA1_Channel1->CPAR = (unsigned int) &(ADC1->DR); // Uout
	DMA1_Channel2->CPAR = (unsigned int) &(ADC2->DR); // Il

	// Настраиваем адрес приёмника данных
	DMA1_Channel1->CMAR = (unsigned int) &ADC_BUFFER[1];
	DMA1_Channel2->CMAR = (unsigned int) &ADC_BUFFER[0];

	// Настраиваем количество данных для передачи (0 - 216-1)
	DMA1_Channel1->CNDTR = 3;
	DMA1_Channel2->CNDTR = 1;

	// Установить приоритет
	DMA1_Channel1->CCR |= DMA_CCR_PL_1; //Uout -  Hhigh (10)
	DMA1_Channel2->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1; //Il - Veri high (11)

	// Направление передачи данных - 0 их переферии в память
	DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
	DMA1_Channel2->CCR &= ~DMA_CCR_DIR;

	// Круговой режим включён
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	DMA1_Channel2->CCR |= DMA_CCR_CIRC;

	// Режим увеличения адреса памяти
	DMA1_Channel1->CCR |= DMA_CCR_MINC;   // Включён
	DMA1_Channel2->CCR &= ~DMA_CCR_MINC; // Отключён

	// Режим увеличения адреса переферии отключён
	DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
	DMA1_Channel2->CCR &= ~DMA_CCR_PINC;

	// Размер ячейки памяти - 32 бита
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1;
	DMA1_Channel2->CCR |= DMA_CCR_MSIZE_1;

	// Размер данных переферии - 32 бита
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1;
	DMA1_Channel2->CCR |= DMA_CCR_PSIZE_1;

	// Разрешение прерывания по окончанию передачи

	DMA1_Channel1->CCR |= DMA_CCR_EN;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

}
