/*

 * File Name: controlECU.c
 *
 * Created on: Dec 14, 2020
 *
 * Description: Source file for the control ECU that control
 * a motor and buzzer in a locker system it also save the password
 * in M24C16 external eeprom
 *
 * Author: AYA ESSAM
 */
#include "micro_config.h"
#include "external_eeprom.h"
#include "uart.h"
#include "timer.h"
#include "app_uart_messeges.h"
/************************************************************************
 *                           Global variables                           *
 ************************************************************************/
/* variable that holds number of wrong trials for entering password */
uint8 g_trialNumber =0 ;
/* holds the UDR of UART whenever there's data in it */
volatile uint8 g_recievedValue=0;
/*Flag that is set to one whenever there's data in UDR */
volatile uint8 g_recievedFlag=0 ;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * [function Name] : storeNewPassword
 * [Description] : A function to store new password in eeprom
 * [Args] :
 * [in] uint8* a_storedPassword_Ptr:
 * This argument shall indicate an array that is passed
 * by reference which store the password in it
 */
void storeNewPassword (uint8* a_storedPassword_Ptr);
/*
 * [function Name] : checkPassword
 * [Description] : A function to check entered password
 * [Args] :
 * [in] uint8* a_checkPassword_Ptr:
 * This argument shall indicate an array that is passed
 * by reference which store the password in it
 */
void checkPassword (uint8* a_checkPassword_Ptr) ;
/*
 * [function Name] : compareTwoPasswords
 * [Description] : A function to compare 2 passwords
 * [Args] :
 * [in] uint8* a_checkPassword_Ptr:
 * This argument shall indicate an array that is passed
 * by reference which store the first password in it
 * [in] uint8* a_checkPassword_Ptr:
 * This argument shall indicate an array that is passed
 * by reference which store the secoend password in it
 * [Returns] :
 */
uint8 compareTwoPasswords(uint8* a_passwordOne_Ptr, uint8* a_passwordTwo_Ptr) ;
/* [function Name] : rotateMotorCW
 * [Description] : A function to rotate motor to open the door */
void rotateMotorCW (void) ;
/* [function Name] : holdMotor
 * [Description] : A function to hold the motor (door) */
void holdMotor (void) ;
/* [function Name] : rotateMotorCCW
 * [Description] :A function to rotate motor to close the door */
void rotateMotorCCW (void) ;
/* [function Name] : stopMotor
 * [Description] : A function to stop the motor (door) */
void stopMotor(void) ;
/* [function Name] : buzzerOff
 * [Description] : A function to turn off the buzzer */
void buzzerOff(void);

