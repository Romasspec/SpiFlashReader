#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>
#include <MDR32F9Qx_adc.h>
#include <MDR32F9Qx_uart.h>
#include "define.h"
#include <string.h>
#include <math.h>

void RST_clk_Init (void);
void myPort_Init (void);
void myTimer1_Init (void);
void myTimer2_Init (void);
void myTimer3_Init (void);
void (*task_m)(void);
void task_m1 (void);
void task_m2 (void);
void task_m3 (void);
void set_Fnom (void);
void myADC_Init(void);
void myUART_Init(void);

uint16_t Tim_CCR, temp_ARR, timer_1ms[4], ADC_Result;
uint8_t	faza_shag, flags, VCNTTimer;
uint32_t Uout;
uint32_t UoutRef;
int32_t ErrorU, SumErrorU;
uint32_t PIDout;
int32_t kp;
int32_t ki;
int32_t kd;
int64_t temp;
uint16_t Cnt_Actuator, Cnt_interrupt;
uint32_t status;

float skor, skor_ref, skor_err, skor_PID, tik, skor_sum_err, Skor_ki;


float Skor_kp;
float ACoef[3], BCoef[3], x[3], y[3];
uint8_t RX_BUF[BUFFSIZE], state_RX, RX_BUF_DATA[LENGHT_FRAME_RX_UART-1];
uint32_t adr_write, lenght_write;
uint8_t buffer_test[2050] = {0};


uint16_t buf[720];
uint16_t *bufpr, *bufpr_end, *bufpr_rend;
uint16_t timer1;

void sst25vf016b_EraseSector_2Kb_RAM(void)
{
	for(uint16_t i = 0; i<2050; i++) {
		buffer_test[i] = 0x00;
	}
}

uint8_t sst25vf016b_Write_buf_RAM(uint32_t addr, uint8_t *buf, uint32_t length)
{
	for(uint32_t i = 0; i<length; i++) {
		buffer_test[addr + i] = buf[i];
	}

	return 0;
}

uint8_t sst25vf016b_Read_RAM(uint32_t addr, uint8_t *data, uint32_t nbytes)
{
	uint32_t i;							
	for(i=0; i<nbytes; i++) {
		data[i] = buffer_test[addr+i];		
	}
	
	return 0;
}
void uart_send_message (uint8_t *data, uint16_t lenght)
{	
	for (uint16_t i = 0; i < lenght; i++)
	{
		while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE) == RESET);
		UART_SendData (MDR_UART2, *(data+i));
	}
}
uint32_t startAdr, stopAdr;

int main (void)
{	
	startAdr = stopAdr;
	RST_clk_Init();
	myPort_Init();
	sst25vf016b_initSPI();
	myTimer1_Init();
	myTimer3_Init();
	myUART_Init();
	
//	myADC_Init ();	
//	myTimer2_Init ();
//	myTimer3_Init ();
	
		
	timer_1ms[0] = 0;
	timer_1ms[1] = 0;
	timer_1ms[2] = 0;
	timer_1ms[3] = 0;
			
	task_m = &task_m1;
//	ADC1_Start();
	while (1)
	{				
		(*task_m)();
	}
}

