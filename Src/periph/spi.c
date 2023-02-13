#include "stm32f3xx.h"
#include "spi.h"

/**
 * \brief Инициализация SPI
 */
void init_spi(void)
{
	// Тактирование SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Частота SCK = 64 МГц / 16 = 4 МГц
	SPI1->CR1 |= SPI_CR1_BR_0| SPI_CR1_BR_1;

	// Начало передачи по второму фронту
	SPI1->CR1 |= SPI_CR1_CPHA;

	// Режим передачи по двум линиям: Tx и Rx
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;

	// Передача/приём MSB first
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

	//---------------------------------------------------------------ADD
	// Software slave management and  Internal slave select
	SPI1->CR1 |= SPI_CR1_SSM |  SPI_CR1_SSI;
	//---------------------------------------------------------------

	// Конфигурыция = ведущий(мастер)
	SPI1->CR1 |= SPI_CR1_MSTR;

	// Передача / приём по 16 бит
	SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3;

	// SPI Motorola mode
	SPI1->CR2 &= ~SPI_CR2_FRF;

	// Передача/приём через 16-битный сдвиговый регистр
	SPI1->CR2 &= ~SPI_CR2_FRXTH;


	//---------------------------------------------------------------ADD
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
   //---------------------------------------------------------------
	//asm("nop");
	//for (int i = 0; i < 100000; i++)		{asm("nop");}

}

/**
 * \brief 	Запись в регистр датчка температуры DS1722
 * \param 	address: адрес регистра
 * \param 	data: данные для адреса
 */
void write_DS1722(uint8_t address, uint8_t data)
{
	// Поднять вывод СЕ
	GPIOC->ODR |= (1 << 7);

	// Отплавить адрес в старшем бите, данные в младшем
	SPI1->DR = ((uint16_t)address << 8) | data;

	// Ждём снятия флага занятости SPI
	while (SPI1->SR & SPI_SR_BSY);

	// Опустить вывод CE
	GPIOC->ODR &= ~(1 << 7);
}


/**
 * \brief 	Чтение регистра датчка температуры DS1722
 * \param 	address: адрес регистра
 * \param 	data: данные из регистра
 */
uint8_t read_DS1722(uint8_t address)
{
	// Поднять вывод СЕ
	GPIOC->ODR |= (1 << 7);

	// Отправить адрес необходимых данных
	SPI1->DR = ((uint16_t)address << 8);

	// Ждём снятия флага занятости SPI (время передачи + приёма)
	while (SPI1->SR & SPI_SR_BSY);

	// Опустить вывод CE
	GPIOC->ODR &= ~(1 << 7);

	// Чтение данных (выделяем первые 8 бит)
	return SPI1->DR & 0xFF;

}


