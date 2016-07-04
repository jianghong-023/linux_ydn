#ifndef __KEY_CTL__H
#define __KEY_CTL__H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>


void lcd_Write_String( int y, char *c );


void ctl_fun( int lcdfd, uint8_t post );


void ctl_cursor( int lcdfd, uint32_t cursor, uint8_t post );


void key_input_char( char rightlift, char updown_date, uint8_t change_g, int lcdfd );





void lcd_clear( int lcdfd );


void cursor_onoff( int lcdfd, uint32_t cursor, uint32_t cursor_offon );


void cursor_move( int lcdfd, uint32_t cursor, uint32_t move );


#endif
