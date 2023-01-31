#include "stm32f3xx.h"
#include "timer.h"

#define Fsw (300e3)
#define DEADTIME (100e-9) // 868 ps resolution  ( ! 222ns MAX ! )



#define DUTY_MIN_BUCK (0.05)
#define DUTY_MAX_BUCK (0.9)
#define DUTY_MIN_BOOST  (0.1)
#define DUTY_MAX_BOOST (0.95)

#define U_MIN_BUCK DUTY_MIN_BUCK
#define U_MAX_BUCK DUTY_MAX_BUCK

#define U_MIN_BOOST 1 + DUTY_MIN_BOOST
#define U_MAX_BOOST 1 + DUTY_MAX_BOOST

void init_timer(void)
{

	// Тактирование таймера 2*PLL = 144 MHz
	RCC->CFGR3 |= RCC_CFGR3_HRTIM1SW_PLL;

	// Включить тактирование HTIM1
	RCC->APB2ENR |= RCC_APB2ENR_HRTIM1EN;

	//Калибровка таймера
	HRTIM1_COMMON->DLLCR |= HRTIM_DLLCR_CALRTE_0 | HRTIM_DLLCR_CALRTE_1; // Период калибровки наименьший по рекомендации (14 us)
	HRTIM1_COMMON->DLLCR |= HRTIM_DLLCR_CALEN; 			// Разрешение калибровки
	HRTIM1_COMMON->DLLCR |= HRTIM_DLLCR_CAL; 				// Старт калибровки
	while (!(HRTIM1_COMMON->ISR & HRTIM_ISR_DLLRDY));					// Ожидание кокнчание калибровки

	// -- Master timer --------------------------------------------------------------

	// Буферизация
	HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_PREEN;

	// Период таймера
	HRTIM1->sMasterRegs.MPER = (uint32_t) ((float) (144e6 * 32.f / Fsw));

	// -- Timer D - Boost timer -------------------------------------------------------

	// Буферизация
	//HRTIM1->sTimerxRegs[3].TIMxCR |= HRTIM_TIMCR_PREEN;

	// Период таймера
	HRTIM1->sTimerxRegs[3].PERxR = (uint32_t) ((float) (144e6 * 32.f / Fsw)); //15360


	// Значение регистров сравнения 1
	HRTIM1->sTimerxRegs[3].CMP1xR = 0;

	// Событие нарастающего фронта
	HRTIM1->sTimerxRegs[3].SETx1R |= HRTIM_SET1R_CMP1;

	// Событие спадающего фронта
	HRTIM1->sTimerxRegs[3].RSTx1R |= HRTIM_RST1R_PER;

	// Инверсия выходов
//	HRTIM1->sTimerxRegs[3].OUTxR |=  HRTIM_OUTR_POL1 | HRTIM_OUTR_POL2;


	// Deadtime enable
	HRTIM1->sTimerxRegs[3].OUTxR |= HRTIM_OUTR_DTEN;

	// Rasing Deadtime
	HRTIM1->sTimerxRegs[3].DTxR |= (uint8_t) ((float) (DEADTIME / 0.868e-9));

	// Falling Deadtime
	HRTIM1->sTimerxRegs[3].DTxR |= (uint8_t) ((float) (DEADTIME / 0.868e-9))<< 16;

	// Подключение результата сравнения на выход
	HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

	// Непрерывный режим работы
	HRTIM1->sTimerxRegs[3].TIMxCR |= HRTIM_TIMCR_CONT;






	// -- Timer E - Buck timer -------------------------------------------------------

	// Буферизация
	//HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_PREEN;

	// Период таймера
	HRTIM1->sTimerxRegs[4].PERxR = (uint32_t) ((float) (144e6 * 32.f / Fsw)); //15360

	// Значение регистров сравнения
	HRTIM1->sTimerxRegs[4].CMP1xR = 0;

	// Событие нарастающего фронта
	HRTIM1->sTimerxRegs[4].SETx1R |= HRTIM_SET1R_CMP1;

	// Событие спадающего фронта
	HRTIM1->sTimerxRegs[4].RSTx1R |= HRTIM_RST1R_PER;

	// Инверсия выходов
	//HRTIM1->sTimerxRegs[4].OUTxR |= HRTIM_OUTR_POL1 | HRTIM_OUTR_POL2;

	// Deadtime enable
	HRTIM1->sTimerxRegs[4].OUTxR |= HRTIM_OUTR_DTEN;

	// Rasing Deadtime
	HRTIM1->sTimerxRegs[4].DTxR |= (uint8_t) ((float) (DEADTIME / 0.868e-9));

	// Falling Deadtime
	HRTIM1->sTimerxRegs[4].DTxR |= (uint8_t) ((float) (DEADTIME / 0.868e-9)) << 16;

	// Подключение результата сравнения на выход
	HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN;

	// Непрерывный режим работы
	HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_CONT;


	//--- Триггеры выборки
	// Значение регистров сравнения 2 - Положение триггера выборки АЦП
	HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (0.5));

	// 101 - Timer E источник триггера выборки для ADC1
	HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC1USRC_0 | HRTIM_CR1_ADC1USRC_2;

	// 101 - Timer E источник триггера выборки для ADC2
	HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC2USRC_0 | HRTIM_CR1_ADC2USRC_2;

	// ADC1 trigger 1 event: Timer E compare 2
	HRTIM1->sCommonRegs.ADC1R |= HRTIM_ADC1R_AD1TEC2;

	// ADC2 trigger 1 event: Timer E compare 2
	//HRTIM1->sCommonRegs.ADC1R |= HRTIM_ADC1R_AD1TEC2;



	// Включение счёта
	HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TECEN | HRTIM_MCR_TDCEN;
}

