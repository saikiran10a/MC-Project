/* 
 * File:   uart.h
 * Name: A Saikiran
 */

#ifndef UART_H
#define	UART_H

#define FOSC                20000000

/* Function prototype to initialize UART and getchar, putchar and puts */
void init_uart(unsigned long baud);
unsigned char getchar(void);
void putchar(unsigned char data);
void puts(const char *s);

#endif	/* UART_H */