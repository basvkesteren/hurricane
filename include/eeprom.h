/*
    Hurricane driverboard

    eeprom.h:
                  EEPROM driver definitions

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
On-chip EEPROM read/write access function definitions
*/
#ifndef EEPROM_H
#define EEPROM_H

#include "config.h"

unsigned char eeprom_readbyte(const unsigned char address);
unsigned char eeprom_writebyte(const unsigned char address, const unsigned char eeprombyte);

#endif /* EEPROM_H */
