#ifndef    __COMMON_H__
#define  __COMMON_H__

#include <stdint.h>
#include <time.h>

#include "common.h"

int gettimeofday( struct timeval *tv, struct timezone *tz );


uint8_t usb_ts_connect( void );


#endif