/*
    Hurricane driverboard
 
    main.c:
 
    copyright:
              Copyright (c) 2010 Bastiaan van Kesteren <bas@edeation.nl>
 
              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.
 
 
 
    remarks:
 
 */
/*!
 * \file
 * Main code
 */
#include "config.h"
#include "cpusettings.h"

#include "io.h"
#include "eeprom.h"
#include "serial.h"
#include "delay.h"
#include "motor.h"
#include "joystick.h"

#include <pic16/signal.h>
#include <pic16/usart.h>
#include <pic16/stdio.h>
#include <pic16/stdlib.h>

/* Settings */
#define JOYSTICK_BUTTON_DEBOUNCE_PRESCALER_VALUE    3       //+/-96ms
#define BUTTON_DEBOUNCE_PRESCALER_VALUE             3       //+/-96ms
#define BUTTON_SCANRATE                             1       //+/-32ms
#define LEDBLINK_PRESCALER                          17      //+/-544ms
#define CALIBRATE_CENTERSETTLE_PRESCALER            51      //+/-1632ms
#define UART_RAWDATASIZE                            250

/* buttons (var 'buttonstate') */
#define BUTTON_NORTH            (1<<0)
#define BUTTON_EAST             (1<<1)
#define BUTTON_SOUTH            (1<<2)
#define BUTTON_WEST             (1<<3)

/* Joystick buttons (var 'joystick_buttonstate') */
#define BUTTON_TRIGGER          (1<<0)
#define BUTTON_TOP              (1<<1)

DEF_INTHIGH(high_int) /* begin the definition of the interrupt dispatch table for high priority interrupts. name is the
function name to use. */

DEF_HANDLER(SIG_TMR0, _tmr0_handler)    /* define a handler for signal tmr0 (motor driver) */

END_DEF               /* end the declaration of the dispatch table. */


DEF_INTLOW(low_int)  /* begin the definition of the interrupt dispatch table for low priority interrupts. name is the
function name to use. */

DEF_HANDLER(SIG_AD,	  _ad_handler)      /* define a handler for signal ad (joystick X/Y) */
DEF_HANDLER(SIG_TMR1, _tmr1_handler)    /* define a handler for signal tmr1 (system tick) */
DEF_HANDLER(SIG_INT1, _int1_handler)    /* define a handler for signal int1 (joystick trigger) */
DEF_HANDLER(SIG_INT2, _int2_handler)    /* define a handler for signal int2 (joystick button) */
DEF_HANDLER(SIG_RC,   _rc_handler)      /* define a handler for signal rc (uart receive) */
DEF_HANDLER(SIG_RB,   _rb_handler)      /* define a handler for signal rb (buttons) */
DEF_HANDLER(SIG_BCOL, _bcol_handler)    /* define a handler for signal bus-collision (panic) */
DEF_HANDLER(SIG_LVD , _lvd_handler )    /* define a handler for signal low-voltage detect (panic) */

END_DEF               /* end the declaration of the dispatch table. */

/* Global state variable */
unsigned char state;

/* pushed buttons */
unsigned char buttonstate;

/* pushed joystick buttons */
unsigned char joystick_buttonstate;  

/* Do we have a joystick? */
bool joystick_active;

/* And is it calibrated? */
bool joystick_calibrated;

/* When set (from timer interrupt), the buttons are read and handled, and this flag cleared */
bool button_scan_flag;

#define GUIDELEDS_MODE_RANDOM       0
#define GUIDELEDS_MODE_TIME         1
/* What to do with the guide LED's */
char guideleds_mode;
char guideleds_on;

/* Joystick calibration data */
extern calibrationdata_t calibrationdata;

/* And the possible states */
#define STATE_NOJOYSTICK            0
#define STATE_CALIBRATING_MINMAX    1
#define STATE_CALIBRATING_CENTER    2
#define STATE_CALIBRATED            3
#define STATE_JOYSTICK              4

/* Incoming, unparsed, UART data */
unsigned char uart_rawdata[UART_RAWDATASIZE];
unsigned char uart_pointer;

/* Random seed */
unsigned char randomkey;

void handle_joystick(void);
void handle_buttons(void);
void start_joystick(void);
unsigned char random(void);

