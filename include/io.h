/*
    Hurricane driverboard

    io.h:
             
    copyright:
              Copyright (c) 2010 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.


    remarks:

*/
/*!
\file
Hurricane driverboard I/O function definitions and macro's
*/
#ifndef IO_H
#define IO_H

#include "config.h"

/*
 *  Status LEDs
 */

#define statusled0_on()				do { \
										PORTDbits.RD0 = 1; \
									} while(0)

#define statusled0_off()			do { \
										PORTDbits.RD0 = 0; \
									} while(0)

#define statusled1_on()				do { \
										PORTDbits.RD1 = 1; \
									} while(0)

#define statusled1_off()			do { \
										PORTDbits.RD1 = 0; \
									} while(0)

/*
  Guide LEDs (underwater)
*/

// South-East LED
#define guideled0_on()				do { \
										PORTCbits.RC0 = 0; \
									} while(0)

#define guideled0_off()				do { \
										PORTCbits.RC0 = 1; \
									} while(0)

// South-West LED			
#define guideled1_on()				do { \
										PORTCbits.RC1 = 0; \
									} while(0)

#define guideled1_off()				do { \
										PORTCbits.RC1 = 1; \
									} while(0)
					
// Center LED					
#define guideled2_on()				do { \
										PORTCbits.RC2 = 0; \
									} while(0)

#define guideled2_off()				do { \
										PORTCbits.RC2 = 1; \
									} while(0)
				
// North-East LED				
#define guideled3_on()				do { \
										PORTCbits.RC3 = 0; \
									} while(0)

#define guideled3_off()				do { \
										PORTCbits.RC3 = 1; \
									} while(0)
				
// North-West LED				
#define guideled4_on()				do { \
										PORTCbits.RC4 = 0; \
									} while(0)

#define guideled4_off()				do { \
										PORTCbits.RC4 = 1; \
									} while(0)

/*
  Guide sensors
*/
// South east
#define guidesensor0_state()		(PORTA & (1<<4))
// South west
#define guidesensor1_state()		(PORTA & (1<<5))
// Center
#define guidesensor2_state()		(PORTE & (1<<0))
// North east
#define guidesensor3_state()		(PORTE & (1<<1))
// North west
#define guidesensor4_state()		(PORTE & (1<<2))

/*
	Joystick
*/

#define joystick_button_state()		((~PORTB)&(1<<2))

#define joystick_trigger_state()	((~PORTB)&(1<<1))

/*
  Motordrivers
*/

#define north_on()					do { \
										PORTDbits.RD4 = 1; \
									} while(0)

#define north_off()					do { \
										PORTDbits.RD4 = 0; \
									} while(0)

#define east_on()					do { \
										PORTDbits.RD5 = 1; \
									} while(0)

#define east_off()					do { \
										PORTDbits.RD5 = 0; \
									} while(0)

#define south_on()					do { \
										PORTDbits.RD6 = 1; \
									} while(0)

#define south_off()					do { \
										PORTDbits.RD6 = 0; \
									} while(0)

#define west_on()					do { \
										PORTDbits.RD7 = 1; \
									} while(0)

#define west_off()					do { \
										PORTDbits.RD7 = 0; \
									} while(0)

/*
  Buttons
*/

#define button_north_pushed()		(PORTB & (1<<7))
#define button_east_pushed()		(PORTB & (1<<6))
#define button_south_pushed()		(PORTB & (1<<5))
#define button_west_pushed()		(PORTB & (1<<4))

#endif /* IO_H */
