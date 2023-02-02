#include "control.h"
#include "stm32f3xx.h"
#include "dma.h"

Measure_Struct BB_Measure =
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
	ADC_Data_Hanler();
}

void ADC_Data_Hanler(void)
{
	extern volatile unsigned int ADC_Buffer[4];
	BB_Measure.data.iL = BB_Measure.scale.iL * ADC_Buffer[0] + BB_Measure.shift.iL;
	BB_Measure.data.uout = BB_Measure.scale.uout * ADC_Buffer[1] + BB_Measure.shift.uout;
	BB_Measure.data.inj = BB_Measure.scale.inj * ADC_Buffer[2] + BB_Measure.shift.inj;
	BB_Measure.data.uin = BB_Measure.scale.uin * ADC_Buffer[2] + BB_Measure.shift.uin;
}
