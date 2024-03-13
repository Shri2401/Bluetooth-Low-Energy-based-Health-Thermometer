/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Date:        02-25-2022
 * Author:      Dave Sluiter
 * Description: This code was created by the Silicon Labs application wizard
 *              and started as "Bluetooth - SoC Empty".
 *              It is to be used only for ECEN 5823 "IoT Embedded Firmware".
 *              The MSLA referenced above is in effect.
 *
 *
 *
 * Student edit: Add your name and email address here:
 * @student    Vignesh Vadivel , Vignesh.Vadivel@Colorado.edu
 *
 *
 *
 ******************************************************************************/

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "em_common.h"
#include "em_chip.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_status.h"             // for sl_status_print()

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdint.h>

// *************************************************
// Students: It is OK to modify this file.
//           Make edits appropriate for each
//           assignment.
// *************************************************

/*********************************************** LIBRARIES FROM THIS PROJECT ***********************************************/
#include "src/ble_device_type.h"
#include "src/gpio.h"
#include "src/lcd.h"
#include "src/oscillators.h"
#include "src/irq.h"
#include "src/timers.h"
#include "src/scheduler.h"
#include "src/i2c.h"
#include "src/ble.h"
#include "app.h"
#include <bme280.h>
#include <ccs811.h>
#include <stdio.h>
#include "sl_i2cspm_instances.h"
#include "sl_sleeptimer.h"

// Students: Here is an example of how to correctly include logging functions in
//           each .c file.
//           Apply this technique to your other .c files.
//           Do not #include "src/log.h" in any .h file! This logging scheme is
//           designed to be included at the top of each .c file that you want
//           to call one of the LOG_***() functions from.

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

/*********************************************** MACROS ***********************************************/



// *************************************************
// Power Manager
// *************************************************

// See: https://docs.silabs.com/gecko-platform/latest/service/power_manager/overview
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

// -----------------------------------------------------------------------------
// defines for power manager callbacks
// -----------------------------------------------------------------------------
// Return values for app_is_ok_to_sleep():
//   Return false to keep sl_power_manager_sleep() from sleeping the MCU.
//   Return true to allow system to sleep when you expect/want an IRQ to wake
//   up the MCU from the call to sl_power_manager_sleep() in the main while (1)
//   loop.
//
// Students: We'll need to modify this for A2 onward so that compile time we
//           control what the lowest EM (energy mode) the MCU sleeps to. So
//           think "#if (expression)".
//#define APP_IS_OK_TO_SLEEP      (false)
#if (LOWEST_ENERGY_MODE)
#define APP_IS_OK_TO_SLEEP      (true)
#else
#define APP_IS_OK_TO_SLEEP      (false)
#endif

// Return values for app_sleep_on_isr_exit():
//   SL_POWER_MANAGER_IGNORE; // The module did not trigger an ISR and it doesn't want to contribute to the decision
//   SL_POWER_MANAGER_SLEEP;  // The module was the one that caused the system wakeup and the system SHOULD go back to sleep
//   SL_POWER_MANAGER_WAKEUP; // The module was the one that caused the system wakeup and the system MUST NOT go back to sleep
//
// Notes:
//       SL_POWER_MANAGER_IGNORE, we see calls to app_process_action() on each IRQ. This is the
//       expected "normal" behavior.
//
//       SL_POWER_MANAGER_SLEEP, the function app_process_action()
//       in the main while(1) loop will not be called! It would seem that sl_power_manager_sleep()
//       does not return in this case.
//
//       SL_POWER_MANAGER_WAKEUP, doesn't seem to allow ISRs to run. Main while loop is
//       running continuously, flooding the VCOM port with printf text with LETIMER0 IRQs
//       disabled somehow, LED0 is not flashing.

#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_IGNORE)
//#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_SLEEP)
//#define APP_SLEEP_ON_ISR_EXIT   (SL_POWER_MANAGER_WAKEUP)


#endif // defined(SL_CATALOG_POWER_MANAGER_PRESENT)




// *************************************************
// Power Manager Callbacks
// The values returned by these 2 functions AND
// adding and removing power manage requirements is
// how we control when EM mode the MCU goes to when
// sl_power_manager_sleep() is called in the main
// while (1) loop.
// *************************************************

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)

bool app_is_ok_to_sleep(void)
{
  return APP_IS_OK_TO_SLEEP;
} // app_is_ok_to_sleep()

sl_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  return APP_SLEEP_ON_ISR_EXIT;
} // app_sleep_on_isr_exit()

