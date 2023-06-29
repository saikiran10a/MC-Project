/*
 * File:   car_black_box_1.c
 * Author: A Saikiran
 */
#include "main.h"
#include <xc.h>

/* Global Variable declaration */
unsigned char clock_reg[3];
char time[7]; // "HHMMSS" - 7
char log[11]; //HH:MM:SS Event Speed
char pos = -1;
int access = 0, sec, min, return_time;
char *menu[] = {"View log", "Clear log", "Download log", "Set time", "Change passwd"};
int timer_flag = 1;

/* Function to get time from RTC */
void get_time(void)
{
    /* Read time from ds1307 register */
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    /* Convert BCD to ascii */
    // HH  
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

/* Function to display time in CLCD */
void display_time(unsigned char addr)
{
    /* Get time */
    if (timer_flag == 1)
    {
        get_time();
    }
    
    /* Display time HH:MM:SS */
    //HH
    clcd_putch(time[0], addr++);
    clcd_putch(time[1], addr++);
    clcd_putch(':', addr++);
    //MM
    clcd_putch(time[2], addr++);
    clcd_putch(time[3], addr++);
    clcd_putch(':', addr++);
    //SS
    clcd_putch(time[4], addr++);
    clcd_putch(time[5], addr);
}

/* Function definition to Reset time */
void set_time( unsigned char key)
{
    /* Declare variables */
    static int delay = 0, index = 5; static char flag = 0xC7;
    /* Print in CLCD line 1 */
    clcd_print("TIME (HH:MM:SS) ", LINE1(0));
    
    /* Display time */
    if (timer_flag)
    {
        get_time();
        timer_flag = 0;
        
        display_time(0xC0);
    }
    /* If Switch 4 is pressed increment the values based on field selected */
    if (key == SW4)
    {
        /* If fields are seconds or minutes */
        if (index == 3 || index == 5)
        {
            time[index]++;
            if (time[index] > '9')
            {
                time[index] = '0';
                time[index - 1]++;
                if (time[index - 1] > '5')
                {
                    time[index - 1] = '0';
                }
            }
        }
        /* For hour */
        else if (index == 1)
        {
            time[1]++;
            if (time[1] > '9')
            {
                time[1] = '0';
                time[0]++;
                if (time[0] > '2')
                {
                    time[0] = '0';
                }
            }
            /* If reaches 2 in tens position and 4 in units position */
            if (time[0] == '2' && time[1] == '4')
            {
                time[0] = '0';
                time[1] = '0';
            }
        }
        display_time(0xC0);
    }
    /* If switch 5 is pressed change field */
    if (key == SW5)
    {
        index -= 2;
        flag -= 3;
        if (index <= 0)
        {
            index = 5;
        }
        if (flag < 0xC0)
        {
            flag = 0xC7;
        }
        display_time(0xC0);
    }
    /* Delay for selected field */
    if (delay++ == 7)
    {
        clcd_print("  ", flag - 1);
    }
    else if (delay == 14)
    {
        display_time(0xC0);
        delay = 0;
    }
}

/* Function definition to display dashboard */
void display_dash_board(unsigned char event[], unsigned char speed)
{
    /* Display title line in LINE1 */
    clcd_print("TIME     E  SP", LINE1(2));
    
    /* Display time using RTC */
    display_time(0xC2);
    /* Display Event */
    clcd_print(event, LINE2(11));
    
    /* Display speed */
    clcd_putch(speed / 10 + '0', LINE2(14));
    clcd_putch(speed % 10 + '0', LINE2(15));
}

/* Function definition  to store log events*/
void log_car_event(void)
{
    char addr, flag = 1;
    addr = 0x05;
    pos++;
    if (pos == 10)
    {
        pos = 0;
    }
    
    addr = pos * 10 + addr;
    
    /* Store the log events values in EEPROM */
    eeprom_at24c04_str_write(addr,log);
    if (access < 9)
    {
        access++;
    }
}

/* Function definition for logging the events  */
void log_event(unsigned char event[], unsigned char speed)
{
    static int once = 1;
    if (once)
    {
        get_time();
        once = 0;
    }
    /* Store information into log array */
    strncpy(log, time, 6);            //HHMMSS
    strncpy(&log[6], event, 2);       //Event
    log[8] = speed / 10 + '0';        //Speed
    log[9] = speed % 10 + '0';
    log[10] = '\0';
    
    log_car_event();
}

/* Function definition for Login */
unsigned char login(unsigned char reset_flag, unsigned char key)
{
    /* Declare variables */
    static char npassword[4], i;
    static unsigned char attempt_left;
    
    /* If reset flag is equal to reset password */
    if (reset_flag == RESET_PASSWORD)
    {
        i = 0;
        attempt_left = 3;
        npassword[0] = '\0';
        npassword[1] = '\0';
        npassword[2] = '\0';
        npassword[3] = '\0';
        key = ALL_RELEASED;
        sec = 0;
        min = 0;
        return_time = 3;
    }
    /* If return time is 0, then return back to Dash board */
    if (return_time == 0)
    {
        return RETURN_BACK;
    }
    /* If switch 4 is pressed print store 1 and display '*' */
    if (key == SW4 && i < 4) 
    {
        /* Store password in array and increment position */
        npassword[i] = '1';
        i++;
        clcd_putch('*', LINE2(4+i));
        return_time = 3;
    }
    /* If switch 5 is pressed print store 0 and display '*' */
    else if (key == SW5 && i < 4) 
    {
        /* Store password in array and increment position */
        npassword[i] = '0';
        i++;
        clcd_putch('*', LINE2(4+i));
        return_time = 3;
    }
    
    /* If 4 digit password is entered */
    if (i == 4)
    {
        __delay_ms(300);
        char spassword[4];
        /* Read system password from EEPROM */
        for (int j = 0; j < 4; j++)
        {
            spassword[j] = eeprom_at24c04_read(j);
        }
        /* Compare entered password with system password */
        if (strncmp(spassword, npassword, 4) == 0)
        {
            /* If correct, display success message and display main menu */
            clear_screen();
            clcd_print("Login Success", LINE1(2));
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_ms(1000);
            return LOGIN_SUCCESS;
        }
        /* If enterd password is wrong */
        else
        {
            /* Reduce number of attempts */
            attempt_left--;
            
            /* If no attempts left */
            if (attempt_left == 0)
            {
                /* Block user for 15 mins */
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are blocked", LINE1(0));
                clcd_print("min left", LINE2(6));
                clcd_putch(':', LINE2(2));
                /* Timer2 for 15min delay */
                sec = 60;
                min = 14;
                while(min)
                {
                    clcd_putch(sec / 10 + '0',LINE2(3));
                    clcd_putch(sec % 10 + '0',LINE2(4));
                    clcd_putch(min / 10 + '0',LINE2(0));
                    clcd_putch(min % 10 + '0',LINE2(1));
                }
                attempt_left = 3;
            }
            /* Display number of attempts left for every wrong entry */
            else
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Wrong password", LINE1(1));
                clcd_putch(attempt_left + '0', LINE2(0));
                clcd_print("attempts left", LINE2(2));
                __delay_ms(3000);
            }
            /* Enter the password again after 15 mins */
            clear_screen();
            clcd_print("Enter Password", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
            return_time = 3;
        }
    }
}
/* Function definition to display main menu */
unsigned char login_menu(unsigned char reset_flag, unsigned char key)
{
    static unsigned char menu_pos;
    
    /* If reset flag equal to reset password change position to 0 */
    if (reset_flag == RESET_PASSWORD)
    {
        clear_screen();
        menu_pos = 0;
    }
    /* If switch 5 is pressed next menu */
    if (key == SW5 && menu_pos < 4)
    {
        clear_screen();
        menu_pos++;
    }
    /* If switch 5 is pressed previous menu */
    else if (key == SW4 && menu_pos > 0)
    {
        clear_screen();
        menu_pos--;
    }
    /* Indicate the selected menu based on position of character '*' */
    /* For changing the position from 4th and 5th place */
    if (menu_pos == 4)
    {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1],LINE1(2));
        clcd_print(menu[menu_pos],LINE2(2));
    }
    /* For all other positions */
    else if (menu_pos < 4)
    {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos],LINE1(2));
        clcd_print(menu[menu_pos + 1],LINE2(2));
    }
    return menu_pos;
}

