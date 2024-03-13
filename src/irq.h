/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (tempertature code)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart-rate code)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : irq.h
 *
 */

#ifndef __SRC_INTERRUPT_HANDLER_H_
#define __SRC_INTERRUPT_HANDLER_H_

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>


/*
 * Function to return actual time since the controller start-up
 * Parameters : none
 * Returns    : uint32_t => Delay since the MCU is UP
 *
 */
uint32_t letimerMilliseconds();

#endif // __SRC_INTERRUPT_HANDLER_H_ //
