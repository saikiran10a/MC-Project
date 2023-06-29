/* 
 * File: at24c04.c
 * Name: A Saikiran
 */

#include <xc.h>
#include"at24c04.h"
#include"i2c.h"
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)

/* Function to read into external EEPROM */
unsigned char eeprom_at24c04_read(unsigned char addr)
{
    /* Declaration of local variable */
    unsigned char data;
    
    /* Sending start bit in I2C bus */
    i2c_start();
    
    /* Sending the Slave address byte with write bit LSB */
    i2c_write(SLAVE_WRITE_EEPROM); 
    /* Writing the address to the I2C line to read the data from that address */
    i2c_write(addr);
    /* Restarting the I2C bus */
    i2c_rep_start();
    /* Sending the Slave address byte with read bit LSB */
    i2c_write(SLAVE_READ_EEPROM);
    
    /* Reading the data from SSPBUFF */
    data = i2c_read(0);
    
    /* Sending Stop bit to make ready the I2C bus for further transmissions */
    i2c_stop();
    
    return data;
}

/* Function to write into external EEPROM */
void eeprom_at24c04_byte_write(unsigned char addr,  char data)
{
    /* Sending start bit in I2C bus */
    i2c_start();
    
    /* Sending the Slave address byte with write bit LSB */
    i2c_write(SLAVE_WRITE_EEPROM);
    
    /* Writing the address to the slave to write the data in that address */
    i2c_write(addr);
    
    /* Writing the data to the slave */
    i2c_write(data);
    
    /* Sending Stop bit to make ready the I2C bus for further transmissions */
    i2c_stop();
}

/* Function to write a string into EEPROM*/
void eeprom_at24c04_str_write(unsigned char addr , char *data)
{
    while(*data != 0)
    {
        eeprom_at24c04_byte_write(addr,*data);
        data++;
        addr++;
    }
 }

