/*
    Hurricane driverboard

    serial.c:
             UART initialisation and output functions

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
On-chip UART access
*/
#include "serial.h"
#include "io.h"
#include <pic16/stdio.h>

void serial_init(void)
/*!
  Configure UART serial transmit
*/
{
#if 0
    usart_open(   USART_TX_INT_OFF
                & USART_RX_INT_ON
                & USART_BRGH_HIGH
                & USART_ASYNCH_MODE
                & USART_CONT_RX
                & USART_EIGHT_BIT,
                BAUDRATE );
#endif

    stdout = STREAM_USER;

    // TRISC - Tri-state Data Direction Register for port C
    // RC6 - 6th pin of port C - used for Serial Transmit
    // RC7 - 7th pin of port C - used for Serial Receive
    TRISCbits.TRISC6 = 0; // (0 = pin set as output)
    TRISCbits.TRISC7 = 1; // (1 = pin set as input)

    TXSTA = 0;
    RCSTA = 0;

    // BAUDCON - baudrate generation control Register
    #if defined(pic18f4520)
  //  BAUDCON = 0;  // 8 bit baudrate generator, auto-baud disabled
    #endif

    // SPBRG - Baud Rate Generator Register
    SPBRG = BAUDRATE;

    // BRGH - High Baud Rate Select Bit
    TXSTAbits.BRGH = 1; // (1 = high speed)

    // SYNC - USART Mode select Bit
    TXSTAbits.SYNC = 0; // (0 = asynchronous)

    // TX9 - 9-bit Transmit Enable Bit
    TXSTAbits.TX9 = 0; // (0 = 8-bit transmit)

    // RX9 - 9-bit Receive Enable Bit
    RCSTAbits.RX9 = 0; // (0 = 8-bit reception)

    // TXIE - USART Transmit Interupt Enable Bit
    PIE1bits.TXIE = 0; // (0 = disabled)

    // RCIE - USART Receive Interupt Enable Bit
    PIE1bits.RCIE = 1; // (1 = enabled)

    IPR1bits.RCIP = 0; // Low priority

    // CREN - Continuous Receive Enable Bit
    RCSTAbits.CREN = 1; // (1 = Enables receiver)

    // TXEN - Trasmit Enable Bit
    TXSTAbits.TXEN = 1; // (1 = transmit enabled)

    // SPEN - Serial Port Enable Bit
    RCSTAbits.SPEN = 1; // (1 = serial port enabled)
}

void serial_putchar(const unsigned char c)
/*!
  Send a character on the UART
*/
{
    //while(TXSTAbits.TRMT);
    while(PIR1bits.TXIF==0);
    TXREG = c;
}

void putchar (char c) __wparam
/*!
  This function is called by printf to print a character
*/
{
    serial_putchar(c);
}
