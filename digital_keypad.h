/* 
 * File:   digital_keypad.h
 * Author: A Saikiran
 */

#ifndef DIGITAL_KEYPAD_H
#define	DIGITAL_KEYPAD_H

/* Create macro for TRISB and PORTB */
#define LEVEL_DETECTION    0
#define LEVEL              0
#define STATE_DETECTION    1
#define STATE              1
#define KEYPAD_PORT        PORTB
#define KEYPAD_PORT_DDR    TRISB

#define INPUT_LINES        0x3F

/* Create macro for all 6 switches along with long-press switches */
#define SW1                0x3E
#define SW2                0x3D
#define SW3                0x3B
#define SW4                0x37
#define SW5                0x2F
#define SW6                0x1F
#define LPSW4              0x11
#define LPSW5              0x22

#define  ALL_RELEASED       INPUT_LINES  

/* Function prototype to read switch and initialize configuration */
unsigned char read_digital_keypad(unsigned char mode);
void init_digital_keypad(void);

#endif	/* DIGITAL_KEYPAD_H */