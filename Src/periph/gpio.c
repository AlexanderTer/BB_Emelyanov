#include "gpio.h"
#include "stm32f3xx.h"

void init_GPIO(void)
{

	// Тактирование портов
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// LED
	//init_GPIO_Output(GPIOA, 5); 	//  Green LED PA5
	//init_GPIO_Output(GPIOA, 15); 	//  NOT WORK VD34
	init_GPIO_Output(GPIOB, 7); 	//  Red LED VD35
	init_GPIO_Output(GPIOC, 10); 	//  Red LED VD31
	init_GPIO_Output(GPIOC, 11); 	//  Red LED VD32
	init_GPIO_Output(GPIOC, 12); 	//  Red LED VD33

	// SPI
	init_GPIO_AFunction(GPIOB,3,5); // SPI1_SCK
	init_GPIO_AFunction(GPIOB,4,5); // SPI1_MISO
	init_GPIO_AFunction(GPIOB,5,5); // SPI1_MOSI
	init_GPIO_Output(GPIOC, 7); 	// SPI1_CE

	//HRTIM1
	init_GPIO_AFunction(GPIOC,8,3);	 // HRTIM1_CHE1


	init_GPIO_AFunction(GPIOC,9,3);  // HRTIM1_CHE2
	init_GPIO_AFunction(GPIOB,14,13);// HRTIM1_CHD1
	init_GPIO_AFunction(GPIOB,15,13);// HRTIM1_CHD2


	init_GPIO_Analog(GPIOA,3);		 // ADC1IN4
	init_GPIO_Analog(GPIOB,12);	     // ADC2IN13
	init_GPIO_Analog(GPIOC,4);

}

void init_GPIO_Output(GPIO_TypeDef *gpio, unsigned int pin)
{

	gpio->MODER |= 1 << (2 * pin);
}

void init_GPIO_AFunction(GPIO_TypeDef *gpio, unsigned int pin, unsigned int AF)
{
	if (pin < 8)
		gpio->AFR[0] |= AF << (4 * pin);
	else
		gpio->AFR[1] |= AF << (4 * (pin - 8));
	gpio->MODER |= 2 << (2 * pin);
	gpio->OSPEEDR |= 3 << (2 * pin);
}

void init_GPIO_Analog(GPIO_TypeDef *gpio, unsigned int pin)
{
	gpio->MODER |= 3 << (2 * pin);
}