/* Function definition for viewing the recorded logs */
void view_log (unsigned char reset_flag, unsigned char key)
{
    /* Declare variables */
    static char addr = 5; static int log_count = 0; char event[11];
    
    /* Display logs if access if not equal to 0 */
    if (access != 0)
    {
        /* Display static message in CLCD line 1 */
        clcd_print("# TIME     E  SP", LINE1(0));
        /* Read values one by one from external EEPROM */
        for(int i = 0; i < 10; i++)
        {
            event[i] = eeprom_at24c04_read(addr + i);
        }
        /* Print the values into CLCD line2 */
        if (log_count >= 0 && log_count < access)
        {
            clcd_putch(log_count + '0', LINE2(0));
            clcd_putch(event[0], LINE2(2));
            clcd_putch(event[1], LINE2(3));
            clcd_putch(':', LINE2(4));
            clcd_putch(event[2], LINE2(5));
            clcd_putch(event[3], LINE2(6));
            clcd_putch(':', LINE2(7));
            clcd_putch(event[4], LINE2(8));
            clcd_putch(event[5], LINE2(9));
            clcd_putch(event[6], LINE2(11));
            clcd_putch(event[7], LINE2(12));
            clcd_putch(event[8], LINE2(14));
            clcd_putch(event[9], LINE2(15));
        }
        
        /* If switch 4 is pressed decrement the logs index with values in it */
        if (key == SW4 && log_count > 0) // 0 - 9
        {
            addr = addr - 10;
            log_count--;
        }
         /* If switch 5 is pressed Increment the logs index with values in it */
        else if (key == SW5 && log_count < access)
        {
            addr = addr + 10;
            log_count++;
        }
    }
    /* If no logs are present display message in CLCD */
    else if (access == 0)
    {
        clcd_print("   No log are   ", LINE1(0));
        clcd_print("   Available    ", LINE2(0));
    }
}