void task_m1 ()
{				
	static int32_t adc_result;
	static uint8_t type = 0, btn_cnt;
	static uint32_t adc_buf = 0;
	static uint16_t adc_cnt = 0;
	
			if (timer_1ms[2] > 1000)
			{
				timer_1ms[2]	=	0;
				static uint16_t cnt;
				if(cnt>9999) cnt = 0;
				cnt++;				
				//UART_SendData (MDR_UART2, (uint8_t)cnt);
					
			}
			
			if (timer_1ms[3] > 2)
			{
				timer_1ms[3]	=	0;
//				ADC1_Start();
//				if (ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == SET) {					
//					adc_buf += ((int16_t) ADC1_GetResult() - (int16_t)2045) *((int16_t) ADC1_GetResult() - (int16_t)2045);
//					adc_cnt++;
//					
//					if(adc_cnt >= 512) {
//						adc_result = (uint16_t) (sqrtf(adc_buf/adc_cnt) / 4.08264f);
//						
//						adc_buf = 0;
//						adc_cnt = 0;
//					}
//	//					adc_result = ADC1_GetResult();
//					
//				}
				
				if (BTN_PUSH) {
					if (btn_cnt < 20) {
						btn_cnt++;
					}
				} else {
					if (btn_cnt >= 20) {
						type ^= 1;
					}
					btn_cnt = 0;
				}
			}

			
			if ((timer_1ms[1] > 20) && (flags & (1<<UART_RX_MESS)))
			{	
//				static uint8_t i = 0;
				timer_1ms[1]	=	0;
//				UART_SendData (MDR_UART2, RX_BUF_DATA[i]);
//				i++;
//				if (i == LENGHT_FRAME_RX_UART-1)
//				{
//					flags &=~(1<<UART_RX_MESS);
//					i = 0;
//				}
				
				if (RX_BUF_DATA[0] == 'r')
				{
					//NVIC_SystemReset();
					MDR_WWDG->CFR	= 0x00000000;
					MDR_WWDG->CR	= 0x000000F1;
				}
				
				if (RX_BUF_DATA[0] == 0x01)
				{
					UoutRef = RX_BUF_DATA[4]<<8 |RX_BUF_DATA[3];
					UoutRef = UoutRef<<8 |RX_BUF_DATA[2];
					UoutRef = UoutRef<<8 |RX_BUF_DATA[1];
					flags |= (1<<FORWARD);
					memcpy(&skor_ref, &UoutRef, 4);
//					skor_ref = (float) UoutRef;
				}
				else if (RX_BUF_DATA[0] == 0x02)
				{					
					UoutRef = RX_BUF_DATA[4]<<8 |RX_BUF_DATA[3];
					UoutRef = UoutRef<<8 |RX_BUF_DATA[2];
					UoutRef = UoutRef<<8 |RX_BUF_DATA[1];
					flags &=~(1<<FORWARD);
//					skor_ref = (float) UoutRef;
					memcpy(&skor_ref, &UoutRef, 4);
				}					
				else if (RX_BUF_DATA[0] == 0x03)
				{
					Tim_CCR = RX_BUF_DATA[2]<<8 |RX_BUF_DATA[1] ;
					if (Tim_CCR > MAX_CCR)
					{
						Tim_CCR = MAX_CCR;
					}
					flags |= (1<<CCR_UPDATE_FD);
				}	
				else if (RX_BUF_DATA[0] == 0x04)
				{
					Tim_CCR = RX_BUF_DATA[2]<<8 |RX_BUF_DATA[1] ;
					if (Tim_CCR > MAX_CCR)
					{
						Tim_CCR = MAX_CCR;
					}
					flags |= (1<<CCR_UPDATE_BC);
				}			
				else if (RX_BUF_DATA[0] == 0x05)
				{
					bufpr_rend = &bufpr[0] + (RX_BUF_DATA[2]<<8 |RX_BUF_DATA[1]);
					bufpr = &buf[0];
					flags |= (1<<BUFF_OUT);
				}
				else if (RX_BUF_DATA[0] == 0x06 )
				{
					flags ^= (1<<START_UART_TX_MESS);
				}
				else if (RX_BUF_DATA[0] == 0x07)
				{
					kp = RX_BUF_DATA[4]<<8 |RX_BUF_DATA[3];
					kp = kp<<8 |RX_BUF_DATA[2];
					kp = kp<<8 |RX_BUF_DATA[1];					
					memcpy(&Skor_kp, &kp, 4);
				}
				else if (RX_BUF_DATA[0] == 0x08)
				{
					ki = RX_BUF_DATA[4]<<8 |RX_BUF_DATA[3];
					ki = ki<<8 |RX_BUF_DATA[2];
					ki = ki<<8 |RX_BUF_DATA[1];					
					memcpy(&Skor_ki, &ki, 4);
				}
//********************************************
				else if (RX_BUF_DATA[0] == 0x09)
				{
					flashInfo_t flashinfo;
					sst25vf016b_getId(&flashinfo);
					
					uart_send_message((uint8_t*) (&flashinfo), 3);
				}
				
				else if (RX_BUF_DATA[0] == 0x0A)
				{
					uint8_t buf[10];
					sst25vf016b_getId2(buf,0);
					
					uart_send_message((uint8_t*) (buf), 4);
				}
				else if (RX_BUF_DATA[0] == 0x0B)
				{
					uint8_t buf[10];
					sst25vf016b_getId2(buf,1);
					
					uart_send_message((uint8_t*) (buf), 4);
				}
				else if (RX_BUF_DATA[0] == 0x0C)	// считать буфер
				{
					uint8_t buf[TX_BUF_SIZE];
					uint32_t adr = (RX_BUF_DATA[3]<<16)|(RX_BUF_DATA[2]<<8)|RX_BUF_DATA[1];
					uint32_t nbyte = ((RX_BUF_DATA[6]<<16)|(RX_BUF_DATA[5]<<8)|(RX_BUF_DATA[4])) - adr + 1;
//					uart_send_message((uint8_t*) (&nbyte), 1);
					
					while (nbyte>0)
					{					
						if (nbyte > TX_BUF_SIZE) {
							sst25vf016b_Read(adr, buf, TX_BUF_SIZE);
							//sst25vf016b_Read_RAM(adr, buf, TX_BUF_SIZE);							
							uart_send_message((uint8_t*) (buf), TX_BUF_SIZE);							
							nbyte -= TX_BUF_SIZE;
							adr+=TX_BUF_SIZE;
						} else {
							sst25vf016b_Read(adr, buf, nbyte);
							//sst25vf016b_Read_RAM(adr, buf, nbyte);
							uart_send_message((uint8_t*) (buf), nbyte);
							nbyte = 0;
						}
					}
					
				}
				else if (RX_BUF_DATA[0] == 0x0D)
				{
					uint8_t status;
					sst25vf016b_status(&status);					
					uart_send_message(&status, 1);
				}
				else if (RX_BUF_DATA[0] == 0x0E)
				{
					uint8_t b = 0xCC;
					sst25vf016b_Write_byte((RX_BUF_DATA[2]<<8)|RX_BUF_DATA[1], RX_BUF_DATA[3]);
					//sst25vf016b_GlobalProtect();
					uart_send_message(&b, 1);
				}
				else if (RX_BUF_DATA[0] == 0x0F)  //запись байта
				{
					uint8_t b = 0;
					uint32_t adr = (RX_BUF_DATA[3]<<16)|(RX_BUF_DATA[2]<<8)|RX_BUF_DATA[1];
					sst25vf016b_Read(adr, &b, 1);
					sst25vf016b_GlobalUnprotect();
					if (b != 0xFF) {
						sst25vf016b_EraseSector_4Kb(adr);
					}					
					sst25vf016b_Write_byte(adr, RX_BUF_DATA[4]);
					sst25vf016b_GlobalProtect();
//					uart_send_message(&b, 1);					
				}
				else if (RX_BUF_DATA[0] == 0x1F)  //запись буфера
				{
					uint8_t b = 0x1F;
					adr_write = (RX_BUF_DATA[3]<<16)|(RX_BUF_DATA[2]<<8)|RX_BUF_DATA[1];
					lenght_write = (RX_BUF_DATA[6]<<16)|(RX_BUF_DATA[5]<<8)|RX_BUF_DATA[4];
					task_m = &task_m3;
					flags &=~(1<<UART_RX_MESS);					
					return;
				}				
				else if (RX_BUF_DATA[0] == 0x10)	// воспроизвести буфер
				{
					startAdr = (RX_BUF_DATA[3]<<16)|(RX_BUF_DATA[2]<<8)|RX_BUF_DATA[1];
					stopAdr = (RX_BUF_DATA[6]<<16)|(RX_BUF_DATA[5]<<8)|RX_BUF_DATA[4];
					if (stopAdr > _FLASH_SIZE_)
						stopAdr = _FLASH_SIZE_;
				}
				else if (RX_BUF_DATA[0] == 0x11)
				{
					sst25vf016b_EraseSector_2Kb_RAM();
				}
				
				else if (RX_BUF_DATA[0] == 0x12)
				{
					RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTD, 	ENABLE);
					PORT_InitTypeDef Port_Initstructure;
					// Configure PORTE pins5  for debug
					Port_Initstructure.PORT_Pin		= PORT_Pin_4;
					Port_Initstructure.PORT_OE		= PORT_OE_OUT;
					Port_Initstructure.PORT_FUNC	= PORT_FUNC_PORT;
					Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
					Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
					Port_Initstructure.PORT_PD 		= PORT_PD_DRIVER;
					PORT_Init (MDR_PORTD, &Port_Initstructure);
					uint8_t b = 0x12;
					uart_send_message(&b, 1);
				}
				

				
				

				flags &=~(1<<UART_RX_MESS);
			}			

			if ((timer_1ms[0] > timer1) && (flags & (1<<BUFF_OUT)))
			{				
				union {						
					uint8_t message_b[4];
					uint32_t message;					
				}mes;
				
				uint8_t i;
				timer_1ms[0]	=	0;
								
				if(bufpr < bufpr_rend)
				{					
					mes.message = *bufpr;
					bufpr++;
					UART_SendData (MDR_UART2, START_BYTE);
					for (i = 0; i < 4; i++)
					{
						while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE) == RESET);
						UART_SendData (MDR_UART2, mes.message_b[i]);
					}
				}
				else
				{
					flags &=~(1<<BUFF_OUT);
					bufpr = &buf[0];
				}
			}
			
			if ((timer_1ms[2] > 100) && (flags & (1<<START_UART_TX_MESS)))
			{
				uint8_t i;
				union {						
					uint8_t message_b[4];
					uint32_t message;					
				}mes;
				memcpy(&mes.message, &skor, 4);
				//mes.message = enc_a;
				timer_1ms[2] = 0;
				UART_SendData (MDR_UART2, START_BYTE);
				for (i = 0; i < 4; i++)
					{
						while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE) == RESET);
						UART_SendData (MDR_UART2, mes.message_b[i]);
					}
				flags &=~(1<<START_UART_TX_MESS);
			}
	
	task_m = &task_m2;
}

