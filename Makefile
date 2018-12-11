#
#    PIC16 SDCC - printf example for the pic18f452
#
#    example-makefile-pic16:
#                           Example Makefile for PIC18 SDCC projects
#
#    copyright:
#              Copyright (c) 2006-2007 Bastiaan van Kesteren <bas@edeation.nl>
#
#              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
#              This program is free software; you can redistribute it and/or modify it under the terms
#              of the GNU General Public License as published by the Free Software Foundation; either
#              version 2 of the License, or (at your option) any later version.
#
#    remarks:
#            -Rename this file to 'Makefile', and issue 'make' on the console in the same directory
#
#    revisions:
#        08-10-2007  Add linkerscript setting, add 'make load', fix bug whit sourcecode organized in directories
#                    (asm file ended up in the project-root)
#        17-10-2007  Add startup selection


# The PIC type
PIC = 18f4520
# Name of the project
NAME = hurricane
# All C code files, seperated whit spaces
SRC = delay.c eeprom.c main.c serial.c motor.c joystick.c
# All ASM code files, seperated whit spaces
SRCASM =

# The linkerscript to use. Leave empty to use the default
LINKERSCRIPT =

LIBRARYPATH = C:\picmicro\SDCC\lib\pic16

# The SDCC PIC16 port provides three startup files:
# -crt0.o       Basic startup (Default if none selected)
# -crt0i.o      Startup with initialisation
# -crt0iz.o     Startup with initialisation and clearing of unused RAM
# You can ofcourse use your own startup file, if you want to.
CRT = crt0iz.o

# Shouldn't need to change anything below this line
INCLUDE = -Iinclude -IC:\picmicro\SDCC\include -IC:\picmicro\SDCC\include\pic16

CC = sdcc
ASM = gpasm
DOXYGEN=doxygen

ASMFLAGS = -c
CFLAGS = -mpic16 -p$(PIC) --use-non-free
ifdef CRT
CFLAGS += --use-crt=$(CRT)
endif
LDFLAGS = $(CFLAGS) --lib-path $(LIBRARYPATH) -l libc18f.lib libio$(PIC).lib libdev$(PIC).lib libsdcc.lib
ifdef LINKERSCRIPT
LDFLAGS += -Wl'-s $(LINKERSCRIPT)'
endif

OBJ = $(patsubst %.c,%.o,$(SRC))
ASMOBJ = $(patsubst %.asm,%.o,$(SRCASM))
LSTS = $(patsubst %.c,%.lst,$(SRC)) $(patsubst %.asm,%.lst,$(SRCASM))
ASMS = $(patsubst %.c,%.asm,$(SRC))

all: $(NAME).hex

$(NAME).hex : $(OBJ) $(ASMOBJ)
	@echo -n "Linking ... "
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(ASMOBJ)
	@echo "done"

$(ASMOBJ): %.o: %.c Makefile
	@echo "Assembling ... $<"
	@$(ASM) $(ASMFLAGS) $<

$(OBJ): %.o: %.c Makefile
	@echo "Compiling ... $<"
	@$(CC) $(INCLUDE) $(CFLAGS) -S -c $< -o $(patsubst %.c,%.asm,$<)
	@$(ASM) $(ASMFLAGS) $*.asm

clean :
	@rm -f $(ASMS) $(LSTS) $(OBJ) $(ASMOBJ) $(NAME).lst $(NAME).hex $(NAME).cod
