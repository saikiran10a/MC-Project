/*
 * File : isr.c
 * Name : A Saikiran
 */ 
#include <xc.h>
#include "main.h"

/* Extern variables from other files */
extern int sec, return_time, min;
void __interrupt() isr(void)
{
    /* Declare variables */
    static unsigned long int count = 0;
    
    /* Check if timer flag is set or not */
    if (TMR2IF == 1)
    {
        /* Interrupt for one second */
        if (++count == 1250)
        {
            /* Decrement min and sec */
            if (min > 0)
            {
                sec--;
                if (sec == 0)
                {
                    min--;
                    sec = 60;
                }
            }
            /* Decrement return_time */
            else if (min == 0 && return_time > 0)
                return_time--;
            count = 0;
        }
        
        TMR2IF = 0; 
    }
}