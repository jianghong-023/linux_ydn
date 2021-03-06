#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <time.h>
#include <menu_foun.h>
#include <lcd1602.h>
#include <gen_alltable.h>
#include <common_conf.h>
#include <key-ctl.h>
#include <common_interface.h>
#include <readbus.h>
#include <writebus.h>
#include <debug.h>

extern struct  MenuItem * MenuPoint;
extern uint8_t		t_date[16];
extern struct cache_t	*cachep;
#define MAPBITRATE_SIZE (0x41000)
#define USBBITRATE_SIZE (0x21000)

struct cache_t cache_null[1] = { { &null_cahebuf, 0x00 } };

struct cache_t *cachep = cache_null;

extern dis_contrl discontrl;


static loopdsplay desplay_hand;

static usb_handler usb_wrhandler;


static _stream_info wr_stream_info;

extern uint8_t write_bus( void );


static void mod_bitraet();


static int comm_nitlcn_mod( char * );


int comm_save_mod( char * );


static void systemserial();


static void systemVersion();


static int eauto_record_commom( char *buffer );


void text_videocpbright_save( char *, char * );


static void bin_nitlcn_save( char *, char * );


static int program_enable_commom( char * );


int rec_usb_commom( char * );


int root_lock_commom( char * );


int comm_play_mod( char * );


static void text_videocontrast_save( char *, char * );


void text_videocpcontrast_save( char *, char * );


static void text_videosaturation_save( char *, char * );


void text_videocpsaturation_save( char *, char * );


static void text_videohue_save( char *, char * );


void text_videocphue_save( char *, char * );


static int comm_videonorm( char * );


static void text_streamtsid_save( char *, char * );


static void text_streamonid_save( char *, char * );


static void text_modulatorrf_save( char *, char * );


static int rf_comm( char * );


static int text_audioformat_save( char * );


typedef struct glob_signal {
	int	signal_open_colose;
	int	count;
}glob_signal_t;

static glob_signal_t g_signal_t = {
	.signal_open_colose	= 0,
	.count			= 0,
};

void null_handler( int none )
{
}


static DSPLAY_SIGNAL_SET_T signal_init_t =
{
	.user_opt_coder		= NONE,
	.user_dsplay_flag	= NONE,
	.auto_monitor_flag	= MONITOER_FALSE,
};

DSPLAY_SIGNAL_SET_T* get_DSPLAY_SIGNAL_SET_T( void )
{
	return(&signal_init_t);
}


void function_inter( loopdsplay desplay )
{
	desplay_hand = desplay;
}


int32_t usb_wr_handler( usb_handler _handler )
{
	usb_wrhandler = _handler;

	return(0);
}


/*
 * 设置显示状态
 */
void set_state_desplay( int monitor_flag, int coder, int flags )
{
	signal_init_t.auto_monitor_flag = monitor_flag;
	signal_init_t.user_opt_coder	= coder;
	signal_init_t.user_dsplay_flag	= flags;
}


void get_state_desplay( int *monitor_flag, int *coder, int *flags )
{
	*monitor_flag	= signal_init_t.auto_monitor_flag;
	*coder		= signal_init_t.user_opt_coder;
	*flags		= signal_init_t.user_dsplay_flag;
}


/*
 * 显示状态置位
 */
static void reset_state_desplay( void )
{
	signal_init_t.auto_monitor_flag = MONITOER_FALSE;
	signal_init_t.user_opt_coder	= NONE;
	signal_init_t.user_dsplay_flag	= NONE;
}


void reset_desplay( void )
{
	reset_state_desplay();
}


void comm_handler( int coder )
{
	if ( desplay_hand != null_handler )
		desplay_hand( coder );
}


void usb_wroperator( void *handler )
{
	if ( usb_wrhandler != usb_nothandler )
		usb_wrhandler( handler );
}


/* 3?ê??ˉ?o′??à1?1¤×÷ */
void init_cache()
{
	discontrl.changemenuflag = CHAR_INPUT_ON;
	s_config *dconfig = config_t();
	dconfig->scfg_Param.system_lck_st = NOTPWD;
	memcpy( t_date, MenuPoint[discontrl.UserChoose].DisplayString, 16 );
	memset( discontrl.cechebuf, 0, 17 );

	memset( discontrl.lcd_locknote, '\0', 17 );

	discontrl.changeflag			= ENTER_KEY;
	discontrl.lcd_b_last_dsp.i_bdsp_length	= 0;
	discontrl.lcd_b_last_dsp.i_ldsp_length	= 0;
	memset( discontrl.lcd_b_last_dsp.b_dsp, '\0', 3 );
	memset( discontrl.lcd_b_last_dsp.l_dsp, '\0', 5 );
}


uint8_t file_size()
{
	discontrl.write_size = WRITELIMIT;

	init_cache();
	int input_lenth = 4;
	discontrl.lcd_b_last_dsp.i_ldsp_length	= 2;
	discontrl.lcd_b_last_dsp.desplay_b_pos	= 1;
	discontrl.lcd_b_last_dsp.desplay_l_pos	= input_lenth + 3;
	snprintf( discontrl.lcd_b_last_dsp.l_dsp, 3, "%s", "MB" );

	s_config *dconfig = config_t();

	discontrl.write_size = WRITELIMIT & 0x00;

	int get_value = dconfig->configParam.usb_tsfilesize;
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 4096, 10, INPUT_DIGIT_STATUS, input_lenth, &text_digfilesize_save );


	return(0);
}


uint8_t key_lock()
{
	/* config.key_statuus = LOCK_ON; */
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_lck_state );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, root_lock_commom );

	return(0);
}


uint8_t key_passwrd_cfg()
{
	init_cache();

	double date = 100000;
	cfg_discontrl( date, DIGIT_STATUS, CURSOR_ON, 999999, 000000, INPUT_DIGIT_STATUS, 6, &passwrd_filename_save );

	dev_config_printf();

	return(0);
}


uint8_t vedeio_bright()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	if ( (strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) || \
	     (strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) )
	{
		int get_value = dconfig->scfg_Param.encoder_video_cp_brightness;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videocpbright_save );
	}else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name, \
			    strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		int get_value = dconfig->scfg_Param.encoder_video_brightness;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videobright_save );
	}


	return(0);
}


uint8_t videocontrast()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	if ( (strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) || \
	     (strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) )
	{
		int get_value = dconfig->scfg_Param.encoder_video_cp_contrast;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videocpcontrast_save );
	}else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name, \
			    strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		int get_value = dconfig->scfg_Param.encoder_video_contrast;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videocontrast_save );
	}


	return(0);
}


uint8_t videosaturation()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();


	if ( (strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) || \
	     (strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) )
	{
		int get_value = dconfig->scfg_Param.encoder_video_cp_saturation;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videocpsaturation_save );
	}else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name, \
			    strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		int get_value = dconfig->scfg_Param.encoder_video_saturation;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 255, 0, INPUT_DIGIT_STATUS, 3, &text_videosaturation_save );
	}


	return(0);
}


uint8_t videohue()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int input_lenth = 3;
	discontrl.lcd_b_last_dsp.i_bdsp_length		= 1;
	discontrl.lcd_b_last_dsp.contian_decimal_length = input_lenth;
	discontrl.lcd_b_last_dsp.desplay_b_pos		= 1;
	snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", "+" );
	if ( (strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) || \
	     (strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) )
	{
		int get_value = dconfig->scfg_Param.encoder_video_cp_hue;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 127, -128, INPUT_DIGIT_STATUS, input_lenth, &text_videocphue_save );
	}else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name, \
			    strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		int get_value = dconfig->scfg_Param.encoder_video_hue;
		cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 127, -128, INPUT_DIGIT_STATUS, input_lenth, &text_videohue_save );
	}

	return(0);
}


uint8_t streamtsid()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	int get_value = dconfig->scfg_Param.stream_tsid;
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_streamtsid_save );

	return(0);
}


void pwd_enter()
{
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	memset( t_date, '\0', 17 );

	snprintf( (char *) t_date, 17, " Enter password " );
	double date = atof( (char *) dconfig->scfg_Param.system_root_pwd );
	cfg_discontrl( date, DIGIT_STATUS, CURSOR_ON, 999999, 0, INPUT_DIGIT_STATUS, 6, NULL );

	/* return(0); */
}


uint8_t rf_level()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	int input_lenth = 5;
	discontrl.lcd_b_last_dsp.i_ldsp_length		= 3;
	discontrl.lcd_b_last_dsp.desplay_b_pos		= 1;
	discontrl.lcd_b_last_dsp.contian_decimal_length = input_lenth;
	discontrl.lcd_b_last_dsp.desplay_l_pos		= input_lenth + 3;
	discontrl.lcd_b_last_dsp._power			= 1;
	snprintf( discontrl.lcd_b_last_dsp.l_dsp, 4, "%s", "dBm" );


	cfg_discontrl( dconfig->scfg_Param.modulator_rf_level, DIGIT_STATUS, CURSOR_ON, -37.0, -16.0, INPUT_FLOAT_STATUS, input_lenth, &text_modulatorrf_save );
	return(0);
}


uint8_t streamonid()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	int get_value = dconfig->scfg_Param.stream_onid;
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_streamonid_save );

	return(0);
}


