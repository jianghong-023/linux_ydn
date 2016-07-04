#ifndef __I2C_WR__H

#define __I2C_WR__H

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>



int set_i2c_register(int file,uint8_t addr,uint8_t reg,uint8_t value);
uint8_t get_i2c_register_value(int file,uint8_t addr,uint8_t reg);

#endif