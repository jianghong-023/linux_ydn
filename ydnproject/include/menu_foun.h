#ifndef __MENU_FOUN__H

#define __MENU_FOUN__H
#include <stdint.h>
#include "init-configuration.h"
#include "dev-info.h"

void signal_open( int );


void gpiocfg( struct h64_gpio_et_cfg_t *pcfg );


void gpio_mask( uint8_t setval );


void h46cfg( struct h64_gpio_et_cfg_t *pcfg );


uint8_t systemstat_cfg( void );


void pr_cfg();


void cfg_discontrl( double cfg_size, int char_digit_status, int curos_stat, double max_regin, int min_region,
		    int f_status, int input_lenth, void (* g_configfile)( char *, char * ) );


void menu_general_enter( char  **Bandwidth, int menu_item_lenth, int (* menu_cfg_fun)( char * ) );


void cfg_discontrl_c( int char_digit_status, int curos_stat,
		      char *inputename, int input_lenth, int orgstrlen, void (* config_f)( char *, char * ),
		      void (* loopdsplay)() );


uint8_t ts();


uint8_t videocontrast();


s_config *config_t();


uint8_t run_time();


uint8_t usb_rec_enable();


void pwd_enter();


void get_cfg_menu_item( char *cfg_name );


void bin_language_save( char *orgbuf, char *repbuf );


void init_cache();


void lockdsprate();


uint8_t file_size();


uint8_t Bandwidth_Cfg( void );


uint8_t mbitrate();


uint8_t coderate_cfg( void );


uint8_t guardIntv_dalay_cfg( void );


uint8_t RF_frequency_status( void );


uint8_t eauto_wr_usb_Cfg(void);


uint8_t sys_serial();


uint8_t sys_Version();


int guardIntv_comm( char* buffer );


int coderate_comm( char* buffer );


int comm_constell( char *buffer );


int comm_video_interface( char *buffer );


void auto_usb_test(void );


int eit_insert_commom( char *buffer );


int comm_Bandwidth( char* buffer );


int fft_comm( char*buffer );


int comm_ratemode( char *buffer );


int comm_h264profile( char *buffer );


int comm_h264level( char *buffer );


int comm_lcdtime( char *buffer );


int comm_factory_set( char *buffer );


int text_audiotare_save( char *buffer );


void text_program_num_save( char *orgbuf, char *repbuf );


void passwrd_filename_save( char *orgbuf, char *repbuf );


void text_digapid_save( char *orgbuf, char *repbuf );


void text_videotare_save( char *orgbuf, char *repbuf );


void text_digvpid_save( char *orgbuf, char *repbuf );


void text_digprc_save( char *orgbuf, char *repbuf );


void text_digpmt_save( char *orgbuf, char *repbuf );


void text_digfilesize_save( char *orgbuf, char *repbuf );


void text_videobright_save( char *orgbuf, char *repbuf );


void text_networkid_save( char *orgbuf, char *repbuf );


void str_netwrorkname_save( char *orgbuf, char *repbuf );


void text_networknumb_save( char *orgbuf, char *repbuf );


void text_PrivateData_save( char *orgbuf, char *repbuf );


void text_Programname_save( char *orgbuf, char *repbuf );


void text_servername_save( char *orgbuf, char *repbuf );


void bin_eit_year_save( char *orgbuf, char *repbuf );


void bin_eit_next_year_save( char *orgbuf, char *repbuf );


void bin_starteventname_save( char *orgbuf, char *repbuf );


void bin_nexteventname_save( char *orgbuf, char *repbuf );


void bin_eit_time_save( char *orgbuf, char *repbuf );


void bin_eit_duration_save( char *orgbuf, char *repbuf );


uint8_t program_num();


uint8_t language_Cfg();


uint8_t eit_insert_Cfg();


uint8_t start_eventname_Cfg();


uint8_t next_eventname_Cfg();


void str_filename_save( char *orgbuf, char *repbuf );


void bin_eit_nexttime_save( char *orgbuf, char *repbuf );


void bin_eit_nextduration_save( char *orgbuf, char *repbuf );


void fre_dig_save( char *orgbuf, char *repbuf );


uint8_t Constell_cfg();


void nano_sleep( int sec, uint64_t nsec );


uint8_t fft_NK_cfg();


uint8_t video_inerface_cfg();


uint8_t video_lock_status_Cfg();


uint8_t video_resolution();


uint8_t video_rate_mode();


uint8_t video_bitrate();


uint8_t h264_profile_cfg();


uint8_t h264_level();


uint8_t factory_reset();


uint8_t vedeio_bright();


uint8_t network_name();


uint8_t nit_version_numb();


uint8_t PrivateData();


uint8_t audio_bitrate();


uint8_t program_name();


uint8_t Service_name();


uint8_t start_time();


uint8_t pmt_pid();


uint8_t prc_pid();


uint8_t video_pid();


uint8_t audio_pid();


uint8_t network_id();


uint8_t key_passwrd_cfg();


uint8_t key_lock();


uint8_t usb_recode();


uint8_t start_year();


uint8_t next_year();


uint8_t start_duration();


uint8_t next_time();


uint8_t next_duration();


uint8_t nit_lcn_mod_cfg();


uint8_t programinfo_lcn();


uint8_t program_ouput();


uint8_t videosaturation();


uint8_t record_save_mod();


uint8_t videohue();


uint8_t videonorm();


uint8_t send_usb_writ_message();


uint8_t streamtsid();


uint8_t streamonid();


uint8_t rf_level();


uint8_t rf_out();


uint8_t audio_format();


uint8_t lcd_timeout();


uint8_t play_mod( void );


void lock_dspbitraet( int flag );


void reset_desplay();


void runtime( int flag );


typedef void (* loopdsplay)( int );

typedef int32_t (*usb_handler)( void * );

typedef uint8_t (*_stream_info)( void );


void function_inter( loopdsplay desplay );


int32_t usb_wr_handler( usb_handler );


void reset_desplay( void );


void get_state_desplay( int *monitor_flag, int *coder, int *flags );


void set_state_desplay( int monitor_flag, int coder, int flags );


void function_inter( loopdsplay desplay );


void comm_handler( int coder );


void null_handler( int none );


void start_alarm( void );


void stop_alarm( void );


int test_signal( void );


int signal_close( void );


void usb_read_bitrate( float * );


int32_t usb_nothandler( void * );


void usb_wroperator( void * );


int32_t read_usb( void * hand );


uint8_t comom_stream_info( _stream_info );

uint8_t desplay_stream_info();


#endif
