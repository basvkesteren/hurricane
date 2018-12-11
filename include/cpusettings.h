/*
    Hurricane driverboard

    cpusettings.h:
                  CONFIG settings and stack declaration

    copyright:
              Copyright (c) 2006-2007 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -All CONFIG settings will be ignored when the resulting HEX file is
             used whit the tiny bootloader!

*/
/*!
\file
CPU configuration (config bits)
*/
#ifndef _CPUSETTINGS_H_
#define _CPUSETTINGS_H_

#include "config.h"

/* Initialise a stack of 255 bytes at RAM address 0x300 */
#pragma stack 0x300 0xFF

#pragma config XINST=OFF

#if defined(pic18f452)

#if INTOSC
#error "MCU doesn't have an internal oscillator"
#else
/* Oscillator settings */
#if FOSC <= 200000
__code char __at __CONFIG1H _conf0 = _OSC_LP_1H;
#elif FOSC < 4000000
__code char __at __CONFIG1H _conf0 = _OSC_XT_1H;
#elif PLL
__code char __at __CONFIG1H _conf0 = _OSC_HS_PLL_1H;
#else
__code char __at __CONFIG1H _conf0 = _OSC_HS_1H;
#endif
#endif

/* Watchdog */
#if WATCHDOG == 0
__code char __at __CONFIG2H _conf2 = _WDT_OFF_2H;
#elif WATCHDOG == 1
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_1_2H;
#elif WATCHDOG == 2
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_2_2H;
#elif WATCHDOG == 4
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_4_2H;
#elif WATCHDOG == 8
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_8_2H;
#elif WATCHDOG == 16
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_16_2H;
#elif WATCHDOG == 32
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_32_2H;
#elif WATCHDOG == 64
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_64_2H;
#elif WATCHDOG == 128
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_128_2H;
#else
#error Invalid watchdog prescaler selected!
#endif

/* Power up timeout and brown out detection enabled */
__code char __at __CONFIG2L _conf1 = _PUT_ON_2L & _BODEN_ON_2L;
/* CCP2 Mux on RC1 */
__code char __at __CONFIG3H _conf3 = _CCP2MUX_RC1_3H;
/* Low voltage programming disabled, Stack Overflow Reset enabled */
__code char __at __CONFIG4L _conf4 = _LVP_OFF_4L & _STVR_ON_4L;
/* Code protection disabled */
__code char __at __CONFIG5L _conf5 = _CP_0_OFF_5L & _CP_1_OFF_5L & _CP_2_OFF_5L & _CP_3_OFF_5L;
/* EEPROM read protection disabled, code protection boot disabled */
__code char __at __CONFIG5H _conf6 = _CPD_OFF_5H & _CPB_OFF_5H;
/* Table write protection disabled */
__code char __at __CONFIG6L _conf7 = _WRT_0_OFF_6L & _WRT_1_OFF_6L & _WRT_2_OFF_6L & _WRT_3_OFF_6L;
/* EEPROM write protection disabled, table write protect boot disabled, config write protect disabled */
__code char __at __CONFIG6H _conf8 = _WRTD_OFF_6H & _WRTB_OFF_6H & _WRTC_OFF_6H;
/* Table read protect disabled */
__code char __at __CONFIG7L _conf9 = _EBTR_0_OFF_7L & _EBTR_1_OFF_7L & _EBTR_2_OFF_7L & _EBTR_3_OFF_7L;
/* Table read protect boot disabled */
__code char __at __CONFIG7H _conf10 = _EBTRB_OFF_7H;

#elif defined(pic18f4520)

#if INTOSC
__code char __at __CONFIG1H _conf0 = _OSC_INTIO67_1H;
#else
/* Oscillator settings */
#if FOSC <= 200000
__code char __at __CONFIG1H _conf0 = _OSC_LP_1H;
#elif FOSC < 4000000
__code char __at __CONFIG1H _conf0 = _OSC_XT_1H;
#elif PLL
__code char __at __CONFIG1H _conf0 = _OSC_HS_PLL_1H;
#else
__code char __at __CONFIG1H _conf0 = _OSC_HS_1H;
#endif
#endif

/* Watchdog */
#if WATCHDOG == 0
__code char __at __CONFIG2H _conf2 = _WDT_OFF_2H;
#elif WATCHDOG == 1
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1_2H;
#elif WATCHDOG == 2
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_2_2H;
#elif WATCHDOG == 4
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_4_2H;
#elif WATCHDOG == 8
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_8_2H;
#elif WATCHDOG == 16
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_16_2H;
#elif WATCHDOG == 32
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_32_2H;
#elif WATCHDOG == 64
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_64_2H;
#elif WATCHDOG == 128
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_128_2H;
#elif WATCHDOG == 256
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_256_2H;
#elif WATCHDOG == 512
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_512_2H;
#elif WATCHDOG == 1024
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_1024_2H;
#elif WATCHDOG == 2048
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_2048_2H;
#elif WATCHDOG == 4096
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_4096_2H;
#elif WATCHDOG == 8192
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_8192_2H;
#elif WATCHDOG == 16384
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_16384_2H;
#elif WATCHDOG == 32768
__code char __at __CONFIG2H _conf2 = _WDT_ON_2H & _WDTPS_32768_2H;
#else
#error Invalid watchdog prescaler selected!
#endif

/* Power up timeout and brown out detection enabled */
__code char __at __CONFIG2L _conf1 = _PWRT_ON_2L & _BOREN_ON_2L;
/* MCLR settings */
__code char __at __CONFIG3H _conf3 = _MCLRE_ON_3H & _PBADEN_OFF_3H;
/* Low voltage programming disabled, Stack Overflow Reset enabled */
__code char __at __CONFIG4L _conf4 = _LVP_OFF_4L & _STVREN_ON_4L & _XINST_OFF_4L;
/* Code protection disabled */
__code char __at __CONFIG5L _conf5 = _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L;
/* EEPROM read protection disabled, code protection boot disabled */
__code char __at __CONFIG5H _conf6 = _CPD_OFF_5H & _CPB_OFF_5H;
/* Table write protection disabled */
__code char __at __CONFIG6L _conf7 = _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L;
/* EEPROM write protection disabled, table write protect boot disabled, config write protect disabled */
__code char __at __CONFIG6H _conf8 = _WRTD_OFF_6H & _WRTB_OFF_6H & _WRTC_OFF_6H;
/* Table read protect disabled */
__code char __at __CONFIG7L _conf9 = _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L;
/* Table read protect boot disabled */
__code char __at __CONFIG7H _conf10 = _EBTRB_OFF_7H;

#endif


#endif /* _CPUSETTINGS_H_ */
