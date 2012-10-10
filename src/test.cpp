/*
 * test.cpp
 *
 *  Created on: 6 Oct 2012 Author: dan.oprescu
 */

#include "TM1638.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"


#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

int main(void){
    // Setup the system clock to run at 50 Mhz from PLL with crystal reference (SYSCTL_SYSDIV_4)
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Enable and configure the GPIO port for the LED operation.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, GREEN_LED);


    TM1638 segmentDisplay(SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, true, 7);


	unsigned char buttons;
	while (true) {
		buttons = segmentDisplay.getButtons();
		if(buttons == 1){
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);

			for(char i=0; i<8; i++){
				segmentDisplay.clearDisplay();
				segmentDisplay.setDisplay(std::string("HELLO W"), 0x00, i);
				SysCtlDelay(10000000);
			}
		}else if (buttons != 0x00){
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, 0x00);

			segmentDisplay.clearDisplay();

			for(char i = 0; i < 8; i++){
				if(buttons & 1){
					segmentDisplay.setLED(i, LED_COLOR_RED);
					segmentDisplay.setDisplayDigit(7, i, 1);
				}else {
					segmentDisplay.setLED(i, LED_COLOR_GREEN);
				}

				// go the the bit for the next button
				buttons >>= 1;
			}
		}else {
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED);

			segmentDisplay.setDisplay("ABCDEFGH", 0, 0);
		}
	}
}

