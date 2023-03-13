#include "stm32f3xx.h"
#include "dma.h"
#include "control.h"
#include "dsp.h"
#include "timer.h"
#include <math.h>
<<<<<<< HEAD
#define TRIG_KOEF  (0.5f)		 // Положение триггера выборки от коэф заполнения (по осцилограмме 0.7 - середина открытого состояния ключа)
volatile uint32_t counterADC = 0;
=======

// Переменные для обеспечения гистерезиса
float u_max_buck = U_MAX_BUCK;
float u_min_boost = U_MIN_BOOST;
float u_center = U_CENTER;
>>>>>>> контрольный

// Начальное состояние - понижающий
State BB_State = BUCK;

// ---------------- Реализация структуры процесса регулирования ----------------
Control_Struct BB_Control =
{
		#define F_CALC (50000.f) 		// Частота обсчёта контура
		#define T_CALC  (1.f / F_CALC)

		.pid_current =
		{
<<<<<<< HEAD
				.kp_boost =5.5110e-03f,
=======
				.kp_boost = 5.5848e-03f,
				.kp_bb =  6.6164e-03f,
>>>>>>> контрольный
				.kp_buck =  6.6164e-03f,
				.kp =  5.5110e-03f,

				.integrator =
				{
<<<<<<< HEAD
						.k_boost =  10.641f * T_CALC,
=======
						.k_boost = 12.353f * T_CALC,
						.k_bb =   19.047f * T_CALC,
>>>>>>> контрольный
						.k_buck =  19.047f * T_CALC,
						.k =  10.641f * T_CALC,
						.sat = {.min = 0.f, .max = 2.f},
				},
				.diff =
				{
<<<<<<< HEAD
					.k_boost = 7.1313e-07f * F_CALC,
=======
					.k_boost = 6.1223e-07f * F_CALC,
					.k_bb = 4.1481e-07f * F_CALC,
>>>>>>> контрольный
					.k_buck = 4.1481e-07f * F_CALC,
					.k =  7.1313e-07f * F_CALC,

				},
				.sat = {.min = 0.f, .max = 2.f},
		},

		.pid_voltage =
		{
				.kp_boost = 1.2088f,
				.kp_bb = 0.7068f,
				.kp_buck = 0.7068f,
				.kp = 1.2088f,

				.integrator =
				{
						.k_boost = 5322.7f * T_CALC,
						.k_bb =  4.0959e+04f * T_CALC,
						.k_buck =  4.0959e+04f * T_CALC,
						.k =   5322.7f * T_CALC,
						.sat = {.min = 0, .max = 14.8f},
				},

				.diff =
				{
					.k_boost = 0.f * F_CALC,
					.k_bb= 0.f * F_CALC,
					.k_buck = 0.f * F_CALC,
					.k = 0.f * F_CALC,

				},
				.sat = {.min = 0, .max = 14.8f},
		},

};

// ------------- Реализация структуры измерений Measure_Struct -------------
Measure_Struct BB_Measure =
{

#define K_ADC (3.3f / 1024.f)
		// ------ Смещение -----
		.shift =
		{
				.iL = 0.f,
				.uout = 12.1324f / K_ADC,
<<<<<<< HEAD
				.inj = 0.f,
				.uin = 0.f,
=======
				.inj = -0.160491273f,
				.uin = 0,
>>>>>>> контрольный
		},

		// ------ Множитель -----
		.scale =
		{
				.iL =   K_ADC * 5.0505f,
				.uout = K_ADC * 1.4749f,
				.inj =  K_ADC * 0.1f,
				.uin =  K_ADC * 14.5f,
		},

		.dac[0] =
		{
				.shift = 0.f,
<<<<<<< HEAD
				.scale = 4095.f / 15.f,
=======
				.scale = 4095.f / 7.f,
>>>>>>> контрольный
		},

		.dac[1] =
		{
				.shift = 0.f,
<<<<<<< HEAD
				.scale = 4095.f / 21.f,
=======
				.scale = 4095.f / 7.f,
>>>>>>> контрольный
		},

};// end Measure_Struct BB_Measure ------------------------------------------


// ------------- Реализация структуры защит Protect_Struct ------------------
Protect_Struct BB_Protect =
{

		.iL_max    = 18.f,
		.uin_max   = 42.f,
		.uin_min   = 7.5f,
		.uout_max  = 23.f,
		.power_max = 130.f,

};// end Protect_Struct BB_Protect -----------------------------------------

/**
 * \brief Прерывание - обработчик HRTIM (Событие Preload)
 * \details Главное прерывание-обработчик коонтура ОС
 */

