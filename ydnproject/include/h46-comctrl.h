#ifndef __H46_CONFIG__H
#define __H46_CONFIG__H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/time.h>



int time_out(struct timeval tpstart);
int set_uart(int fd,int nSpeed, int nBits, char nEvent, int nStop);
//open
int uart_open(int fd,int comport);
// rcv
int uart_rcv_ok(int fd);
// send
int uart_send(int fd,char *buf);

int uart_rcv_str(int fd);


#endif