/* 
 * File:   car_black_box_1.h
 * Author: A Saikiran
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

/* Function prototype for Dash board */
void display_dash_board(unsigned char event[], unsigned char speed);

/* Function prototype for logging the events */
void log_event(unsigned char event[], unsigned char speed);

/* Function prototype to clear screen */
void clear_screen(void);

/* Function prototype for Login */
unsigned char login(unsigned char reset_flag, unsigned char key);

/* Function prototype for login screen */
unsigned char login_menu(unsigned char return_flag, unsigned char key);

/* Function prototype for viewing the recorded logs */
void view_log (unsigned char reset_flag, unsigned char key);

/* Function definition to clear log record */
void clear_log(unsigned char reset_flag);

/* Function definition for downloading logs */
void download_log(void);

/* Function definition for changing password */
void change_password(unsigned char reset_flag, unsigned char key);

/* Function prototype to Reset time */
void set_time(unsigned char key);



#endif	/* CAR_BLACK_BOX_H */