void task_m2 ()
{	
	static uint8_t rxByte = 0;
	if (UART_GetFlagStatus (MDR_UART2, UART_FLAG_RXFF) == SET)
	{		
		RX_BUF [rxByte] = UART_ReceiveData (MDR_UART2);
		
		if (rxByte == LENGHT_FRAME_RX_UART-1)
		{
			if (RX_BUF[0] == 0x55)
			{
				uint8_t i;
				for (i=1; i < LENGHT_FRAME_RX_UART; i++)
				{
					RX_BUF_DATA [i-1] = RX_BUF [i];
					flags |= (1<<UART_RX_MESS);
				}
			}
			rxByte = 0;
			state_RX = 0;
		}
		else
		{
			switch (state_RX)
			{
			case 0:
				if (RX_BUF[rxByte] == 0x55)
				{
					rxByte = 1;
					state_RX = 1;
				}				
			break;
			
			case 1:
				if (RX_BUF[rxByte] == 0xDA)
				{
					state_RX = 2;
				}
				else if (RX_BUF[rxByte] == 0x55)
				{
					rxByte = 1;
					state_RX = 1;
				}
				else
				{
					rxByte++;
				}
			break;
				
			case 2:
				if (RX_BUF[rxByte] == 0xDB)
				{
					RX_BUF[rxByte] = 0x55;					
				}
				else if (RX_BUF[rxByte] == 0xDC)
				{
					RX_BUF[rxByte] = 0xDA;
				}
				else if (RX_BUF[rxByte] == 0x55)
				{
					rxByte = 1;
					state_RX = 1;
				}
				else
				{
					rxByte = 0;
					state_RX = 0;
					break;
				}
				
				rxByte++;
				state_RX = 1;
			break;			
			}
		}		
	}
	
	task_m = &task_m1;
}

