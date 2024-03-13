/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

    Editor: Feb 26, 2022, Dave Sluiter
    Change: Added comment about use of .h files.

 *
 * Student edit: Add your name and email address here:
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 *
 
 */


// Students: Remember, a header file (a .h file) generally defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.


#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <stdbool.h>
#include "em_gpio.h"

/*********************************************** MACROS ***********************************************/
#define LED_port         (5)
#define LED0_pin         (4)
#define LED1_pin         (5)
#define SI7021_pin       (15)
#define I2C0_SCL_pin     (10)
#define I2C0_SDA_pin     (11)
#define LCD_PIN_EXTCOMIN (3)  // it is in GPIO PORT F
#define PB0_PIN          (6)
#define PB1_PIN          (7)
#define PULSE_PIN        (2)   // PA02

// Function prototypes
void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioSi7021TurnOn();
void gpioSi7021TurnOff();
void gpioI2CTurnOff();
void gpioSetDisplayExtcomin(bool value);






#endif /* SRC_GPIO_H_ */
