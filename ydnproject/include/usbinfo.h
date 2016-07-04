#ifndef __USBINFO__H
#define __USBINFO__H

#include <sys/vfs.h>
#include <stdint.h>

uint8_t get_usb_usrinfo( void );


uint8_t manu_do_mount_dev( void );

uint64_t get_usb_free_size(char * );


#endif