void task_m3 ()
{	
	uint32_t rxByte = 0;
	uint32_t ncykl = lenght_write / BUFFSIZE;
	uint32_t last = lenght_write % BUFFSIZE;
	uint32_t i;	
	
	
	i=0;
	sst25vf016b_GlobalUnprotect();
	sst25vf016b_EraseSector_4Kb(adr_write);
	while (i<ncykl) {
		if (UART_GetFlagStatus (MDR_UART2, UART_FLAG_RXFF) == SET) {	
			RX_BUF [rxByte++] = UART_ReceiveData (MDR_UART2);
			
			if (rxByte == BUFFSIZE) {
				rxByte = 0;			
				sst25vf016b_Write_buf(adr_write + BUFFSIZE*i, RX_BUF, BUFFSIZE);
				//sst25vf016b_Write_buf_RAM(adr_write + BUFFSIZE*i, RX_BUF, BUFFSIZE);
				i++;
				if ((i < ncykl) || (last != 0)) {
					uart_send_message ((uint8_t*)("NEXT"),4);
				}				
			}
		}
		
	}
	rxByte = 0;
	i=0;
	while(i<last) {
		if (UART_GetFlagStatus (MDR_UART2, UART_FLAG_RXFF) == SET) {	
			RX_BUF [rxByte++] = UART_ReceiveData (MDR_UART2);
			
			if (rxByte == last) {
				rxByte = 0;
				sst25vf016b_Write_buf(adr_write + BUFFSIZE*ncykl, RX_BUF, last);
//				sst25vf016b_Write_buf_RAM(adr_write + BUFFSIZE*ncykl, RX_BUF, last);
			}
			i++;
		}
		
	}
	sst25vf016b_GlobalProtect();
	i=lenght_write;
	uart_send_message ((uint8_t*)("FINISH"),6);
//	if (i > 1030) {
//		i=1030;
//	}
//	uart_send_message(buffer_test, i);
	task_m = &task_m2;
}

