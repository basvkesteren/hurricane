/*
    Hurricane driverboard

    eeprom.c:
                  EEPROM driver

    copyright:
              Copyright (c) 2007 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -

*/
/*!
\file
On-chip EEPROM read/write access
*/
#include "eeprom.h"

unsigned char eeprom_readbyte(const unsigned char address)
/*!
  Read one single byte from the EEPROM at address 'address'
*/
{
    /* Point to EEPROM memory */
    EECON1bits.EEPGD = 0;

    /* Load address */
    EEADR = address;
    /* Start read, data is available in eedata the next cycle. */
    EECON1bits.RD = 1;
    /* Store byte */
    return EEDATA;
}

unsigned char eeprom_writebyte(const unsigned char address, const unsigned char eeprombyte)
/*!
  Write one single byte to the EEPROM at address 'address'
*/
{
    /* Point to EEPROM memory */
    EECON1bits.EEPGD = 0;
    /* Enable EEPROM writes */
    EECON1bits.WREN = 1;

    /* Load address */
    EEADR = address;
    /* And data */
    EEDATA = eeprombyte;

    /* Disable interrupts (the writecycle may not be interrupted) */
    INTCONbits.GIE = 0;
    /* Required write sequence */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    /* Enable interrupts */
    INTCONbits.GIE = 1;

    /* Wait until write-action is complete */
    while(!PIR2bits.EEIF);
    PIR2bits.EEIF = 0;

    /* Disable write */
    EECON1bits.WREN = 0;
	
	return eeprombyte;
}