/* Function definition to clear log record */
void clear_log(unsigned char reset_flag)
{
    static char addr = 5;
    access = 0;
    pos = -1;
    /* Clear the recorded log values in the address */
    for(int  j = 0 ; j < 10 ; j++ )
    {
        eeprom_at24c04_str_write(addr, "          ");
    }
    addr += 10;
    /* Display message in CLCD after clearing */
    clcd_print("  Logs Cleared  ", LINE1(0));
    clcd_print("  Successfully  ", LINE2(0));
}

/* Function definition for downloading logs */
void download_log(void)
{
    /* Declare variables */
    static char addr = 5; char data[11]; static int log_count = 0, flag = 1;
    
    /* Download logs if access is not equal to 0 */
    if (access != 0 && log_count < access)
    {
        /* Display message in Cute-com */
        if (log_count == 0)
        {
            puts("\n\r   SAVED LOGS  \n\r"); 
        }
        /* Read recorded logs from external EEPROM */
        for(int i = 0; i < 10; i++)
        {
            data[i] = eeprom_at24c04_read(addr + i);
        }
        
        /* Print the recorded logs in Cute-com */
        if (log_count >= 0 && log_count < access)
        {
            putchar(log_count + '0');
            putchar(' ');
            putchar(data[0]);
            putchar(data[1]);
            putchar(':');
            putchar(data[2]);
            putchar(data[3]);
            putchar(':');
            putchar(data[4]);
            putchar(data[5]);
            puts("  ");
            putchar(data[6]);
            putchar(data[7]);
            putchar(' ');
            putchar(data[8]);
            putchar(data[9]);
            puts("\n\r");
        }
        
        log_count++;
        addr = addr + 10;
        
    }
    /* Print success message after downloading */
    if (log_count == access && flag)
    {
        puts("\n\r Downloaded logs successfully  \n\r"); 
    	clcd_print("Your logs has   ", LINE1(0));
		clcd_print("been downloaded ", LINE2(0));
        flag = 0;
	}
    /* If no are logs are available */
    if (access == 0) 
    {
        /* Display message in Cute-com and CLCD */
        if (flag)
        {
            puts("\n\rYou don't have any saved logs\n\r");
            flag = 0;
        }
        clcd_print("You don't have  ", LINE1(0));
        clcd_print("  any saved logs", LINE2(0));
    }
}

/* Function definition for changing password */
void change_password(unsigned char reset_flag, unsigned char key)
{
    /* Declare variables */
    static char password[4], i, flag = 1, repassword[4], j, flag_1 = 1;
    
    /* If reset flag is equal to reset-password assign values to variables */
    if (reset_flag == RESET_PASSWORD)
    {
        i = 0;
        j = 0;
        flag = 1;
        flag_1 = 1;
    }
    /* Print the message in CLCD and set the cursor position */
    if (flag)
    {
        clcd_print(" Enter Password ", LINE1(0));
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
        flag = 0;
    }
    /* If switch 4 is pressed print store 1 and display '*'  */
    if (key == SW4 && i < 4) 
    {
        /* Store password in array and increment position */
        password[i] = '1';
        clcd_putch('*', LINE2(4+i));
        i++;
    }
    /* If switch 5 is pressed print store 0 and display '*'  */
    else if (key == SW5 && i < 4) 
    {
        /* Store password in array and increment position */
        password[i] = '0';
        clcd_putch('*', LINE2(4+i));
        i++;
    }
    /* If 4 digits password is entered then display Re-enter password */
    if (i == 4 && flag_1)
    {
        clear_screen();
        clcd_print("ReEnter Password", LINE1(0));
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
        j = 0; i++; flag_1 = 0;
    }   
    else if (key == SW4 && j < 4 && i > 4) //store 1
    {
        repassword[j] = '1';
        clcd_putch('*', LINE2(4+j));
        j++;
    }
    else if (key == SW5 && j < 4 && i > 4) //store 0
    {
        repassword[j] = '0';
        clcd_putch('*', LINE2(4+j));
        j++;
    }
    /* If 4 digits are re-entered */
    if (j == 4)
    {
        /* Compare both the passwords */
        if (strncmp(password, repassword, 4) == 0)
        {
            /* If same store it in EEPROM and print success message in CLCD */
            eeprom_at24c04_str_write(0x00, repassword);
            clcd_print("Password changed", LINE1(0));
            clcd_print("  successfully  ", LINE2(0));
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        }
        /* If not same, display mismatch message in CLCD */
        else
        {
            clcd_print("Passwrd mismatch", LINE1(0));
            clcd_print("    Re-enter    ", LINE2(0));
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        }
    }
}

/* Function to clear CLCD screen */
void clear_screen(void) 
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}