void RST_clk_Init()
{
	RST_CLK_DeInit();
	RST_CLK_HSEconfig (RST_CLK_HS_CONTROL_HSE_ON);								// eternal oscillator on
	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_HSERDY)));				// waiting
	RST_CLK_CPU_PLLcmd (ENABLE);																	// PLL on
	// configures the CPU_PLL clock source and multiplication factor
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);//RST_CLK_CPU_PLLsrcHSEdiv1
	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_PLLCPURDY)));
	// select the CPU_PLL output as input for CPU_C2_SEL
	RST_CLK_CPU_PLLuse (ENABLE);
	// configures the CPU_C3_SEL division factor
	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	// select the HCLK clock source
	RST_CLK_CPUclkSelection (RST_CLK_CPUclkCPU_C3);
	// Enable peripheral clocks  
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_RST_CLK, ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTA, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTB, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTC, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTD, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTE, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTF, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_EEPROM,	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER1,	ENABLE);
	//RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER2,	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER3,	ENABLE);
	RST_CLK_PCLKcmd	( RST_CLK_PCLK_UART2, 	ENABLE);
	//RST_CLK_PCLKcmd	( RST_CLK_PCLK_ADC,	  	ENABLE);
	RST_CLK_PCLKcmd (RST_CLK_PCLK_WWDG, 		ENABLE);
	RST_CLK_PCLKcmd (RST_CLK_PCLK_SSP2, ENABLE);
}

