/*
  TM1638.h

  Stellaris Launchpad EK-LM4F120XL driver for TM1638

  Copyright (C) Oct 2012 Dan Oprescu (robotrandi <at> gmail <dot> com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the version 3 GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Built with CCS Version 5.2.1
 */

#ifndef TM1638_H_
#define TM1638_H_

#include "TM1638Fonts.h"
#include <string>
#include "inc/hw_types.h"



#define ADDRSET 0xC0						//Command to set address 0x00
#define DATA_WRITE_INCR_ADDR 0x40			//Command to switch TM1638 for automatic increment address mode
#define DATA_WRITE_FIX_ADDR 0x44			//Command to switch TM1638 for fix address mode
#define DATA_READ_KEY_SCAN_MODE 0x42		//Command for read key code from TM1638

#define LED_COLOR_RED    0x01
#define LED_COLOR_GREEN  0x02


class TM1638{
public:
    /**
     *  Instantiate a TM1638 module specifying the display state, the starting intensity (0-7) data, and the necessary pins on a specified port.
     *  ALL the pins have to be on the SAME port !
     **/
    TM1638(unsigned long portSysctlPeriph, unsigned long portBase, unsigned char clockPin, unsigned char dataPin, unsigned char strobePin, tBoolean activateDisplay = true, unsigned char intensity = 7);

    /* Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
    void setupDisplay(tBoolean active, unsigned char intensity);

    void setDisplayDigit(unsigned char digit, unsigned char pos, unsigned char dot, const unsigned char numberFont[] = NUMBER_FONT);

    void clearDisplayDigit(unsigned char pos, tBoolean dot);

    void setDisplay(std::string str, const unsigned char dots = 0, const unsigned char pos = 0, const unsigned char font[] = FONT_DEFAULT);

	void clearDisplay();

    /** Set the LED at pos to color (LED_COLOR_RED, LED_COLOR_GREEN or both) */
    void setLED(const unsigned char pos, const unsigned char color);

    unsigned char getButtons();

protected:
    void sendData(const unsigned char address, const unsigned char data);
    void sendChar(const unsigned char pos, const unsigned char data, tBoolean dot);
    void sendCommand(const unsigned char command);

    void send(unsigned char data);
    unsigned char receive();

    void strobeSelect();
    void strobeDeselect();

    unsigned long portBase;
    unsigned char clockPin;
    unsigned char dataPin;
    unsigned char strobePin;

};

#endif /* TM1638_H_ */
