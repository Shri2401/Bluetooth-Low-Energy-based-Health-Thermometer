/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.
   
   Jan 24, 2023
   Dave Sluiter: Cleaned up gpioInit() to make it less confusing for students regarding
                 drive strength setting. 

 *
 * Student edit: Add your name and email address here:
 * @student    Vignesh Vadivel, Vignesh.Vadivel@Colorado.edu
 *
 
 */


// *****************************************************************************
// Students:
// We will be creating additional functions that configure and manipulate GPIOs.
// For any new GPIO function you create, place that function in this file.
// *****************************************************************************

/*********************************************** STANDARD C LIBRARIES ***********************************************/
#include <stdbool.h>
#include <string.h>

/******************************************** LIBRARIES FROM THIS PROJECT ********************************************/
#include "gpio.h"


// Student Edit: Define these, 0's are placeholder values.
//
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.
// If these links have gone bad, consult the reference manual and/or the datasheet for the MCU.
// Change to correct port and pins:




// Set GPIO drive strengths and modes of operation
void gpioInit()
{
    // Student Edit:

    // Set the port's drive strength. In this MCU implementation, all GPIO cells
    // in a "Port" share the same drive strength setting. 
	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong); // Strong, 10mA
	//GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
	
	// Set the 2 GPIOs mode of operation
  GPIO_PinModeSet(LED_port, LED0_pin, gpioModePushPull, false);
  GPIO_PinModeSet(LED_port, LED1_pin, gpioModePushPull, false);
  GPIO_PinModeSet(gpioPortD, SI7021_pin, gpioModePushPull, false);
  GPIO_PinModeSet(gpioPortF, LCD_PIN_EXTCOMIN, gpioModePushPull, false);

  GPIO_PinModeSet(gpioPortF, PB0_PIN, gpioModeInputPullFilter, true);
  GPIO_PinModeSet(gpioPortF, PB1_PIN, gpioModeInputPullFilter, true);
  GPIO_PinModeSet(gpioPortA, PULSE_PIN, gpioModeInputPull, 0);

	//GPIO_PinModeSet(LED_port, LED1_pin, gpioModePushPull, false);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);

  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  GPIO_ExtIntConfig(gpioPortF, PB0_PIN, PB0_PIN, true, true, true);
  GPIO_ExtIntConfig(gpioPortF, PB1_PIN, PB1_PIN, true, true, true);


} // gpioInit()


void gpioLed0SetOn(){
	GPIO_PinOutSet(LED_port, LED0_pin);
}


void gpioLed0SetOff(){
	GPIO_PinOutClear(LED_port, LED0_pin);
}


void gpioLed1SetOn(){
	GPIO_PinOutSet(LED_port, LED1_pin);
}

void gpioSi7021TurnOn(){
  GPIO_PinOutSet(gpioPortD, SI7021_pin);
}

void gpioSi7021TurnOff(){
  GPIO_PinOutClear(gpioPortD, SI7021_pin);
}

void gpioLed1SetOff(){
	GPIO_PinOutClear(LED_port, LED1_pin);
}

void gpioI2CTurnOff(){
  GPIO_PinOutClear(gpioPortC, I2C0_SDA_pin);
  GPIO_PinOutClear(gpioPortC, I2C0_SCL_pin);
}

void gpioSetDisplayExtcomin(bool value)
{
  (value == true)?GPIO_PinOutSet(gpioPortF, LCD_PIN_EXTCOMIN):GPIO_PinOutClear(gpioPortF, LCD_PIN_EXTCOMIN);
}




