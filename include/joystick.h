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
            -

*/
/*!
\file
Joystick X/Y reading
*/
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "config.h"

typedef struct {
    unsigned int minX, minY, centerX, centerY, maxX, maxY;
} calibrationdata_t;

void joystick_init(void);
void joystick_go(void);
void joystick_tick(void);
void joystick_calibrate(bool start);
bool joystick_calibrating(void);
void joystick_setcenter(void);
int joystick_getX(void);
int joystick_getY(void);

#endif /* EEPROM_H */
