/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (tempertature code)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart-rate code)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : i2c.c
 *
 *
 */

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>
#include <bme280.h>
#include <ccs811.h>
#include <stdio.h>
#include <stdlib.h>
#include "sl_i2cspm_instances.h"
#include "sl_sleeptimer.h"

/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "i2c.h"
#include "app.h"
#include "ble.h"
#include "lcd.h"
#include "gpio.h"

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "em_i2c.h"

/******************************************************* MACROS *******************************************************/
#define INCLUDE_LOG_DEBUG  1
#include "src/log.h"

#define SI7021_DEVICE_ADDR 0x40
#define MSB_MASK           0xFF00
#define LSB_MASK           0x00FF

I2C_TransferSeq_TypeDef    transferSequence;
uint8_t                    cmd_data = 0xF3;
uint16_t                   read_data;

void I2C_initialize()
{
  I2CSPM_Init_TypeDef I2C_Config =
      {
          .port = I2C0,
          .sclPort = gpioPortC,
          .sclPin = 10 ,
          .sdaPort = gpioPortC,
          .sdaPin = 11,
          .portLocationScl = 14,
          .portLocationSda = 16,
          .i2cRefFreq = 0,
          .i2cMaxFreq = 92000,
          .i2cClhr = i2cClockHLRStandard
      };
  I2CSPM_Init(&I2C_Config);
}

void startTempMeasurement()
{
  I2C_TransferReturn_TypeDef transferStatus;
  I2C_initialize();                                                             // Initialize before reading the temperature //
  transferSequence.addr = SI7021_DEVICE_ADDR << 1;                              // Set the device address //
  transferSequence.flags = I2C_FLAG_WRITE;                                      // Write operation on I2C bus //
  transferSequence.buf[0].data = &(cmd_data);                                   // Pointer to the command //
  transferSequence.buf[0].len = sizeof(cmd_data);                               // Number of bytes to be written to the I2C bus //
  NVIC_EnableIRQ(I2C0_IRQn);                                                    // Enable I2C Interrupt //
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);                   // Initiate I2C Transactions //
  if(transferStatus < 0)                                                        // Check for the error conditions //
    LOG_ERROR ("I2CInit_Transfer: I2C bus write failed with the error Code = %d\n\r", transferStatus);
}

void readTemperature()
{
  I2C_TransferReturn_TypeDef transferStatus;
  transferSequence.addr = SI7021_DEVICE_ADDR << 1;                              // Set the device address //
  transferSequence.flags = I2C_FLAG_READ;                                       // Read operation on I2C bus //
  transferSequence.buf[0].data = (uint8_t*)&read_data;                          // Pointer to store the data read from the bus //
  transferSequence.buf[0].len = sizeof(read_data);                              // Number of bytes to be read from the I2C bus //
  NVIC_EnableIRQ(I2C0_IRQn);                                                    // Enable I2C Interrupt //
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);                   // Initiate I2C Transactions //
  if(transferStatus < 0)                                                        // Check for the error conditions //
    LOG_ERROR ("I2CInit_Transfer: I2C bus read failed with the error Code = %d\n\r", transferStatus);
}

void convertTemperature(){

  uint16_t msbData = read_data & MSB_MASK;                                      // Get MSB Data //
  msbData = msbData >> 8;                                                       // Right shift by 8 //

  uint16_t lsbData = read_data & LSB_MASK;                                      // Get LSB Data //
  lsbData = lsbData << 8;                                                       // Left shift by 8 //

  float final_temp = msbData | lsbData;                                         // Combine both LSB and MSB bits //

  final_temp = ((175.72*final_temp)/65536) - 46.85;                             // Converting to degree //
  indicate_temperature((uint32_t)final_temp);                                   // Send temperature data to BlueTooth module //
  displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp = %u C", (uint32_t)final_temp);
  LOG_INFO("temperature reading = %d C \r\n", (int)final_temp);
}


void measureTemperature(){
  bme280_ctrl_measure_set_to_work();

  int32_t temp = 0;
  if (bme280_read_temperature(&temp) == STATUS_OK) {
    printf("\n\r Temperature %ld,%ld %cCelsius \n\r", temp / 100,
    (temp % 100) / 10, 0XF8);
  }
  displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp : %d C", temp/100);
  uint8_t temp_eight = (uint8_t)(temp/100);
  writeIntoAttribute(temp_eight);
  bme280_ctrl_measure_set_to_sleep();
}




void measureHeartRate(void)
{

  uint32_t heart_rate = 1;
  if (GPIO_PinInGet(gpioPortA, PULSE_PIN) == 1) {
         // Rising edge, start of pulse
      ++heart_rate;
  }
  if (GPIO_PinInGet(gpioPortA, PULSE_PIN) == 1) {
        // Rising edge, start of pulse
      ++heart_rate;
  }
  displayPrintf(DISPLAY_ROW_8, "Heart Rate : %d C", heart_rate*60);
  LOG_INFO("HEart_rate = %d \n\r", heart_rate*60);

 }



