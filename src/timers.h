/*
 *  Author      : Vignesh Vadivel
 *  email       : viva9969@colorado.edu
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : timers.h
 *
 */

#ifndef __SRC_TIMERS_H_
#define __SRC_TIMERS_H_

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>

/*
 * Function to initialize Low Energy TIMER0
 * Parameters  : void
 * Return type : void    => Function returns nothing
 *
 */
void initLETimer0(void);

/*
 * Function to wait for given duration of time in microseconds by polling in a loop
 * Parameters : uint32_t => Duration of wait time in microseconds
 * Returns    : none     => Function returns nothing
 *
 */
void timerWaitUs_polled(uint32_t);

/*
 * Function to initialize interrupt driven timer wait
 * Parameters : uint32_t => Duration of wait time in microseconds
 * Returns    : none     => Function returns nothing
 *
 */
void timerWaitUs_irq(uint32_t);

/*
 * Function to return topValue which is loaded to COMP-0 register
 * Parameters : none
 * Returns    : uint32_t => Value loaded to COMP-0 register
 *
 */
uint32_t getTopValue();

#endif  // __SRC_TIMERS_H_ //