int main(void)
{
	/* 3 variables used to represent arrays indexes while accessing them*/
	uint8 countArray = 0;
	uint8 countArray2 =0 ;
	uint8 countArray3 =0 ;
	/* 3 arrays used to store data (password) received from MC1*/
	uint8 arr_storedData1 [12]  ;
	uint8 arr_storedData2 [12] ;
	uint8 arr_storedData3 [12] ;
	/* Enable interrupts by setting I-bit */
	SREG  |= (1<<7);
	/* initialize UART */
	UART_ConfigType UART_Config ={EIGHT,ONE,DISABLE_PARITY,9600};
	UART_init(&UART_Config);
	/* Configure INT0/PD3 as output pin for buzzer */
	DDRD  |= (1<<PD2) ;
	/* Configure first two pins in PORTB as output pins (Motor Pins) */
	DDRB  |= (1<<PB0) | (1<<PB1);
	/* motor is stopped at the beginning */
	PORTB &= ~(1<<PB0);
	PORTB &= ~(1<<PB1);
	/* Initialize EEPROM */
	EEPROM_init();
	while(1)
	{
		/* every loop the program clear content of this elements to
		 *  be used as indexes again */
		countArray = 0;
		countArray2 =0 ;
		countArray3 =0 ;
		/* The program begins by switching the received data
		 * in the UDR to take action according to it */
		switch(g_recievedValue) {
		case newPasswordFirst :
			/* every time after entering the case you should clear
			 * 1- the received value so the switch case is performed
			 * one time only
			 * 2- the flag because if you want to store the received
			 * value in this case ,clearing the flag will make
			 * sure you want store the initialize communication
			 * message */
			g_recievedValue=0;
			g_recievedFlag=0 ;
			while(g_recievedValue != finishedEntery ) {
				/* &&(g_recievedValue != finishedEntery : This statement
				 * is important to avoid storing (finishedEntery) if the
				 * g_recievedValue changed after checking the while
				 * condition  */
				if((g_recievedFlag ==1) &&(g_recievedValue != finishedEntery )) {
					arr_storedData1[countArray] =g_recievedValue ;
					countArray++ ;
					g_recievedFlag=0 ;
				}
			}
			arr_storedData1[countArray]=endOfPass ;
			break ;
		case newPasswordSecoend :
			/* every time after entering the case you should clear
			 * 1- the received value so the switch case is performed
			 * one time only
			 * 2- the flag because if you want to store the received
			 * value in this case ,clearing the flag will make
			 * sure you want store the initialize communication
			 * message */
			g_recievedValue=0;
			g_recievedFlag=0 ;
			while(g_recievedValue != finishedEntery ) {
				/* &&(g_recievedValue != finishedEntery : This statement
				 * is important to avoid storing (finishedEntery) if the
				 * g_recievedValue changed after checking the while
				 * condition  */
				if((g_recievedFlag ==1) &&(g_recievedValue != finishedEntery )) {
					arr_storedData2[countArray2] =g_recievedValue ;
					countArray2++ ;
					g_recievedFlag=0 ;
				}
			}
			arr_storedData2[countArray2]=endOfPass ;
			if((compareTwoPasswords(arr_storedData1,arr_storedData2)) ==0) {
				storeNewPassword(arr_storedData1) ;
			}
			else {
				UART_sendByte(passwordsUnmatched) ;
			}
			break ;
		case uncheckedPassword :
			g_recievedFlag=0 ;
			g_recievedValue=0;
			while(g_recievedValue != finishedEntery ) {
				if((g_recievedFlag ==1) &&(g_recievedValue != finishedEntery )) {
					arr_storedData3[countArray3] =g_recievedValue ;
					countArray3++ ;
					g_recievedFlag=0 ;
				}
			}
			arr_storedData3[countArray3]=endOfPass ;
			checkPassword(arr_storedData3) ;
			break ;
		case openDoor :
			g_recievedValue=0;
			rotateMotorCW() ;
			break ;
		default:
			break;
		}

	}
}
void storeNewPassword (uint8* a_storedPassword_Ptr) {
	uint8 index=0 ;
	/* address of the eeprom */
	uint16 add=0x0302;
	while(a_storedPassword_Ptr[index] != endOfPass) {
		/* Write in the external EEPROM */
		EEPROM_writeByte(add, a_storedPassword_Ptr[index]);
		/* delay time as the eeprom take 10ms to make a write*/
		_delay_ms(10) ;
		add++ ;
		index++ ;
	}
	EEPROM_writeByte(add, endOfPass);
	UART_sendByte(returnMainOptions) ;
}
void checkPassword (uint8* a_checkPassword_Ptr) {
	uint8 index=0 ;
	uint16 add=0x0302 ;
	uint8 c[12] ;
	do{
		/* Read the pass from external EEPROM */
		EEPROM_readByte(add, c+index);
		/* delay time as the eeprom take 10ms to make a write*/
		_delay_ms(10) ;
		add++ ;
		index++;
	}
	while(c[index-1] != endOfPass) ;
	if(compareTwoPasswords(a_checkPassword_Ptr,c) == 0) {
		g_trialNumber=0 ;
		UART_sendByte(rightPassword) ;
	}
	else{
		g_trialNumber++ ;
		if (g_trialNumber ==3){
			UART_sendByte(errorScreen) ;
			Timer_setCallBack(buzzerOff);
			SET_BIT(PORTD,PD2) ; //OPEN BUZZER
			Timer_ConfigType Timer_Config = {F_CPU_1024,COMPARE,ENABLE,0,65000};
			Timer_init(&Timer_Config);
			Timer_setCallBack(buzzerOff);

		}
		else{
			UART_sendByte(wrogPassword) ;
		}}
}
void rotateMotorCW (void) {
	UART_sendByte(doorOpening) ;
	Timer_setCallBack(holdMotor);
	Timer_ConfigType Timer_Config = {F_CPU_1024,COMPARE,ENABLE,0,65000};
	SET_BIT(PORTB,PB0) ;
	CLEAR_BIT(PORTB,PB1) ;
	Timer_init(&Timer_Config);

}
void holdMotor (void) {
	Timer_setCallBack(rotateMotorCCW);
	Timer_ConfigType Timer_Config = {F_CPU_1024,COMPARE,ENABLE,0,20000};
	CLEAR_BIT(PORTB,PB0) ;
	CLEAR_BIT(PORTB,PB1) ;
	Timer_init(&Timer_Config);

}
void rotateMotorCCW (void) {
	UART_sendByte(doorClosing) ;
	Timer_setCallBack(stopMotor);
	Timer_ConfigType Timer_Config = {F_CPU_1024,COMPARE,ENABLE,0,65000};
	CLEAR_BIT(PORTB,PB0) ;
	SET_BIT(PORTB,PB1) ;
	Timer_init(&Timer_Config);
}
void stopMotor(void) {
	Timer_DeInit() ;
	CLEAR_BIT(PORTB,PB0) ;
	CLEAR_BIT(PORTB,PB1) ;
	UART_sendByte(returnMainOptions) ;
}
void buzzerOff(void) {
	CLEAR_BIT(PORTD,PD2) ;
	UART_sendByte(returnMainOptions) ;

}
uint8 compareTwoPasswords(uint8* a_passwordOne_Ptr, uint8* a_passwordTwo_Ptr) {
	uint8 numberOfDifference = 0;
	uint8 index=0;
	while(a_passwordOne_Ptr[index] != endOfPass)  {
		if (a_passwordOne_Ptr[index] == a_passwordTwo_Ptr[index]) {
		}
		else{
			numberOfDifference ++;
		}
		index++;
	}
	/*this statment check if they have the same length or not
	 * because if it wasn't there and the first password is 123
	 * and the seconend is 1234 the compare function would
	 * think they are identical while they are not*/
	if(a_passwordTwo_Ptr[index] == endOfPass) {

	}
	else{
		numberOfDifference ++;
	}
	return numberOfDifference ;
}
