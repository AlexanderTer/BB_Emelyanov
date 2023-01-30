#ifndef __SPI_H__
#define __SPI_H__

void init_spi(void);
void write_DS1722(uint8_t addres, uint8_t data);
uint8_t read_DS1722(uint8_t addres);
#endif