uint8_t videonorm()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.encoder_video_norm_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"AUTO     ",
		"PAL-BGHID",
		"PAL-N    ",
		"PAL-CN   ",
		"PAL-M    ",
		"NTSC-M   ",
		"NTSC-J   ",
		"NTSC-4.43"
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_videonorm );

	return(0);
}


uint8_t factory_reset()
{
	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"restart OS",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_factory_set );

	return(0);
}


uint8_t h264_level()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.encoder_video_level_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Level   4",
		"Level 4.1",
		"Level 4.2",
		"Level   3",
		"Level 3.1",
		"Level 3.2",
		"Default  ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_h264level );

	return(0);
}


uint8_t lcd_timeout()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.system_lcd_time_out_name );
	init_cache();

	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"5s  ",
		"10s ",
		"30s ",
		"45s ",
		"60s ",
		"90s ",
		"120s",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_lcdtime );

	return(0);
}


uint8_t play_mod()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_usb_play_mod );
	init_cache();

	discontrl.write_char_dig_status = MENU_STATUS;
	static char *Bandwidth[] = {
		"Single file",
		"Single loop",
		"Play all   ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_play_mod );

	return(0);
}


uint8_t record_save_mod()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_usb_record_save_mod );
	init_cache();

	discontrl.write_char_dig_status = MENU_STATUS;
	static char *Bandwidth[] = {
		"Single file   ",
		"Segmented file",
		"loop record   ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_save_mod );

	return(0);
}


/* 没有具体设置参数 20160419 */
uint8_t nit_lcn_mod_cfg()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.stream_nit_lcn_mode_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"European ",
		"NorDig V1",
		"NorDig V2",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_nitlcn_mod );

	return(0);
}


uint8_t h264_profile_cfg()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.encoder_video_profile_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"High Profile",
		"Main Profile",
		"Base Profile",
		"Default       ",
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_h264profile );
	return(0);
}


uint8_t audio_bitrate()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.encoder_audio_bitrate_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"64  kbps",
		"96  kbps",
		"112 kbps",
		"128 kbps",
		"160 kbps",
		"192 kbps",
		"224 kbps",
		"256 kbps",
		"320 kbps",
		"384 kbps",
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, text_audiotare_save );
	return(0);
}


uint8_t audio_format()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.encoder_audio_format_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"MPEG-2    ",
		"MPEG-2 AAC",
		"MPEG-4 AAC",
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, text_audioformat_save );
	return(0);
}


uint8_t  video_rate_mode()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.encoder_video_ratemode );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"CBR",
		"VBR",
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_ratemode );
	return(0);
}


uint8_t video_bitrate()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int input_lenth = 5;
	discontrl.lcd_b_last_dsp.i_ldsp_length	= 4;
	discontrl.lcd_b_last_dsp.desplay_b_pos	= 1;
	discontrl.lcd_b_last_dsp.desplay_l_pos	= input_lenth + 3;
	snprintf( discontrl.lcd_b_last_dsp.l_dsp, 5, "%s", "Kbps" );


	int get_value = dconfig->scfg_Param.encoder_video_bitrate;
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 29500, 500, INPUT_DIGIT_STATUS, input_lenth, &text_videotare_save );

	return(0);
}


uint8_t audio_pid()
{
	s_config *dconfig = config_t();
	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.encoder_audio_pid );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_digapid_save );

	return(0);
}


uint8_t video_pid()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.encoder_video_pid );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_digvpid_save );

	return(0);
}


uint8_t program_num()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.encoder_program_number );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_program_num_save );

	return(0);
}


uint8_t network_id()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.stream_nit_network_id );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_networkid_save );

	return(0);
}


uint8_t programinfo_lcn()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = (int) dconfig->scfg_Param.stream_nit_programifo_lcn;
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 900, 0, INPUT_DIGIT_STATUS, 3, &bin_nitlcn_save );

	return(0);
}


uint8_t nit_version_numb()  /* PrivateData */
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.stream_nit_version_number );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 2, &text_networknumb_save );

	return(0);
}


uint8_t PrivateData()  /*  */
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );

	int write_size = strlen( (char *) dconfig->scfg_Param.stream_nit_private_data );


	snprintf( filename, write_size + 1, "%s", dconfig->scfg_Param.stream_nit_private_data );
	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 8, 8, text_PrivateData_save, NULL );


	return(0);
}


uint8_t language_Cfg()  /*  */
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );


	if ( dconfig->scfg_Param.encoder_eit_now_eventname == NULL )
	{
		DEBUG( "eventname void" );
		return(-1);
	}

	int i = 0;
	while ( dconfig->scfg_Param.encoder_eit_now_eventname[i] != '\b' )
	{
		filename[i] = dconfig->scfg_Param.encoder_eit_now_eventname[i];
		++i;
	}
	filename[i] = '\0';
	DEBUG( "%d  %s", i, filename );
	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 3, 3, bin_language_save, NULL );


	return(0);
}


uint8_t eit_insert_Cfg()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_eit_insert );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, eit_insert_commom );

	return(0);
}


uint8_t nit_insert_Cfg()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_nit_insert );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, nit_insert_commom );

	return(0);
}


uint8_t eauto_wr_usb_Cfg()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_usb_record_auto );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, eauto_record_commom );

	return(0);
}


uint8_t usb_rec_enable()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.stream_usb_enable );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;
	int ret = usb_probe();
	if ( ret < 0 )
	{
		paren_menu();
		return(-1);
	}

	if ( discontrl.usb_wr_flag == USBWRITESET || discontrl.usb_wr_flag == USBWRITECTL )
		discontrl.usb_wr_flag = USBWRITESTART; /* 开启写 */
	clean_lcd();
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, null_Subcfg );/* rec_usb_commom */

	return(0);
}


uint8_t program_ouput()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.encoder_program_output );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Enable ",
		"Disable",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, program_enable_commom );

	return(0);
}


void runtime( int flag )
{
	int		i;
	int		day, hours, min, sec, t_day;
	static int	b_day = 0;


	char ch[20] = "";
	get_system_time( &day, &hours, &min, &sec );

	if ( b_day == 0 )
		t_day = 0;
	else {
		t_day = day - b_day;
	}

	sprintf( ch, "%d Days-%02d:%02d:%02d", t_day, hours, min, sec );

	for ( i = 0; i < strlen( ch ); i++ )
		key_input_char( 1 + i, ch[i], CHANGE_G, discontrl.lcdfd );

	b_day = day;
}


static int open_fd()
{
	int mfd;

	mfd = open( "/dev/mem", O_RDWR | O_SYNC );
	if ( mfd < 0 )
	{
		perror( "bitrate_meminit()\n" );
		return(-1);
	}

	return(mfd);
}


static void  close_fd( int fd )
{
	close( fd );
}


static uint8_t* bitrate_meminit( int fd, size_t mapsize )
{
	uint8_t*map_base = NULL;

	map_base = mmap( NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MAP_BASE_ADDR );
	if ( map_base == MAP_FAILED )
	{
		perror( "bitrate_meminit()\n" );
	}

	return(map_base);
}


static void bitratr_destory( void *addr, size_t length )
{
	munmap( addr, length );
}


static void usb_bitrate( float *usbbitrate, uint8_t*mapbase )
{
	uint8_t		byterate1_1;
	uint8_t		byterate1_2;
	uint8_t		byterate1_3;
	uint8_t		byterate1_4;
	uint32_t	bytecnt_1;
	uint32_t	delat_pcr_1;


	byterate1_1	= mapbase[(BUS_INIT_BASE + 0x2000) / sizeof(uint8_t)];
	byterate1_2	= mapbase[(BUS_INIT_BASE + 0x2001) / sizeof(uint8_t)];
	byterate1_3	= mapbase[(BUS_INIT_BASE + 0x2002) / sizeof(uint8_t)];
	byterate1_4	= mapbase[(BUS_INIT_BASE + 0x2003) / sizeof(uint8_t)];

	bytecnt_1	= ( (byterate1_1 << 10) & 0x3fc00) + ( (byterate1_2 << 2) & 0x3fc) + ( (byterate1_3 >> 6) & 0x03);
	delat_pcr_1	= ( (byterate1_3 & 0x3f) << 8) + (byterate1_4 & 0xff);
	*usbbitrate	= divide( bytecnt_1 * 9 * 8 * 4, delat_pcr_1 );
}


static void atc_bitrate( float  *actbitrate, uint8_t*mapbase )
{
	uint8_t		byterate2_1;
	uint8_t		byterate2_2;
	uint8_t		byterate2_3;
	uint8_t		byterate2_4;
	uint32_t	bytecnt_2;
	uint32_t	delat_pcr_2;


	byterate2_1	= mapbase[(BUS_INIT_BASE + 0x4000) / sizeof(uint8_t)];
	byterate2_2	= mapbase[(BUS_INIT_BASE + 0x4001) / sizeof(uint8_t)];
	byterate2_3	= mapbase[(BUS_INIT_BASE + 0x4002) / sizeof(uint8_t)];
	byterate2_4	= mapbase[(BUS_INIT_BASE + 0x4003) / sizeof(uint8_t)];
	bytecnt_2	= ( (byterate2_1 << 10) & 0x3fc00) + ( (byterate2_2 << 2) & 0x3fc) + ( (byterate2_3 >> 6) & 0x03);
	delat_pcr_2	= ( (byterate2_3 & 0x3f) << 8) + (byterate2_4 & 0xff);
	*actbitrate	= divide( bytecnt_2 * 9 * 8 * 4, delat_pcr_2 );
}


