/*
    Hurricane driverboard
 
    joystick.c:
                  Joystick driver
 
    copyright:
              Copyright (c) 2010 Bastiaan van Kesteren <bas@edeation.nl>
 
              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.
 
    remarks:
            -AN0 = x, AN1 = y
  
 */
/*!
 * \file
 * Joystick X/Y reading
 */
#include "joystick.h"
#include "debug.h"
#include <signal.h>

calibrationdata_t calibrationdata;

static unsigned int X,Y;
static bool calibrating;

void joystick_init(void)
/*
 * 
 */
{
    X = 0;
    Y = 0;
    calibrationdata.minX = 0xFFFF;
    calibrationdata.minY = 0xFFFF;
    calibrationdata.centerX = 0;
    calibrationdata.centerY = 0;
    calibrationdata.maxX = 0;
    calibrationdata.maxY = 0;
    calibrating = FALSE;
    
    /* Supply voltage is reference */
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    /* Channel 0 and 1 enabled, others digital I/O */
    ADCON1bits.PCFG0 = 1;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 1;
    ADCON1bits.PCFG3 = 1;
    
    /* Default to channel 0 */
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;
    
    /* Right justified result */
    ADCON2bits.ADFM = 1;
    /* Timing stuff (TODO: figure this out) */
    ADCON2bits.ACQT0 = 1;
    ADCON2bits.ACQT1 = 0;
    ADCON2bits.ACQT2 = 1;
    ADCON2bits.ADCS0 = 0;
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS2 = 1;
    
    PIR1bits.ADIF = 0;
    IPR1bits.ADIP = 0;	// low priority
    PIE1bits.ADIE = 1;	// enable interrupt
    
    /* Turn converter on */
    ADCON0bits.ADON = 1;
}

void joystick_go(void)
{
    ADCON0bits.GO = 1;
}

void joystick_tick(void)
{
    static unsigned char channel = 0;
    
    if(channel == 0) {
        X = (ADRESH << 8) | ADRESL;
        channel = 1;
        if(calibrating) {
            if(X < calibrationdata.minX) {
                calibrationdata.minX = X;
            }
            if(X > calibrationdata.maxX) {
                calibrationdata.maxX = X;
            }
        }
    }
    else {
        Y = (ADRESH << 8) | ADRESL;
        channel = 0;
        if(calibrating) {
            if(Y < calibrationdata.minY) {
                calibrationdata.minY = Y;
            }
            if(Y > calibrationdata.maxY) {
                calibrationdata.maxY = Y;
            }
        }
    }
    
    /* Switch channel and start next conversion */
    ADCON0bits.CHS0 = channel;
    ADCON0bits.GO = 1;
}

void joystick_calibrate(bool start)
{
    if(start) {
        calibrationdata.minX = 0xFFFF;
        calibrationdata.minY = 0xFFFF;
        calibrationdata.centerX = 0;
        calibrationdata.centerY = 0;
        calibrationdata.maxX = 0;
        calibrationdata.maxY = 0;
    }
    calibrating = start;
}

bool joystick_calibrating(void)
{
    return calibrating;
}

void joystick_setcenter(void)
{
    calibrationdata.centerX = X;
    calibrationdata.centerY = Y;
    /* Setting the center is the second and last step of the calibration thingy, so we're done now */
    calibrating = FALSE;
}

int joystick_getX(void)
/*
 * 
 */
{
    unsigned int myX = X;
    unsigned int result;
    
    if(myX > (calibrationdata.centerX - 25) && myX < (calibrationdata.centerX + 25) ) {
        /* With some tolerance, we say this is the centre.. */
        result = 0;
    }
    else if(myX < (calibrationdata.minX + 25)) {
        /* With some tolerance, we say this is the minimum.. */
        result = -100;
    }
    else if(myX > (calibrationdata.maxX - 25)) {
        /* With some tolerance, we say this is the maximum.. */
        result = 100;
    }
    else if(myX < calibrationdata.centerX) {
        /* the lower 100 percent; delta is centerX - minX */
        result = (-100) + ((myX - calibrationdata.minX) * 100) / (calibrationdata.centerX - calibrationdata.minX);
    }
    else {
        /* the upper 100 percent; delta is maxX - centerX */
        result = ((myX - calibrationdata.centerX) * 100) / (calibrationdata.maxX - calibrationdata.centerX);
    }
    
    return result;
}

int joystick_getY(void)
/*
 * 
 */
{
    unsigned int myY = Y;
    int result;
    
    if(myY > (calibrationdata.centerY - 25) && myY < (calibrationdata.centerY + 25) ) {
        /* With some tolerance, we say this is the centre.. */
        result = 0;
    }
    else if(myY < (calibrationdata.minY + 25)) {
        /* With some tolerance, we say this is the minimum.. */
        result = -100;
    }
    else if(myY > (calibrationdata.maxY - 25)) {
        /* With some tolerance, we say this is the maximum.. */
        result = 100;
    }
    else if(myY < calibrationdata.centerY) {
        /* the lower 100 percent; delta is centerY - minY */
        result = (-100) + ((myY - calibrationdata.minY) * 100) / (calibrationdata.centerY - calibrationdata.minY);
    }
    else {
        /* the upper 100 percent; delta is maxY - centerY */
        result = ((myY - calibrationdata.centerY) * 100) / (calibrationdata.maxY - calibrationdata.centerY);
    }
    
    return result;
}
