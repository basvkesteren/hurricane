/*
    Hurricane driverboard

    delay.c:
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
Simple delay functions
*/
#include "delay.h"
#include <pic16/delay.h>

void delay_us(volatile unsigned int time)
/*!
  1us delay @ 12MHz (0.33uS/instruction)
*/
{
    while(time>0) {
        #if WATCHDOG != 0
        __asm clrwdt __endasm;
        #else
        __asm nop __endasm;
        #endif
        __asm nop __endasm;
        __asm nop __endasm;
        time--;
    }
}

void delay_ms(volatile unsigned int time)
/*!
  1ms delay @ 12MHz (0.33uS/instruction)
*/
{
    while(time>0) {
        delay1ktcy(1);
        #if WATCHDOG != 0
        __asm clrwdt __endasm;
        #else
        __asm nop __endasm;
        #endif
        delay1ktcy(1);
        #if WATCHDOG != 0
        __asm clrwdt __endasm;
        #else
        __asm nop __endasm;
        #endif
        delay1ktcy(1);
        #if WATCHDOG != 0
        __asm clrwdt __endasm;
        #else
        __asm nop __endasm;
        #endif
        time--;
    }
}

void delay_s(volatile unsigned int time)
/*!
  1s delay @ 12MHz (0.33uS/instruction)
*/
{
    while(time>0) {
        delay_ms(250);
        delay_ms(250);
        delay_ms(250);
        delay_ms(250);
        time--;
    }
}
