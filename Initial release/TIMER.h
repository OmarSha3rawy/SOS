/*
 * TIMER.h
 *
 *  Created on: Oct 13, 2019
 *      Author: Mohammed Nassar
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "TIMER_cfg.h"

typedef struct
{
	uint8 TIMER_ID;
	uint8 WGM_mode;
	uint8 COM_mode;
	uint16 ticks ;
	uint8 prescaler;
	uint8 interrupt;
} TIMER_cfg_t ;

#include "TIMER_Pbcfg.h"

typedef const TIMER_cfg_t* TIMER_cfgPtr_t ;
typedef TIMER_cfg_t* APP_TIMER_cfgPtr_t ;

typedef uint8 TIMER_error_t ;
#define TIMER_OK            0
#define TIMER_INVALID_CFG   1
#define TIMER_INVALID_ARG   2

extern TIMER_error_t TIMER_init(TIMER_cfgPtr_t, void_func_ptr);
extern TIMER_error_t TIMER_start(TIMER_cfgPtr_t);
extern TIMER_error_t TIMER_stop(TIMER_cfgPtr_t);
extern TIMER_error_t TIMER_configure(APP_TIMER_cfgPtr_t , double systemTick );  // systemTick in Mili

#endif /* TIMER_H_ */
