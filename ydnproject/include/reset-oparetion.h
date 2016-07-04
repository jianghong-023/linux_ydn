
#ifndef REST_OPARETION__H
#define REST_OPARETION__H

#include <stdint.h>

#include "init-configuration.h"

void init_bus( void );


void put_tale( char *, int );


extern struct _bus_initial * malloc_bus_addr( int *fd_mmap );


void free_bus_addr( struct _bus_initial *bus_map, int fd );


void filter_ts_pmt( uint8_t* map_base ,char pmt_0 ,char pmt_1);


extern void audio_video_pid( uint8_t *bus_addr, s_config *dconfig );


void reset_dvb_t(char value , uint8_t* map_base);


void modulator_set( uint8_t* map_base, s_config *dconfig );


void fpga_w_addr( bus_init *bus_inttt, uint8_t* map_base );


void fpga_r_addr( bus_init *bus_inttt, uint8_t* map_base );


void usb_start_rwop( char value , uint8_t* map_base );


void usb_dir( char value , uint8_t* map_base );


void reset_usb_op(char value, uint8_t* map_base );

void usbr_interrupt_enable(char value, uint8_t* map_base );

void eit_insert_table(uint8_t* map_base, char eitflags);
void nit_insert_table(uint8_t* map_base, char nitflags);


#endif
