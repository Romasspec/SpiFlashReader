#include "spi.h"


//#define SSP_FLASH 	MDR_SSP2
MDR_SSP_TypeDef* SSP_FLASH = MDR_SSP2;

uint8_t spi_init(MDR_SSP_TypeDef* ssp)
{
	uint8_t err = 0;
	SSP_FLASH = ssp;
	// Initialize SPI
	SSP_InitTypeDef sSSP;
	// Reset all SSP settings
	SSP_Cmd(SSP_FLASH, DISABLE);
	SSP_DeInit(SSP_FLASH);
	SSP_BRGInit(SSP_FLASH, SSP_HCLKdiv1);
	// MASTER configuration
	SSP_StructInit (&sSSP);
	//частота передачи данных SSPCLK / ( CPSDVR * (1 + SCR) )
	sSSP.SSP_SCR  = 0;  
	sSSP.SSP_CPSDVSR = 100;
	sSSP.SSP_Mode = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength8b;
	sSSP.SSP_SPH = SSP_SPH_1Edge;
	sSSP.SSP_SPO = SSP_SPO_Low;
	sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
	SSP_Init (SSP_FLASH, &sSSP);
	SSP_Cmd (SSP_FLASH, ENABLE);
	SSP_ITConfig (SSP_FLASH, SSP_IT_TX | SSP_IT_RX, DISABLE);
	// Transfer complete
	while((SSP_GetFlagStatus (SSP_FLASH, SSP_FLAG_BSY)))
	;
	return err;
}

uint8_t spi_write(uint8_t b)
{
	uint8_t err = 0;
	//Если буфер приемника не пуст, вычитываем оттуда все содержимое
	while(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_RNE) == SET)
		SSP_ReceiveData(SSP_FLASH);
	// если буфер передатчика не полный
	if(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_TNF) == SET)
	{
		SSP_SendData(SSP_FLASH, b);
		// ожидание окончания обмена
		while(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_BSY) == SET)						//SSP_FLAG_RNE
		;
	}
	return err;
}

uint8_t spi_read(uint8_t * b)
{
	uint8_t err = 0;
	//Если буфер приемника не пуст, вычитываем оттуда все содержимое
	while(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_RNE) == SET)
		SSP_ReceiveData(SSP_FLASH);
	spi_write(0xFF);
	// если буфер приемника не пустой
	//if(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_RNE) == SET)
	while (SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_RNE) == RESET);
	*b = (uint8_t)SSP_ReceiveData(SSP_FLASH);
	return err;
}

uint8_t spi_get(uint8_t * b)
{
	return spi_read(b);
}

// Функция для записи команды 
// Параметры  cmd - код команды (последовательный порт)
// Возв.знач. err - код ошибки (0x00 - нет ошибок)
//
uint8_t spi_set(uint8_t cmd) 
{
	uint8_t err = 0;
	spi_write(cmd);
	if(SSP_GetFlagStatus(SSP_FLASH, SSP_FLAG_RNE) == SET) 
	{
		err = (uint8_t)SSP_ReceiveData(SSP_FLASH);
		if(err) 
			return err; // запись команды выполнена с ошибкой
	} // запись команды выполнена без ошибок
	return err;
}
