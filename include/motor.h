/*
    Hurricane driverboard

    motor.h:
            motor driver function definitions

    copyright:
              Copyright (c) 2006 Bastiaan van Kesteren <bas@edeation.nl>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -

*/
#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "config.h"

void motor_init(void);

void motor_setspeed(unsigned char motor,unsigned char newspeed);
unsigned char motor_getspeed(unsigned char motor);

void motor_update(void);

#endif /* _MOTOR_H_ */
