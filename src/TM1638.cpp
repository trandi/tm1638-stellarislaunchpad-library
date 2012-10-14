/*
  TM1638.cpp

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

#include "TM1638.h"
#include <algorithm>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


TM1638::TM1638(unsigned long portSysctlPeriph, unsigned long portBase, unsigned char clockPin, unsigned char dataPin, unsigned char strobePin, tBoolean activateDisplay, unsigned char intensity){
	this->portBase = portBase;
	this->clockPin = clockPin;
	this->dataPin = dataPin;
	this->strobePin = strobePin;

	// GPIO port needs to be enabled so these pins can be used.
	SysCtlPeripheralEnable(portSysctlPeriph);

    // all the pins are outputs
    GPIOPinTypeGPIOOutput(portBase, clockPin | dataPin | strobePin);

    // init them all in a known state
    //GPIOPinWrite(portBase, clockPin | dataPin | strobePin, 0xFF);

	// send the init commands
	sendCommand(DATA_WRITE_INCR_ADDR);
	setupDisplay(activateDisplay, intensity);

	strobeSelect();
	send(ADDRSET);
	for (int i = 0; i < 16; i++) {
		send(0x00);
	}
	strobeDeselect();
}


void TM1638::send(unsigned char data) {
	for (int i = 0; i < 8; i++) {
		GPIOPinWrite(portBase, clockPin, 0x00);
		GPIOPinWrite(portBase, dataPin,  data & 1 ? 0xFF : 0x00);
		data >>= 1;
		GPIOPinWrite(portBase, clockPin, 0xFF);
	}
}


unsigned char TM1638::receive() {
	unsigned char result = 0;

	// change the type of the data pin into an input
	GPIOPinTypeGPIOInput(portBase, dataPin);
	GPIOPinWrite(portBase, dataPin, 0xFF);

	// start with the most significant bit
	for (int i = 0; i < 8; i++) {
		result >>= 1;

		GPIOPinWrite(portBase, clockPin, 0x00);
		SysCtlDelay(1);  // completely empirical value, but without a small break here it doesn't work. Probably the TM1638 chip needs some time after the clock edge to prepare the data...

		if (GPIOPinRead(portBase, dataPin)) {
			result |= 0x80;
		}

		GPIOPinWrite(portBase, clockPin, 0xFF);
	}

	// put it back as output
	GPIOPinTypeGPIOOutput(portBase, dataPin);
	GPIOPinWrite(portBase, dataPin, 0x00);

	return result;
}



void TM1638::sendCommand(const unsigned char cmd){
	strobeSelect();
	send(cmd);
	strobeDeselect();
}

void TM1638::sendData(const unsigned char address, const unsigned char data) {
	sendCommand(DATA_WRITE_FIX_ADDR);
	strobeSelect();
	send(ADDRSET | address);
	send(data);
	strobeDeselect();
}

/**
 * 0b10000000 = 0x80
 */
void TM1638::sendChar(const unsigned char pos, const unsigned char data, tBoolean dot) {
	sendData(pos << 1, data | (dot ? 0x80 : 0));
}


unsigned char TM1638::getButtons() {
	unsigned char result = 0x00;

	strobeSelect();
	send(DATA_READ_KEY_SCAN_MODE);
	// we should wait to scan keys ready (see datasheet). BUT it seems to work like this...
	for (char i = 0; i < 4; i++) {
		result |= receive() << i;
	}
	strobeDeselect();

	return result;
}


void TM1638::setupDisplay(tBoolean active, const unsigned char intensity){
	sendCommand(0x80 | (active ? 8 : 0) | std::min((unsigned char)7, intensity));
}

void TM1638::setDisplayDigit(const unsigned char digit, const unsigned char pos, tBoolean dot, const unsigned char numberFont[]){
	sendChar(pos, numberFont[digit & 0xF], dot);
}

void TM1638::clearDisplayDigit(const unsigned char pos, tBoolean dot) {
	sendChar(pos, 0, dot);
}

void TM1638::setDisplay(std::string str, const unsigned char dots, const unsigned char pos, const unsigned char font[]){
	for (int i = 0; i < 8 - pos; i++) {
		if (str[i] != '\0') {
			sendChar(i + pos, font[str[i] - 32], (dots & (1 << (8 - i - 1))) != 0);
		} else {
			break;
		}
	}
}

void TM1638::setDisplay(signed long number, const unsigned char font[]){
	unsigned long absNum = number < 0 ? -number : number;
	tBoolean minus = number < 0;
	if(absNum > 99999999L){
		setDisplay("N TOO BG");
	}else{
		for(signed char pos = 7; pos >= 0; pos --){
			if(absNum != 0){
				setDisplayDigit(absNum % 10, pos, false);
				absNum /= 10; // move to the next digit to the left
			}else{
				// we are done, clear the rest of the display and or put a minus if necessary
				if(minus){
					sendChar(pos, font['-' - 32], false);
					minus = false; // only 1 is enough ! :)
				}else{
					clearDisplayDigit(pos);
				}
			}
		}
	}
}

void TM1638::clearDisplay() {
	for (int i = 0; i < 8; i++) {
		sendData(i << 1, 0x00);
	}
}


void TM1638::setLED(const unsigned char pos, const unsigned char color) {
	sendData((pos << 1) + 1, color);
}


void TM1638::strobeSelect() {
	GPIOPinWrite(portBase, strobePin, 0x00);
}

void TM1638::strobeDeselect() {
	GPIOPinWrite(portBase, strobePin, 0xFF);
}
