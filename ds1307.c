/* 
 * File : ds1307.c
 * Name : A Saikiran
 */

#include <xc.h>
#include "i2c.h"
#include "ds1307.h"

/* Initialize ds1307 */
void init_ds1307(void)
{
    unsigned char dummy;
   
    /* Read and write into the initial address */
    dummy = read_ds1307(SEC_ADDR);
    dummy = dummy & 0x7F;
    write_ds1307(SEC_ADDR, dummy);// ch = 0
}

/* Function to Read data from the RTC address */
unsigned char read_ds1307(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

/* Function to write data into the RTC address */
void write_ds1307(unsigned char addr, unsigned char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(SLAVE_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}