static void mod_bitraet()
{
	char	ch[20]	= "";
	char	ch1[20] = "";
	float	actbitrate;
	int	fd = open_fd();
	if ( fd < 0 )
		return;

	uint8_t *mapbase = bitrate_meminit( fd, MAPBITRATE_SIZE );

	if ( mapbase == MAP_FAILED )
	{
		close_fd( fd );

		return;
	}

	atc_bitrate( &actbitrate, mapbase );
	bitratr_destory( mapbase, (size_t) MAPBITRATE_SIZE );
	close_fd( fd );
	float maxbitrate = 9.36;
	sprintf( ch, "Bitrate(Act/Max)" );
	sprintf( ch1, " %.3fM/%.3fM ", actbitrate / 100, maxbitrate );
	lcd_Write_String( 0, ch );
	lcd_Write_String( 1, ch1 );
}


void lock_dspbitraet( int flag )
{
	char	ch[20]	= "";
	char	ch1[20] = "";

	s_config *dconfig = config_t();

	video_status_lock();

	float	actbitrate;
	int	fd = open_fd();
	if ( fd < 0 )
		return;

	uint8_t *mapbase = bitrate_meminit( fd, MAPBITRATE_SIZE );

	if ( mapbase == MAP_FAILED )
	{
		close_fd( fd );

		return;
	}

	atc_bitrate( &actbitrate, mapbase );

	bitratr_destory( mapbase, (size_t) MAPBITRATE_SIZE );
	close_fd( fd );
	lcd_Write_String( 0, "                " );
	lcd_Write_String( 1, "                " );

	snprintf( ch, 16, "DVB-T %.3fM", dconfig->localstatus.cfig_ad9789_ftw_bpf );
	snprintf( ch1, 16, "%s %.2fMbps", dconfig->localstatus.encoder_video_shrot_resolution, actbitrate / 100 );
	lcd_Write_String( 0, ch );
	lcd_Write_String( 1, ch1 );
}


void usb_read_bitrate( float *usbrbitrate )
{
	float	usbbitrate;
	int	fd = open_fd();
	if ( fd < 0 )
		return;

	uint8_t *mapbase = bitrate_meminit( fd, USBBITRATE_SIZE );

	if ( mapbase == MAP_FAILED )
	{
		close_fd( fd );

		return;
	}

	usb_bitrate( &usbbitrate, mapbase );

	bitratr_destory( mapbase, (size_t) USBBITRATE_SIZE );
	close_fd( fd );

	*usbrbitrate = usbbitrate / 100;
}


static void systemserial()
{
	char	ch[20]	= "";
	char	ch1[20] = "";
	sprintf( ch, "0355611100000552" );
	sprintf( ch1, "000asdf123cvf000" );
	lcd_Write_String( 0, ch );
	lcd_Write_String( 1, ch1 );
}


static void systemVersion()
{
	char	ch[20]	= "";
	char	ch1[20] = "";
	sprintf( ch, "EncoderModulator" );
	sprintf( ch1, "SW 0.27 HW 1.5A" );
	lcd_Write_String( 0, ch );
	lcd_Write_String( 1, ch1 );
}


#if 0
uint8_t run_time()
{
	int		day, hours, min, sec, t_day;
	static int	b_day = 0;

	init_cache();

	discontrl.lcd_time_key		= LCD_LOOP_ON;
	discontrl.call_time_enter	= 1;
	char ch[20] = "";
	get_system_time( &day, &hours, &min, &sec );

	if ( b_day == 0 )
		t_day = 0;
	else {
		t_day = day - b_day;
	}

	sprintf( ch, "%d Days-%02d:%02d:%02d", t_day, hours, min, sec );
	b_day = t_day;

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, ch, strlen( ch ), strlen( ch ), NULL, runtime );

	return(0);
}


#endif


uint8_t run_time()
{
	static int count = 0;
	signal_open( NORMAL_SIG );
	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( runtime );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}


	return(0);
}


uint8_t mbitrate()
{
	static int count = 0;
	signal_open( NORMAL_SIG );


	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( mod_bitraet );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}


	return(0);
}


void lockdsprate()
{
	init_cache();

	discontrl.lcd_time_key		= LCD_LOOP_ON;
	discontrl.call_time_enter	= 0;
	char ch[20] = " ";

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, ch, strlen( ch ), strlen( ch ), NULL, lock_dspbitraet );
}


uint8_t sys_serial()
{
	static int count = 0;
	signal_open( NORMAL_SIG );

	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( systemserial );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}

	return(0);
}


uint8_t sys_Version()
{
	static int count = 0;
	signal_open( NORMAL_SIG );

	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( systemVersion );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}


	return(0);
}


uint8_t start_eventname_Cfg() /*  */
{
	init_cache();
	char	tmpdata_2[17], odertmpdata_2[17];
	int	i, n, length, back_size;

	s_config *dconfig = config_t();

	if ( dconfig->scfg_Param.encoder_eit_now_eventname == NULL )
		return(-1);

	back_size = length = strlen( (char *) dconfig->scfg_Param.encoder_eit_now_eventname );

	i = 0;
	while ( dconfig->scfg_Param.encoder_eit_now_eventname[back_size] != '\b' ) /* '\b'??oóμ?êy?Y */
	{
		tmpdata_2[i] = dconfig->scfg_Param.encoder_eit_now_eventname[back_size];
		++i;
		back_size = back_size - 1;
	}


	for ( n = i - 1, i = 0; n > 0; n-- )
	{
		odertmpdata_2[i] = tmpdata_2[n];
		++i;
	}
	odertmpdata_2[i] = '\0';

	DEBUG( "%d  %s", i, odertmpdata_2 );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, odertmpdata_2, 8, 8, bin_starteventname_save, NULL );


	return(0);
}


uint8_t next_eventname_Cfg() /*  */
{
	init_cache();
	char	tmpdata_2[17], odertmpdata_2[17];
	int	i, n, length, back_size;

	s_config *dconfig = config_t();

	if ( dconfig->scfg_Param.encoder_eit_next_eventname == NULL )
		return(-1);

	back_size = length = strlen( (char *) dconfig->scfg_Param.encoder_eit_next_eventname );

	i = 0;
	while ( dconfig->scfg_Param.encoder_eit_next_eventname[back_size] != '\b' ) /* '\b'??oóμ?êy?Y */
	{
		tmpdata_2[i] = dconfig->scfg_Param.encoder_eit_next_eventname[back_size];
		++i;
		back_size = back_size - 1;
	}


	for ( n = i - 1, i = 0; n > 0; n-- )
	{
		odertmpdata_2[i] = tmpdata_2[n];
		++i;
	}
	odertmpdata_2[i] = '\0';

	DEBUG( "%d  %s", i, odertmpdata_2 );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, odertmpdata_2, 8, 8, bin_nexteventname_save, NULL );


	return(0);
}


uint8_t program_name()
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, ' ', 17 );

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_program_name );

	snprintf( filename, write_size + 1, "%s", dconfig->scfg_Param.encoder_program_name );
	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 16, write_size, text_Programname_save, NULL );


	return(0);
}


uint8_t Service_name()
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_service_name );

	snprintf( filename, write_size + 1, "%s", dconfig->scfg_Param.encoder_service_name );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 16, write_size, text_servername_save, NULL );


	return(0);
}


uint8_t pmt_pid()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int get_value = atoi( (const char *) &dconfig->scfg_Param.encoder_pmt_pid );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_digpmt_save );


	return(0);
}


uint8_t prc_pid()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();

	int get_value = atoi( (const char *) &dconfig->scfg_Param.encoder_pcr_pid );
	cfg_discontrl( get_value, DIGIT_STATUS, CURSOR_ON, 9000, 0, INPUT_DIGIT_STATUS, 4, &text_digprc_save );


	return(0);
}


uint8_t Constell_cfg()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.modulelate_mode_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"64 QAM",
		"QPSK  ",
		"16 QAM"
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_constell );

	return(0);
}


static void chech_lock()
{
	/*
	 * mis_lck 失锁
	 * mis_sum 失锁总数
	 */
	int		mis_lck = 1, mis_sum = 1;
	static int	count	= 0;
	int		menu_item_lenth;

	memset( discontrl.lcd_locknote, 0, 17 );


	static char *Bandwidth[2];

	if ( (mis_lck == 0) && (mis_sum == 0) )
	{
		Bandwidth[0]	= "Video 1 not lock";
		menu_item_lenth = 1;
	}else if ( (mis_lck == 1) && (mis_sum == 1) )
	{
		Bandwidth[0]	= "TS not lock     ";
		menu_item_lenth = 1;
	}else if ( (mis_lck == 1) && (mis_sum == 2) )
	{
		Bandwidth[0]	= "Video 1 not lock";
		Bandwidth[1]	= "TS not lock     ";
		menu_item_lenth = 2;
	}else if ( (mis_lck == -1) && (mis_sum == -1) )
	{
		Bandwidth[0]	= "System is normal";
		menu_item_lenth = 1;
	}


//	DEBUG( "menu_item_lenth : %d  count : %d ", menu_item_lenth, count );

	if ( count >= menu_item_lenth )
		count = 0;


	if ( (mis_lck == -1) && (mis_sum == -1) )
	{
		snprintf( discontrl.lcd_locknote, 17, "%s", "Alarm           " );
	}else{
		snprintf( discontrl.lcd_locknote, 17, "Alarm (%d/%d)     ", count + 1, mis_sum );
	}

	lcd_Write_String( 0, discontrl.lcd_locknote );
	lcd_Write_String( 1, Bandwidth[count] );
	count++;
}


