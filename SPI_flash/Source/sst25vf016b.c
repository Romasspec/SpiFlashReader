#include "sst25vf016b.h"
#include <string.h>

#define SET_CS  PORT_WriteBit(cs_port, cs_pin, Bit_SET)//; 		PORT_WriteBit (MDR_PORTE, PORT_Pin_3, Bit_SET)
#define CLR_CS  PORT_WriteBit(cs_port, cs_pin, Bit_RESET)//; 	PORT_WriteBit (MDR_PORTE, PORT_Pin_3, Bit_RESET)

//#define SET_CS  /*PORT_WriteBit(MDR_PORTD, PORT_Pin_4, Bit_SET);*/ 		PORT_WriteBit (MDR_PORTE, PORT_Pin_3, Bit_SET)
//#define CLR_CS  /*PORT_WriteBit(MDR_PORTD, PORT_Pin_4, Bit_RESET);*/ 	PORT_WriteBit (MDR_PORTE, PORT_Pin_3, Bit_RESET)

// device commands
#define FLASH_CMD_READ					0x03	// read
#define FLASH_CMD_FASTREAD			0x0B	// high-speed read
#define FLASH_CMD_BLOCKERASE		0x20	// block erase 4k
#define FLASH_CMD_BLOCKERASE_32 0x52 	// block erase 32k
#define FLASH_CMD_BLOCKERASE_64 0xD8 	// block erase 64k
#define FLASH_CMD_CHIPERASE			0xC7	// chip erase				(0x60)
#define FLASH_CMD_BYTEPROG			0x02	// byte program
#define FLASH_CMD_AUTOADRINC		0xAD	// Auto Address Increment Programming
#define FLASH_CMD_RDSR					0x05	// read status register
#define FLASH_CMD_EWSR					0x50	// enable write status register
#define FLASH_CMD_WRSR					0x01	// write status register
#define FLASH_CMD_WREN					0x06	// write enable
#define FLASH_CMD_WRDI					0x04	// write disable
#define FLASH_CMD_RDID					0x90	// read ID register
#define FLASH_CMD_RJDID					0x9F	// read ID register
#define FLASH_CMD_EBSY					0x70	// Enable SO to output RY/BY# status during AAI programming
#define FLASH_CMD_DBSY					0x80	// Disable SO to output RY/BY# status during AAI programming
// status register bits
#define FLASH_STATUS_BUSY				0x01	// busy, write in progress
#define FLASH_STATUS_WEN				0x02	// write enable
#define FLASH_STATUS_WPBP0			0x04	// block protect 0
#define FLASH_STATUS_WPBP1			0x08	// block protect 1
#define FLASH_STATUS_WPBP2			0x10	// block protect 2
#define FLASH_STATUS_WPBP3			0x20	// block protect 2
#define FLASH_STATUS_AAI				0x40	// Auto Address Increment Enable
#define FLASH_STATUS_BPL				0x80	// Sector protection registers locked

// device constants
#define FLASH_PAGESIZE					0x1000		// 256 bytes/page
#define FLASH_BLOCKSIZE					0x8000 		// 4096 block size 0x1000
#define FLASH_SIZE							0x200000	// 0x800000

uint32_t cs_pin;
MDR_PORT_TypeDef* cs_port;

uint8_t sst25vf016b_init (uint32_t pin, MDR_PORT_TypeDef* port)
{
	uint8_t err = 0;     // код ошибки
	cs_pin = pin;
	cs_port = port;
	SET_CS;
	spi_init(SPI_FLASH);
	return err; 
}

uint8_t sst25vf016b_initSPI (void) 
{    
	return sst25vf016b_init(FSS_PIN, PORT_FSS_PIN);
}

uint8_t sst25vf016b_status(uint8_t * b)
{
	uint8_t err = 0;
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	spi_get(b);
	SET_CS;
	return err;
}

uint8_t sst25vf016b_getId(flashInfo_t * flashInfo)
{
	uint8_t err = 0;
	memset (flashInfo, 0, sizeof(flashInfo_t));
	CLR_CS;
	spi_set (FLASH_CMD_RJDID);
	spi_get (&flashInfo->manufacturerId);
	spi_get (&flashInfo->deviceId_memoryType);
	spi_get (&flashInfo->deviceId_memoryCapacity);	
	SET_CS;
	return err;
}

uint8_t sst25vf016b_getId2(uint8_t * buf, uint8_t addr)
{
	uint8_t err = 0, nbytes = 4;
	memset (buf, 0, 4);
	CLR_CS;
	spi_set(FLASH_CMD_RDID);
	spi_set (0x00);
	spi_set (0x00);
	spi_set (addr);
	while(nbytes--)
		spi_get(buf++);
	SET_CS;
	return err;
}

uint8_t sst25vf016b_Read(uint32_t addr, uint8_t *data, uint16_t nbytes)
{
	CLR_CS;
	// begin read
	// issue read command
	spi_set(FLASH_CMD_READ);
	// send address
	spi_set ((addr>>16)&0xFF);
	spi_set ((addr>>8) &0xFF);
	spi_set ((addr>>0) &0xFF);
	//FlashSet (0x00);
	// read data
	while(nbytes--)
		spi_get(data++);
	SET_CS;
	return 0;
}

uint8_t sst25vf016b_Status(uint8_t * b)
{
	uint8_t err = 0;
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	spi_get(b);
	SET_CS;
	return err;
}

