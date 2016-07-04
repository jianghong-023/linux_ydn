/*
 * Cy22393.h - platform_data for the Cy22393 (and variants) rtc driver
 *
 * same license as the driver
 */

#ifndef _LINUX_Cy22393_H
#define _LINUX_Cy22393_H

#include <linux/types.h>

int call_cy22393_adv7842_set( uint8_t slave_addr, int adapter_nr, int optype );


void input_mod_gpio( uint8_t i_mod );


#endif /*  */