uint8_t systemstat_cfg()
{
	static int count = 0;
	signal_open( NORMAL_SIG );

	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( chech_lock );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}

	return(0);
}


void null_cahebuf()
{
	printf( "null test -^-^- \n" );
}


uint8_t  NullSubs( void )
{
	return(0);
}


void str_filename_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "tsfilename" );
	}
	config_read( get_profile()->script_configfile );
}


void str_netwrorkname_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "NetWorkName" );
	}
}


void passwrd_filename_save( char *orgbuf, char *repbuf )
{
	if ( repbuf != NULL )
	{
		DEBUG( "repbuf =%s \n", repbuf );
		int fd;

		fd = open( ACSSECC_PATH, O_RDWR | O_CREAT | O_TRUNC );
		if ( fd < 0 )
		{
			DEBUG( "file open error" );
			return;
		}
		int pas_size = strlen( repbuf );
		if ( pas_size > 6 )
			pas_size = 6;
		_passwrd	passwrd[2];
		s_config	*dconfig = config_t();
		memset( passwrd, 0, sizeof(passwrd) );

		snprintf( passwrd[0].pwrd, pas_size + 1, "%s", (char *) dconfig->scfg_Param.system_def_root_pwd );


		snprintf( passwrd[1].pwrd, pas_size + 1, "%s", repbuf );

		/* 写默认密码 */

		write( fd, passwrd, sizeof(passwrd) );


		close( fd );
	}
}


uint8_t ts()
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );

	int write_size = strlen( dconfig->configParam.usb_tsfilename );

	snprintf( filename, write_size + 1, "%s", dconfig->configParam.usb_tsfilename );
	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 10, write_size, str_filename_save, NULL );

	return(0);
}


uint8_t network_name()
{
	init_cache();
	char		filename[16];
	s_config	*dconfig = config_t();
	memset( filename, 0, 16 );

	int write_size = strlen( (char *) dconfig->scfg_Param.stream_nit_network_name );

	snprintf( filename, write_size + 1, "%s", dconfig->scfg_Param.stream_nit_network_name );
	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 10, 10, str_netwrorkname_save, NULL );
	config_read( get_profile()->script_configfile );
	return(0);
}


void cfg_discontrl_c( int char_digit_status, int curos_stat,
		      char *inputename, int input_lenth, int orgstrlen, void (* config_f)( char *, char * ),
		      void (* loopdsplay)() )
{
	DEBUG( " orgstrlen = %d  %s ", orgstrlen, inputename );
	memset( discontrl.filename, 0, 16 );
	discontrl.write_char_dig_status = char_digit_status;
	discontrl.write_size		= orgstrlen;
	discontrl.input_lenth		= input_lenth;
	snprintf( discontrl.filename, input_lenth + 1, "%s", inputename );
	discontrl.filename[input_lenth + 1]	= '\0';
	discontrl.curos_stat			= curos_stat;
	discontrl.wd_configfile			= config_f;
	discontrl.lcd_loopdsplay		= loopdsplay;
}


int comm_video_interface( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "HDMI ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = HDMI_M;

			sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_interface_name );

			config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "videomod" );

			/* dev_config_printf(); */
			adv_defualt_set(); /* hdmi */
		} else if ( strncmp( "YPbPr", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = YPbPr_M;
			sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_interface_name );

			config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "videomod" );

			/* dev_config_printf(); */
			YPbPr_def();
		} else if ( strncmp( "CVBS ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = CVBS_M;
			sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_interface_name );

			config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "videomod" );

			/* dev_config_printf(); */

			cvbs_def();
		}
		config_read( get_profile()->script_configfile );
		return(0);
	}
}


static int write_char( int size, int interval, int change, char *data )
{
	int i;

	for ( i = 0; i < size; i++ )
		key_input_char( i + interval, data[i], change, discontrl.lcdfd );
	return(i);
}


static void video_lock_status( int flag )
{
	video_status_lock();

	s_config *dconfig = config_get_config(); /*  */

	switch ( dconfig->localstatus.encoder_video_ident )
	{
	case ENCODE_IDENT_LOCK: {
		if ( VIDEO_INPUT_LOCK == dconfig->localstatus.encoder_video_input_lock )
		{
			char	*arr_cfg	= "lock    ";
			int	arr_size	= strlen( arr_cfg );

			write_char( 16, 1, CHANGE_G, "                " );
			write_char( arr_size, 2, CHANGE_G, arr_cfg );
		} else if ( VIDEO_INPUT_UNLOCK == dconfig->localstatus.encoder_video_input_lock )
		{
			char	*arr_cfg	= "Not lock";
			int	arr_size	= strlen( arr_cfg );
			write_char( 16, 1, CHANGE_G, "                " );
			write_char( arr_size, 2, CHANGE_G, arr_cfg );
		}
	}
	break;

	case ENCODE_IDENT_RES: {
		if ( VIDEO_INPUT_LOCK == dconfig->localstatus.encoder_video_input_lock )
		{
			int arr_size = strlen( dconfig->localstatus.encoder_video_resolution );
			if ( flag == 0x300 )
			{
				write_char( 16, 1, ~CHANGE_G, "Video resolution" );
				ctl_fun( discontrl.lcdfd, 0x00 );
			}
			write_char( 16, 1, CHANGE_G, "                " );
			write_char( arr_size, 2, CHANGE_G, dconfig->localstatus.encoder_video_resolution );
		} else if ( VIDEO_INPUT_UNLOCK == dconfig->localstatus.encoder_video_input_lock )
		{
			int arr_size = strlen( dconfig->localstatus.encoder_video_resolution );

			if ( flag == 0x300 )
			{
				write_char( 16, 1, ~CHANGE_G, "Video resolution" );
				ctl_fun( discontrl.lcdfd, 0x00 );
			}

			write_char( 16, 1, CHANGE_G, "                " );
			write_char( arr_size, 2, CHANGE_G, dconfig->localstatus.encoder_video_resolution );
		}
	}
	break;
	}
}


int test_signal( void )
{
	return(g_signal_t.signal_open_colose);
}


int signal_close( void )
{
	g_signal_t.signal_open_colose = 0;

	return(g_signal_t.signal_open_colose);
}


void signal_open( int open_flag )
{
	g_signal_t.signal_open_colose = open_flag;
}


uint8_t  video_lock_status_Cfg()
{
	static int	count		= 0;
	s_config	*dconfig	= config_get_config(); /*  */
	dconfig->localstatus.encoder_video_ident = ENCODE_IDENT_LOCK;
	signal_open( NORMAL_SIG );


	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( video_lock_status );


		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}


	return(0);
}


static void video_status_lock_( int falg )
{
	video_status_lock();
	video_lock_status( 0x300 );
}


uint8_t  video_resolution()
{
	static int count = 0;
	signal_open( NORMAL_SIG );
	s_config *dconfig = config_get_config(); /*  */
	dconfig->localstatus.encoder_video_ident = ENCODE_IDENT_RES;


	if ( !count )
	{
		start_alarm();
		set_state_desplay( MONITOER_TRUE, SYS_RUN_TIME_START, SYS_RUN_TIME_FLAG );
		function_inter( video_status_lock_ );

		count = 1;
	}else{
		stop_alarm();
		signal_close();
		count = 0;
	}
	return(0);
}


uint8_t  video_inerface_cfg()
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();

	get_cfg_menu_item( dconfig->scfg_Param.encoder_video_interface_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"HDMI ",
		"YPbPr",
		"CVBS "
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_video_interface );

	return(0);
}


void get_cfg_menu_item( char *cfg_name )
{
	if ( !cfg_name )
		return;
	memset( discontrl.cfg_menu, '\0', 17 );
	snprintf( discontrl.cfg_menu, strlen( cfg_name ) + 1, "%s", cfg_name );
}


s_config *config_t()
{
	s_config *dconfig = config_get_config(); /*  */
	return(dconfig);
}


void auto_usb_test( void )
{
	s_config *dconfig = config_t();
	if ( strncmp( dconfig->scfg_Param.stream_usb_record_auto, EIT_ENABLE,
		      sizeof(dconfig->scfg_Param.stream_usb_record_auto) ) == 0 )
		discontrl.record_auto_flag = USB_AUTO_HAND;
}


uint8_t Bandwidth_Cfg()
{
	/* ??±?LCD2?μ￥1|?ü */

	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.channel_width_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"8MHz",
		"7MHz",
		"6MHz"
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, comm_Bandwidth );
	return(0);
}


uint8_t fft_NK_cfg()
{
	/* ??±?LCD2?μ￥1|?ü */

	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.fft_mode_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"FFT 2K",
		"FFT 8K",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, fft_comm );

	return(0);
}


