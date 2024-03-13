/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (temperature sensor)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart rate sensor)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : scheduler.h
 *
 */

#ifndef __SRC_SCHEDULERS_H_
#define __SRC_SCHEDULERS_H_

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>

#include "sl_bt_api.h"
#include "sl_bgapi.h"
#include "sl_component_catalog.h"
#include "sl_power_manager.h"
#include "ble_device_type.h"

/*
 * Function to schedule Underflow Interrupt
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void schedulerSetEventUF();

/*
 * Function to schedule COMP_1 Interrupt
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void schedulerSetEventComp1();

/*
 * Function to schedule I2C Interrupt
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void schedulerSetEventI2C();

void schedulerSetEventButtonPress();

void schedulerSetEventButtonRelease();

void readTemperatureValue(sl_bt_msg_t* evt);
void schedulerSetEventButtonReleasePB1();

void schedulerSetEventButtonPressPB1();

#if (DEVICE_IS_BLE_SERVER)
/*
 * Function to initiate the temperature state machine
 * Parameters : uint32_t  => Represents the current EVENT
 * Returns    : none      => Function returns nothing
 *
 *
 */
void myEventHandler(sl_bt_msg_t*);
#else

/*
 * Function to get one of the events to the main code
 * Parameters : none
 * Returns    : uint32_t  => Value corresponding to one of the interrupts
 *
 */
//uint32_t getNextEvent();
void myDiscoveryStateMachine(sl_bt_msg_t* evt);
#endif

#endif // __SRC_SCHEDULERS_H_ //
