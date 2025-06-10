#ifndef SST25VF016B_H
#define SST25VF016B_H

#include <stdint.h>
#include <MDR32F9Qx_port.h>
#include "spi.h"

// PORTD

#define SPI_FLASH					MDR_SSP2
#define MOSI_PIN    			PORT_Pin_6 // 
#define SCK_PIN    				PORT_Pin_5 // 
#define FSS_PIN    				PORT_Pin_0 // Выбор микросхемы FLESH
#define MISO_PIN    			PORT_Pin_2 // 
#define PORT_FSS_PIN			MDR_PORTB

#define _FLASH_SIZE_ 0x0200000

#pragma anon_unions

typedef struct {
	uint8_t manufacturerId;
	uint8_t deviceId_memoryType;
	uint8_t deviceId_memoryCapacity;
} flashInfo_t;

uint8_t sst25vf016b_initSPI(void);
uint8_t sst25vf016b_getId(flashInfo_t * flashInfo);
uint8_t sst25vf016b_getId2(uint8_t * buf, uint8_t addr);
uint8_t sst25vf016b_Read(uint32_t addr, uint8_t *data, uint16_t nbytes);
uint8_t sst25vf016b_GlobalUnprotect(void);
uint8_t sst25vf016b_GlobalProtect(void);
uint8_t sst25vf016b_Write_byte (uint32_t addr, uint8_t data);
uint8_t sst25vf016b_Write_buf(uint32_t addr, uint8_t *buf, uint32_t length);
uint8_t sst25vf016b_status(uint8_t * b);
uint8_t sst25vf016b_EraseSector_4Kb (uint32_t addr);
uint8_t sst25vf016b_EraseBlock_32Kb (uint32_t addr);
uint8_t sst25vf016b_EraseBlock_64Kb (uint32_t addr);
uint8_t sst25vf016b_EraseChip (uint32_t addr);


#endif // SST25VF016B
