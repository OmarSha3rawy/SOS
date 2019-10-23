/*
 * SOS.c
 *
 *  Created on: Oct 23, 2019
 *      Author: Mohammed Nassar
 */

#include "TIMER.h"
#include "SOS_cfg.h"
#include "SOS.h"


typedef enum{
	RUNNING,
	READY,
	WAITING,
	DELETED,
}ST_SOS_TaskState;


typedef struct {
	void_func_ptr task_ptr;
	ST_SOS_TaskState state;
	uint8 periority;
}ST_OS_Task;
   
// **** OS PRIVATE VARIABLES ***//
static ST_OS_Task OS_TasksBuffer[MAX_PERIOD][MAX_NUMBER_OF_TASKS];
static volatile uint32 system_Tick = 0;
static uint32 counter[MAX_PERIOD] = {0};
static TIMER_cfg_t APP_TIMER=  {TIMER0,NORMAL_MODE,NORMAL,0x00,PRESCALER256,INTERRUPT} ;


// **** OS PRIVATE FUNCTIONS ***//

static EnmOSError_t Scheduler(void);
static void SystemSleep(void);
static EnmOSError_t OS_Search_Task(void_func_ptr app,uint32* Periodicity_index, uint8* Priority_index);
static EnmOSError_t OS_shift_Tasks(uint32 Periodicity_index,uint8 Priority_index);
static void ISR_CALLBACK(void);



EnmOSError_t OS_Init (OS_cfgPtr_t SOS ) /* TODO : system tick needs to be configurable */
{
	EnmOSError_t retval = OS_OK;
	TIMER_error_t TIMER_retval; 
	APP_TIMER.TIMER_ID = SOS->Timer_ID;
	APP_TIMER.prescaler = SOS->prescaler;
	TIMER_retval = TIMER_configure(&APP_TIMER, SOS->SOS_tick);
	if(TIMER_retval)
	{
		retval = OS_INVALID_CFG;
	}
	else
	{
		TIMER_init(&APP_TIMER, ISR_CALLBACK);
		TIMER_start(&APP_TIMER);
	}

	return retval;
}

EnmOSError_t OS_DeInit (void)
{
	EnmOSError_t retval = OS_OK; /* TODO : add errors checking */
	/* TODO : set the prescaler to be 0 to stop the timer */
	TIMER_stop(&APP_TIMER);

	return retval;
}



EnmOSError_t OS_Create_Task(void_func_ptr app, uint8 piriority, uint8 periodicity)
{
	uint32 periodicity_index, j;
	sint32 k;
	EnmOSError_t retval = OS_OK;
	for(periodicity_index = 0; periodicity_index < MAX_PERIOD; periodicity_index++)
	{
		if( periodicity_index % periodicity ==0 /*&& (periodicity_index != 0 || periodicity == 1 )*/ )
		{
			for(j = 0; j < counter[periodicity_index]; j++)
			{
				if( OS_TasksBuffer[periodicity_index][j].periority < piriority)
				{
					break;
				}
			}
			for(k=counter[periodicity_index]; k>j; k--)
			{
				OS_TasksBuffer[periodicity_index][k].task_ptr = OS_TasksBuffer[periodicity_index][k-1].task_ptr;
				OS_TasksBuffer[periodicity_index][k].state = OS_TasksBuffer[periodicity_index][k-1].state;
				OS_TasksBuffer[periodicity_index][k].periority = OS_TasksBuffer[periodicity_index][k-1].periority;

			}
					
			OS_TasksBuffer[periodicity_index][j].task_ptr = app;
			OS_TasksBuffer[periodicity_index][j].state = READY;
			OS_TasksBuffer[periodicity_index][j].periority = piriority;
			counter[periodicity_index]++;
		}
	}


	return retval;
}


EnmOSError_t OS_Run(void)
{

	EnmOSError_t retval = OS_OK;
	while(1)
	{

		Scheduler();

		//   SystemSleep();  // SYSTEM SLEEP   NOTE THAT THE SYSTEM WAKE UP WITH TIMER 2 ONLY

	}
	return retval;
}



EnmOSError_t Scheduler(void)
{
	EnmOSError_t retval = OS_OK;

	for(uint8 Priority = 0 ; Priority < counter[ system_Tick ] ; Priority++  )
	{
		DDRC = 0xff;
		PORTC = system_Tick;
		if ( OS_TasksBuffer[ system_Tick ][ Priority ].state == READY )
		{
			// OS_TasksBuffer[ system_Tick ][ Priority ].state = RUNNING ;
			(OS_TasksBuffer[ system_Tick ][ Priority ].task_ptr)();
			DDRD = 0xff;
			PORTD = Priority;
		}
	}

	return retval;
}



EnmOSError_t OS_Delete_Task(void_func_ptr app)
{
	EnmOSError_t retval = OS_OK;
	uint32 Periodicity_index=0;
	uint8 Priority_index=0;
	uint32 Periodicity_found_index=0;
	retval = OS_Search_Task( app , & Periodicity_index, & Priority_index);
	Periodicity_found_index = Periodicity_index ;
	for (; Periodicity_index < MAX_NUMBER_OF_TASKS ; Periodicity_index += Periodicity_found_index )
	{
		retval = OS_shift_Tasks(Periodicity_index,Priority_index);
	}

	return retval;
}

EnmOSError_t OS_shift_Tasks(uint32 Periodicity_index,uint8 Priority_index)
{
	EnmOSError_t retval = OS_OK;

	for(uint8 Priority = Priority_index ; Priority < ( counter[ system_Tick ]-1 ) ; Priority++  )
	{
		OS_TasksBuffer[ Periodicity_index ][ Priority ].task_ptr = OS_TasksBuffer[ Periodicity_index ][ Priority+1 ].task_ptr;
		OS_TasksBuffer[ Periodicity_index ][ Priority ].state =  OS_TasksBuffer[ Periodicity_index ][ Priority+1 ].state ;
		OS_TasksBuffer[ Periodicity_index ][ Priority ].periority =  OS_TasksBuffer[ Periodicity_index ][ Priority+1 ].periority ;
	}

	return retval;
}


EnmOSError_t OS_Search_Task(void_func_ptr app,uint32* Periodicity_index, uint8* Priority_index)
{
	EnmOSError_t retval = OS_OK;

	for(uint32 Periodicity = * Periodicity_index ; Periodicity < MAX_NUMBER_OF_TASKS ; Periodicity++  )
	{
		for(uint8 Priority = 0 ; Priority < counter[ system_Tick ] ; Priority++  )
		{

			if( (OS_TasksBuffer[ Periodicity ][ Priority ].task_ptr) == app )
			{
				* Periodicity_index = Periodicity ;
				*Priority_index = Priority ;
				retval = OS_OK;
				Priority = counter[ system_Tick ] + 1 ;
				Periodicity = MAX_NUMBER_OF_TASKS + 1 ;
			}
			else
			{
				retval = OS_INVALID_ARG;
			}

		}
	}

	return retval;
}


void SystemSleep(void)
{
	MCUCR |= (1<<SE);                                  // SLEEP ENABLE   NOTE THAT THE SYSTEM WAKE UP WITH TIMER 2 ONLY
	MCUCR &=  ~( (1<<SM2) | (1<<SM1) | (1<<SM0) );     // SLEEP MODE IDLE
}


void ISR_CALLBACK(void)
{
	if( system_Tick == MAX_PERIOD )
	{
		system_Tick = 0 ;
	}
	system_Tick++;
	PORTB ^= 1<<6;
}