void setDuty(float u)
{
	float dutyBuck;
	float dutyBoost;

	if (u < U_MIN_BUCK)
	{
		// Отключить все выходы
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS | HRTIM_ODISR_TE2ODIS
				| HRTIM_ODISR_TD1ODIS | HRTIM_ODISR_TD2ODIS;

		dutyBuck = 0;
		dutyBoost = 0;
	}
	else if ((u >= U_MIN_BUCK) && (u < U_MAX_BUCK))
	{
		// Включить E1 E2 D1
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN
				| HRTIM_OENR_TD1OEN;

		// Отключить D2
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TD2ODIS;

		dutyBuck = u;
		dutyBoost = 1; // D1 в 1
	}
	else if ((u >= U_MAX_BUCK) && (u < 1))
	{
		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN
				| HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		dutyBuck = u * (1 - DUTY_MIN_BOOST);
		dutyBoost = DUTY_MIN_BOOST;
	}
	else if ((u >= 1) && (u < U_MIN_BOOST))
	{
		// Включить все таймеры
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN
				| HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		dutyBuck = DUTY_MAX_BUCK;
		dutyBoost = 1 - DUTY_MAX_BUCK * (2 - u);
	}
	else if ((u >= U_MIN_BOOST) && (u < U_MAX_BOOST))
	{
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		dutyBuck = 0; // Для открытия E2
		dutyBoost = u - 1.0;
	}
	else if (u >= U_MAX_BOOST)
	{
		// Включить E2 D1 D2
		HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE2OEN | HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

		// Отключить E1
		HRTIM1->sCommonRegs.ODISR |= HRTIM_ODISR_TE1ODIS;

		dutyBuck = 0; // Для открытия E2
		dutyBoost = DUTY_MAX_BOOST;
	}

	// E compare
	HRTIM1->sTimerxRegs[4].CMP1xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (dutyBuck));
	// D compare
	HRTIM1->sTimerxRegs[3].CMP1xR = (uint32_t) (((float) (144e6 * 32.f / Fsw)) * (dutyBoost));

}