#endif // defined(SL_CATALOG_POWER_MANAGER_PRESENT)




/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  // Put your application 1-time initialization code here.
  // This is called once during start-up.
  // Don't call any Bluetooth API functions until after the boot event.

  // Switching energy modes based on POUND define //
  if(LOWEST_ENERGY_MODE == 1){
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  }

  if(LOWEST_ENERGY_MODE == 2){
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
  }
  CHIP_Init();
  gpioInit();                                              // Initialize GPIO pins //
  initOscillators();                                       // Initialize Oscillators //
  initLETimer0();                                          // Initialize Low Energy Timer //

  NVIC_ClearPendingIRQ(LETIMER0_IRQn);                     // Clear Pending Interrupts //
  NVIC_EnableIRQ(LETIMER0_IRQn);                           // Enable Interrupt //

  printf("\r\nInit\r\n");
  // Initialize I2C for the sensor instances.
  bme280_i2c_t init_b = BME280_I2C_DEFAULT;
  bme280_i2c(&init_b);
  ccs811_i2c_t init_c = CCS811_I2C_DEFAULT;
  ccs811_i2c(&init_c);

  if (bme280_init() == STATUS_OK) {
    printf("\n\r BME280 on 0x%X I2C address found"
    "\n\r and initialized", BME_280_DEFAULT_I2C_ADDR);
  } else {
    printf("\n\r BME280 on 0x%X I2C address not found. Check cables. "
    "\n\r Try also alternative address", BME_280_DEFAULT_I2C_ADDR);

    if (BME_280_DEFAULT_I2C_ADDR == 0x77) {
      printf(" 0x76");
    } else {
      printf(" 0x77");
    }
  }

  if (ccs811_init () == STATUS_PASSED) {
    printf("\n\r CCS811 on 0x%X I2C address found"
    "\n\r and initialized", CCS_811_DEFAULT_I2C_ADDR);
  } else {
    printf("\n\r CCS811 on 0x%X I2C address not found. Check cables. "
    "\n\r Try also alternative address ", CCS_811_DEFAULT_I2C_ADDR);

    if (CCS_811_DEFAULT_I2C_ADDR == 0x5B) {
      printf(" 0x5A");
    } else {
      printf(" 0x5B");
    }
  }
}



/*****************************************************************************
 * delayApprox(), private to this file.
 * A value of 3500000 is ~ 1 second. After assignment 1 you can delete or
 * comment out this function. Wait loops are a bad idea in general.
 * We'll discuss how to do this a better way in the next assignment.
 *****************************************************************************/
/* static void delayApprox(int delay)
{
  volatile int i;

  for (i = 0; i < delay; ) {
      i=i+1;
  }

}*/ // delayApprox()


uint32_t currentEvent = 0x00000000;


/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{

  // Put your application code here for A1 to A4.
  // This is called repeatedly from the main while(1) loop
  // Notice: This function is not passed or has access to Bluetooth stack events.
  //         We will create/use a scheme that is far more energy efficient in
  //         later assignments.

  //delayApprox(3500000);
  //gpioLed0SetOn();
  //gpioLed1SetOn();
  //delayApprox(3500000);
  //gpioLed0SetOff();
  //gpioLed1SetOff();
  //uint32_t nextEvent = 0x0000;                                          // Next event to be read //
  //nextEvent = getNextEvent();                                           // Get next event from the scheduler //
  //myStateMachine(nextEvent);                                            // Send Next Event to the state machine //
  //LOG_INFO("Current Event before IF block %u\n\r", currentEvent);
  //LOG_INFO("Timer Event before IF block %u\n\r", timerEvent);
  //LOG_INFO("Timer Event after IF block %u\n\r", timerEvent);
  //LOG_INFO("Current Event after IF block %u\n\r", currentEvent);

} // app_process_action()


/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *
 * The code here will process events from the Bluetooth stack. This is the only
 * opportunity we will get to act on an event.
 *
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{

  // Just a trick to hide a compiler warning about unused input parameter evt.
  (void) evt;

  // For A5 onward:
  // Some events require responses from our application code,
  // and don’t necessarily advance our state machines.
  // For A5 uncomment the next 2 function calls
  handle_ble_event(evt); // put this code in ble.c/.h
  // sequence through states driven by events
#if (DEVICE_IS_BLE_SERVER)
  myEventHandler(evt);
#else
  myDiscoveryStateMachine(evt);
  readTemperatureValue(evt);
#endif
} // sl_bt_on_event()

