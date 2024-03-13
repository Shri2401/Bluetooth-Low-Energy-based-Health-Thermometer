/*
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 * @student    Shrinithi Venkatesan, shve4446@colorado.edu
 *  Course      : IOT Embedded Firmware
 *  Code        : ECEN 5823
 *  University  : University of Colorado at Boulder
 *  File        : oscillators.c
 *
 */

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdbool.h>

/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "oscillators.h"
#include "app.h"
#include "em_cmu.h"

void initOscillators(void){
  if(LOWEST_ENERGY_MODE == 0 || LOWEST_ENERGY_MODE == 1 || LOWEST_ENERGY_MODE == 2) { // LFXO Clock for high power modes //
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);                                    // Enable oscillator to use LFXO //
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);                                 // Select LFXO clock //
    CMU_ClockDivSet(cmuClock_LETIMER0, 4);                                            // Pre-scale clock with four to get reduced input frequency //
  }

  if(LOWEST_ENERGY_MODE == 3){                                                        // ULFRCO CLock for low power modes //
    CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);                                  // Enable oscillator to use ULFRCO //
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);                               // Select clock for LFXO in EM3 mode //
  }
  CMU_ClockEnable(cmuClock_LETIMER0, true);                                           // Enable clock to LETIMER0 //
  CMU_ClockEnable(cmuClock_GPIO, true);


}
