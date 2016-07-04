#ifndef __C_FILE__H
#define __C_FILE__H

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*  */


void creat_t_f( int n, char reval[], char * );


void is_current_size( int size_count, int flag, int* n );


/*
 * 若是指定了文件大小，返回 DIG_SZIE
 * 若是用信号方式，返回 SIGL_C
 */
int is_size_sign( int w_size, char *top_sign, int falg );


#endif