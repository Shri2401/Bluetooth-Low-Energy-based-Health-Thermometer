/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (tempertature code)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart-rate code)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : i2c.h
 *
 *
 */

#ifndef __SRC_I2C_H_
#define __SRC_I2C_H_

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "sl_i2cspm.h"

/*
 * Function to initialize I2C transactions
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void I2C_initialize();

/*
 * Function to start the temperature measurement
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void startTempMeasurement();

/*
 * Function to read the temperature value from the I2C bus
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void readTemperature();

/*
 * Function to convert temperature to degrees
 * Parameters : none
 * Returns    : none  => Function returns nothing
 *
 *
 */
void convertTemperature();

void measureTemperature();

void measureHeartRate();

#endif /* __SRC_I2C_H_ */
