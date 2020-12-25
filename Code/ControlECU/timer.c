/*
 * timer.c
 *
 *  Created on: Dec 13, 2020
 *      Author: new
 */

#include "timer.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackTimerPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
#if (Timer_number ==0)
ISR(TIM0_OVF_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}

ISR(TIM0_COMP_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}
#elif (Timer_number ==1)

ISR(TIMER1_OVF_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}
#if(Timer_channel ==0)
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}
#elif(Timer_channel ==1)
ISR(TIM1_COMPB_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}
#endif
#elif (Timer_number ==2)
ISR(TIM2_OVF_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}

ISR(TIM2_COMP_vect)
{
	if(g_callBackTimerPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackTimerPtr)();
	}
}
#endif
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Function to initiate Timer1
 */
#if (Timer_number ==0)
void Timer_init(const Timer_ConfigType *Config_Ptr) {

	/* Normal port operation, OC0 disconnected */
	TCCR0 &= 0xCF ;
	/*NON PWM MODE */
	SET_BIT(TCCR0,FOC0) ;
	/*insert the required mode (overflow or compare)
	 * in WGM00 and WGM01 bits in TCCR0 Register
	 */
	CLEAR_BIT(TCCR0,WGM00) ;
	TCCR0 = (TCCR0 & 0xF7) | ((Config_Ptr->mode)<<3);
	/* insert the required intIial value in TCNT1 register */
	TCNT0=Config_Ptr->initialValue ;

	/* Insert the mode of interrupt (enable/disable) IN the right bit */
	TIMSK = (TIMSK & ~(1<<(Config_Ptr->mode))) | ((Config_Ptr->interruptMode)<<(Config_Ptr->mode)) ;
	 /* insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR0 Register
	 */
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->clock);

	/* insert the required compare value in OCR1A register */

	OCR0=Config_Ptr->compareValue ;

}
#elif(Timer_number == 1)
#if (Timer_channel ==0)
void Timer_init(const Timer_ConfigType *Config_Ptr) {

	/* Normal port operation, OC1A disconnected */
	TCCR1A &= 0x30 ;
	/*NON PWM MODE */
	SET_BIT(TCCR1A,FOC1A) ;
	/*insert the required mode (overflow or compare)
	 * in WGM12 and WGM13 bits in TCCR1B Register
	 */
	CLEAR_BIT(TCCR1B,WGM13) ;
	TCCR1B = (TCCR1B & 0xF7) | ((Config_Ptr->mode)<<2);
	/* insert the required intIial value in TCNT1 register */
	TCNT1=Config_Ptr->initialValue ;

	/* Insert the mode of interrupt (enable/disable)*/
	TIMSK = (TIMSK & ~(1<<(2+(Config_Ptr->mode)))) | ((Config_Ptr->interruptMode)<<(2+(Config_Ptr->mode))) ;
	 /* insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->clock);

	/* insert the required compare value in OCR1A register */

	OCR1A=Config_Ptr->compareValue ;

}
#elif(Timer_channel == 1)
void Timer_init(const Timer_ConfigType *Config_Ptr) {

	/* Normal port operation, OC1B disconnected */
	TCCR1A &= 0xC0 ;
	/*NON PWM MODE */
	SET_BIT(TCCR1A,FOC1B) ;
	/*insert the required mode (overflow or compare)
	 * in WGM12 and WGM13 bits in TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xE7) | ((Config_Ptr->mode)<<3);
	/* insert the required intIial value in TCNT1 register */
	TCNT1=Config_Ptr->initialValue ;

	/* Insert the mode of interrupt (enable/disable)*/
	TIMSK = (TIMSK & ~(1<<(2+(Config_Ptr->mode)))) | ((Config_Ptr->interruptMode)<<(2+(Config_Ptr->mode))) ;
	 /* insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->clock);

	/* insert the required compare value in OCR1A register */

	OCR1B=Config_Ptr->compareValue ;

}
#endif
#elif(Timer_number == 2)
void Timer_init(const Timer_ConfigType *Config_Ptr) {

	/* Normal port operation, OC2 disconnected */
	TCCR2 &= 0xCF ;
	/*NON PWM MODE */
	SET_BIT(TCCR2,FOC2) ;
	/*insert the required mode (overflow or compare)
	 * in WGM20 and WGM21 bits in TCCR2 Register
	 */
	CLEAR_BIT(TCCR2,WGM20) ;
	TCCR2 = (TCCR2 & 0xF7) | ((Config_Ptr->mode)<<3);
	/* insert the required intIial value in TCNT2 register */
	TCNT2=Config_Ptr->initialValue ;

	/* Insert the mode of interrupt (enable/disable) IN the right bit */
	TIMSK = (TIMSK & ~(1<<((Config_Ptr->mode)+6))) | ((Config_Ptr->interruptMode)<<((Config_Ptr->mode)+6)) ;
	 /* insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR2 Register
	 */
	TCCR2 = (TCCR2 & 0xF8) | (Config_Ptr->clock);

	/* insert the required compare value in OCR1A register */

	OCR2=Config_Ptr->compareValue ;

}
#endif

/*
 * Description: Function to get the Timer1 Value when the input is captured
 *              The value stored at Input Capture Register ICR1
 */
uint16 Timer_getTimerValue(void)
{
	return TCNT1;
}

/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void Timer_clearTimerValue(void)
{
	TCNT1 = 0;
}

/*
 * Description: Function to disable the Timer1
 */
void Timer_DeInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0 ;
	OCR1B = 0 ;


}
/*
 * Description: Function to set the Call Back function address.
 */
void Timer_setCallBack( void(*aTimer_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackTimerPtr = aTimer_ptr;
}
