/*
    Hurricane driverboard

    delay.h:
            Simple delay functions

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
Simple delay function definitions
*/
#ifndef DELAY_H
#define DELAY_H

#include "config.h"

void delay_s(volatile unsigned int time);
void delay_us(volatile unsigned int time);
void delay_ms(volatile unsigned int time);

#endif /* DELAY_H */
