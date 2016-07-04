#ifndef __CY22393_SET__H
#define __CY22393_SET__H

#include <stdint.h>


void refclk_13_5MHz(int fd,uint32_t slave_addr);
void refclk_27MHz(int fd,uint32_t slave_addr);
void refclk_27MHz_1_001MHz(int fd,uint32_t slave_addr);
void refclk_74_25MHz(int fd,uint32_t slave_addr);
void refclk_74_25MHz_1_001MHz(int fd,uint32_t slave_addr);
void refclk_148_5MHz(int fd,uint32_t slave_addr);

#endif