void myPort_Init ()
{
	PORT_InitTypeDef Port_Initstructure;
	
	PORT_DeInit (MDR_PORTA);
	PORT_DeInit (MDR_PORTB);
	PORT_DeInit (MDR_PORTC);
	PORT_DeInit (MDR_PORTD);
	PORT_DeInit (MDR_PORTE);
	PORT_DeInit (MDR_PORTF);
	
	
	PORT_StructInit (&Port_Initstructure);
		// Configure PORTD pins  for UART
	Port_Initstructure.PORT_Pin		= PORT_Pin_0;
	Port_Initstructure.PORT_OE		= PORT_OE_IN;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_ALTER;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	PORT_Init (MDR_PORTD, &Port_Initstructure);
	
	Port_Initstructure.PORT_Pin		= PORT_Pin_1;
	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_ALTER;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	PORT_Init (MDR_PORTD, &Port_Initstructure);
	
	// Configure PORTD
	// Configure SSP pins: FSS, CLK, RXD, TXD
	PORT_StructInit (&Port_Initstructure);	
	Port_Initstructure.PORT_FUNC  = PORT_FUNC_ALTER;   	
	Port_Initstructure.PORT_MODE  = PORT_MODE_DIGITAL;   
	Port_Initstructure.PORT_SPEED = PORT_SPEED_MAXFAST;
//	Port_Initstructure.PORT_PULL_UP = PORT_PULL_UP_ON;
	Port_Initstructure.PORT_Pin   = MISO_PIN;
	Port_Initstructure.PORT_OE    = PORT_OE_IN;
	PORT_Init(MDR_PORTD, &Port_Initstructure);
//	Port_Initstructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	Port_Initstructure.PORT_Pin   = SCK_PIN | MOSI_PIN;
	Port_Initstructure.PORT_OE    = PORT_OE_OUT;
	PORT_Init(MDR_PORTD, &Port_Initstructure);
	Port_Initstructure.PORT_FUNC  = PORT_FUNC_PORT;   	
	Port_Initstructure.PORT_Pin   = FSS_PIN;
	Port_Initstructure.PORT_OE    = PORT_OE_OUT;
	Port_Initstructure.PORT_PD 		= PORT_PD_DRIVER;
	PORT_Init(PORT_FSS_PIN, &Port_Initstructure);
	
	// Configure PORTE pins6  for PWM timer3
	Port_Initstructure.PORT_Pin		= PORT_Pin_6;
	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_OVERRID;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	Port_Initstructure.PORT_PD 		= PORT_PD_DRIVER;	
	PORT_Init (MDR_PORTE, &Port_Initstructure);
	
	// Configure PORTE pins5  for debug
	Port_Initstructure.PORT_Pin		= PORT_Pin_3;
	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_PORT;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	Port_Initstructure.PORT_PD 		= PORT_PD_DRIVER;
	PORT_Init (MDR_PORTE, &Port_Initstructure);
}

void myTimer1_Init ()
{
	TIMER_CntInitTypeDef sTim_CntInit;	
	TIMER_CntStructInit (&sTim_CntInit);
	TIMER_DeInit (MDR_TIMER1);
	sTim_CntInit.TIMER_IniCounter 					= 0;
	sTim_CntInit.TIMER_Prescaler						= 9;
	sTim_CntInit.TIMER_Period								= 8000;
	sTim_CntInit.TIMER_CounterMode					= TIMER_CntMode_ClkFixedDir;
	sTim_CntInit.TIMER_CounterDirection			= TIMER_CntDir_Up;
	sTim_CntInit.TIMER_EventSource					= TIMER_EvSrc_None;
	sTim_CntInit.TIMER_FilterSampling				= TIMER_FDTS_TIMER_CLK_div_1;
	sTim_CntInit.TIMER_ARR_UpdateMode				= TIMER_ARR_Update_On_CNT_Overflow;
	
	TIMER_CntInit (MDR_TIMER1, &sTim_CntInit);
	TIMER_BRGInit (MDR_TIMER1, TIMER_HCLKdiv1);
	
	TIMER_ITConfig (MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);
	NVIC_EnableIRQ (Timer1_IRQn);
	
	TIMER_Cmd (MDR_TIMER1, ENABLE);
}

