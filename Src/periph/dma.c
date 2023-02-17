#include "dma.h"
#include "stm32f3xx.h"


volatile unsigned int ADC_Buffer[4]; // 0 - Il 1 - Uout 2 - Inj 3 - Uin

/*
 * \Инициализация контроллера прямого доступа к памяти
 */
void init_dma(void)
{
	// Тактирование DMA
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// Настраиваем адрес источника данных
	DMA1_Channel1->CPAR = (unsigned int) &(ADC1->DR); // Uout
	DMA1_Channel2->CPAR = (unsigned int) &(ADC2->DR); // Il

	// Настраиваем адрес приёмника данных
	DMA1_Channel1->CMAR = (unsigned int) &ADC_Buffer[0]; // 0 - Uout 1 - Vin
	DMA1_Channel2->CMAR = (unsigned int) &ADC_Buffer[2]; // 2 - Il 3 - Inj

	// Настраиваем количество данных для передачи (0 - 216-1)
	DMA1_Channel1->CNDTR = 2;
	DMA1_Channel2->CNDTR = 2;

	// Установить приоритет
	//DMA1_Channel1->CCR |= DMA_CCR_PL_1; //Uout -   Высокий (10)
	DMA1_Channel2->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1; //Il - Очень высокий (11)

	// Направление передачи данных - 0 из переферии в память
	DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
	DMA1_Channel2->CCR &= ~DMA_CCR_DIR;

	// Круговой режим включён
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	DMA1_Channel2->CCR |= DMA_CCR_CIRC;

	// Режим увеличения адреса памяти
	DMA1_Channel1->CCR |= DMA_CCR_MINC;   // Включён
	DMA1_Channel2->CCR |= DMA_CCR_MINC;   // Включён

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
	DMA1_Channel1->CCR |= DMA_CCR_TCIE;
	DMA1_Channel2->CCR |= DMA_CCR_TCIE;

	// Включение каналов
	DMA1_Channel1->CCR |= DMA_CCR_EN;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	DMA1->IFCR = 1 << DMA_IFCR_CTCIF1_Pos;
	DMA1->IFCR = 1 << DMA_IFCR_CTCIF2_Pos;


}
