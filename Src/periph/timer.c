#include "stm32f3xx.h"
#include "timer.h"
#include "control.h"




#define DEADTIME (35e-9f) 		// Величина Deadtime, сек ( ! 222ns MAX ! )
#define DEADTIME_RES (868e-12f) // Разрешение Deadtime, сек


/**
 * \brief Инициализация таймера высокого разрешения
 */
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


	// --- Fault ------------------------------------------------

	// Источник - AC2 - IL protect
	HRTIM1->sCommonRegs.FLTINR1 |= HRTIM_FLTINR1_FLT1SRC;

	// Включить сигнал ошибки
	HRTIM1->sCommonRegs.FLTINR1 |= HRTIM_FLTINR1_FLT1E;


	// Источник - AC6 - Uout protect
	HRTIM1->sCommonRegs.FLTINR1 |= HRTIM_FLTINR1_FLT3SRC;

	// Включить сигнал ошибки
	HRTIM1->sCommonRegs.FLTINR1 |= HRTIM_FLTINR1_FLT3E;

	// Включение прерывания по ошибке3
	HRTIM1->sCommonRegs.IER |= HRTIM_IER_FLT3;

	// Состояние при ошибке - 0
	//HRTIM1->sTimerxRegs[3].OUTxR |=  HRTIM_OUTR_FAULT1_1;
//	HRTIM1->sTimerxRegs[4].OUTxR |=  HRTIM_OUTR_FAULT1_1;

	HRTIM1->sTimerxRegs[3].FLTxR |= HRTIM_FLTR_FLT1EN | HRTIM_FLTR_FLT3EN ;
	HRTIM1->sTimerxRegs[4].FLTxR |= HRTIM_FLTR_FLT1EN | HRTIM_FLTR_FLT3EN ;



	// Период таймера
	HRTIM1->sMasterRegs.MPER = (uint32_t) ((float) PERIOD);




	// -- Timer D - Boost timer -------------------------------------------------------

	// Обновление регистра сравнения по сбросу таймера
	HRTIM1->sTimerxRegs[3].TIMxCR |= HRTIM_TIMCR_TRSTU;

	// Период таймера
	HRTIM1->sTimerxRegs[3].PERxR = (uint32_t) ((float) PERIOD); //15360


	// Значение регистров сравнения 1
	HRTIM1->sTimerxRegs[3].CMP1xR = 0;

	// Событие нарастающего фронта
	HRTIM1->sTimerxRegs[3].SETx1R |= HRTIM_SET1R_CMP1;

	// Событие спадающего фронта
	HRTIM1->sTimerxRegs[3].RSTx1R |= HRTIM_RST1R_PER;

	// Инверсия выходов
//	HRTIM1->sTimerxRegs[3].OUTxR |=  HRTIM_OUTR_POL1 | HRTIM_OUTR_POL2;


	// Включение Deadtime
	HRTIM1->sTimerxRegs[3].OUTxR |= HRTIM_OUTR_DTEN;

	// Спадающий фронт Deadtime
	HRTIM1->sTimerxRegs[3].DTxR |= (uint8_t) ((float) (DEADTIME / DEADTIME_RES));

	// Нарастающий фронт Deadtime
	HRTIM1->sTimerxRegs[3].DTxR |= (uint8_t) ((float) (DEADTIME / DEADTIME_RES)) << 16;

	// Подключение результата сравнения на выход
	HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TD1OEN | HRTIM_OENR_TD2OEN;

	// Непрерывный режим работы
	HRTIM1->sTimerxRegs[3].TIMxCR |= HRTIM_TIMCR_CONT;



	// -- Timer E - Buck timer -------------------------------------------------------

	// Обновление регистра сравнения по сбросу таймера
	HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_TRSTU;

	// Период таймера
	HRTIM1->sTimerxRegs[4].PERxR = (uint32_t) ((float) PERIOD); //15360

	// Значение регистров сравнения
	HRTIM1->sTimerxRegs[4].CMP1xR = 0;

	// Событие нарастающего фронта
	HRTIM1->sTimerxRegs[4].SETx1R |= HRTIM_SET1R_CMP1;

	// Событие спадающего фронта
	HRTIM1->sTimerxRegs[4].RSTx1R |= HRTIM_RST1R_PER;

	// Инверсия выходов
	//HRTIM1->sTimerxRegs[4].OUTxR |= HRTIM_OUTR_POL1 | HRTIM_OUTR_POL2;

	// Включение Deadtime
	HRTIM1->sTimerxRegs[4].OUTxR |= HRTIM_OUTR_DTEN;

	// Нарастающий фронт Deadtime
	HRTIM1->sTimerxRegs[4].DTxR |= (uint8_t) ((float) (DEADTIME / DEADTIME_RES));

	// Спадающий фронт Deadtime
	HRTIM1->sTimerxRegs[4].DTxR |= (uint8_t) ((float) (DEADTIME / DEADTIME_RES)) << 16;

	// Подключение результата сравнения на выход
	HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TE1OEN | HRTIM_OENR_TE2OEN;

	// Непрерывный режим работы
	HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_CONT;


	//--- Триггеры выборки
	// Значение регистров сравнения 2 - Положение триггера выборки АЦП
	HRTIM1->sTimerxRegs[4].CMP2xR = (uint32_t) (((float) PERIOD) * (0.75));

	// 101 - Timer E источник триггера выборки для ADC1
	HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC1USRC_0 | HRTIM_CR1_ADC1USRC_2;

	// 101 - Timer E источник триггера выборки для ADC2
	HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC2USRC_0 | HRTIM_CR1_ADC2USRC_2;

	// Триггер ADC1: Timer E compare 2
	HRTIM1->sCommonRegs.ADC1R |= HRTIM_ADC1R_AD1TEC2;

	// Триггер ADC2: Timer E compare 2
	//HRTIM1->sCommonRegs.ADC1R |= HRTIM_ADC1R_AD1TEC2;

	// Буферизация
	HRTIM1->sTimerxRegs[4].TIMxCR |= HRTIM_TIMCR_PREEN;
	HRTIM1->sTimerxRegs[3].TIMxCR |= HRTIM_TIMCR_PREEN;

	// Включение счёта
	HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TECEN | HRTIM_MCR_TDCEN;
}