void init(void)
/*!
 *  Initialise system
 */
{
    #if INTOSC
    OSCCON &= 0x8F;
    #if FOSC == 8000000
    OSCCON |= 0x70;
    #elif FOSC == 4000000
    OSCCON |= 0x60;
    #elif FOSC == 2000000
    OSCCON |= 0x50;
    #elif FOSC == 1000000
    OSCCON |= 0x40;
    #elif FOSC == 500000
    OSCCON |= 0x30;
    #elif FOSC == 250000
    OSCCON |= 0x20;
    #elif FOSC == 125000
    OSCCON |= 0x10;
    #elif FOSC == 31000
    OSCCON |= 0x0;
    #else
    #error "Internal oscillator doesn't support selected frequency"
    #endif /* FOSC select */
    #if PLL
    #if FOSC == 8000000 || PLL == 4000000
    OSCTUNEbits.PLLEN = 1;
    #else
    #error When using the interal oscillator, the PLL is only usable at 8 and 4MHz
    #endif
    #endif /* PLL */
    #endif /* INTOSC */
    #if WATCHDOG != 0
    WDTCONbits.SWDTEN = 1;
    #endif
    
    ADCON1 = 0x0F;      /* Disable ADC */
    CMCON = 0x07;       /* Disable comperator */
    CVRCON = 0x00;      /* Disable comperator voltage reference */
    
    /* PORT A I/O usage:
     *        0       i       Joystick X
     *        1       i       Joystick Y
     *        2       i       -
     *        3       i       -
     *        4       i       Guidesensor 0
     *        5       i       Guidesensor 1
     *        6       i       -
     *        7       i       - */
    TRISA = 0xFF;       // 0 = output, 1 = input
    PORTA = 0;
    
    /* PORT B I/O usage:
     *        0       i       -
     *        1       i       Joystick button
     *        2       i       Joystick trigger
     *        3       i       -
     *        4       i       Button west
     *        5       i       Button south (and ICSP PGM)
     *        6       i       Button east (and ICSP PGC)
     *        7       i       Button north (and ICSP PGD) */
    TRISB = 0xFF;       // Set = input, clear = output
    PORTB = 0;
    INTCON2bits.RBPU = 0; //Enable pullups
    
    /* PORT C I/O usage:
     *        0       o       Guide LED 0
     *        1       o       Guide LED 1
     *        2       o       Guide LED 2
     *        3       o       Guide LED 3
     *        4       o       Guide lED 4
     *        5       i       -
     *        6       i       -
     *        7       i       - */
    TRISC = 0xE0;       // Set = input, clear = output
    PORTC = 0x1F;
    
    /* PORT D I/O usage:
     *        0       o       status LED 0
     *        1       o       status LED 0
     *        2       i       -
     *        3       i       -
     *        4       o       North
     *        5       o       East
     *        6       o       South
     *        7       o       West */
    TRISD = 0x0C;       // Set = input, clear = output
    PORTD = 0;
    
    /* PORT E I/O usage:
     *        0       i       Guidesensor 2
     *        1       i       Guidesensor 3
     *        2       i       Guidesensor 4
     *        3       i       MLCR */
    TRISE = 0x0F;       // Set = input, clear = output
    PORTE = 0;
}

