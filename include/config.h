/*
    Hurricane driverboard

    config.h:
              Copyright (c) 2010 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    copyright:

    remarks:
            -

*/
/*!
\file
Global configuration
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/*! Firmware version */
#define FIRMWAREVERSION             0x01

/*! Internal or external oscillator? */
#define INTOSC      1
/*! The oscillator input frequency in Hz */
#define FOSC        8000000
/*! PLL enabled (1) or not (0)?
    Note: This setting is ignored when using a FOSC < 4MHz. */
#define PLL         0
/*! Baudrate of the UART. Note that BRGH is enabled by default, so this value is
    calculated whit Baud Rate = FOSC/(16(X+1)) */
#define BAUDRATE    25
/*! Watchdog settings. Use '0' to disable it. Values > 0 are used as the watchdog
    prescaler values. Possible values are 1, 2, 4, 8, 16, 32, 64 and 128 */
#define WATCHDOG    128
/*! Enable (1) or disable (0) debug output */
#define DEBUG       1

/****
    No settings below this line
*/
#include <pic18fregs.h>
#include "debug.h"

#define TRUE    1
#define FALSE   0
#define ON      1
#define OFF     0

typedef unsigned char bool;

#if FOSC >= 4000000 && PLL
#define CCLK    4*FOSC
#else
#define CCLK    FOSC
#endif

#if WATCHDOG != 0
#define CLEAR_WATCHDOG()    do {    \
                                __asm        \
                                    clrwdt  \
                                __endasm;    \
                            } while(0)
#else
#define CLEAR_WATCHDOG()
#endif

#endif /* _CONFIG_H_ */