uint8_t rf_out()
{
	s_config *dconfig = config_t();
	get_cfg_menu_item( (char *) dconfig->scfg_Param.modulator_rf_on_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"Yes",
		"No ",
	};

	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, rf_comm );


	return(0);
}


uint8_t coderate_cfg( void )
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();

	get_cfg_menu_item( dconfig->scfg_Param.encode_rate_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"1/2",
		"2/3",
		"3/4",
		"5/6",
		"7/8"
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, coderate_comm );
	return(0);
}


uint8_t guardIntv_dalay_cfg( void )
{
	/* ??±?LCD2?μ￥1|?ü */
	s_config *dconfig = config_t();
	get_cfg_menu_item( dconfig->scfg_Param.deltal_name );
	init_cache();
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = MENU_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? ,0x03?a2?μ￥???? */
	static char *Bandwidth[] = {
		"1/32",
		"1/16",
		"1/8 ",
		"1/4 ",
	};


	int menu_item_lenth = sizeof(Bandwidth) / sizeof(Bandwidth[0]);
	menu_general_enter( Bandwidth, menu_item_lenth, guardIntv_comm );

	return(0);
}


void menu_general_enter( char  **Bandwidth, int menu_item_lenth, int (* menu_cfg_fun)( char * ) )
{
	discontrl.arr		= Bandwidth;
	discontrl.write_size	= menu_item_lenth;
	discontrl.menu_cfg_fun	= menu_cfg_fun;
}


int guardIntv_comm( char* buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "1/32", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "F_DELTAL_32_1";
		} else if ( strncmp( "1/16", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "F_DELTAL_16_1";
		} else if ( strncmp( "1/8 ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "F_DELTAL_8_1";
		} else if ( strncmp( "1/4 ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "F_DELTAL_4_1";
		}

		DEBUG( "dconfig->scfg_Param.deltal_name=%s", dconfig->scfg_Param.deltal_name );
		sprintf( tmp_buf, "%s", dconfig->scfg_Param.deltal_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "Deltal" );

		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


void fre_dig_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "AD9789CFG" );
		config_read( get_profile()->script_configfile );
	}

	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_digfilesize_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "tsfilesize" );
	}
	config_read( get_profile()->script_configfile );
}


void gener_table()
{
	/* config_read( get_profile()->script_configfile ); */
	s_config	*dconfig	= config_t();
	int		_tsid		= dconfig->scfg_Param.stream_tsid;
	if ( _tsid < 0 )
	{
		_tsid = TS_ID;
	}


	/* pat */
	set_pat_table( atoi( (char *) dconfig->scfg_Param.encoder_program_number ),
		       atoi( (char *) dconfig->scfg_Param.encoder_pmt_pid ), PAT_VAERSION, _tsid );

	usleep( 2000 );

	/* pmt */
	set_pmt_table( atoi( (char *) dconfig->scfg_Param.encoder_audio_pid ), atoi( (char *) dconfig->scfg_Param.encoder_video_pid ),
		       atoi( (char *) dconfig->scfg_Param.encoder_pcr_pid ), atoi( (char *) dconfig->scfg_Param.encoder_program_number ), PMT_VERSION,
		       atoi( (char *) dconfig->scfg_Param.encoder_pmt_pid ) );
	usleep( 2000 );
	/* sdt */

	char	tmp2[16]	= { 0 };        /* "\1\vTV-Provider"; */
	char	tmp[16]		= { 0 };        /* "\6TV-101" */
	sprintf( tmp, "\1\v%s", dconfig->scfg_Param.encoder_service_name );
	sprintf( tmp2, "\6%s", dconfig->scfg_Param.encoder_program_name );
	int	sum			= strlen( tmp );
	int	sum2			= strlen( tmp2 );
	int	sdt_descriptor_length	= sum + sum2;
	DEBUG( "lenth =%d", sdt_descriptor_length );
	uint8_t *service_data;
	service_data	= (uint8_t *) calloc( sdt_descriptor_length, sizeof(char) );
	service_data	= (uint8_t *) link_strncpy( tmp, tmp2, sum, sum2 );
	uint16_t networkid = (uint16_t) atoi( (char *) dconfig->scfg_Param.stream_nit_network_id );
	if ( networkid < 0 )
	{
		networkid = NETWORK_ID;
	}
	DEBUG( "s =%s", service_data );
	DEBUG( "networkid =0x%04x", networkid );
	set_sdt_table( networkid, atoi( (char *) dconfig->scfg_Param.encoder_program_number ),
		       sdt_descriptor_length, SDT_TABLE_ID,
		       SDT_VERSION, service_data );
	usleep( 2000 );
	/* nit */
	uint8_t user_defined_descriptor_length = 4;
	uint8_t user_defined_descriptor[user_defined_descriptor_length];

	uint8_t descriptor[4] = { 0 };
	arr_to_hex( dconfig->scfg_Param.stream_nit_private_data, descriptor );

	user_defined_descriptor[0]	= descriptor[3];
	user_defined_descriptor[1]	= descriptor[2];
	user_defined_descriptor[2]	= descriptor[1];
	user_defined_descriptor[3]	= descriptor[0];

	set_usr_des( user_defined_descriptor, user_defined_descriptor_length,
		     (uint8_t) atoi( (char *) dconfig->scfg_Param.encoder_program_number ),
		     dconfig->scfg_Param.stream_nit_programifo_lcn );

	set_dvb_des( (dconfig->localstatus.cfig_ad9789_ftw_bpf) * 1000 * 100, dconfig->scfg_Param.channel_width,
		     dconfig->scfg_Param.modulelate_mode, dconfig->scfg_Param.encode_rate,
		     CODE_RAT_LP_STREAM, dconfig->scfg_Param.deltal,
		     dconfig->scfg_Param.fft_mode );
	uint8_t network_name_length	= strlen( (char *) dconfig->scfg_Param.stream_nit_network_name );       /* 10; */
	uint8_t network_name[16]	= { 0 };                                                                /* = "network-01"; */

	snprintf( (char *) network_name, network_name_length + 1, (char *) dconfig->scfg_Param.stream_nit_network_name );

	int _nitversion = atoi( (const char *) &dconfig->scfg_Param.stream_nit_version_number );
	if ( _nitversion < 0 )
		_nitversion = NIT_VERSION;
	DEBUG( "_nitversion = %d", _nitversion );
	set_nit_table( networkid, network_name,
		       network_name_length, NIT_TABLE_ID,
		       _nitversion, _tsid );
	usleep( 2000 );

	usr_cfg_eit_t cfg_eit_t;

	sprintf( (char *) cfg_eit_t.nit_private_data, "%s", dconfig->scfg_Param.stream_nit_private_data );

	cfg_eit_t.program_number	= (uint16_t) atoi( (char *) dconfig->scfg_Param.encoder_program_number );
	cfg_eit_t.network_id		= networkid;

	sprintf( (char *) cfg_eit_t.eit_now_startyear, "%s", dconfig->scfg_Param.encoder_eit_now_startyear );
	sprintf( (char *) cfg_eit_t.eit_now_starttime, "%s", dconfig->scfg_Param.encoder_eit_now_starttime );
	sprintf( (char *) cfg_eit_t.eit_now_duration, "%s", dconfig->scfg_Param.encoder_eit_now_duration );
	sprintf( (char *) cfg_eit_t.eit_now_eventname, "%s", dconfig->scfg_Param.encoder_eit_now_eventname );

	sprintf( (char *) cfg_eit_t.eit_next_startyear, "%s", dconfig->scfg_Param.encoder_eit_next_startyear );
	sprintf( (char *) cfg_eit_t.eit_next_starttime, "%s", dconfig->scfg_Param.encoder_eit_next_starttime );
	sprintf( (char *) cfg_eit_t.eit_next_duration, "%s", dconfig->scfg_Param.encoder_eit_next_duration );
	sprintf( (char *) cfg_eit_t.eit_next_eventname, "%s", dconfig->scfg_Param.encoder_eit_next_eventname );


/*
 * if ( strncmp( (char *) dconfig->scfg_Param.stream_eit_insert, EIT_ENABLE,
 *            sizeof( (char *) dconfig->scfg_Param.stream_eit_insert) ) == 0 )
 */
	set_eit_table( &cfg_eit_t );
/*	usleep( 2000 ); */
	/* program output */

	if ( strncmp( (char *) dconfig->scfg_Param.encoder_program_output, PROGRAM_OUT,
		      sizeof( (char *) dconfig->scfg_Param.encoder_program_output) ) == 0 )
		DEBUG( "Program output [%s]", dconfig->scfg_Param.encoder_program_output );
	else
		DEBUG( "Program output [%s]", dconfig->scfg_Param.encoder_program_output );

	free( service_data );
}


void text_program_num_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "ProgrNmu" );
	}
	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();

	discontrl.pid_status = 0x01;
}


void text_networkid_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "NETWORKID" );
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	/* discontrl.pid_status = 0x01; / * ?a?? ????×′ì? * / */
}


/* 以后需要修改 20160419 */
void bin_nitlcn_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		uint8_t nit_private_data[9];
		snprintf( (char *) nit_private_data, 4, "%s", repbuf );
		uint16_t	date		= (uint16_t) atoi( repbuf );
		s_config	*dconfig	= config_t(); /*  */

		dconfig->scfg_Param.stream_nit_programifo_lcn = date;

		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_networknumb_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "NETWORKNUMB" );
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	/* discontrl.pid_status = 0x01; / * ?a?? ????×′ì? * / */
}


