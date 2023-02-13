#include "rcc.h"
#include "stm32f3xx.h"

/**
 *\brief	 	Инициализация модуля тактирования.
 *
 *\details 		Частота внешнего генератора = 8 МГц.
 *				Системная частота = 72 МГц.
 *				Делитель шины APB1 = 2.
 *				Делитель шины APB2 = 1.
 *				Множитель PLL = 9.
 *				Делитель системной частоты = 1.
 *
 */
void init_RCC(void)
{


	// Сброс тактирования шин
	RCC->AHBRSTR = 0xFFFFFFFF;
	RCC->AHBRSTR = 0x00000000;

	RCC->APB1RSTR = 0xFFFFFFFF;
	RCC->APB1RSTR = 0x00000000;

	RCC->APB2RSTR = 0xFFFFFFFF;
	RCC->APB2RSTR = 0x00000000;


	// Обновление переменной с частотой тактирования
	SystemCoreClockUpdate();

	// Включение питания RCC
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// Включение внутреннего генератора 8 МГц
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));
	RCC->CR |= RCC_CR_HSEBYP; // HSE crystal oscillator  bypassed

	// Выключение PLL
	RCC->CR &= ~RCC_CR_PLLON;
	while (RCC->CR & RCC_CR_PLLRDY);

	// Настройка умножителя
	RCC->CFGR |= RCC_CFGR_PLLMUL9 | RCC_CFGR_PPRE1_DIV2;

	// Делитель ацп PLL/1
	RCC->CFGR2|= RCC_CFGR2_ADCPRE12_DIV1;

	// Выбор HSE как источника для PLL
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
	//RCC->CFGR |= RCC_CFGR_MCO_PLL; // Вывод частот на порт PA8

	// Настройка latancy на 2 для 72 МГц
	FLASH->ACR |= FLASH_ACR_LATENCY_2;

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// Выбор PLL как основного источника тактирования
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	SystemCoreClockUpdate();
}
