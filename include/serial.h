/*
    Hurricane driverboard

    serial.h:
             UART function definitions

    copyright:
              Copyright (c) 2006-2007 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -

*/
/*!
\file
On-chip UART access function definitions
*/
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "config.h"

void serial_init(void);
void serial_putchar(const unsigned char c);

#endif /* _SERIAL_H_ */
