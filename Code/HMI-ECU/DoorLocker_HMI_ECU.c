/*

 * File Name: controlECU.c
 *
 * Created on: Dec 14, 2020
 *
 * Description: Source file for the human interface ECU that take
 * input from user using keypad and display data on screen
 * while communicating with the control ECU in a locker system
 *
 * Author: AYA ESSAM
 */
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "app_uart_messeges.h"
/************************************************************************
 *                           Global variables                           *
 ************************************************************************/
/* holds the UDR of UART whenever there's data in it */
volatile uint8 g_recievedValue=0;
/*Flag that is set to one whenever there's data in UDR */
volatile uint8 g_recievedFlag=0 ;
/*this variable holds the value of the number of entering password
 * weather it is the first entry or the seconend one (re -entering) */
uint8 g_enteryCount = 0;
/*this is a flag that help the function askForPassword works until
 * the = sign is written */
uint8 g_passwordEntery=0 ;
/*holds the value of pressed key in the keypad */
uint8 g_key;
/*holds the value of pressed key - or * in the keypad to take action*/
uint8 g_keyOption;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * [function Name] : App_askForPassword
 * [Description] : A function to ask the user about the previously-made
 * password
 */
void App_askForPassword(void) ;
/*
 * [function Name] : App_mainOptions
 * [Description] : A function to display the main options to user
 */
void App_mainOptions (void);
/*
 * [function Name] : App_newPassword
 * [Description] : A function to display the steps that is made to create
 * a new password
 */
void App_newPassword (void) ;
int main(void)
{
	/* Enable interrupts by setting I-bit */
	SREG  |= (1<<7);
	/* initialize UART */
	UART_ConfigType UART_Config ={EIGHT,ONE,DISABLE_PARITY,9600};
	UART_init(&UART_Config);
	/* initialize LCD */
	LCD_init();
	/* checking weather it's first time opening the device or there're
	 * Previously made password*/
	if(EEPROM_read(0x00F0) != notFirstTime ) {
		/* this code is executed only at first time*/
		/*set new password */
		App_newPassword() ;
		EEPROM_write(0x00F0, notFirstTime) ;
	}
	else{
		App_mainOptions() ;
	}

	while(1)
	{
		g_enteryCount=0 ;
		g_passwordEntery =0 ;
		/* The program begins by switching the received data
		 * in the UDR to take action according to it */
		switch(g_recievedValue) {
		case passwordsUnmatched :
			/* must clear the g_recievedValue after every
			 * switch case executed so it will not execute again */
			g_recievedValue=0;
			LCD_clearScreen();
			LCD_displayString("Unmatched");
			_delay_ms(1000) ;
			App_newPassword() ;
			break ;
		case wrogPassword :
			g_recievedValue=0;
			LCD_clearScreen();
			LCD_displayString("Wrong");
			_delay_ms(1000) ;
			App_askForPassword() ;
			break ;
		case errorScreen :
			g_recievedValue=0;
			LCD_clearScreen();
			LCD_displayString("ERROR!!!!!");
			break ;
		case doorOpening :
			g_recievedValue=0;
			LCD_clearScreen();
			LCD_displayString("DOOR IS OPENING");
			break ;
		case doorClosing :
			g_recievedValue=0;
			LCD_clearScreen();
			LCD_displayString("DOOR IS Closing");
			break ;
		case returnMainOptions :
			g_recievedValue=0;
			App_mainOptions() ;

			break ;
		case rightPassword :
			g_recievedValue=0;
			if(g_keyOption== '-'){
				/*Tell the other MC2 to open the door*/
				UART_sendByte(openDoor) ;
			}
			else if(g_keyOption=='*'){
				App_newPassword() ;
			}
			break;
		default:
			break;
		}
	}

}
void App_askForPassword(void) {
	LCD_clearScreen();
	LCD_displayString("Enter Password:");
	LCD_goToRowColumn(1,0);
	/*initiate communication with MC2 to save the entered password and
	 * check if it's right or not */
	UART_sendByte(uncheckedPassword) ;
	while(g_passwordEntery ==0) {
		/* get the pressed key number */
		g_key = KeyPad_getPressedKey();
		if ((g_key >=0 && g_key <=9 ))
		{
			UART_sendByte(g_key) ;
			/* display the pressed keypad switch as * */
			LCD_displayString("*");
		}

		else if(g_key == '=')
		{
			/*Tell the other MC that u finished entering
			 * password and the MC2 have to check if it's right
			 * */
			UART_sendByte(finishedEntery) ;
			/*tells MC1 that i finished writing password and
			 * should terminate the while loop */
			g_passwordEntery++;
		}
		/* Press time */
		/* if any switch pressed for more than 1000 ms it
		 * counts more than one press */
		_delay_ms(1000);
	}

}
void App_mainOptions (void){
	LCD_clearScreen();
	LCD_displayString("x :change password");
	LCD_displayStringRowColumn(1,0,"- :open door") ;
	/* get the pressed key number */
	g_key = KeyPad_getPressedKey();
	if (g_key =='*')
	{
		App_askForPassword() ;
		g_keyOption='*' ;
	}
	else if(g_key == '-')
	{
		App_askForPassword() ;
		g_keyOption='-' ;
	}
	 /* Press time */
	_delay_ms(1000);
}
void App_newPassword (void) {
	LCD_clearScreen();
	LCD_displayString("Enter Password:");
	LCD_goToRowColumn(1,0);
	/*initiate communication with MC2 to save the first entry of
	 * the new password */
	UART_sendByte(newPasswordFirst) ;
	while(g_enteryCount ==0) {
		g_key = KeyPad_getPressedKey(); /* get the pressed key number */
		if ((g_key >=0 && g_key <=9 ))
		{
			UART_sendByte(g_key) ;
			LCD_displayString("*"); /* display the pressed keypad switch as * */
		}

		else if(g_key == '=')
		{
			/*Tell the other MC that u finished entry*/
			UART_sendByte(finishedEntery) ;
			/*tells MC1 that i finished first entry */
			g_enteryCount++;
		}
		_delay_ms(1000);
	}
	LCD_clearScreen();
	LCD_displayString("Re-Enter Password:");
	LCD_goToRowColumn(1,0);
	/*initiate communication with MC2 to save the second enter of
	 * the new password */
	UART_sendByte(newPasswordSecoend) ;
	while (g_enteryCount ==1) {

		g_key = KeyPad_getPressedKey();
		if ((g_key >=0 && g_key <=9 ))
		{
			UART_sendByte(g_key) ;
			/* display the pressed keypad switch as * */
			LCD_displayString("*");
		}

		else if(g_key == '=')
		{/*Tell the other MC that u finished entry*/
			UART_sendByte(finishedEntery) ;
			/*tells MC1 that i finished second entry */
			g_enteryCount++;
		}
		_delay_ms(1000); /* Press time */
	}
}
