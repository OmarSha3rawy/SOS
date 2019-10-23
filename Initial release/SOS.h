/*
 * SOS.h
 *
 *  Created on: Oct 23, 2019
 *      Author: Mohammed Nassar
 */

#ifndef SOS_H_
#define SOS_H_


#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"
#include "TIMER_cfg.h"


typedef enum{
	OS_OK,
	OS_INVALID_CFG,
	OS_TIMER_ERROR,
	OS_INVALID_ARG,
}EnmOSError_t;


typedef struct{
	uint8 SOS_tick;
	uint8 Timer_ID;
	uint8 prescaler;
} OS_cfg_t;

#include "SOS_Pbcfg.h"

typedef const OS_cfg_t* OS_cfgPtr_t;

extern EnmOSError_t OS_Init (OS_cfgPtr_t);
extern EnmOSError_t OS_DeInit ( void );

extern EnmOSError_t OS_Run(void);
extern EnmOSError_t OS_Create_Task(void_func_ptr app, uint8 piriority, uint8 periodicity);
extern EnmOSError_t OS_Delete_Task();



#endif /* SOS_H_ */
