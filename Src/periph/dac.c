#include "stm32f3xx.h"
#include "dac.h"

/**S
 * \brief Инициализация цифро-аналогового пробразователя
 */
void init_dac(void)
{

	// Вкл тактирование ЦАПS
	RCC->APB1ENR |= RCC_APB1ENR_DAC2EN | RCC_APB1ENR_DAC1EN;

	// Включение выходного переключателя
	DAC1->CR |= DAC_CR_OUTEN2; // DAC1 CH2
	DAC2->CR |= DAC_CR_OUTEN1; // DAC1 CH1

	// Включение DAC
	DAC1->CR |= DAC_CR_EN2;
	DAC2->CR |= DAC_CR_EN1;

	// Регистр данных
	DAC1->DHR12R2 =  0; // DAC1 CH2  X16
	DAC2->DHR12R1 =  0; // DAC2 CH1  X17

}