void main(void)
/*!
 *  The main function:
 *  Initialises the system, and starts all interrupt based tasks.
 */
{
    unsigned char uart_pointer_handled=0;
    int i;
    unsigned char checksum;
    
    /* Low level init; oscillator, watchdog, I/O */
    init();
    
    /* Configure external interrupts */
    INTCON2bits.INTEDG1 = 0;    /* Interrupt on falling edge */
    INTCON2bits.INTEDG2 = 0;    /* Interrupt on falling edge */
    INTCON3bits.INT1IP = 0;     /* Make external interrupt 1 a low priority interrupt */
    INTCON3bits.INT1IE = 1;     /* Enable external interrupt 1 */
    INTCON3bits.INT2IP = 0;     /* Make external interrupt 2 a low priority interrupt */
    INTCON3bits.INT2IE = 1;     /* Enable external interrupt 2 */
    /* And enable them */
    INTCON3bits.INT1IF = 0;     /* Clear interrupt flag */
    INTCON3bits.INT1IE = 1;     /* And enable it */
    INTCON3bits.INT2IF = 0;     /* Clear interrupt flag */
    INTCON3bits.INT2IE = 1;     /* And enable it */
    
    /* Configure timer1, which is used as the overall system tick */
    T1CONbits.RD16 = 1;         /* Read/write timer as one 16 bits value */
    T1CONbits.T1CKPS1 = 0;      /* Use a 1:1 prescaler */
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1OSCEN = 0;      /* Disable timer1 oscillator */
    T1CONbits.TMR1CS = 0;       /* Use internal clock as source */
    TMR1H = 0;                  /* Set timer to 0; this gives us a timer interval of 32.768mS */
    TMR1L = 0;
    T1CONbits.TMR1ON = 0;       /* Disable timer (we don't start it just yet) */
    IPR1bits.TMR1IP = 0;        /* Low priority */
    PIE1bits.TMR1IE = 1;        /* Enable timer0 interrupt (note that the timer is still disabled, so nothing will happen) */
    
    /* Configure timer0, which is used by random() */
    TMR0L = 0;
    INTCONbits.TMR0IE = 0;      /* Disable timer0 interrupt */
    T0CONbits.T08BIT = 1;       /* 8 bit timer */
    T0CONbits.T0CS = 0;         /* Use instruction cycle clock */
    T0CONbits.TMR0ON = 1;       /* Start timer */
    
    /* Port B 4:7 interrupt-on-change (buttons) */
    INTCONbits.RBIF = 0;        /* Clear interrupt flag */
    INTCON2bits.RBIP = 0;       /* Make low priority */
    INTCONbits.RBIE = 1;        /* Enable interrupt */
    
    motor_init();
    joystick_init();
    
    randomkey = joystick_getX()%255;
    
    /* Reset state */
    state = STATE_NOJOYSTICK;
    
    /* Initialise UART */
    uart_pointer = 0;
    serial_init();
    
    debug("Hurricane driverboard, build %s,%s\n\r",__DATE__, __TIME__);
    
    /* Enable interrupts */
    RCONbits.IPEN = 1;          /* enable priority interrupts */
    INTCONbits.GIE = 1;         /* enable global interrupts */
    INTCONbits.PEIE = 1;        /* enable peripheral interrupts */
    
    /* All hardware is now initialised, time for a little I/O test */
    guideled2_on();
    
    guideled3_on();
    guideled4_on();
    statusled0_on();
    motor_setspeed(0,100);
    delay_ms(250);
    motor_setspeed(0,0);
    
    guideled4_off();
    guideled0_on();
    statusled1_on();
    motor_setspeed(1,100);
    delay_ms(250);
    motor_setspeed(1,0);
    
    guideled3_off();
    guideled1_on();
    statusled0_off();
    motor_setspeed(2,100);
    delay_ms(250);
    motor_setspeed(2,0);
    
    guideled0_off();
    guideled4_on();
    statusled1_off();
    motor_setspeed(3,100);
    delay_ms(250);
    motor_setspeed(3,0);
    
    guideled4_off();
    guideled1_off();
    guideled2_off();
    
    /* Read calibration data from EEPROM */
    checksum = 0;
    for(i=0;i<sizeof(calibrationdata);i++) {
        ((unsigned char *)(&calibrationdata))[i] = eeprom_readbyte(i);
        checksum += ((unsigned char *)(&calibrationdata))[i];
    }
    checksum += eeprom_readbyte(i);
    
    if(checksum != 0xFF || (joystick_buttonstate == BUTTON_TRIGGER && joystick_trigger_state())) {
        /* EEPROM data corrupt, or calibration overrule (trigger pushed during boot) */
        calibrationdata.minX = 0xFFFF;
        calibrationdata.minY = 0xFFFF;
        calibrationdata.centerX = 0;
        calibrationdata.centerY = 0;
        calibrationdata.maxX = 0;
        calibrationdata.maxY = 0;
        
        if(checksum != 0xFF) {
            debug("EEPROM data corrupt, calibration required\n\r");
        }
        else {
            debug("Trigger pushed during boot, discarding calibration data\n\r");
        }
        joystick_calibrated = FALSE;
    }
    else {
        /* We're calibrated, so start of runnin' */
        state = STATE_JOYSTICK;
        statusled0_on(); 	// joystick calibrated
        joystick_calibrated = TRUE;
        debug("Joystick calibrated: X min: %d, center: %d, max: %d\n\r", calibrationdata.minX, calibrationdata.centerX, calibrationdata.maxX);
        debug("                     Y min: %d, center: %d, max: %d\n\r", calibrationdata.minY, calibrationdata.centerY, calibrationdata.maxY); 
    }
        
    /* OK, ready to rock */
    T1CONbits.TMR1ON = 1;       /* enable timer1 */
    
    /* Assume we don't have a joystick connected (or the user(s) don't want to use it);
     * we start reading X/Y values after one of the buttons has been pushed */
    joystick_active = FALSE;
        
    /* Whe start with the random guideLED mode; all LED's are on initially. When the boat goes over a LED, it's turned off. Once all LED's
     * are of, a random LED is turned on. When the boat goes over this LED, it's turned off, and another one picked at random is turned on,
     * and so on. */
    guideleds_mode = GUIDELEDS_MODE_RANDOM;
    guideled0_on();
    guideled1_on();
    guideled2_on();
    guideled3_on();
    guideled4_on();
    guideleds_on = 5;
        
    /* And loop; handle incoming and outgoing data */
    while(1) {
        /* Handle incoming data */
        while(uart_pointer_handled != uart_pointer) {
            /* Handle data */
            /*if(uart_rawdata[uart_pointer_handled] == 'u' && i < 100) {
                i++;
                motor_setspeed(0,i);
                debug("up: %d\n\r", i);
            }
            else if(uart_rawdata[uart_pointer_handled] == 'd' && i >0) {
                i--;
                motor_setspeed(0,i);
                debug("down: %d\n\r", i);
            }*/
                
            /* And move on to next char, if any.. */
            uart_pointer_handled++;
            if(uart_pointer_handled >= UART_RAWDATASIZE) {
                uart_pointer_handled = 0;
            }
            CLEAR_WATCHDOG();
        }
            
        if(state == STATE_CALIBRATED) {
            /* Joystick calibration done, time to write calibration data to the EEPROM */
            checksum = eeprom_writebyte(0,calibrationdata.minX);
            checksum += eeprom_writebyte(1,calibrationdata.minX>>8);
            checksum += eeprom_writebyte(2,calibrationdata.minY);
            checksum += eeprom_writebyte(3,calibrationdata.minY>>8);
            checksum += eeprom_writebyte(4,calibrationdata.centerX);
            checksum += eeprom_writebyte(5,calibrationdata.centerX>>8);
            checksum += eeprom_writebyte(6,calibrationdata.centerY);
            checksum += eeprom_writebyte(7,calibrationdata.centerY>>8);
            checksum += eeprom_writebyte(8,calibrationdata.maxX);
            checksum += eeprom_writebyte(9,calibrationdata.maxX>>8);
            checksum += eeprom_writebyte(10,calibrationdata.maxY);
            checksum += eeprom_writebyte(11,calibrationdata.maxY>>8);
            eeprom_writebyte(12,0xFF-checksum);
            state = STATE_JOYSTICK;
            debug("Joystick calibrated: X min: %d, center: %d, max: %d\n\r", calibrationdata.minX, calibrationdata.centerX, calibrationdata.maxX);
            debug("                     Y min: %d, center: %d, max: %d\n\r", calibrationdata.minY, calibrationdata.centerY, calibrationdata.maxY);
        }
            
        /* This is why we're doing all this; the motors! */
        if(joystick_active && joystick_calibrated) {
            /* Joystick is free-running; update as fast as we can cycle. */
            handle_joystick();
        }
        else {
            /* Button is timer-based; 
             * this gives us a sane way to prevent powersupply overloads */
            if(button_scan_flag) { 
                handle_buttons();
                button_scan_flag = 0;
            }
        }
            
        /* Update guideLED's */
        if(guidesensor0_state() == 0) {
            guideled0_off();
            guideleds_on--;
        }
        if(guidesensor1_state() == 0) {
            guideled1_off();
            guideleds_on--;
        }
        if(guidesensor2_state() == 0) {
            guideled2_off();
            guideleds_on--;
        }
        if(guidesensor3_state() == 0) {
            guideled3_off();
            guideleds_on--;
        }
        if(guidesensor4_state() == 0) {
            guideled4_off();
            guideleds_on--;
        }
            
        if(guideleds_on <= 0) {
            if(guideleds_mode == GUIDELEDS_MODE_RANDOM) {
                i=random()%5;
                debug("At random picked LED %d\n\r", i);
                /* Pick another LED at random */
                switch(i) {
                    case 0:
                        guideled0_on();
                        break;
                    case 1:
                        guideled1_on();
                        break;
                    case 2:
                        guideled2_on();
                        break;
                    case 3:
                        guideled3_on();
                        break;
                    case 4:
                        guideled4_on();
                        break;
                }
                guideleds_on = 1;
            }
            else {
                /* ToDo */
                    
            }
        }
        
        CLEAR_WATCHDOG();
    }
}

