#ifndef  __USB_SIGNAL___H_

#define __USB_SIGNAL___H_

#include <stdlib.h>
void usb_op_signal(void);


int get_usb_signal(void);


void set_usb_signal( void *usb_action );

void  send_usb_stop_message( pthread_t usb_action_t, int signum, void *dcfig,int is_start );


#endif

