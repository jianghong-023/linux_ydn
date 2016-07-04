#ifndef __COMMON_INTERFACE__H_
#define __COMMON_INTERFACE__H_

#include <sys/types.h>

#include <stdint.h>

int is_usb_online( void );


int usb_probe( void );


int open_stream_file( char * );


void close_stream_file( int );


void close_mem_fd( int );


int open_mem_fd( void );


void destory_mem( void *, size_t );

uint8_t* init_mem( size_t mapsize, size_t fd, off_t map_addr );

uint32_t recv_usb_notify( void );


#endif