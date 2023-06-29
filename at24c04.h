/* 
 * File:   at24c04.h
 * Name: A Saikiran
 */

#ifndef EEPROM_H
#define	EEPROM_H

/* Macros for external EEPROM address for read and write modes */
#define SLAVE_WRITE_EEPROM             0xA0
#define SLAVE_READ_EEPROM              0xA1

/* Function prototypes */
void eeprom_at24c04_str_write(unsigned char addr, char *data);
unsigned char eeprom_at24c04_read(unsigned char addr);
void eeprom_at24c04_byte_write(unsigned char addr,  char data);

#endif	/* EEPROM_H */

