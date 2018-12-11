/*
    Hurricane driverboard

    debug.h:
            debug settings

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
Global DEBUG definition
*/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#if DEBUG
#include "serial.h"
#include <pic16/stdio.h>
#define debug_init  serial_init
#define debug       printf
#else
#define debug_init(x)
#define debug(args...)
#endif

#endif /* _DEBUG_H_ */