void text_PrivateData_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		uint8_t nit_private_data[9];
		snprintf( (char *) nit_private_data, 9, "%s", repbuf );
		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.stream_nit_private_data, 0, 9 );
		/* config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "PRIVATEDATE" ); */
		snprintf( (char *) dconfig->scfg_Param.stream_nit_private_data, 9, "%s", nit_private_data );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	/* discontrl.pid_status = 0x01; / * ?a?? ????×′ì? * / */
}


void bin_eit_year_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		uint8_t tmpdata[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );

		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_now_startyear, 0, 9 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_now_startyear, 9, "%s", tmpdata );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_eit_time_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		char tmpdata[9], buf[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );
		str_prase_pending_colon( tmpdata, buf );
		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_now_starttime, 0, 7 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_now_starttime, 7, "%s", buf );

		DEBUG( "encoder_eit_now_starttime=%s", dconfig->scfg_Param.encoder_eit_now_starttime );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_eit_nexttime_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		char tmpdata[9], buf[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );
		str_prase_pending_colon( tmpdata, buf );
		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_next_starttime, 0, 7 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_next_starttime, 7, "%s", buf );

		DEBUG( "encoder_eit_now_starttime=%s", dconfig->scfg_Param.encoder_eit_next_starttime );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_eit_duration_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		char tmpdata[9], buf[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );
		str_prase_pending_colon( tmpdata, buf );
		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_now_duration, 0, 7 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_now_duration, 7, "%s", buf );

		DEBUG( "encoder_eit_now_starttime=%s", dconfig->scfg_Param.encoder_eit_now_duration );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_eit_nextduration_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		char tmpdata[9], buf[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );
		str_prase_pending_colon( tmpdata, buf );
		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_next_duration, 0, 7 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_next_duration, 7, "%s", buf );

		DEBUG( "encoder_eit_now_starttime=%s", dconfig->scfg_Param.encoder_eit_next_duration );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_eit_next_year_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		uint8_t tmpdata[9];
		snprintf( (char *) tmpdata, 9, "%s", repbuf );

		s_config *dconfig = config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.encoder_eit_next_startyear, 0, 9 );
		snprintf( (char *) dconfig->scfg_Param.encoder_eit_next_startyear, 9, "%s", tmpdata );
		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_language_save( char *orgbuf, char *repbuf )
{
	uint8_t		tmpdata[9]		= { 0 };
	char		tmpdata_2[9]		= { 0 };
	char		odertmpdata_2[9]	= { 0 };
	char		length			= 0;
	s_config	*dconfig		= config_t(); /*  */

	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );

		int	write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_now_eventname );
		int	i;
		length = 3;
		for ( i = 0; i < length; i++ ) /* '\b' ???°μ?êy?Y */
			tmpdata[i] = repbuf[i];

		tmpdata[i] = '\0';

		int back_size = write_size;

		i = 0;
		while ( dconfig->scfg_Param.encoder_eit_now_eventname[back_size] != '\b' ) /* '\b'??oóμ?êy?Y */
		{
			tmpdata_2[i] = dconfig->scfg_Param.encoder_eit_now_eventname[back_size];
			++i;
			back_size = back_size - 1;
		}

		int n;
		for ( n = i - 1, i = 0; n > 0; n-- )
		{
			odertmpdata_2[i] = tmpdata_2[n];
			++i;
		}
		odertmpdata_2[i] = '\0';

		memset( (char *) dconfig->scfg_Param.encoder_eit_now_eventname, 0, 17 );
		sprintf( (char *) dconfig->scfg_Param.encoder_eit_now_eventname, "%s\b%s", tmpdata, odertmpdata_2 );

		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	/* discontrl.pid_status = 0x01; / * ?a?? ????×′ì? * / */
}


void bin_starteventname_save( char *orgbuf, char *repbuf )
{
	uint8_t tmpdata[9] = { 0 };
	/* char		tmpdata_2[9]		= { 0 }; */
	char		odertmpdata_2[9]	= { 0 };
	char		length			= 0;
	int		i;
	s_config	*dconfig = config_t(); /*  */

	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );

		if ( !dconfig->scfg_Param.encoder_eit_now_eventname )
			return;

		length = 3;
		for ( i = 0; i < length && dconfig->scfg_Param.encoder_eit_now_eventname[i] != '\b'; i++ )      /* '\b' ???°μ?êy?Y */
			tmpdata[i] = dconfig->scfg_Param.encoder_eit_now_eventname[i];

		tmpdata[i] = '\0';

		for ( i = 0; i < 8; i++ )                                                                       /* '\b' ??oóμ?êy?Y */
			odertmpdata_2[i] = repbuf[i];
		tmpdata[i] = '\0';

		memset( (char *) dconfig->scfg_Param.encoder_eit_now_eventname, 0, 17 );
		sprintf( (char *) dconfig->scfg_Param.encoder_eit_now_eventname, "%s\b%s", tmpdata, odertmpdata_2 );

		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void bin_nexteventname_save( char *orgbuf, char *repbuf )
{
	uint8_t tmpdata[9] = { 0 };
	/* char		tmpdata_2[9]		= { 0 }; */
	char		odertmpdata_2[9]	= { 0 };
	char		length			= 0;
	int		i;
	s_config	*dconfig = config_t(); /*  */

	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );

		if ( !dconfig->scfg_Param.encoder_eit_next_eventname )
			return;

		length = 3;
		for ( i = 0; i < length && dconfig->scfg_Param.encoder_eit_next_eventname[i] != '\b'; i++ )     /* '\b' ???°μ?êy?Y */
			tmpdata[i] = dconfig->scfg_Param.encoder_eit_next_eventname[i];

		tmpdata[i] = '\0';

		for ( i = 0; i < 8; i++ )                                                                       /* '\b' ??oóμ?êy?Y */
			odertmpdata_2[i] = repbuf[i];
		tmpdata[i] = '\0';

		memset( (char *) dconfig->scfg_Param.encoder_eit_next_eventname, 0, 17 );
		sprintf( (char *) dconfig->scfg_Param.encoder_eit_next_eventname, "%s\b%s", tmpdata, odertmpdata_2 );

		set_nit_table_parm();
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_Programname_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s \n repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "ProgramName" );
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_servername_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "ServiceName" );
	}

	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_digpmt_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "PMTPID" );
	}
	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_digprc_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "PRCPID" );
	}
	config_read( get_profile()->script_configfile );

	pr_cfg();
	gener_table();

	discontrl.pid_status = 0x01;
}


void text_digvpid_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "VidPID" );
	}
	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_digapid_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "AudioPID" );
	}
	config_read( get_profile()->script_configfile );
	pr_cfg();
	gener_table();
	discontrl.pid_status = 0x01;
}


void text_videotare_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "VideoBitrate" );
	}

	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


int text_audiotare_save( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "64  kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "64_kbps";
		} else if ( strncmp( "96  kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "96_kbps";
		} else if ( strncmp( "112 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "112_kbps";
		} else if ( strncmp( "128 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "128_kbps";
		} else if ( strncmp( "160 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "160_kbps";
		} else if ( strncmp( "192 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "192_kbps";
		} else if ( strncmp( "224 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "224_kbps";
		} else if ( strncmp( "256 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "256_kbps";
		} else if ( strncmp( "320 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "320_kbps";
		} else if ( strncmp( "384 kbps", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "384_kbps";
		}


		DEBUG( "dconfig->configParam.encoder_audio_bitrate_name=%s", dconfig->scfg_Param.encoder_audio_bitrate_name );
		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_audio_bitrate_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "AudioBitrate" );

		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int text_audioformat_save( char *buffer )
{
	return(0);

#if 0
	char	tmp_buf[18];
	char	*orgbuf;
/* 保留将来使用 */
	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "MPEG-2    ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MPEG-2";
		} else if ( strncmp( "MPEG-2 AAC", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MPEG-2-AAC";
		} else if ( strncmp( "MPEG-4 AAC", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MPEG-4-AAC";
		}


		DEBUG( "encoder_audio_format_name=%s", dconfig->scfg_Param.encoder_audio_format_name );
		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_audio_format_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "AudioFormat" );

		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
#endif
}


void text_videobright_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "Brightness" );
	}
	config_read( get_profile()->script_configfile );
	
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_videocpbright_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "CpBrightness" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_videocontrast_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "Contrast" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_videocpcontrast_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "CpContrast" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_videosaturation_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "Saturation" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_videocpsaturation_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "CpSaturation" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_streamtsid_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		/* config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "RFLevel" ); */
	}

	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_modulatorrf_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "RFLevel" );
		config_read( get_profile()->script_configfile );
	}

	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
}


void text_streamonid_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "StreamONID" );
	}
	config_read( get_profile()->script_configfile );
	/* discontrl.pid_status = 0x01; / * ?a?? ????×′ì? * / */
}


void text_videohue_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "Videohue" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01;
}


