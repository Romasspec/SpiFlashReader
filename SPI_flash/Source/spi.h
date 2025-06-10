#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include <MDR32F9Qx_ssp.h>

// functions

//uint8_t SPI_Deinit();
uint8_t spi_init(MDR_SSP_TypeDef*);
uint8_t spi_set(uint8_t b);
uint8_t spi_get(uint8_t* b);

#endif // _SPI_H
