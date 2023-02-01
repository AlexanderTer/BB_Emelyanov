#include "control.h"
#include "stm32f3xx.h"
#include "dma.h"

Measure_Struct BB_Control =
{
		// ------ Смещение величин -----
		.shift =
		{
				.iL = 0,
				.uout = 0,
				.inj = 0,
				.uin = 0,
		},

		.scale =
		{
				.iL = 3.3 / 4096.,
				.uout = 3.3 / 4096.,
				.inj = 3.3 / 4096.,
				.uin = 3.3 / 4096.,
		},
};


void DMA1_Channel1_IRQHandler(void)
{
	DMA1->IFCR = 1 << DMA_IFCR_CTCIF1_Pos; //сбрасываем флаг прерывания
}
