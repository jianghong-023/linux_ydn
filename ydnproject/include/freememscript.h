#ifndef __FREEMEMSCRIPT__H
#define __FREEMEMSCRIPT__H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>



void flush_conf(void);
void system_conf(void);
void sys_ect_conf(void);
void system_reset_conf(void);

extern void mem_look_clear(void);
extern void loop_cl_cah(void);

#endif