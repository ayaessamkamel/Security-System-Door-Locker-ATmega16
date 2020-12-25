 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: AYA ESSAM
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/************************************************************************
 *                           Global variables                           *
 ************************************************************************/

extern volatile uint8 g_recievedValue;
extern volatile uint8 g_recievedFlag ;
/*******************************************************************************
 *                      Types Declaration                                     *
 *******************************************************************************/
typedef enum
{
	FIVE,SIX,SEVEN,EIGHT
}UART_DataBits;
typedef enum
{
	ONE,TWO
}UART_StopBit;
typedef enum
{
	DISABLE_PARITY,ENABLE_EVEN=2,ENABLE_ODD=3
}UART_Parity;
/******************************************************************************
Configuration structure for dynamic configuration of uart
like bit number,parity,stop bit and baud rate
 *******************************************************************************/

typedef struct
{
	UART_DataBits dataBits;
	UART_StopBit stopBits;
	UART_Parity parityBits;
	uint32 baudRate ;
}UART_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void UART_init(const UART_ConfigType *Config_ptr);

void UART_sendByte(const uint8 data);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
