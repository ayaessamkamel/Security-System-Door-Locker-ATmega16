 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#include "uart.h"
/*******************************************************************************
 *                             Macros                                 *
 *******************************************************************************/
//#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 8UL))) - 1)

/*******************************************************************************
 *                                ISR                                    *
 *******************************************************************************/
ISR(USART_RXC_vect) {
	g_recievedValue =UDR ;
	g_recievedFlag =1 ;

}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void UART_init(const UART_ConfigType *Config_ptr)
{
	uint32 USART_BAUDRATE=(Config_ptr->baudRate ) ;
	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);
	/************************** UCSRB Description **************************
	 * RXCIE = 1  USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 =0 FOR (5,6,7,8)BIT DATE MODES
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/ 
	UCSRB = (1<<RXEN) | (1<<TXEN)  | (1<<RXCIE);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1); 
	/* UCSZ1:0 = 11 For number of data mode */
	UCSRC =(0XFC &UCSRC) |(Config_ptr->dataBits) ;
	/* UPM1:0  = 00 Disable parity bit */
	UCSRC =(0XCF &UCSRC) |((Config_ptr->parityBits)<<4) ;
	/* USBS    = 0 One stop bit
	 * */
	UCSRC =(0XF7 &UCSRC) |((Config_ptr->stopBits)<<3) ;
	
	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	//UBRRH = BAUD_PRESCALE>>8;
	//UBRRL = BAUD_PRESCALE;
	UBRRH = ((((F_CPU / (USART_BAUDRATE * 8UL))) - 1)>>8) ;
	UBRRL = (((F_CPU / (USART_BAUDRATE * 8UL))) - 1) ;
}
	
void UART_sendByte(const uint8 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for 
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	/* Put the required data in the UDR register and it also clear the UDRE flag as 
	 * the UDR register is not empty now */	 
	UDR = data;
	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transimission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/	
}


void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}
