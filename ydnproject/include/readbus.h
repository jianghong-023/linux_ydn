#ifndef    __COMMON_H__
#define  __COMMON_H__

#include "common.h"

#endif

#ifndef __READ_BUS__H
#define __READ_BUS__H


#include <stdint.h>


unsigned long get_file_size( const char *filename );


void bus_rest_op();


int8_t resval( void );



uint8_t usb_ts_inf( void );


#endif