void HRTIM1_TIME_IRQHandler(void){

<<<<<<< HEAD


=======


	// Выбор коэффициентов

	// ----- Расчёт контура напряжения ---------
	BB_Control.uout_ref = 20.0f;
	BB_Measure.data.inj = BB_Measure.scale.inj * ADC2->JDR1 + BB_Measure.shift.inj;
	BB_Measure.data.uout = BB_Measure.scale.uout * (ADC1->DR + BB_Measure.shift.uout);

	BB_Control.error_voltage = BB_Control.uout_ref - BB_Measure.data.uout;
	BB_Control.iL_ref = PID_Controller(&BB_Control.pid_voltage,BB_Control.error_voltage);


	// ----- Расчёт контура тока ---------
	BB_Measure.data.iL = BB_Measure.scale.iL * ADC2->DR;
	BB_Control.error_current = BB_Control.iL_ref - BB_Measure.data.iL;
	BB_Control.duty =  PID_Controller(&BB_Control.pid_current,BB_Control.error_current);

	// -----------------------------------

	// Вывод данных на ЦАП1 ЦАП2
//	DAC1->DHR12R2 =  BB_Control.iL_ref   * BB_Measure.dac[0].scale; // DAC1 CH2  X16
	//DAC2->DHR12R1 =  il_ref_b *  BB_Measure.dac[1].scale; // DAC2 CH1  X17




	// Применение рачётного коэффициента заполнения к модулятору
	if (BB_State != FAULT)
		{
			set_Duty();
		}
>>>>>>> контрольный

	// Сброс флага прерывания
	HRTIM1->sTimerxRegs[4].TIMxICR |= HRTIM_TIMICR_REPC;


}

/**
 * \brief Прерывание - обработчик аппаратныго сигнала ошибки
 */
void HRTIM1_FLT_IRQHandler(void)
{
	HRTIM1->sCommonRegs.ICR |= HRTIM_ICR_FLT3C;
	//float il_ref = 5.f;
		//	float error_current = il_ref - BB_Measure.data.iL;

}


/**
 * \brief Прерывание - обработчик окончания преобразования группы каналов АЦП
 */
void ADC1_2_IRQHandler(void)
{

	if (counterADC == 4)
	{

		// Выбор коэффициентов

		// ----- Расчёт контура напряжения ---------
		#define u_ref  (20.0f)

		BB_Measure.data.inj = BB_Measure.scale.inj * ADC2->JDR1 + BB_Measure.shift.inj;

		BB_Measure.data.uout = BB_Measure.scale.uout * (ADC1->DR + BB_Measure.shift.uout);

		volatile float error_voltage = u_ref - BB_Measure.data.uout;
	   volatile float iL_ref = PID_Controller(&BB_Control.pid_voltage,error_voltage);
		iL_ref = 13.5f +  BB_Measure.data.inj;


		// ----- Расчёт контура тока ---------
		GPIOB->ODR |= (1 << 7);
		BB_Measure.data.iL = BB_Measure.scale.iL * ADC2->DR;
		volatile float error_current = iL_ref - BB_Measure.data.iL;
		BB_Control.duty = PID_Controller(&BB_Control.pid_current,error_current);
		// -----------------------------------

		// Вывод данных на ЦАП1 ЦАП2
		DAC1->DHR12R2 =  iL_ref  * BB_Measure.dac[0].scale; // DAC1 CH2  X16
		DAC2->DHR12R1 =  BB_Measure.data.uout * BB_Measure.scale.iL *  BB_Measure.dac[1].scale; // DAC2 CH1  X17




		// Применение рачётного коэффициента заполнения к модулятору
		if (BB_State != FAULT)
			{
				set_Duty();
			}
		GPIOB->ODR &= ~(1 << 7);
		counterADC = 0;



	}


	// Обработка преобразований ацп
	//ADC_Data_Hanler();

	// Проверка программных защит
	//software_protection_monitor();

	// Установка смещения inj по нажатию кнопки
	set_shifts();

	counterADC++;

	// Cбрасываем флаг прерывания

    ADC2->ISR |= ADC_ISR_JEOS;



}


/**
 * \brief Функция пересчёта значений АЦП в физические величины
 */