void text_videocphue_save( char *orgbuf, char *repbuf )
{
	if ( orgbuf != NULL && repbuf != NULL )
	{
		DEBUG( "orgbuf =%s     repbuf =%s \n", orgbuf, repbuf );
		config_set_config( SYS_ETC_CONF, orgbuf, (uint8_t *) repbuf, "CpVideohue" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01;
}


void null_Subfun( char *orgbuf, char *repbuf )
{
}


int null_Subcfg( char *cfgbuf )
{
	return(0);
}


/* ?éò?×÷?aêy?μ?????￡°?2??? */
uint8_t  RF_frequency_status()
{
	s_config *dconfig = config_t();

	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	int input_lenth = 7;

	discontrl.lcd_b_last_dsp.i_ldsp_length		= 3;
	discontrl.lcd_b_last_dsp.desplay_b_pos		= 1;
	discontrl.lcd_b_last_dsp.contian_decimal_length = input_lenth;
	discontrl.lcd_b_last_dsp.desplay_l_pos		= input_lenth + 3;
	discontrl.lcd_b_last_dsp._power			= 3;

	snprintf( discontrl.lcd_b_last_dsp.l_dsp, input_lenth, "%s", "MHZ" );
#if 1

	cfg_discontrl( dconfig->localstatus.cfig_ad9789_ftw_bpf, DIGIT_STATUS, CURSOR_ON, 960.000, 30.000, INPUT_FLOAT_STATUS, input_lenth, &fre_dig_save );
#else
	/* 2aê?ò???×?·?ê?è?1|?ü */
	discontrl.write_char_dig_status = 0x02; /* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? */
	char *textstr = "welecom to    ";
	memset( discontrl.filename, 0, 16 );
	snprintf( discontrl.filename, strlen( textstr ), "%s", textstr );
	discontrl.write_size	= strlen( textstr );
	discontrl.curos_stat	= 0x01;         /* 1a±ê?a?? */

#endif
	return(0);
}


/*
 * cfg_size 配置的值
 * char_digit_status  字符输入还是数字输入或者是其它输入方式(CHAR_STATUS,DIGIT_STATUS，MENU_STATUS)等
 * curos_stat  光标闪烁是否开启(CURSOR_ON,CURSOR_OFF)
 * max_regin 配置参数的最大值限制，如果大于这个值后，将默认修改为此值
 * min_region 配置参数的最小值限制
 * f_status 输入的是浮点数还是整数(DIGIT_STATUS,DIGIT_STATUS)
 * input_lenth 要配置数据的bit数的长度(包含小数点的长度)
 * g_configfile 通用函数
 *
 */
void cfg_discontrl( double cfg_size, int char_digit_status, int curos_stat, double max_regin, int min_region,
		    int f_status, int input_lenth, void (* g_configfile)( char *, char * ) )
{
	discontrl.cfg_size		= cfg_size;             /* ′?μY???? */
	discontrl.write_char_dig_status = char_digit_status;    /* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? */
	discontrl.curos_stat		= curos_stat;           /* 1a±ê?a?? */
	discontrl.f_status		= f_status;             /* 0x02 ??êy￡?0x01??μ? */
	discontrl.input_lenth		= input_lenth;          /* ×?·?ê?è?3¤?è */
	discontrl.max_regin		= max_regin;
	discontrl.min_region		= min_region;
	discontrl.wd_configfile		= g_configfile;
}


int coderate_comm( char* buffer )
{
	char	tmp_buf[18];
	char	* orgbuf;
	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "7/8", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "ENCODE_RATE_8_7";
		} else if ( strncmp( "5/6", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "ENCODE_RATE_6_5";
		} else if ( strncmp( "3/4", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "ENCODE_RATE_4_3";
		} else if ( strncmp( "2/3", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "ENCODE_RATE_3_2";
		} else if ( strncmp( "1/2", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "ENCODE_RATE_2_1";
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encode_rate_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "EncodeRate" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int comm_constell( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "64 QAM", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MODULATE_M_64QAM";
		} else if ( strncmp( "QPSK  ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MODULATE_M_QPSK";
		} else if ( strncmp( "16 QAM", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MODULATE_M_16QAM";
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.modulelate_mode_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "ModulelateMode" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int eit_insert_commom( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_ENABLE;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_DESENABLE;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_eit_insert );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "EITISERTENABLE" );
	}
	config_read( get_profile()->script_configfile );
	return(0);
}


int nit_insert_commom( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = NIT_ENABLE;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = NIT_DESENABLE;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_nit_insert );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "NITISERTENABLE" );
	}
	config_read( get_profile()->script_configfile );
	return(0);
}


static int eauto_record_commom( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_ENABLE;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_DESENABLE;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_usb_record_auto );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "AutoModle" );
	}
	config_read( get_profile()->script_configfile );
	return(0);
}


int root_lock_commom( char *buffer )
{
	/* char	tmp_buf[18]; */
	char *orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf					= ENABLE;
			dconfig->scfg_Param.system_pwd_count	= 1;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = DESENABLE;
		}


		int lenght = strlen( orgbuf ) + 1;
		memset( (char *) dconfig->scfg_Param.stream_lck_state, 0, 8 );
		snprintf( (char *) dconfig->scfg_Param.stream_lck_state, lenght, "%s", orgbuf );
	}

	return(0);
}


int rec_usb_commom( char *buffer )
{
	/* char	tmp_buf[18]; */
	char *orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = ENABLE;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = DESENABLE;
		}


		int lenght = strlen( orgbuf ) + 1;
		memset( (char *) dconfig->scfg_Param.stream_usb_enable, 0, 8 );
		snprintf( (char *) dconfig->scfg_Param.stream_usb_enable, lenght, "%s", orgbuf );
	}

	return(0);
}


int program_enable_commom( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Enable ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_ENABLE;
		} else if ( strncmp( "Disable", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_DESENABLE;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_program_output );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "ProgramOutput" );
	}
	config_read( get_profile()->script_configfile );
	discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	return(0);
}


