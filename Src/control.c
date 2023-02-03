#include "control.h"
#include "stm32f3xx.h"
#include "dma.h"
#include "control.h"

#define DUTY_MIN_BOOST  (0.1)//!!


// ------------- Реализация структуры измерений Measure_Struct -------------
Measure_Struct BB_Measure =
{

#define K_ADC (3.3 / 4096.)
		// ------ Смещение -----
		.shift =
		{
				.iL = 0,
				.uout = 12.1324 / K_ADC,
				.inj = -1.65,
				.uin = 0,
		},

		// ------ Множитель -----
		.scale =
		{
				.iL =   K_ADC * 5.0505,
				.uout = K_ADC * 1.4749,
				.inj =  K_ADC,
				.uin =  K_ADC * 16.6,
		},

};// end Measure_Struct BB_Measure ------------------------------------------


// ------------- Реализация структуры защит Protect_Struct ------------------
Protect_Struct BB_Protect =
{

		.iL_max    = 14.,
		.uin_max   = 42.,
		.uin_min   = 7.5,
		.uout_max  = 22.,
		.power_max = 130.,

};// end Protect_Struct BB_Protect -----------------------------------------


//// ------------- Реализация модулятора BB_Modulator  ------------------

Modulator_Struct BB_Modulator =
{


};


// ------------- Главное прерывание - обработчик -------------
void DMA1_Channel1_IRQHandler(void)
{
	DMA1->IFCR = 1 << DMA_IFCR_CTCIF1_Pos; //сбрасываем флаг прерывания

	ADC_Data_Hanler();

	software_protection_monitor();

	BB_Modulator.duty = 1.6;

	setDuty();



}// end DMA1_Channel1_IRQHandler ---------------------------------------------



/**
 * \brief Функция пересчёта значений АЦП в физические величины
 */
void ADC_Data_Hanler(void)
{
	extern volatile unsigned int ADC_Buffer[4];
	BB_Measure.data.iL = BB_Measure.scale.iL * ADC_Buffer[0] + BB_Measure.shift.iL;
	BB_Measure.data.uout = BB_Measure.scale.uout * (ADC_Buffer[1] + BB_Measure.shift.uout);
	BB_Measure.data.inj = BB_Measure.scale.inj * ADC_Buffer[2] + BB_Measure.shift.inj;
	BB_Measure.data.uin = BB_Measure.scale.uin * ADC_Buffer[3] + BB_Measure.shift.uin;

	if (BB_Modulator.dutyBoost > DUTY_MIN_BOOST) BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout * ( 1 - BB_Modulator.dutyBoost);
	else BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout;


}

void software_protection_monitor(void)
{
	if (BB_Measure.data.iL > BB_Protect.iL_max)
		{

			timer_PWM_off();
			GPIOC->ODR |= (1 << 10);
		}
//	else GPIOC->ODR &= ~(1 << 10);

	if (BB_Measure.data.uout > BB_Protect.uout_max)
	{
		timer_PWM_off();
		GPIOC->ODR |= (1 << 11);
	}
	//else GPIOC->ODR &= ~(1 << 11);

	if (BB_Measure.data.uin > BB_Protect.uin_max)
	{
		timer_PWM_off();
		GPIOC->ODR |= (1 << 12);
	}
	//else GPIOC->ODR &= ~(1 << 12);

	//if (BB_Measure.data.uin < BB_Protect.uin_min) GPIOC->ODR |= (1 << 12);
	//else GPIOC->ODR &= ~(1 << 12);


	if (BB_Measure.data.power > BB_Protect.power_max)
		{
			timer_PWM_off();
			GPIOB->ODR |= (1 << 7);
		}
	//else GPIOB->ODR &= ~(1 << 7);

}

void timer_PWM_off(void)
{
	// Отключить все выходы
	HRTIM1->sCommonRegs.ODISR = HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS | HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;
}