void ADC_Data_Hanler(void)
{
	BB_Measure.data.uout = BB_Measure.scale.uout * (ADC1->DR + BB_Measure.shift.uout);
	//BB_Measure.data.uin = BB_Measure.scale.uin * ADC1->JDR1 + BB_Measure.shift.uin;
	BB_Measure.data.iL = BB_Measure.scale.iL * ADC2->DR + BB_Measure.shift.iL;
	//BB_Measure.data.inj = BB_Measure.scale.inj * ADC2->JDR1 + BB_Measure.shift.inj;


	// Вычисление мощности
	//if (BB_Control.duty_Boost > DUTY_MIN_BOOST) BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout * ( 1 - BB_Control.duty_Boost);
//	else BB_Measure.data.power = BB_Measure.data.iL * BB_Measure.data.uout;


}

/**
 * \brief Функция проверки программных защит
 */
void software_protection_monitor(void)
{
	if (BB_Measure.data.iL > BB_Protect.iL_max)
	{
			BB_State = FAULT;
			timer_PWM_off();
			GPIOC->ODR |= (1 << 10);
	}

	if (BB_Measure.data.uout > BB_Protect.uout_max)
	{
		BB_State = FAULT;
		timer_PWM_off();
		GPIOC->ODR |= (1 << 11);
	}

}


/**
 * \brief Функция автоопределения смещения сигнала инжекции
 */
void set_shifts(void)
{
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


/**
 * \brief Функция программного отключения ШИМ
 */
inline void timer_PWM_off(void)
{
	// Отключить все выходы
	HRTIM1->sCommonRegs.ODISR = HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS | HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;
}


/**
 * \brief Функция применения рассчитанного коэффициента заполнения к таймеру HRTIM
 */
void set_Duty(void)
{


	float u = BB_Control.duty;

	if (u < U_MIN_BUCK)
	{
		// Отключить все выходы
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS | HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;

		BB_Control.duty_Buck = 0.f;
		BB_Control.duty_Boost = 0.f;

		// Триггер выборки на половине периода
		HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (PERIOD * TRIG_KOEF);
	}
	else if (u < u_max_buck)
	{
		GPIOB->ODR &= ~(1 << 7);
		u_max_buck = U_MAX_BUCK;
		// Включить E1 E2 D1
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN;

		// Отключить D2
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TD2ODIS;

		BB_Control.duty_Buck = u;
		BB_Control.duty_Boost = 1.f; // D1 в 1

		// Триггер выборки в половине коэффициента заполнения Buck
		HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (PERIOD * TRIG_KOEF * BB_Control.duty_Buck);
	}
	else if (u < u_center)
	{
		GPIOB->ODR |= (1 << 7);
		u_max_buck = U_MAX_BUCK_HYST;
		u_center = U_CENTER_HYST_BOOST;

		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		BB_Control.duty_Buck = u * (1 - DUTY_MIN_BOOST);
		BB_Control.duty_Boost = DUTY_MIN_BOOST;

		// Триггер выборки в половине коэффициента заполнения Buck
		HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (PERIOD * TRIG_KOEF * BB_Control.duty_Buck);
	}
	else if (u < u_min_boost)
	{
		GPIOB->ODR &= ~(1 << 7);
		u_center = U_CENTER;
		u_min_boost = U_MIN_BOOST_HYST;

		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		BB_Control.duty_Buck = DUTY_MAX_BUCK;
		BB_Control.duty_Boost = 1.f - DUTY_MAX_BUCK * (2.f - u);

		// Триггер выборки в половине коэффициента заполнения Boost
		HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t)(PERIOD * BB_Control.duty_Buck * TRIG_KOEF);
	}
	else if (u < U_MAX_BOOST)
	{
		u_min_boost = U_MIN_BOOST;
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		BB_Control.duty_Buck = 0.f; // Для открытия E2
		BB_Control.duty_Boost = u - 1.0f;

		// Триггер выборки в половине коэффициента заполнения Boost
		HRTIM1->sTimerxRegs[4].CMP2xR = (PERIOD * TRIG_KOEF * BB_Control.duty_Boost);
	}
	else  //  (u >= U_MAX_BOOST)
	{
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		BB_Control.duty_Buck = 0.f; // Для открытия E2
		BB_Control.duty_Boost = DUTY_MAX_BOOST;

		// Триггер выборки в половине коэффициента заполнения Boost
		HRTIM1->sTimerxRegs[4].CMP2xR = (PERIOD * TRIG_KOEF * BB_Control.duty_Boost);
	}

	// E compare
	HRTIM1->sTimerxRegs[4].CMP1xR = HRTIM1->sTimerxRegs[4].PERxR  * BB_Control.duty_Buck;

	// D compare
	HRTIM1->sTimerxRegs[3].CMP1xR = HRTIM1->sTimerxRegs[3].PERxR * BB_Control.duty_Boost;

}