int comm_h264profile( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "High Profile", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "HIGH_CFG";
		} else if ( strncmp( "Main Profile", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "MAIN_CFG";
		} else if ( strncmp( "Base Profile", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "BASE_CFG";
		}else if ( strncmp( "Default       ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "DEFAULT_CFG";
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_profile_name );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "ProfileConfig" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int comm_videonorm( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "AUTO     ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "AUTO";
		} else if ( strncmp( "PAL-BGHID", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "PAL-BGHID";
		} else if ( strncmp( "PAL-N    ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "PAL-N";
		} else if ( strncmp( "PAL-CN   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "PAL-CN";
		} else if ( strncmp( "PAL-M    ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "PAL-M";
		} else if ( strncmp( "NTSC-M   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "NTSC-M";
		}else if ( strncmp( "NTSC-J   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "NTSC-J";
		} else if ( strncmp( "NTSC-4.43", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "NTSC-4.43";
		}


		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_norm_name );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "VideoNorm" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}

	
int comm_h264level( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

DEBUG("l :%s",buffer);
	if ( buffer == NULL )
		return(-1);
	else{

		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "Level   4", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf =  "Level4";
		} else if ( strncmp( "Level 4.1", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "Level4_1";
		} else if ( strncmp("Level 4.2", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "Level4_2";
		} else if ( strncmp( "Level   3", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "Level3";
		} else if ( strncmp( "Level 3.1", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf =  "Level3_1";
		} else if ( strncmp( "Level 3.2", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "Level3_2";
		} else if ( strncmp( "Default  ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "Leveldefu";
		}
		


		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_level_name );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "LevelConfig" );
		discontrl.pid_status = 0x01; /* ?a?? ????�'�? */
	}
	return(0);
}


int comm_lcdtime( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "5s  ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t5s";
		} else if ( strncmp( "10s ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t10s";
		} else if ( strncmp( "30s ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t30s";
		} else if ( strncmp( "45s ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t45s";
		} else if ( strncmp( "60s ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t60s";
		} else if ( strncmp( "90s ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t90s";
		}else if ( strncmp( "120s", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "t120s";
		}


		sprintf( tmp_buf, "%s", dconfig->scfg_Param.system_lcd_time_out_name );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "LcdTime" );
	}
	config_read( get_profile()->script_configfile );
	return(0);
}


int comm_play_mod( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "Single file", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_SIG;
		} else if ( strncmp( "Single loop", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_SIG_LOOP;
		} else if ( strncmp( "Play all   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_SIG_PLAY_ALL;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_usb_play_mod );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "PlayModle" );
	}

	config_read( get_profile()->script_configfile );

	return(0);
}


int comm_save_mod( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "Single file   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_RECORD_SIG_MOD;
		}else if ( strncmp( "Segmented file", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_RECORD_SEG_MOD;
		} else if ( strncmp( "loop record   ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = USB_RECORD_LOOP_MOD;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_usb_record_save_mod );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "RecordModle" );
	}

	config_read( get_profile()->script_configfile );

	return(0);
}


int comm_nitlcn_mod( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "European ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "European_Cfg";
		} else if ( strncmp( "NorDig V1", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "NorDig_V1Cfg";
		} else if ( strncmp( "NorDig V1", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "NorDig_V2Cfg";
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.stream_nit_lcn_mode_name );
		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "NITLCNMOD" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int comm_factory_set( char *buffer )
{
	s_config *dconfig = config_t();
	dconfig->scfg_Param.system_restart_flag = SYS_RESTART;


	system_reset_conf();

	system( RES_ETC_CONF );
	return(0);
}


int comm_ratemode( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */
		if ( strncmp( "CBR", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "CBR";
		} else if ( strncmp( "VBR", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "VBR";
		}
		sprintf( tmp_buf, "%s", dconfig->scfg_Param.encoder_video_ratemode );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "RateMode" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}
	return(0);
}


int comm_Bandwidth( char *buffer )
{
	char tmp_buf[12];

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.channel_width_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) buffer, "ChannelWidth" );
		discontrl.pid_status = 0x01;    /* ?a?? ????×′ì? */
	}

	return(0);
}


int fft_comm( char*buffer )
{
	char	tmp_buf[12];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "FFT 8K", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "FFT_8K";
		} else if ( strncmp( "FFT 2K", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = "FFT_2K";
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.fft_mode_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "FftMode" );
		discontrl.pid_status = 0x01; /* ?a?? ????×′ì? */
	}

	return(0);
}


int rf_comm( char *buffer )
{
	char	tmp_buf[18];
	char	*orgbuf;

	if ( buffer == NULL )
		return(-1);
	else {
		s_config *dconfig = config_t(); /*  */

		if ( strncmp( "Yes", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_ENABLE;
		} else if ( strncmp( "No ", buffer, strlen( buffer ) ) == 0 )
		{
			orgbuf = EIT_DESENABLE;
		}

		sprintf( tmp_buf, "%s", dconfig->scfg_Param.modulator_rf_on_name );

		config_set_config( SYS_ETC_CONF, tmp_buf, (uint8_t *) orgbuf, "RFENABLE" );
	}
	config_read( get_profile()->script_configfile );

	int fd;

	if ( (fd = ad9789_configuration() ) < 0 )
	{
		DEBUG( "9789 config fail ..." );
	} else
		close( fd );

	DEBUG( "9789 config over ..." );

	return(0);
}


uint8_t  usb_recode()
{
	/* ??±?LCD2?μ￥1|?ü */
	init_cache();
	discontrl.write_char_dig_status = USB_STATUS;/* 0x01 ?aêy?μ×′ì?￡?0x02?a×?·?×′ì? */
	discontrl.WR_usb		= &NullSubs;

	return(0);
}


void gpiocfg( struct h64_gpio_et_cfg_t *pcfg )
{
	if ( !pcfg )
		return;
	if ( pcfg->gpio_cfg )
		pcfg->gpio_cfg();
}


void h46cfg( struct h64_gpio_et_cfg_t *pcfg )
{
	int fd;
	if ( !pcfg )
		return;

	if ( pcfg->open_h46dev )
		fd = pcfg->open_h46dev();
	if ( pcfg->h46_cfg )
		pcfg->h46_cfg( fd );

	close( fd );
}


void pr_cfg()
{
	struct h64_gpio_et_cfg_t *pcfg;
	pcfg = h64gpio_et_cfg();
	gpiocfg( pcfg );
	h46cfg( pcfg );
}


/*
 *  系统时间设置
 *  strdt   设置系统时间缓存
 *
 *  返回  0 成功，否则失败 返回 -1
 *  char *strdt ="2006-4-20 00:00:00";
 */
int set_system_time( char *strdt )
{
	struct tm	tm_t;
	struct tm	set_tm;
	struct timeval	time_val;
	time_t		timep;

	sscanf( strdt, "%d-%d-%d %d:%d:%d", &tm_t.tm_year,
		&tm_t.tm_mon, &tm_t.tm_mday, &tm_t.tm_hour,
		&tm_t.tm_min, &tm_t.tm_sec );

	set_tm.tm_sec		= tm_t.tm_sec;
	set_tm.tm_min		= tm_t.tm_min;
	set_tm.tm_hour		= tm_t.tm_hour;
	set_tm.tm_mday		= tm_t.tm_mday;
	set_tm.tm_mon		= tm_t.tm_mon - 1;
	set_tm.tm_year		= tm_t.tm_year - 1900;
	timep			= mktime( &set_tm );
	time_val.tv_sec		= timep;
	time_val.tv_usec	= 0;
	if ( settimeofday( &time_val, (struct timezone *) 0 ) < 0 )
	{
		printf( "Set system time error!\n" );
		return(-1);
	}

	return(0);
}


/*
 * 获取系统当前时间
 *
 */
void get_system_time( int *day, int *hours, int *min, int *sec )
{
/*	int year, month ; */

	time_t		nowtime;
	struct tm	*timeinfo;
	time( &nowtime );
	timeinfo = localtime( &nowtime );


/*
 * year		= timeinfo->tm_year + 1900;
 * month		= timeinfo->tm_mon + 1;
 */
	*day	= timeinfo->tm_mday;
	*hours	= timeinfo->tm_hour;
	*min	= timeinfo->tm_min;
	*sec	= timeinfo->tm_sec;
}


uint8_t start_year() /*  */
{
	init_cache();
	char		filename[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_now_startyear );


	if ( write_size == 8 )
	{
		sprintf( filename, "%s", dconfig->scfg_Param.encoder_eit_now_startyear );
		filename[write_size + 1] = '\0';
	}

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 8, 8, bin_eit_year_save, NULL );


	return(0);
}


uint8_t start_time() /*  */
{
	init_cache();
	char		filename[17];
	char		timebuf[17];
	s_config	*dconfig = config_t();
	memset( filename, 0, 17 );

	if ( !dconfig->scfg_Param.encoder_eit_now_starttime )
		return(-1);

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_now_starttime );


	if ( write_size == 6 )
	{
		snprintf( filename, 7, "%s", dconfig->scfg_Param.encoder_eit_now_starttime );
	}

	str_after_pending_colon( (char *) dconfig->scfg_Param.encoder_eit_now_starttime, timebuf );
	DEBUG( "timebuf=%s", timebuf );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, timebuf, 8, 8, bin_eit_time_save, NULL );


	return(0);
}


uint8_t next_time() /*  */
{
	init_cache();
	char		filename[16];
	char		timebuf[16];
	s_config	*dconfig = config_t();
	memset( filename, 0, 16 );

	if ( !dconfig->scfg_Param.encoder_eit_next_starttime )
		return(-1);

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_next_starttime );


	if ( write_size == 6 )
	{
		snprintf( filename, 7, "%s", dconfig->scfg_Param.encoder_eit_next_starttime );
	}

	str_after_pending_colon( (char *) dconfig->scfg_Param.encoder_eit_next_starttime, timebuf );
	DEBUG( "timebuf=%s", timebuf );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, timebuf, 8, 8, bin_eit_nexttime_save, NULL );


	return(0);
}


uint8_t start_duration() /*  */
{
	init_cache();
	char		filename[16];
	char		timebuf[16];
	s_config	*dconfig = config_t();
	memset( filename, 0, 16 );

	if ( !dconfig->scfg_Param.encoder_eit_now_duration )
		return(-1);

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_now_duration );


	if ( write_size == 6 )
	{
		snprintf( filename, 7, "%s", dconfig->scfg_Param.encoder_eit_now_duration );
	}

	str_after_pending_colon( (char *) dconfig->scfg_Param.encoder_eit_now_duration, timebuf );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, timebuf, 8, 8, bin_eit_duration_save, NULL );


	return(0);
}


uint8_t next_duration() /*  */
{
	init_cache();
	char		filename[16];
	char		timebuf[16];
	s_config	*dconfig = config_t();
	memset( filename, 0, 16 );

	if ( !dconfig->scfg_Param.encoder_eit_next_duration )
		return(-1);

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_next_duration );


	if ( write_size == 6 )
	{
		snprintf( filename, 7, "%s", dconfig->scfg_Param.encoder_eit_next_duration );
	}

	str_after_pending_colon( (char *) dconfig->scfg_Param.encoder_eit_next_duration, timebuf );

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, timebuf, 8, 8, bin_eit_nextduration_save, NULL );


	return(0);
}


uint8_t next_year() /*  */
{
	init_cache();
	char		filename[16];
	s_config	*dconfig = config_t();
	memset( filename, 0, 16 );

	int write_size = strlen( (char *) dconfig->scfg_Param.encoder_eit_next_startyear );


	if ( write_size == 8 )
	{
		sprintf( filename, "%s", dconfig->scfg_Param.encoder_eit_next_startyear );
		filename[write_size + 1] = '\0';
	}

	cfg_discontrl_c( CHAR_STATUS, CURSOR_ON, filename, 8, 8, bin_eit_next_year_save, NULL );


	return(0);
}


/*
 * 延时函数
 * nsec 纳妙
 * sec  秒
 */
void nano_sleep( int sec, uint64_t nsec )
{
	struct timespec ts, ts1;

	ts.tv_nsec	= nsec;
	ts.tv_sec	= sec;

	if ( nanosleep( &ts, &ts1 ) != 0 )
	{
		if ( errno == EINTR )

		{
			ts = ts1;
		}else{
			perror( "nano_sleep!\n" );
		}
	}
}


int32_t usb_nothandler( void * nothand )
{
	DEBUG( "I didn't do anything.^_^" );
	return(-1);
}


uint8_t comom_stream_info( _stream_info ts_stream_info )
{
	wr_stream_info = ts_stream_info;

	return(0);
}


uint8_t desplay_stream_info()
{
	if ( wr_stream_info != NULL )
		wr_stream_info();

	return(0);
}


