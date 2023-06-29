/*
 * File:     main.c
 * Author:   A Saikiran
 * Project : Car Black Box
 */

#include "main.h"

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)

/* Extern variables form other files */
extern unsigned char clock_reg[3];
extern char time[7];
extern int timer_flag;

/* Write initialization code here */
static void init_config(void) {
    /* Initialize CLCD */
    init_clcd();
    /* Initialize i2c */
    init_i2c(100000);
    /* Initialize RTC */
    init_ds1307();
    /* Initialize Digital-keypad */
    init_digital_keypad();
    /* Initialize ADC */
    init_adc();
    /* Initialize timer2 */ 
    init_timer2();
    /* Initialize UART */
    init_uart(9600);
    
    /* Enable global and peripheral interrupts */
    PEIE = 1;
    GIE = 1;
}

void main(void) {
    init_config();
    
    /* Variable declaration */
    unsigned char control_flag = DASH_BOARD_FLAG, speed = 0, return_value, key, menu_pos, pre_key, reset_flag;         //default screen
    char event[3] = "ON";
    char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4"};
    unsigned int gr = 0, delay = 0;
    
    /* Write initial password into an address */
    eeprom_at24c04_str_write(0x00, "1010");
    
    /* Store initial info */
    log_event(event, speed);
    
    while (1) {
        /* Read ADC value */
        speed = read_adc() / 10;               // 0 - 1023
        if (speed >= 99)                       // If speed above 100
        {
            speed = 99;
        }
        
        /* Read digital-keypad  */
        key = read_digital_keypad(STATE);
        for (int j = 300; j--;);              //To avoid bouncing effect
        /* if Switch 1 is pressed -> Collision */
        if (key == SW1)
        {
            strcpy(event, "C ");
            log_event(event, speed);
        }
        /* Increase the GEAR if Switch 2 is pressed */
        else if (key == SW2 && gr < 6)
        {
            strcpy(event, gear[gr]);
            gr++;
            log_event(event, speed);
        }
        /* Decrease the GEAR if Switch 3 is pressed */
        else if (key == SW3 && gr > 0)
        {
            gr--;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        }
        /* If switch 4 or switch 5 is pressed read the password */
        else if((control_flag == DASH_BOARD_FLAG) && (key == SW4 || key == SW5))
        {
            control_flag = LOGIN_FLAG;
            clear_screen();
            clcd_print("Enter Password", LINE1(1));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            reset_flag = RESET_PASSWORD;
            TMR2ON = 1;
        }
        
        /* If switch 4 is long pressed open the chosen menu list based menu position */
        else if (key == LPSW4 && control_flag == LOGIN_MENU_FLAG)
        {
            switch (menu_pos)
           {
                /* Control jumps to view log flag */
                case 0:                              
					clear_screen();
					control_flag = VIEW_LOG_FLAG;
					reset_flag = RESET_VIEW_LOG_POS ;
					break;
                 /* Control jumps to Clear log flag */
				case 1 :             
					clear_screen();
					control_flag = CLEAR_LOG_FLAG ;
					reset_flag = RESET_MEMORY;
					break;
                 /* Control jumps to download log flag */
				case 2 :
                    clear_screen();
					control_flag = DOWNLOAD_LOG_FLAG;
					break;
                /* Control jumps to set-time log flag */
				case 3 :
					clear_screen();
					control_flag = SET_TIME_FLAG;
					break;
                 /* Control jumps to change password log flag */
				case 4:
					clear_screen();
					control_flag = CHANGE_PASSWORD_FLAG;
					reset_flag = RESET_PASSWORD;
					TMR2ON = 1;
					break;
            }
        }
        /* If switch 5 is long pressed go back to Dash board */
        else if (key == LPSW5 && control_flag == LOGIN_MENU_FLAG)
        {
            control_flag = DASH_BOARD_FLAG;
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        }
        /* If switch 4 is long pressed go back log menu */
        else if (key == LPSW4 && (control_flag == VIEW_LOG_FLAG || control_flag == CLEAR_LOG_FLAG || control_flag == DOWNLOAD_LOG_FLAG || control_flag == CHANGE_PASSWORD_FLAG || control_flag == SET_TIME_FLAG))
        {
            /* If control flag is set-time flag then reset the time values based on changes into ds1307 */
            if (control_flag == SET_TIME_FLAG)
            {
                timer_flag = 1;
                write_ds1307(SEC_ADDR, (time[4] - '0') << 4 | (time[5] - '0'));
                write_ds1307(MIN_ADDR, (time[2] - '0') << 4 | (time[3] - '0'));
                write_ds1307(HOUR_ADDR, (time[0] - '0') << 4 | (time[1] - '0'));
                clear_screen();
                clcd_print("    Time set    ", LINE1(0));
                clcd_print("   Successful   ", LINE2(0));
                __delay_ms(2000);
            }
            control_flag = LOGIN_MENU_FLAG;
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        }
        for(int b = 3000; b--; );                   //delay
        
        /* Based on control flag Call functions */
        switch(control_flag)
        {
            case DASH_BOARD_FLAG:                   //0x01 -> dash board
                display_dash_board(event, speed);
                break;
            case LOGIN_FLAG:                        //0x02  -> login screen
                switch(login(reset_flag, key))
                {
                    /* If no key is pressed return back */
                    case RETURN_BACK:
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_ms(1000);
                        control_flag = DASH_BOARD_FLAG;
                        TMR2ON = 0;
                        break;
                    /* If login is success, display menu */
                    case LOGIN_SUCCESS:
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_ms(1000);
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_PASSWORD;
                        continue;
                        break;
                }
                break;
            /* Display menu in CLCD */
            case LOGIN_MENU_FLAG:
                menu_pos = login_menu(reset_flag, key);
                break;
            /* View the recorded logs */
            case VIEW_LOG_FLAG:
                view_log(reset_flag, key);
                reset_flag = RESET_VIEW_LOG_POS;
                break;
            /* Clear recorded logs */
            case CLEAR_LOG_FLAG:
                clear_log(reset_flag);
                reset_flag = RESET_MEMORY;
                break;
            /* Download logs  in Cute-com using UART protocol */
            case DOWNLOAD_LOG_FLAG:
                download_log();
                break;
            /* Change time using digital keypad in CLCD */
            case SET_TIME_FLAG:
                set_time(key);
                break;
            /* Change password using digital keypad */
            case CHANGE_PASSWORD_FLAG:
                change_password(reset_flag, key);
                reset_flag = RESET_PASSWORD;
                break;
        }
        reset_flag = RESET_NOTHING;
    }
    return;
}