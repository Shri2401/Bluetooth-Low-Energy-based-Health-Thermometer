/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : timers.c
 *
 */

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdbool.h>

/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "app.h"
#include "log.h"
#include "timers.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "em_cmu.h"
#include "em_letimer.h"

/****************************************************** MACROS ******************************************************/
#define LETIMER_RANGE 0xFFFF
uint32_t topValue;
void initLETimer0(void){
  topValue = (CMU_ClockFreqGet(cmuClock_LETIMER0) * LETIMER_PERIOD_MS)/MILLI_SEC_TO_SECONDS;   // Calculate top value //

  const LETIMER_Init_TypeDef timerInitData = {                                  // Timer interrupt initialization structure //
      .enable = false,
      .debugRun = true,
      .comp0Top = true,
      .bufTop = false,
      .out0Pol = 0,
      .out1Pol = 0,
      .ufoa0 = letimerUFOANone,
      .ufoa1 = letimerUFOANone,
      .repMode = letimerRepeatFree,
      .topValue = topValue
  };
  LETIMER_Init(LETIMER0, &timerInitData);                                       // Initialize LETIMER0 //
  LETIMER_CompareSet(LETIMER0, 0, topValue);                                    // Load top value to COMP0 //

  LETIMER_IntClear(LETIMER0, 0xFFFFFFFF);                                       // Clear previous interrupts flag //
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);                                  // Underflow Interrupt enable //
  LETIMER_Enable(LETIMER0, true);                                               // Timer enable //
}

uint32_t getTopValue(){
  return topValue;
}

void timerWaitUs_polled(uint32_t us_wait){
  uint32_t diff;
  uint32_t I2C_WaitTicks = (CMU_ClockFreqGet(cmuClock_LETIMER0) * us_wait)/MICRO_SEC_TO_SECONDS;      // Calculate number of ticks for given delay //
  if (I2C_WaitTicks > LETIMER_RANGE){                                           // Check for Upper Threshold //
      LOG_ERROR("Given delay in micoseconds is out of range \n\rs");
      return;
  }
  uint32_t minWait_US = MICRO_SEC_TO_SECONDS/CMU_ClockFreqGet(cmuClock_LETIMER0);
  if (us_wait < minWait_US){                                                    // Check for Lower Threshold //
    LOG_ERROR("Given delay in micoseconds is LESS than the MIN_DELAY \n\r");
    return;
  }
  uint32_t current_count = LETIMER_CounterGet(LETIMER0);                        // Get current value from LETIMER counter register //

  /*
   * Two edge cases
   * 1. Timer is counting from TOP value and is greater than the requested delay value
   * 2. Timer has crossed the expected ticks value and has to be wrapped around
   *
   */
  if(current_count > I2C_WaitTicks){                                            // Value in the counter is greater than the required ticks //
      diff = current_count - I2C_WaitTicks;                                     // The counter has to count down till this difference //
  }
  else{                                                                         // Value in the counter is lesser or equal to the required ticks //
      diff = I2C_WaitTicks - current_count;
      diff = topValue - diff;
  }

  while(LETIMER_CounterGet(LETIMER0) != diff);                                  // Wait till the required delay //
}

void timerWaitUs_irq(uint32_t us_wait){
  uint32_t I2C_WaitTicks = (CMU_ClockFreqGet(cmuClock_LETIMER0) * us_wait)/MICRO_SEC_TO_SECONDS;      // Calculate number of ticks for given delay //
  if (I2C_WaitTicks > LETIMER_RANGE){                                           // Check for Upper Threshold //
    LOG_ERROR("Given delay in micoseconds is out of range \n\r");
    return;
  }
  uint32_t minWait_US = MICRO_SEC_TO_SECONDS/CMU_ClockFreqGet(cmuClock_LETIMER0);
  if (us_wait < minWait_US){                                                    // Check for Lower Threshold //
    LOG_ERROR("Given delay in micoseconds is LESS than the MIN_DELAY \n\r");
    return;
  }
  uint32_t current_count = LETIMER_CounterGet(LETIMER0);                        // Get current value from LETIMER counter register //
  uint32_t comp1_value = 0x0000;

  /*
   * Two edge cases
   * 1. Timer is counting from TOP value and is greater than the requested delay value
   * 2. Timer has crossed the expected ticks value and has to be wrapped around
   *
   */
  if (I2C_WaitTicks <= current_count){                                          // Value in the counter is greater than the required ticks //
    comp1_value = current_count - I2C_WaitTicks;                                // Required Ticks //
  }
  else{                                                                         // Value in the counter is lesser or equal to the required ticks //
    comp1_value = I2C_WaitTicks - current_count;
    comp1_value = topValue - comp1_value;                                       // Required Ticks //
  }

  LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);                                // Clear Previous Comp-1 INterrupt //
  LETIMER_CompareSet(LETIMER0, 1, comp1_value);                                 // Load COMP-1 value //
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);                               // Comp 1 Interrupt enable //
  LETIMER0->IEN |= LETIMER_IEN_COMP1;
}
