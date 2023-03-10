#include <stdint.h>
#include "stm32f3xx.h"

#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "control.h"
#include "comp.h"
#include "interrupt.h"

uint32_t TEMPERATURE;

extern float u_max_buck;
extern float u_min_boost;
extern float u_center;

int main(void)
{

	init_RCC();
	init_GPIO();
	init_spi();
	//init_comp();
	init_timer();

	init_adc();
	init_dac();
	init_interrupt();

	//  Записываем конфигурацию датчика
	// -Режим непрерывного измерения
	// -Точность 8 бит

	// Сконфигурировать датчик
	//write_DS1722(0x80, 0xE0);

	while (1)
	{
		updating_coefficients();
		/**
		 * Переключатель коэффициентов Buck <-> Boost
		 */

//		// Проверяем PB1 (SW1) на ноль.
//		if (!(GPIOD->IDR & (1 << 2)))
//		{
//			BB_Measure.count = SET_SHIFTS_MAX_COUNT;
//		}
	}

}

volatile void updating_coefficients(void)
{
	if (BB_State != FAULT)
	{

		if (BB_Control.duty < u_max_buck)
		{
			BB_State = BUCK;
			BB_Control.pid_current.kp = BB_Control.pid_current.kp_buck;
			BB_Control.pid_current.integrator.k =
					BB_Control.pid_current.integrator.k_buck;
			BB_Control.pid_current.diff.k = BB_Control.pid_current.diff.k_buck;

			BB_Control.pid_voltage.kp = BB_Control.pid_voltage.kp_buck;
			BB_Control.pid_voltage.integrator.k =
					BB_Control.pid_voltage.integrator.k_buck;
			BB_Control.pid_voltage.diff.k = BB_Control.pid_voltage.diff.k_buck;

		}
		else if (BB_Control.duty > u_min_boost)
		{

			BB_State = BOOST;
			BB_Control.pid_current.kp = BB_Control.pid_current.kp_boost;
			BB_Control.pid_current.integrator.k =
					BB_Control.pid_current.integrator.k_boost;
			BB_Control.pid_current.diff.k = BB_Control.pid_current.diff.k_boost;

			BB_Control.pid_voltage.kp = BB_Control.pid_voltage.kp_boost;
			BB_Control.pid_voltage.integrator.k =
					BB_Control.pid_voltage.integrator.k_boost;
			BB_Control.pid_voltage.diff.k = BB_Control.pid_voltage.diff.k_boost;
		}
		else
		{

			BB_Control.pid_current.kp = BB_Control.pid_current.kp_bb;
			BB_Control.pid_current.integrator.k =
					BB_Control.pid_current.integrator.k_bb;
			BB_Control.pid_current.diff.k = BB_Control.pid_current.diff.k_bb;

			BB_Control.pid_voltage.kp = BB_Control.pid_voltage.kp_bb;
			BB_Control.pid_voltage.integrator.k =
					BB_Control.pid_voltage.integrator.k_bb;
			BB_Control.pid_voltage.diff.k = BB_Control.pid_voltage.diff.k_bb;
			BB_State = BUCK_BOOST;
		}
	}
}

