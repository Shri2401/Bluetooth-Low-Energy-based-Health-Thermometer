/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu (tempertature code)
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu (heart-rate code)
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : irq.c
 *
 *
 */

/*********************************************** LIBRARIES FROM THIS PROJECT ***********************************************/
#include "scheduler.h"
#include "app.h"
#include "timers.h"
#include "gpio.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

/*********************************************** LIBRARIES FROM SILABS ***********************************************/
#include "em_letimer.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_gpio.h"

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdio.h>

static uint32_t microseconds_count = 0;
bool ledOn = false;
void LETIMER0_IRQHandler(void){
  uint32_t IRQ_source = LETIMER_IntGet(LETIMER0);                        // Determine Interrupt source //

  LETIMER_IntClear(LETIMER0, IRQ_source);                                // Clear the interrupt flag //

  if(IRQ_source & LETIMER_IF_UF){
    schedulerSetEventUF();                                               // Schedule Underflow event //
    ++microseconds_count;                                                // Update the time //
  }
  if(IRQ_source & LETIMER_IF_COMP1){
    schedulerSetEventComp1();                                            // Schedule Comp1 event //
    LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);                     // Disable Interrupt //
  }
}

void GPIO_EVEN_IRQHandler(void){
  uint32_t source = GPIO_IntGetEnabled();
  GPIO_IntClear(source);
  //LOG_ERROR("Interrupt Source - %d \n\r", source);
  //GPIO_IntClear(0x5555);
  //schedulerSetEventPB0();
  //GPIO_PinOutToggle(LED_port, LED0_pin);

  if (source == 1<<PB0_PIN){
      if (GPIO_PinInGet(gpioPortF,PB0_PIN)){
          schedulerSetEventButtonRelease();
          //ledOn = false;
      }
      else{
          schedulerSetEventButtonPress();
          //ledOn = true;
      }

  }
}

void GPIO_ODD_IRQHandler(void){
  uint32_t source = GPIO_IntGetEnabled();
  GPIO_IntClear(source);
  //LOG_ERROR("Interrupt Source - %d \n\r", source);
  //GPIO_IntClear(0x5555);
  //schedulerSetEventPB0();
  //GPIO_PinOutToggle(LED_port, LED0_pin);

  if (source == 1<<PB1_PIN){
      if (GPIO_PinInGet(gpioPortF,PB1_PIN)){
          schedulerSetEventButtonReleasePB1();
          //ledOn = false;
      }
      else{
          schedulerSetEventButtonPressPB1();
          //ledOn = true;
      }

  }
}

uint32_t letimerMilliseconds(){
  uint32_t topValue = getTopValue();                                     // Get TOP Value //
  uint32_t actualDelay = microseconds_count * LETIMER_PERIOD_MS;         // Delay in multiple of 3000 millisecs //
  uint32_t currentCount = LETIMER_CounterGet(LETIMER0);                  // Current value from the counter //
  topValue -= currentCount;
  topValue = (topValue * MILLI_SEC_TO_SECONDS) / CMU_ClockFreqGet(cmuClock_LETIMER0);
  actualDelay += topValue;                                               // Remaining delay from the counter //
  return actualDelay;
}

