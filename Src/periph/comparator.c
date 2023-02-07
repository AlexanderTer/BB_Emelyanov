#include "comparator.h"
#include "stm32f3xx.h"

void init_ac(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// ---- AC2 ------ Il protect -------------------

	//* Выход AC2 подключён к HRTIM1 непосредственно для уменьшения задержки

	// Напряжение срабатывания - 3.3V (16.6 A)
	COMP2->CSR |= COMP2_CSR_COMP2INSEL_0 | COMP2_CSR_COMP2INSEL_1 | (1 << 22);

	// Включение модуля компаратора
	COMP2->CSR |= COMP2_CSR_COMP2EN;



	// ---- AC6 ------ Uout protect -------------------
	// Напряжение срабатывания - 2.475 (21.55 В)
	COMP6->CSR |=   COMP6_CSR_COMP6INSEL_1 | (1 << 22);

	// Включение модуля компаратора
	COMP6->CSR |= COMP6_CSR_COMP6EN;

	NVIC_EnableIRQ(COMP2_IRQn);
	NVIC_EnableIRQ(COMP4_6_IRQn);
}
