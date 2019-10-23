/*
 * TIMER.c
 *
 *  Created on: Oct 13, 2019
 *      Author: Mohammed Nassar
 */
#include "lcd.h" // FOR TESTING
#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"
#include "TIMER.h"


// **** TIMER PRIVATE VARIABLES ***//
static void_func_ptr g_TIMER_consumer = NULL;
static uint16 Pre_loadedValue = 0 ;
static uint16 prescaler_Values[]={1,8,64,256,1024,0x00,0x00,32,128};

// **** TIMER PRIVATE FUNCTIONS ***//
static TIMER_error_t TIMER_interrupt(TIMER_cfgPtr_t);


//******************************** INTERRUPT TIMERS HANDELER *****************************************//
//				Timer0 COMP Interrupt

ISR(TIMER0_COMP_vect)
{

	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}
}

 //				Timer0 OVF Interrupt

ISR(TIMER0_OVF_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}
}


 //				Timer1 OVF Interrupt

ISR(TIMER1_OVF_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}

}


 //				Timer1 COMPB Interrupt

ISR(TIMER1_COMPB_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}

}


 //					Timer1 COMPA Interrupt

ISR(TIMER1_COMPA_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}

}


 //					Timer2 OVF Interrupt

ISR(TIMER2_OVF_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}
}

//					Timer2 CMP Interrupt

ISR(TIMER2_COMP_vect)
{
	if(g_TIMER_consumer != NULL)
	{
		g_TIMER_consumer();
	}
}

//TIMER_cfg_t TIMER = {TIMER_ID,WGM_mode,COM_mode,ticks,Prescaler,interrupt};
//TIMER_cfg_t TIMER = {TIMER0,NORMAL_MODE,NORMAL,PRESCALER0,INTERRUPT};  LCD_displayCharacter('8');// FOR TESTING

TIMER_error_t TIMER_configure(APP_TIMER_cfgPtr_t APP_TIMER , double systemTick )
{
	TIMER_error_t retval = TIMER_OK ;

	uint16 ticks=0;

	ticks = ( ( (double)systemTick MILI ) * ( (double)( F_CPU/  prescaler_Values[ APP_TIMER->prescaler ] ) ) );

	switch(APP_TIMER->TIMER_ID)
	{
	case TIMER0:
		if(ticks > 255)
		{
			retval = TIMER_INVALID_CFG ;
		}
		else
		{
          (APP_TIMER->ticks) = ticks;
		}

		break ;

	case TIMER2:
		if(ticks>255)
		{
			retval = TIMER_INVALID_CFG ;
		}
		else
		{
			 (APP_TIMER->ticks) = ticks;
		}

		break ;

	case TIMER1:
		if(ticks>65535)
		{
			retval = TIMER_INVALID_CFG ;
		}
		else
		{
		     (APP_TIMER->ticks) = ticks;
		}

		break ;
	default: retval = TIMER_INVALID_CFG ;
	break;

	}
	/*********** END OF TIMER_configure  ***********/
	return retval;
}