void handle_joystick(void)
{
    static int lastx=0, lasty=0;
    int x,y;
    
    x = joystick_getX();
    if(x != lastx) {
        if(x < 0) {
            motor_setspeed(0,0-x);
            motor_setspeed(2,0);
        }
        else if(x > 0) {
            motor_setspeed(0,0);
            motor_setspeed(2,x);
        }
        else {
            motor_setspeed(0,0);
            motor_setspeed(2,0);
        }
    }
    
    y = joystick_getY();
    if(y != lasty) {
        if(y < 0) {
            motor_setspeed(3,0-y);
            motor_setspeed(1,0);
        }
        else if(y > 0) {
            motor_setspeed(3,0);
            motor_setspeed(1,y);
        }
        else {
            motor_setspeed(3,0);
            motor_setspeed(1,0);
        }
    }
    
    if(x != lastx || y != lasty) {
        debug("X: %d, Y: %d\n\r",x,y);
        lastx = x;
        lasty = y;
    }
}

void handle_buttons(void)
{
    static int north=0, east=0, south=0, west=0;
    static unsigned char last_buttonstate = 0;
    char motorsstarting = 0;	// Don't start more than one side at a time (prevents power supply overload)
    
    if(buttonstate & BUTTON_NORTH) {
        if(north < 100) {
            north+=10;
            motorsstarting = 1;
            motor_setspeed(0,north);
        }
    }
    else if (north != 0){
        north = 0;
        motor_setspeed(0,0);
    }
    
    if(buttonstate & BUTTON_EAST) {
        if(motorsstarting == 0) {
            if(east < 100) {
                east+=10;
                motorsstarting = 1;
                motor_setspeed(1,east);
            }
        }
    }
    else if(east != 0){
        east = 0;
        motor_setspeed(1,0);
    }
    
    if(buttonstate & BUTTON_SOUTH) {
        if(motorsstarting == 0) {
            if(south < 100) {
                south+=10;
                motorsstarting = 1;
                motor_setspeed(2,south);
            }
        }
    }
    else if(south != 0){
        south = 0;
        motor_setspeed(2,0);
    }
    
    if(buttonstate & BUTTON_WEST) {
        if(motorsstarting == 0) {
            if(west < 100) {
                west+=10;
                motorsstarting = 1;
                motor_setspeed(3,west);
            }
        }
    }
    else if(west != 0) {
        west = 0;
        motor_setspeed(3,0);
    }
    
    if(buttonstate != last_buttonstate) {
        debug("buttonchange: %d\n\r", buttonstate);
        last_buttonstate = buttonstate;
    }
}

