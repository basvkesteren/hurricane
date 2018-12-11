/*
    Hurricane driverboard
 
    motor.c:
            drivercode for the BA6418N motordrivers
 
    copyright:
              Copyright (c) 2010 Bastiaan van Kesteren <bas@edeation.nl>
 
              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.
 
    remarks:
 
 */
#include "motor.h"
#include "io.h"
#include <signal.h>

unsigned char speed[4];

void motor_init()
/*
 *  Initialise I/O for motor controllers
 */
{
    speed[0]=0;	//North
    speed[1]=0;	//East
    speed[2]=0;	//South
    speed[3]=0;	//West
    
    /* Configure and start timer */
    T0CONbits.T08BIT = 1;   /* timer0 is an 8bit counter */
    T0CONbits.T0CS = 0;     /* use instruction cycle clock as clocksource */
    T0CONbits.T0PS0 = 0;
    T0CONbits.T0PS1 = 0;    /* 1:8 prescaler */
    T0CONbits.T0PS2 = 0;
    
    T0CONbits.PSA = 0;      /* assign prescaler to timer0 */
    TMR0L = 0;              /* clear timer0 */
    T0CONbits.TMR0ON = 1;   /* enable timer0 */
    
    INTCONbits.T0IE = 1;    /* enable timer0 interrupt */
}

void motor_setspeed(unsigned char motor, unsigned char newspeed)
{
    speed[motor]=newspeed;
}

unsigned char motor_getspeed(unsigned char motor)
/*
 *  Return current speed of motor
 */
{
    return speed[motor];
}

void motor_update()
{
    static unsigned char ticks=0;
    
    ticks++;
    if(ticks == 100) {
        ticks = 0;
        
        if(speed[0] > 0) {
            north_on();
        }
        else {
            north_off();
        }
        if(speed[1] > 0) {
            east_on();
        }
        else {
            east_off();
        }
        if(speed[2] > 0) {
            south_on();
        }
        else {
            south_off();
        }
        if(speed[3] > 0) {
            west_on();
        }
        else {
            west_off();
        }
    }
    else {
        if(ticks == speed[0]) {
            north_off();
        }
        if(ticks == speed[1]) {
            east_off();
        }
        if(ticks == speed[2]) {
            south_off();
        }
        if(ticks == speed[3]) {
            west_off();
        }
    }
}