TIMER_error_t TIMER_init(TIMER_cfgPtr_t TIMER, void_func_ptr TIMER_consumer )
{

	TIMER_error_t retval = TIMER_OK ;

	if (TIMER->interrupt == INTERRUPT )
	{
		g_TIMER_consumer = TIMER_consumer ;
	}

		switch(TIMER->WGM_mode)
		{   /********************  WGM mode    ********************/

		case NORMAL_MODE:
		{

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:   /*********** TIMER0 NORMAL_MODE *********/
				Pre_loadedValue = ( 255 - (TIMER->ticks) );
				TCNT0 = (uint8) Pre_loadedValue ; //timer initial value
				TCCR0 &=  ~ ( (1u<<WGM01) | (1u<<WGM00) );	// NORMAL_MODE WGM01=0 & WGM00=0
				TCCR0 = (1<<FOC0); //Non PWM mode
				//  END OF TIMER0 NORMAL_MODE

			break;

			case TIMER2: /*********** TIMER2 NORMAL_MODE *********/
				Pre_loadedValue = ( 255 - (TIMER->ticks) );
				TCNT2 = (uint8) Pre_loadedValue ; //timer initial value
				TCCR2 &=  ~ ( (1u<<WGM21) | (1u<<WGM20) );	// NORMAL_MODE WGM21=0 & WGM20=0
				TCCR2 = (1<<FOC2); //Non PWM mode
				// END OF TIMER2 NORMAL_MODE

			break;

			case TIMER1:/*********** TIMER1 NORMAL_MODE *********/
				Pre_loadedValue = ( (uint16) 65535 - (TIMER->ticks) );
				TCNT1= Pre_loadedValue;
				TCCR1A &= (~(1<<WGM10) & ~(1<<WGM11));
				TCCR1B &= (~(1<<WGM13) & ~(1<<WGM12));
				// END OF TIMER1 NORMAL_MODE

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}
		}
		// END OF NORMAL_MODE
		break;


		case CTC_MODE:
		{

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 CTC MODE *********/

				TCNT0 = 0; //timer initial value
				TCCR0 |= (1u<<WGM01) ; // CTC WGM01=1
				TCCR0 &= ( ~ (1u<<WGM00) );  // CTC  WGM00=0
				TCCR0 = (1<<FOC0); //Non PWM mode
				OCR0 = (uint8) (TIMER->ticks) ;
				// END OF TIMER0 CTC_MODE

			break;

			case TIMER2:/*********** TIMER2 CTC MODE *********/

				TCNT2 = 0; //timer initial value
				TCCR2 |= (1u<<WGM21) ; // CTC WGM21=1
				TCCR2 &= ( ~ (1u<<WGM20) );  // CTC  WGM20=0
				TCCR2 = (1<<FOC2); //Non PWM mode
				OCR2 = (uint8) (TIMER->ticks) ;
				// END OF TIMER2 CTC_MODE

			break;

			case TIMER1:/*********** TIMER1 CTC MODE *********/

				TCNT1H=0;
				TCNT1L=0;  //TCNT1=0;
				TCCR1A &= (~(1<<WGM10)) & (~(1<<WGM11));
				TCCR1B |= (1<<WGM12);
				TCCR1B &= (~(1<<WGM13));
				OCR1A = (uint16)(TIMER->ticks);
				// END OF TIMER1 CTC_MODE

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}

		}
		// END OF CTC_MODE
		break;

		default: retval = TIMER_INVALID_CFG ;
		break;

		/******************** END OF SWITCH WGM mode    ********************/
		}

		/////****************************************************************************************************************////

		switch(TIMER->COM_mode)
		{   /********************  COM mode    ********************/

		case NORMAL:
		{  /***********   NORMAL  *********/

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:   /***********   TIMER0 NORMAL    *********/

				TCCR0 &= ~((1u << COM01) | (1u << COM00));// NORMAL COM01=0 & COM00=0
				//  END OF TIMER0 NORMAL

			break;

			case TIMER2: /***********   TIMER2 NORMAL  *********/

				TCCR2 &=  ~ ( (1u<<COM21) | (1u<<COM20) );	// NORMAL COM21=0 & COM20=0
				// END OF TIMER2 NORMAL

			break;

			case TIMER1:/*********** TIMER1 NORMAL  *********/

				TCCR1A &= ~ ( (1<<COM1A1) | (1<<COM1A0) );
				// END OF TIMER1 NORMAL

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}
		}
		// END OF NORMAL
		break;


		case TOGGLE:
		{ /*********** TOGGLE *********/

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 TOGGLE *********/

				TCCR0 &= (~(1u << COM01)); // TOGGLE COM01=0
				TCCR0 |= (1u << COM00);  // TOGGLE  COM00=1
				// END OF TIMER0 TOGGLE

			break;

			case TIMER2:/*********** TIMER2 TOGGLE *********/

				TCCR2 &= ( ~(1u<<COM21) ); // TOGGLE COM21=0
				TCCR2 |=  (1u<<COM20);  // TOGGLE  COM20=1
				// END OF TIMER2 TOGGLE

			break;

			case TIMER1:/*********** TIMER1 TOGGLE *********/

				TCCR1A &= (~(1<<COM1A1));
				TCCR1A |= (1<<COM1A0);
				// END OF TIMER1 TOGGLE

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}

		}
		// END OF TOGGLE
		break;


		case CLEAR:
		{ /*********** CLEAR *********/

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 CLEAR *********/

				TCCR0 |= (1u << COM01); // CLEAR COM01=1
				TCCR0 &= (~(1u << COM00));  // CLEAR  COM00=0
				// END OF TIMER0 CLEAR

			break;

			case TIMER2:/*********** TIMER2 CLEAR *********/

				TCCR2 |= (1u<<COM21) ; // CLEAR COM21=1
				TCCR2 &= ( ~ (1u<<COM20) );  // CLEAR  COM20=0
				// END OF TIMER2 CLEAR

			break;

			case TIMER1:/*********** TIMER1 CLEAR *********/

				TCCR1A |= (1<<COM1A0);
				TCCR1A &= (~(1<<COM1A1));
				// END OF TIMER1 CLEAR

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}

		}
		// END OF CLEAR
		break;

		case SET:
		{ /*********** SET *********/

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 SET *********/

				TCCR0 |= ((1u << COM01) | (1u << COM00)); // SET COM01=1 & COM00=1
				// END OF TIMER0 SET

			break;

			case TIMER2:/*********** TIMER2 SET *********/

				TCCR2 |= ( (1u<<COM21) | (1u<<COM20) ) ; // SET COM21=1 & COM20=1
				// END OF TIMER2 SET

			break;

			case TIMER1:/*********** TIMER1 SET *********/

				TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
				// END OF TIMER1 SET

			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}

		}
		// END OF SET
		break;


		default: retval = TIMER_INVALID_CFG ;
		break;

		/******************** END OF SWITCH COM mode    ********************/
		}


	/*********** END OF TIMER_init  ***********/
		return retval;
}