void start_joystick(void)
{
    if(joystick_active == FALSE) {		
        /* Do initial joystick ADC read; when the read is done, the ad interrupt
         * fires, which will read the result and start the next conversion, again
         * triggering the ad interrupt, ... */
        joystick_go();  
        joystick_active = TRUE;
        statusled1_on();	// joystick active
    }
    
    if(joystick_calibrated == FALSE) {
        joystick_calibrate(!joystick_calibrating());
        
        if(joystick_calibrating()) {
            state = STATE_CALIBRATING_MINMAX;
        }
        else {
            state = STATE_CALIBRATING_CENTER;
        }
    }
}

unsigned char random(void)
/*
 *  Generate a 'random' value
 *  Code is based on a paper from Deva Seetharam and Sokwoo Rhee,
 *   "An Efficient Random Number Generator for Low-Power Sensor Networks"
 *  The (original) generator was tested with ENT (http://www.fourmilab.ch/random/) with pretty good results..
 */
{
    unsigned char rv = 0;
    unsigned char tv = 0;
    tv = TMR0L;
    
    rv = tv ^ randomkey;
    randomkey = ~tv;
    tv = ~rv;
    TMR0L = tv;
    
    /* rv is now somewhere between 0 and 255 */
    return rv;
}

SIGHANDLER(_tmr0_handler)
/*
 *  Drive the motors
 */
{
    /* Clear timer interrupt flag */
    INTCONbits.T0IF = 0;
    
    /* Update motors */
    motor_update();
}

