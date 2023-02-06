#include "stm32f3xx.h"
#include "dma.h"
#include "control.h"
#include "dsp.h"
#include "timer.h"
#include <math.h>

#define DUTY_MIN_BOOST  (0.1)//!!
#define TSW  (1. / 100000.)


State BB_State = BUCK;

// ---------------- Реализация структуры процесса регулирования ----------------
Control_Struct BB_Control =
{
		.pid_current =
		{
				.kp = 0,
						.integrator =
						{
								.k = 100 * TSW,
								.sat = {.min = 0, .max = 0.98},
						},
						.diff =
						{
							.k = 0,

						},
						.sat = {.min = 0, .max = 1},
		},

};

// ------------- Реализация структуры измерений Measure_Struct -------------
Measure_Struct BB_Measure =
{

#define K_ADC (3.3 / 4096.)
		// ------ Смещение -----
		.shift =
		{
				.iL = 0,
				.uout = 12.1324 / K_ADC,
				.inj = 0,
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




// ------------- Главное прерывание - обработчик -------------
void DMA1_Channel1_IRQHandler(void)
{
	GPIOB->ODR |= (1 << 7);

	DMA1->IFCR = 1 << DMA_IFCR_CTCIF1_Pos; //сбрасываем флаг прерывания

	// Обработка преобразований ацп
	ADC_Data_Hanler();

	// Проверка программных защит
	software_protection_monitor();

	float il_ref = 5.f;
	float error_current = il_ref - BB_Measure.data.iL;

	BB_Control.duty = PID_Controller(&BB_Control.pid_current,error_current);



	// Применение рачётного коэффициента заполнения к модулятору
	//if (BB_State != FAULT) set_Duty();

	GPIOB->ODR &= ~(1 << 7);

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

	// Вычисление мощности
	//if (BB_Control.duty_Boost > DUTY_MIN_BOOST) BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout * ( 1 - BB_Control.duty_Boost);
//	else BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout;


}

void software_protection_monitor(void)
{
	if (BB_Measure.data.iL > BB_Protect.iL_max)
		{
			BB_State = FAULT;
			timer_PWM_off();
			GPIOC->ODR |= (1 << 10);
		}
//	else GPIOC->ODR &= ~(1 << 10);

	if (BB_Measure.data.uout > BB_Protect.uout_max)
	{
		BB_State = FAULT;
		timer_PWM_off();
		GPIOC->ODR |= (1 << 11);
	}
	//else GPIOC->ODR &= ~(1 << 11);

	if (BB_Measure.data.uin > BB_Protect.uin_max)
	{
		BB_State = FAULT;
		timer_PWM_off();
		GPIOC->ODR |= (1 << 12);
	}
	//else GPIOC->ODR &= ~(1 << 12);

	//if (BB_Measure.data.uin < BB_Protect.uin_min) GPIOC->ODR |= (1 << 12);
	//else GPIOC->ODR &= ~(1 << 12);


//	if (BB_Measure.data.power > BB_Protect.power_max)
//		{
//		 	BB_State = FAULT;
//			timer_PWM_off();
//			GPIOB->ODR |= (1 << 7);
//		}
	//else GPIOB->ODR &= ~(1 << 7);

}



// Функция автоопределения смещения для ацп
void set_shifts(void) {
	if (BB_Measure.count == 0)
		return;

	// Обнуление текущего смещения и суммы при старте алгоритма автоопределения смещения.
	if (BB_Measure.count == SET_SHIFTS_MAX_COUNT)
		BB_Measure.shift.inj = BB_Measure.sum.inj = 0;

	// Накапливаем сумму.
	BB_Measure.sum.inj += BB_Measure.data.inj
			* (1.f / SET_SHIFTS_MAX_COUNT);

	// Декремент счётчика и проверка окончания автоопределения смещений.
	if (--BB_Measure.count == 0)
		BB_Measure.shift.inj = -BB_Measure.sum.inj;
}



void timer_PWM_off(void)
{
	// Отключить все выходы
	HRTIM1->sCommonRegs.ODISR = HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS | HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;
}


/**
 * \brief Функция применения рассчитанного коэффициента заполнения к таймеру HRTIM
 */
inline void set_Duty(void)
{

float u = BB_Control.duty;

	if (u < U_MIN_BUCK)
	{
		// Отключить все выходы
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS | HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;

		BB_Control.duty_Buck = 0;
		BB_Control.duty_Boost = 0;

		// Триггер выборки на половине периода
		//HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (0.75));
	}
	else if ((u >= U_MIN_BUCK) && (u < U_MAX_BUCK))
	{
		// Включить E1 E2 D1
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN;

		// Отключить D2
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TD2ODIS;

		BB_Control.duty_Buck = u;
		BB_Control.duty_Boost = 1; // D1 в 1

		// Триггер выборки в половине коэффициента заполнения Buck
		//HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (BB_Control.duty_Buck) * 0.75);
	}
	 if ((u >= U_MAX_BUCK) && (u < 1))
	{
		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		BB_Control.duty_Buck = u * (1 - DUTY_MIN_BOOST);
		BB_Control.duty_Boost = DUTY_MIN_BOOST;

		// Триггер выборки в половине коэффициента заполнения Buck
		HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (BB_Control.duty_Buck) * 0.75);
	}
	else if ((u >= 1) && (u < U_MIN_BOOST))
	{
		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		BB_Control.duty_Buck = DUTY_MAX_BUCK;
		BB_Control.duty_Boost = 1 - DUTY_MAX_BUCK * (2 - u);

		// Триггер выборки в половине коэффициента заполнения Boost
		//HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (BB_Control.duty_Boost) * 0.75);
	}
	else if ((u >= U_MIN_BOOST) && (u < U_MAX_BOOST))
	{
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		BB_Control.duty_Buck = 0; // Для открытия E2
		BB_Control.duty_Boost = u - 1.0;

		// Триггер выборки в половине коэффициента заполнения Boost
		//HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (BB_Control.duty_Boost) * 0.75);
	}
	else if (u >= U_MAX_BOOST)
	{
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		BB_Control.duty_Buck = 0; // Для открытия E2
		BB_Control.duty_Boost = DUTY_MAX_BOOST;

		// Триггер выборки в половине коэффициента заполнения Boost
		//HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (BB_Control.duty_Boost) * 0.75);
	}

	// E compare
	HRTIM1->sTimerxRegs[4].CMP1xR = HRTIM1->sTimerxRegs[4].PERxR  * BB_Control.duty_Buck;

	// D compare
	HRTIM1->sTimerxRegs[3].CMP1xR = HRTIM1->sTimerxRegs[3].PERxR * BB_Control.duty_Boost;

}