/////****************************************************************************************************************////
/////****************************************************************************************************************////
/////****************************************************************************************************************////


TIMER_error_t TIMER_interrupt(TIMER_cfgPtr_t TIMER){

	TIMER_error_t retval = TIMER_OK ;

		switch(TIMER->interrupt)
		{   /********************  INTERRUPT  ********************/

		case INTERRUPT: 		sei();
		{  /***********   INTERRUPT  *********/

			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 INTERRUPT *********/
			{
				switch(TIMER->WGM_mode)
				{   //WGM mode
				case NORMAL_MODE: TIMSK |= (1u<<TOIE0); //Overflow Interrupt Enable
				break;
				case CTC_MODE: 	TIMSK |= (1u<<OCIE0); // Output Compare Match Interrupt Enable
				break;
				default: retval = TIMER_INVALID_CFG ;
				break;
				}
				// END OF TIMER0 INTERRUPT
			}
			break;

			case TIMER2:/*********** TIMER2 INTERRUPT *********/
			{
				switch(TIMER->WGM_mode)
				{   //WGM mode
				case NORMAL_MODE: TIMSK |= (1u<<TOIE2); //Overflow Interrupt Enable
				break;
				case CTC_MODE:	  TIMSK |= (1u<<OCIE2); // Output Compare Match Interrupt Enable
				break;
				default: retval = TIMER_INVALID_CFG ;
				break;
				}

				// END OF TIMER2 INTERRUPT
			}
			break;

			case TIMER1:/*********** TIMER1 INTERRUPT *********/
			{
				switch(TIMER->WGM_mode)
				{   //WGM mode
				case NORMAL_MODE: TIMSK|= (1<<TOIE1);     //Overflow Interrupt Enable
				break;
				case CTC_MODE:  TIMSK |= (1<<OCIE1A); 	//Output Compare Interrupt Enable  1A.
				//TIMSK|= (1<<OCIE1B)					//Output Compare Interrupt Enable  1B.
				break;
				default: retval = TIMER_INVALID_CFG ;
				break;
				}
				// END OF TIMER1 INTERRUPT
			}
			break;

			default: retval = TIMER_INVALID_CFG ;
			break;

			// END OF SWITCH TIMER ID
			}
		}
		// END OF INTERRUPT
		break;

		case POOLING:
		{  /***********   POOLING *********/
			switch(TIMER->TIMER_ID)
			{

			case TIMER0:/*********** TIMER0 POOLING *********/
			{
				TIMSK &= (~ ( (1u<<OCIE0) |(1u<<TOIE0) ) );  //Overflow Interrupt & Output Compare Match Interrupt disabled
				// END OF TIMER0 POOLING
			}
			break;

			case TIMER2:/*********** TIMER2 POOLING *********/
			{
				TIMSK &= (~ ( (1u<<OCIE2) |(1u<<TOIE2) ) );  //Overflow Interrupt & Output Compare Match Interrupt disabled
				// END OF TIMER2 POOLING
			}
			break;

			case TIMER1:/*********** TIMER1 POOLING *********/
			{
				TIMSK &= (~  ( (1<<TOIE1) | (1<<OCIE1A) | (1<<OCIE1B) ) ); // Output Compare A Match Interrupt Enable & Output Compare B Match Interrupt Enable & Overflow Interrupt Enable disabled

				// END OF TIMER1 POOLING
			}
			break;


			default: retval = TIMER_INVALID_CFG ;
			break;
			// END OF SWITCH TIMER ID
			}
		}
		// END OF POOLING
		break;


		default: retval = TIMER_INVALID_CFG ;
		break;

		/******************** END OF SWITCH INTERRUPT    ********************/
		}


	/*********** END OF TIMER_INTERRUPT  ***********/
		return retval;
}