SIGHANDLER(_ad_handler)
{
    /* Clear AD converter interrupt flag */
    PIR1bits.ADIF = 0;
    
    joystick_tick();
}

SIGHANDLER(_tmr1_handler)
/*!
 *  Timer1, triggers every 32.768 ms
 */
{
    static unsigned char joystick_button_debounce_prescaler=0;
    static unsigned char button_debounce_prescaler=0;
    static unsigned char button_scanrate_prescaler=0;
    static unsigned char ledblink_prescaler=0;
    static unsigned char calibrate_centersettle_prescaler=0;
    static bool ledblink=FALSE;
    
    /* Clear timer interrupt flag */
    PIR1bits.TMR1IF = 0;
    
    /* Reload timer */
    TMR0H = 0;
    TMR0L = 0;
    
    /* Joystick button debounce */
    switch(joystick_buttonstate) {
        case BUTTON_TRIGGER:
            /* Trigger pushed! */
            if(joystick_trigger_state() == 0) {
                /* No it ain't; bouncing line! */
                joystick_button_debounce_prescaler = 0;
                joystick_buttonstate &= ~BUTTON_TRIGGER;
                INTCON3bits.INT1IF = 0;
                INTCON3bits.INT1IE = 1;
            }
            else {
                joystick_button_debounce_prescaler++;
                if(joystick_button_debounce_prescaler == JOYSTICK_BUTTON_DEBOUNCE_PRESCALER_VALUE) {
                    joystick_button_debounce_prescaler = 0;
                    joystick_buttonstate &= ~BUTTON_TRIGGER;
                    INTCON3bits.INT1IF = 0;
                    INTCON3bits.INT1IE = 1;
                    debug("Joystick trigger\n\r");
                    start_joystick();
                }
            }
            break;
        case BUTTON_TOP:
            /* Topbutton pushed! */
            if(joystick_button_state() == 0) {
                /* No it ain't; bouncing line! */
                joystick_button_debounce_prescaler = 0;
                joystick_buttonstate &= ~BUTTON_TOP;
                INTCON3bits.INT2IF = 0;
                INTCON3bits.INT2IE = 1;
            }
            else {
                joystick_button_debounce_prescaler++;
                if(joystick_button_debounce_prescaler == JOYSTICK_BUTTON_DEBOUNCE_PRESCALER_VALUE) {
                    joystick_button_debounce_prescaler = 0;
                    INTCON3bits.INT2IF = 0;
                    INTCON3bits.INT2IE = 1;
                    joystick_buttonstate &= ~BUTTON_TOP;
                    debug("Joystick button\n\r");
                    start_joystick();
                }
            }
            break;
    }
    
    switch(state) {
        case STATE_CALIBRATING_MINMAX:
            ledblink_prescaler++;
            if(ledblink_prescaler == LEDBLINK_PRESCALER) {
                ledblink_prescaler = 0;
                if(ledblink) {
                    statusled0_on();
                    ledblink=FALSE;
                }
                else {
                    statusled0_off();
                    ledblink=TRUE;
                }
            }
            break;
        case STATE_CALIBRATING_CENTER:
            statusled0_on();
            ledblink_prescaler++;
            if(ledblink_prescaler == LEDBLINK_PRESCALER) {
                ledblink_prescaler = 0;
                if(ledblink) {
                    statusled1_on();
                    ledblink=FALSE;
                }
                else {
                    statusled1_off();
                    ledblink=TRUE;
                }
            }
            calibrate_centersettle_prescaler++;
            if(calibrate_centersettle_prescaler == CALIBRATE_CENTERSETTLE_PRESCALER) {
                joystick_setcenter();
                calibrate_centersettle_prescaler = 0;
                state = STATE_CALIBRATED;
                joystick_calibrated = TRUE;
                statusled1_on();
            }
        case STATE_NOJOYSTICK:
            /* fall through */
            case STATE_JOYSTICK:
                if(joystick_active == FALSE && button_scan_flag == FALSE) {
                    button_scanrate_prescaler++;
                    if(button_scanrate_prescaler == BUTTON_SCANRATE) {
                        button_scan_flag = TRUE;
                        button_scanrate_prescaler = 0;
                    }
                }
    }
}

