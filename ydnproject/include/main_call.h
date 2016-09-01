#ifndef __MAIN_CALL__H
#define __MAIN_CALL__H

#include "init-configuration.h"

void set_progr_bar( int value );


int get_progr_bar( void );


void h46_set( int optcode );


void interrupt_signals_mask( int opcode );


int peripheral_dev_config( struct dvb_peripheral *devconfig );


#endif