/////****************************************************************************************************************////
/////****************************************************************************************************************////
/////****************************************************************************************************************////

TIMER_error_t TIMER_start(TIMER_cfgPtr_t TIMER)
{
	TIMER_error_t retval = TIMER_OK ;

	TIMER_interrupt(TIMER);                             // SET The interrupt

	switch(TIMER->TIMER_ID)
	{

	case TIMER0:   /*********** TIMER0  *********/
	{
		switch(TIMER->prescaler)
		{
		case PRESCALER0: TCCR0 |= 0b00000001;
		break;
		case PRESCALER8: TCCR0 |= 0b00000010;
		break;
		case PRESCALER64: TCCR0  |= 0b00000011;
		break;
		case PRESCALER256: TCCR0  |= 0b00000100;
		break;
		case PRESCALER1024: TCCR0  |= 0b00000101;
		break;
		case EXTERNAL_CLK_FALLING : TCCR0  |= 0b00000110;
		break;
		case EXTERNAL_CLK_RISING: TCCR0 |= 0b00000111;
		break;
		default: retval = TIMER_INVALID_CFG ;
		break;
		}
		//  END OF TIMER0
	}
	break;

	case TIMER2: /*********** TIMER2  *********/
	{
		switch(TIMER->prescaler)
		{
		case PRESCALER0: TCCR2 |= 0b00000001;
		break;
		case PRESCALER8: TCCR2 |= 0b00000010;
		break;
		case PRESCALER32: TCCR2  |= 0b00000011;
		break;
		case PRESCALER64: TCCR2  |= 0b00000100;
		break;
		case PRESCALER128: TCCR2  |= 0b00000101;
		break;
		case PRESCALER256: TCCR2  |= 0b00000110;
		break;
		case PRESCALER1024: TCCR2 |= 0b00000111;
		break;
		default: retval = TIMER_INVALID_CFG ;
		break;
		}
		// END OF TIMER2
	}
	break;

	case TIMER1:/*********** TIMER1  *********/
	{
		switch(TIMER->prescaler)
		{
		case PRESCALER0: TCCR1B |= 0b00000001;
		break;
		case PRESCALER8: TCCR1B |= 0b00000010;
		break;
		case PRESCALER64: TCCR1B |= 0b00000011;
		break;
		case PRESCALER256: TCCR1B |= 0b00000100;
		break;
		case PRESCALER1024: TCCR1B |= 0b00000101;
		break;
		case EXTERNAL_CLK_FALLING : TCCR1B |= 0b00000110;
		break;
		case EXTERNAL_CLK_RISING: TCCR1B |= 0b00000111;
		break;
		default: retval = TIMER_INVALID_CFG ;
		break;
		}

		break;
		// END OF TIMER1
	}
	break;

	default: retval = TIMER_INVALID_CFG ;
	break;

	// END OF SWITCH TIMER ID
	}


	/*********** END OF TIMER_start  ***********/
	return retval;
}



/////****************************************************************************************************************////
/////****************************************************************************************************************////
/////****************************************************************************************************************////



TIMER_error_t TIMER_stop(TIMER_cfgPtr_t TIMER)
{

	TIMER_error_t retval = TIMER_OK ;

	switch(TIMER->TIMER_ID)
	{

	case TIMER0:   /*********** TIMER0  *********/
	{
		TCCR0 &= 0b11111000;
		//  END OF TIMER0
	}
	break;

	case TIMER2: /*********** TIMER2  *********/
	{
		TCCR2 &= 0b11111000;
		// END OF TIMER2
	}
	break;

	case TIMER1:/*********** TIMER1  *********/
	{
		TCCR1B &= 0b11111000;
		// END OF TIMER1
	}
	break;

	default: retval = TIMER_INVALID_CFG ;
	break;

	// END OF SWITCH TIMER ID
	}


	/*********** END OF TIMER_stop  ***********/
	return retval;
}

