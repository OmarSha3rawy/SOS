/*
 * TIMER_Pbcfg.c
 *
 *  Created on: Oct 13, 2019
 *      Author: Mohammed Nassar
 */


#include "TIMER_Pbcfg.h"



//TIMER_cfg_t TIMER = {TIMER_ID,WGM_mode,COM_mode,ticks,Prescaler,interrupt};

const TIMER_cfg_t TIMER = {TIMER0,CTC_MODE,NORMAL,250,PRESCALER8,INTERRUPT} ;