void myTimer3_Init ()
{
	TIMER_CntInitTypeDef 			sTim_CntInit;
	TIMER_ChnInitTypeDef			sTim_ChnInit;
	TIMER_ChnOutInitTypeDef		sTim_ChnOutInit;
	
	TIMER_CntStructInit (&sTim_CntInit);
	TIMER_DeInit (MDR_TIMER3);
	sTim_CntInit.TIMER_IniCounter 							= 0;
	sTim_CntInit.TIMER_Prescaler								= 12;
	sTim_CntInit.TIMER_Period										= 0x100;
	sTim_CntInit.TIMER_CounterMode							= TIMER_CntMode_ClkFixedDir;
	sTim_CntInit.TIMER_CounterDirection					= TIMER_CntDir_Up;
	sTim_CntInit.TIMER_EventSource							= TIMER_EvSrc_None;
	sTim_CntInit.TIMER_FilterSampling						= TIMER_FDTS_TIMER_CLK_div_4;
	sTim_CntInit.TIMER_ARR_UpdateMode						= TIMER_ARR_Update_On_CNT_Overflow;
//	sTim_CntInit.TIMER_ARR_UpdateMode						= TIMER_ARR_Update_Immediately;
	TIMER_CntInit (MDR_TIMER3, &sTim_CntInit);
	
	
	TIMER_ChnStructInit (&sTim_ChnInit);
	sTim_ChnInit.TIMER_CH_Number 								= TIMER_CHANNEL3;
	sTim_ChnInit.TIMER_CH_Mode									= TIMER_CH_MODE_PWM;
	sTim_ChnInit.TIMER_CH_ETR_Ena								= DISABLE;
	sTim_ChnInit.TIMER_CH_ETR_Reset							= TIMER_CH_ETR_RESET_Disable;
	sTim_ChnInit.TIMER_CH_BRK_Reset							= TIMER_CH_BRK_RESET_Disable;
	sTim_ChnInit.TIMER_CH_REF_Format						= TIMER_CH_REF_Format6;
	sTim_ChnInit.TIMER_CH_Prescaler							= TIMER_CH_Prescaler_None;
	sTim_ChnInit.TIMER_CH_EventSource						= TIMER_CH_EvSrc_NE;
	sTim_ChnInit.TIMER_CH_FilterConf						= TIMER_Filter_1FF_at_TIMER_CLK;
	sTim_ChnInit.TIMER_CH_CCR_UpdateMode				= TIMER_CH_CCR_Update_On_CNT_eq_0;
	sTim_ChnInit.TIMER_CH_CCR1_Ena							= DISABLE;
	sTim_ChnInit.TIMER_CH_CCR1_EventSource			= TIMER_CH_CCR1EvSrc_PE;
	TIMER_ChnInit (MDR_TIMER3, &sTim_ChnInit);
	
	TIMER_ChnOutStructInit (&sTim_ChnOutInit);
	sTim_ChnOutInit.TIMER_CH_Number							= TIMER_CHANNEL3;
	sTim_ChnOutInit.TIMER_CH_DirOut_Polarity		= TIMER_CHOPolarity_NonInverted;
	sTim_ChnOutInit.TIMER_CH_DirOut_Source			= TIMER_CH_OutSrc_REF;						//TIMER_CH_OutSrc_DTG;
	sTim_ChnOutInit.TIMER_CH_DirOut_Mode				= TIMER_CH_OutMode_Output;
	sTim_ChnOutInit.TIMER_CH_NegOut_Polarity		= TIMER_CHOPolarity_NonInverted;
	sTim_ChnOutInit.TIMER_CH_NegOut_Source			= TIMER_CH_OutSrc_DTG;
	sTim_ChnOutInit.TIMER_CH_NegOut_Mode				= TIMER_CH_OutMode_Input;					//TIMER_CH_OutMode_Output;
	sTim_ChnOutInit.TIMER_CH_DTG_MainPrescaler	= 40;
	sTim_ChnOutInit.TIMER_CH_DTG_AuxPrescaler		= 0;
	sTim_ChnOutInit.TIMER_CH_DTG_ClockSource		= TIMER_CH_DTG_ClkSrc_TIMER_CLK;
	TIMER_ChnOutInit (MDR_TIMER3, &sTim_ChnOutInit);
	
	TIMER_ITConfig (MDR_TIMER3, TIMER_STATUS_CNT_ARR, ENABLE);
	MDR_TIMER3->STATUS = 0;
	NVIC_EnableIRQ (Timer3_IRQn);

	TIMER_BRGInit (MDR_TIMER3, TIMER_HCLKdiv1);
	TIMER_Cmd (MDR_TIMER3, ENABLE);
	
	TIMER_SetChnCompare(MDR_TIMER3, TIMER_CHANNEL3, 10);
}

