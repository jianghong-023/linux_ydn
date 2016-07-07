#ifndef __COMMON__H
#define __COMMON__H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <stddef.h>
#include <poll.h>
#include <signal.h>


#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdint.h>


extern int socket_event( void );


extern void listen_info( void );


#define MAX_NAME		16
#define TS_FILE_NAME_LENGTH	(30)
#define TS_FILE_PATH_LENGTH	(150)


typedef struct _bus_init_ {
	uint8_t reset;
	uint8_t rest_usb_op;

	uint8_t usb_dir;
	uint8_t usb_wraddr_b3124;
	uint8_t usb_wraddr_b2316;
	uint8_t usb_wraddr_b158;
	uint8_t usb_wraddr_b70;
	uint8_t usb_rdaddr_b3124;
	uint8_t usb_rdaddr_b2316;
	uint8_t usb_rdaddr_b158;
	uint8_t usb_rdaddr_b70;
	uint8_t usb_op;
	uint8_t rewrite;
	uint8_t write_bus_reset;
	uint8_t write_bus_reset_2;

	char		usb_tsfilename[16];
	uint32_t	usb_tsfilesize;

} bus_init;

typedef struct _cfg_param_ {
	uint8_t cfg_len;                  /* for check struct length */
	/* ºÏ≤‚±Í÷æ */
	int8_t encoder_video_interface;

	uint8_t encoder_video_ratemode[4];

	uint8_t encoder_video_profile;
	uint8_t encoder_video_level;
	uint8_t encoder_video_standard;

	int16_t encoder_audio_bitrate;

	uint16_t	encoder_video_brightness;
	uint16_t	encoder_video_contrast;
	uint16_t	encoder_video_saturation;
	int16_t		encoder_video_hue;

	uint8_t encoder_video_cp_brightness;
	uint8_t encoder_video_cp_contrast;
	uint8_t encoder_video_cp_saturation;
	int8_t	encoder_video_cp_hue;

	uint8_t		encoder_audio_format;
	uint8_t		encoder_program_output[8];
	uint8_t		encoder_program_name[16];
	uint8_t		encoder_service_name[16];
	uint8_t		encoder_program_number[6];
	uint8_t		encoder_pmt_pid[5];
	uint8_t		encoder_pcr_pid[5];
	uint8_t		encoder_video_pid[5];
	uint16_t	encoder_video_norm;

	uint8_t encoder_audio_pid[5];
	uint8_t encoder_program_lcn[3];
	uint8_t encoder_eit_now_startyear[9];
	uint8_t encoder_eit_now_starttime[7];
	uint8_t encoder_eit_now_duration[7];
	uint8_t encoder_eit_now_eventname[17];
	uint8_t encoder_eit_next_startyear[9];
	uint8_t encoder_eit_next_starttime[7];
	uint8_t encoder_eit_next_duration[7];
	uint8_t encoder_eit_next_eventname[17];
	


	/* modulator/ */
	uint8_t encode_rate;
	uint8_t modulelate_mode;
	uint8_t fft_mode;
	uint8_t deltal;
	uint8_t channel_width;
	uint8_t modulator_rf_frequency[6];
	float	modulator_rf_level;
	uint8_t modulator_rf_on;
	uint8_t modulator_rf_on_name[8];

	/* stream */
	uint32_t	stream_tsid;
	uint32_t	stream_onid;
	uint8_t		stream_nit_network_id[5];
	uint8_t		stream_nit_network_name[16];
	uint8_t		stream_nit_version_mode;
	uint8_t		stream_nit_version_number[2];
	uint8_t		stream_nit_lcn_mode;
	uint16_t	stream_nit_programifo_lcn;
	uint8_t		stream_nit_private_data[9];
	uint8_t		stream_nit_insert;
	uint8_t		stream_eit_language_code[3];
	uint8_t		stream_eit_insert[8];
	uint8_t		stream_lck_state[8];
	uint8_t		stream_usb_enable[8];
	int32_t  stream_usb_used_count;  
	uint8_t stream_usb_mod;
	uint8_t stream_usb_record_mod;
	char  stream_usb_record_auto[8];
	uint8_t system_lck_st;
	uint8_t system_pwd_count;

	/* system */
	uint8_t system_lcd_time_out;
	uint8_t system_lcd_backlighting;
	uint8_t system_gpio_mask;
	uint8_t system_key_password;
	uint8_t system_lock_keyboard;
	uint8_t system_product_id[32];
	uint8_t system_version_date[8];
	uint8_t system_version_sw[4];
	uint8_t system_version_hw[4];
	uint8_t system_root_pwd[7];
	uint8_t system_def_root_pwd[7];

	int	encoder_video_ratemode_fre;
	int	encoder_video_bitrate;

	char	* stream_usb_play_mod;
	char	* stream_usb_record_save_mod;
	char	* encoder_video_profile_name;
	char	* encoder_video_level_name;
	char	* encoder_audio_bitrate_name;
	char	* encoder_video_interface_name;
	char	* stream_nit_lcn_mode_name;
	char	* encoder_video_norm_name;
	char	* encoder_audio_format_name;
	char	* system_lcd_time_out_name;
	char	* modulelate_mode_name;
	char	* encode_rate_name;
	char	* fft_mode_name;
	char	* deltal_name;
	char	* channel_width_name;
} scfgParam;


typedef struct _local_status_ {
/* encoder */
	uint8_t encoder_video_input_lock; /* 0x01 */
	char	encoder_video_resolution[16];
	char	encoder_video_shrot_resolution[16];
	uint8_t encoder_video_ident;
/* modulator */
//	uint8_t modulator_bitrate_act[5];
//	uint8_t modulator_bitrate_max[5];
/* usb device */
//	uint8_t usb_ts_play_lock;
/* system */
	uint8_t system_adv7441a_status;
	uint8_t system_mb86h46_status;
	uint8_t system_ad9789_status;
	uint8_t system_adf4350_status;
	uint8_t system_usb3300_status;
	uint8_t system_uptime_status;


	float cfig_ad9789_ftw_bpf;
} localStatus;

typedef struct _alarm_status_ {
	uint8_t encoder_video_unlock_alarm;
	uint8_t usb_ts_play_unlock_alarm;
	uint8_t system_adv7441a_alarm;
	uint8_t system_mb86h46_alarm;
	uint8_t system_ad9789_alarm;
	uint8_t system_adf4350_alarm;
	uint8_t system_usb3300_alarm;
} alarmStatus;


typedef struct {
	char pwrd[17];
}_passwrd;

union cfg_date {
	int f;
	struct {
		char	byte_1;
		char	byte_2;
		char	byte_3;
		char	byte_4;
	} hex;
};

typedef struct  channel_info_table
{
	uint8_t		Pass;
	uint16_t	ServiceID;
	uint16_t	PmtPid;
	uint16_t	PcrPid;
	uint16_t	VidPid;
	uint16_t	AudPid[2];
	char		ServiceName[MAX_NAME + 1];
} CHANNEL_INFO_STRUCT;


typedef struct _dsplay_signal_set_ {
	uint32_t	user_opt_coder;
	uint32_t	user_dsplay_flag;
	uint32_t	auto_monitor_flag;
}DSPLAY_SIGNAL_SET_T;


typedef struct usb_operation_ {
	off_t	ts_size;
	int8_t	op_mod;
	int8_t	is_start;
} usb_operation_t;


#define GET_HEX( x, n ) ( ( (union cfg_date) { .f = x }).hex.byte_ ## n)

#define pwd_val( x )	( (x).pwrd)
#define write_pwrd( x ) ( (_passwrd) { (x) })


#endif


