/*
 * SOS_testCases.c
 *
 *  Created on: Oct 23, 2019
 *      Author: MOhammed Nassar
 */

#include "micro_config.h"
//#include "LCD.h" // FOR  TESTING
#include "SOS.h"


void app1(void);
void app2(void);
void app3(void);


int main()
{
	DDRB = 0xff;
	
	OS_Init(&SOS);
	OS_Create_Task(app1, 1, 1);
	OS_Create_Task(app2, 2, 2);
	//OS_Create_Task(app3, 3, 3);
	//PORTA |= 1;
	
	OS_Run();

	return 0;
}


void app1(void)
{
	PORTB ^= 1<<1;
}

void app2(void)
{
	PORTB ^= 1<<2;
}

void app3(void)
{
	PORTB ^= 1<<3;
}