void myADC_Init()
{
	
	ADC_InitTypeDef		ADC_InitStructure;
	ADCx_InitTypeDef 	ADCx_InitStructure;
	
	ADC_StructInit (&ADC_InitStructure);
	ADC_DeInit ();
	ADC_InitStructure.ADC_SynchronousMode 				= ADC_SyncMode_Independent;
	ADC_InitStructure.ADC_StartDelay							= 0;
	ADC_InitStructure.ADC_TempSensor							= ADC_TEMP_SENSOR_Disable;
	ADC_InitStructure.ADC_TempSensorAmplifier			= ADC_TEMP_SENSOR_AMPLIFIER_Disable;
	ADC_InitStructure.ADC_TempSensorConversion		= ADC_TEMP_SENSOR_CONVERSION_Disable;
	ADC_InitStructure.ADC_IntVRefConversion				= ADC_VREF_CONVERSION_Disable;
	ADC_InitStructure.ADC_IntVRefTrimming					= 0;
	ADC_Init (&ADC_InitStructure);
	
	ADCx_StructInit (&ADCx_InitStructure);
	ADCx_InitStructure.ADC_ClockSource						= ADC_CLOCK_SOURCE_CPU;
	ADCx_InitStructure.ADC_SamplingMode						= ADC_SAMPLING_MODE_SINGLE_CONV;
	ADCx_InitStructure.ADC_ChannelSwitching				= ADC_CH_SWITCHING_Disable;
	ADCx_InitStructure.ADC_ChannelNumber					= ADC_CH_ADC2;
	ADCx_InitStructure.ADC_Channels								= 0;
	ADCx_InitStructure.ADC_LevelControl						= ADC_LEVEL_CONTROL_Disable;
	ADCx_InitStructure.ADC_LowLevel								= 0;
	ADCx_InitStructure.ADC_HighLevel							= 0;
	ADCx_InitStructure.ADC_VRefSource							= ADC_VREF_SOURCE_INTERNAL;
	ADCx_InitStructure.ADC_IntVRefSource					= ADC_INT_VREF_SOURCE_INEXACT;
	ADCx_InitStructure.ADC_Prescaler							= ADC_CLK_div_8;
	ADCx_InitStructure.ADC_DelayGo								= 0;
	ADC1_Init (&ADCx_InitStructure);
	
	ADC1_Cmd (ENABLE);	
}

void myUART_Init ()
{	
	UART_InitTypeDef UART_InitStructure;
	
	UART_DeInit (MDR_UART2);
	UART_BRGInit (MDR_UART2, UART_HCLKdiv1);
	
	UART_StructInit (&UART_InitStructure);
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength8b;
	UART_InitStructure.UART_StopBits = UART_StopBits1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART_Init (MDR_UART2, &UART_InitStructure);
	
	UART_Cmd (MDR_UART2, ENABLE);
	
	//ADC1_Start();
}
void Timer1_IRQHandler ()
{	
	timer_1ms[0] ++;
	timer_1ms[1] ++;
	timer_1ms[2] ++;
	timer_1ms[3] ++;	
	
	TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ARR_EVENT);
}

void Timer3_IRQHandler ()
{
	static uint8_t presc = 0;
	uint8_t ccr;
	MDR_PORTD->RXTX ^= PORT_Pin_3;
	MDR_PORTD->RXTX ^= PORT_Pin_4;	
	TIMER_ClearITPendingBit(MDR_TIMER3, TIMER_STATUS_CNT_ARR);
	
	if ((startAdr < stopAdr) && (presc % 3 == 0))
	{
		sst25vf016b_Read(startAdr, &ccr, 1);
		MDR_TIMER3->CCR3 = ccr;
		startAdr++;
	} else if (startAdr >= stopAdr) {
//		MDR_TIMER3->CCR3 = 0;
	}
	presc++;
}