uint8_t sst25vf016b_IsBusy(void)
{
	uint8_t b = 0xFF;
	sst25vf016b_Status(&b);
	b = (b & FLASH_STATUS_BUSY);
	return b;
}

uint8_t sst25vf016b_GlobalUnprotect()
{
	uint8_t err = 0;
	CLR_CS;
	spi_set(FLASH_CMD_WREN);
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	spi_set(FLASH_CMD_WRSR);
	spi_set(0x00);
	SET_CS;	
	return err;
}

uint8_t sst25vf016b_GlobalProtect()
{
	uint8_t err = 0;
	CLR_CS;
	spi_set(FLASH_CMD_WREN);
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	spi_set(FLASH_CMD_WRSR);
	spi_set(0x1C);
	SET_CS;	
	return err;
}

uint8_t sst25vf016b_Write_buf(uint32_t addr, uint8_t *buf, uint32_t length)
{
	uint32_t i;
	uint8_t b = 0xFF;
	CLR_CS;
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	
	for(i=0; i<length; i+=2) {
		spi_set(0x00);
		CLR_CS;
		// begin write
		// issue write command
		spi_set(FLASH_CMD_AUTOADRINC);
		if (i == 0) {
			// send address at startup
			spi_set ((addr>>16)&0xFF);
			spi_set ((addr>>8) &0xFF);
			spi_set ((addr>>0) &0xFF);
		}
		spi_set (*(buf+i));
		if (i+1 == length) {			// т.к. запись по 2 байта, если нечетное кол-во, тогда последний байт 0xFF
			spi_set (0xFF);
		} else {
			spi_set (*(buf+1+i));
		}
		SET_CS;
		spi_set(0x00);
		// wait until write is done
		CLR_CS;
		spi_set(FLASH_CMD_RDSR);
		do
		{
			spi_get(&b);
		} while(b & FLASH_STATUS_BUSY);
		SET_CS;		
	}
	spi_set(0x00);
	CLR_CS;	
	spi_set(FLASH_CMD_WRDI);
	SET_CS;	
	return 0;
}

uint8_t sst25vf016b_Write_byte (uint32_t addr, uint8_t data)
{
	uint8_t b = 0xFF;
	CLR_CS;
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	// begin write
	// issue write command
	spi_set(FLASH_CMD_BYTEPROG);
	// send address
	spi_set ((addr>>16)&0xFF);
	spi_set ((addr>>8) &0xFF);
	spi_set ((addr>>0) &0xFF);
	// write data
	spi_set (data);	
	SET_CS;
	spi_set(0x00);
	// wait until write is done
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	do
	{
		spi_get(&b);
	} while(b & FLASH_STATUS_BUSY);
	SET_CS;
	return 0;
}

uint8_t sst25vf016b_EraseChip (uint32_t addr)
{
	uint8_t b = 0xFF;
	CLR_CS;
	// enable write
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	// do chip erase
	spi_set(FLASH_CMD_CHIPERASE);	
	SET_CS;	
	spi_set(0x00);
	// wait until write is done
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	do
	{
		spi_get(&b);
	} while(b & FLASH_STATUS_BUSY);
	SET_CS;
	return b;
}

uint8_t sst25vf016b_EraseSector_4Kb (uint32_t addr)
{
	uint8_t b = 0xFF;
	CLR_CS;
	// enable write
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	// do chip erase 4Kb sector
	spi_set(FLASH_CMD_BLOCKERASE);
	// send address
	spi_set ((addr>>16)&0xFF);
	spi_set ((addr>>8) &0xFF);
	spi_set ((addr>>0) &0xFF);
	SET_CS;	
	spi_set(0x00);
	// wait until write is done
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	do
	{
		spi_get(&b);
	} while(b & FLASH_STATUS_BUSY);
	SET_CS;
	return b;
}

uint8_t sst25vf016b_EraseBlock_32Kb (uint32_t addr)
{
	uint8_t b = 0xFF;
	CLR_CS;
	// enable write
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	// do chip erase 32Kb block
	spi_set(FLASH_CMD_BLOCKERASE_32);
	// send address
	spi_set ((addr>>16)&0xFF);
	spi_set ((addr>>8) &0xFF);
	spi_set ((addr>>0) &0xFF);
	SET_CS;	
	spi_set(0x00);
	// wait until write is done
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	do
	{
		spi_get(&b);
	} while(b & FLASH_STATUS_BUSY);
	SET_CS;
	return b;
}

uint8_t sst25vf016b_EraseBlock_64Kb (uint32_t addr)
{
	uint8_t b = 0xFF;
	CLR_CS;
	// enable write
	spi_set(FLASH_CMD_WREN);	
	SET_CS;
	spi_set(0x00);
	CLR_CS;
	// do chip erase 32Kb block
	spi_set(FLASH_CMD_BLOCKERASE_64);
	// send address
	spi_set ((addr>>16)&0xFF);
	spi_set ((addr>>8) &0xFF);
	spi_set ((addr>>0) &0xFF);
	SET_CS;	
	spi_set(0x00);
	// wait until write is done
	CLR_CS;
	spi_set(FLASH_CMD_RDSR);
	do
	{
		spi_get(&b);
	} while(b & FLASH_STATUS_BUSY);
	SET_CS;
	return b;
}