SIGHANDLER(_rc_handler)
/*!
 *  Uart RX
 */
{
    while(PIR1bits.RCIF) {
        if(RCSTAbits.OERR) {
            /* Overrun error. Clear by toggeling RCSTA.CREN */
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        else if(RCSTAbits.FERR) {
            /* Framing error. Clear by reading RCREG */
            uart_rawdata[uart_pointer] = RCREG;
        }
        else {
            uart_rawdata[uart_pointer] = RCREG;
            uart_pointer++;
            if(uart_pointer >= UART_RAWDATASIZE) {
                uart_pointer = 0;
            }
        }
    }
}

SIGHANDLER(_int1_handler)
/*!
 *  Joystick trigger
 */
{
    /* Clear interrupt flag */
    INTCON3bits.INT1IF = 0;
    
    /* This interrupt is disabled by clearing INTCON3bits.INT1IE, for the debouncing.
     * The SDCC generated interrupt table doesn't check these bits, so we have to do that here. */
    if(INTCON3bits.INT1IE) {
        if(joystick_buttonstate == 0) {
            /* Disable interrupt; it will be re-enabled in the timer0 handler (debounce handling) */
            INTCON3bits.INT1IE = 0;
            
            joystick_buttonstate |= BUTTON_TRIGGER;
        }
    }
}

SIGHANDLER(_int2_handler)
/*!
 *  Joystick button
 */
{
    /* Clear interrupt flag */
    INTCON3bits.INT2IF = 0;
    
    /* This interrupt is disabled by clearing INTCON3bits.INT2IE, for the debouncing.
     * The SDCC generated interrupt table doesn't check these bits, so we have to do that here. */
    if(INTCON3bits.INT2IE) {
        if(joystick_buttonstate == 0) {
            /* Disable interrupt; it will be re-enabled in the timer0 handler (debounce handling) */
            INTCON3bits.INT2IE = 0;
            
            joystick_buttonstate |= BUTTON_TOP;
        }
    }
}

SIGHANDLER(_rb_handler)
/*!
 *  Button(s) pushed or released
 */
{
    unsigned char value,i,pushedbuttons;
    
    /* Clear interrupt flag */
    INTCONbits.RBIF = 0;
    
    /* Get state of buttons (note that the buttons are active low!) */
    value = ~(PORTB>>4) & 0xF;
    
    /* Clear out released buttons */
    buttonstate &= value;
    
    /* Count no. of pushed buttons; we never have more than two sides active */
    pushedbuttons = 0;
    for(i=1;i<16;i<<=1) {
        if(buttonstate & i) {
            pushedbuttons++;
        }
    }
    
    /* Right, now that we know the amount of pushed buttons, we can add the currently pushed buttons,
     * aslong as the total remains lesser than or equal to 2 */
    i=0;
    while(pushedbuttons <2 && i<4) {
        if((value & (1<<i)) && !(buttonstate & (1<<i))) {
            buttonstate |= (1<<i);
            pushedbuttons++;
        }
        i++;
    }
}

SIGHANDLERNAKED(_lvd_handler)
/*!
 *  Action to be taken when a low voltage detect occurs (we do a software reset)
 */
{
    __asm reset __endasm;
}

SIGHANDLERNAKED(_bcol_handler)
/*!
 *  Action to be taken when a bus-collision occurs (we do a software reset)
 */
{
    __asm reset __endasm;
}
