
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <init-configuration.h>
#include <gen_alltable.h>
#include <debug.h>
#include <menu_foun.h>

s_config config;

static int parse_boolean_value( char *line );


static int get_nit_table_parm();


static volatile int vic_code;

void set_vic_code( int code )
{
	vic_code = code;
}


int get_vic_code( void )
{
	return(vic_code);
}


static const struct {
	const char	*name;
	OpCodes		opcode;
} keywords[] = {
	{ "daemon",	    oDaemon	    },
	{ "ProgramName",    oProgramName    },
	{ "ServiceName",    oServiceName    },

	{ "ProgramOutput",  oProgramOutput  },
	{ "VideoNorm",	    oVideoNorm	    },
	{ "NITLCNMOD",	    oNITLCNMOD	    },
	{ "StreamTSID",	    oStreamTSID	    },
	{ "StreamONID",	    oStreamONID	    },
	/* usb config */
	{ "tsfilename",	    oTsFileName	    },
	{ "tsfilesize",	    oTsFileSize	    },
	/* 调制模式 */
	{ "videomod",	    oModcode	    },

	{ "EncodeRate",	    oEncodeRate	    },
	{ "ModulelateMode", oModulelateMode },
	{ "FftMode",	    oFftMode	    },
	{ "Deltal",	    oDeltal	    },
	{ "ChannelWidth",   oChannelWidth   },

	{ "AD9789CFG",	    oAd9789config   },
	{ "RFLevel",	    oRFLevel	    },
	{ "LcdTime",	    oLcdTime	    },
	{ "PlayModle",	    oPlayMode	    },
	{ "RecordModle",    oRecordModle    },
	{ "AutoModle",	    oAutoModle	    },
	{ "PMTPID",	    oPMTPID	    },
	{ "PRCPID",	    oPRCPID	    },
	{ "VidPID",	    oVideo_PID	    },
	{ "AudioPID",	    oAudioPID	    },
	{ "ProgrNmu",	    oProgrNmu	    },

	{ "RateMode",	    oRateMode	    },
	{ "VideoBitrate",   oVideoBitrate   },
	{ "ProfileConfig",  oProfileConfig  },
	{ "LevelConfig",    oLevelConfig    },
	{ "Brightness",	    oBrightness	    },
	{ "Contrast",	    oContrast	    },
	{ "Saturation",	    oSaturation	    },
	{ "Videohue",	    oVideohue	    },

	{ "CpBrightness",   oCpBrightness   },
	{ "CpContrast",	    oCpContrast	    },
	{ "CpSaturation",   oCpSaturation   },
	{ "CpVideohue",	    oCpVideohue	    },

	{ "AudioBitrate",   oAudioBitrate   },
	{ "WriteRecord",    oWriteRecord    },

	{ "NETWORKID",	    oNETWORKID	    },
	{ "NetWorkName",    oNetWorkName    },
	{ "NETWORKNUMB",    oNETWORKNUMB    },
	{ "EITISERTENABLE", oEITISERTENABLE },
	{ "NITISERTENABLE", oNITISERTENABLE },
	{ "RFENABLE",	    oRFENABLE	    },
	{ "LockPwdStu",	    oLockPwdStu	    },
	{ "AudioFormat",    oAudioFormat    },
	{ NULL,		    oBadOption	    },
};

cgf_stor_data stor_data[] = {
/*
 * 带宽
 * 00->8MHz,01->7MHz,10->6MH
 */
	{ "8MHz",	       0x00	       },
	{ "7MHz",	       0x01	       },
	{ "6MHz",	       0x02	       },

	{ "Enable",	       0x2580	       },
	{ "Denable",	       0x2500	       },


/*
 * 帧保护间�?
 * 00->1/32,01->1/16,10->1/8,11->1/4
 */
	{ "F_DELTAL_32_1",     0x00	       },
	{ "F_DELTAL_16_1",     0x01	       },
	{ "F_DELTAL_8_1",      0x02	       },
	{ "F_DELTAL_4_1",      0x03	       },


/*
 * 调制器FFT模式
 * 0->FFT 2K;1->FFT 8K;
 */
	{ "FFT_2K",	       0x00	       },
	{ "FFT_8K",	       0x01	       },


/*
 * 调制器星�?
 * Modulate mode:000->QPSK,001->16QAM,010->64QAM;default->64QAM
 */
	{ "MODULATE_M_QPSK",   0x00	       },
	{ "MODULATE_M_16QAM",  0x01	       },
	{ "MODULATE_M_64QAM",  0x02	       },


/*
 * 调制器编码码�?
 * Encode rate:000->1/2,001->2/3,010->3/4,011->5/6,100->7/8;default->100
 */
	{ "ENCODE_RATE_2_1",   0x00	       },
	{ "ENCODE_RATE_3_2",   0x01	       },
	{ "ENCODE_RATE_4_3",   0x02	       },
	{ "ENCODE_RATE_6_5",   0x03	       },
	{ "ENCODE_RATE_8_7",   0x04	       },

/* h.264运行速率 */
	{ "CBR",	       0x0030	       },
	{ "VBR",	       0x0031	       },
	{"DEFAULT_CFG",	0x0000},
	{ "HIGH_CFG",	       0x0001	       },
	{ "MAIN_CFG",	       0x0002	       },
	{ "BASE_CFG",	       0x0003	       },
	
	{ "Leveldefu", 0x0000},
	{ "Level4",	       0x0028	       },
	{ "Level4_1",	       0x0029	       },
	{ "Level4_2",	       0x002A	       },
	{ "Level3",	       0x001E	       },
	{ "Level3_1",	       0x001F	       },
	{ "Level3_2",	       0x0020	       },

	{ "European_Cfg",      0x02	       }, /* ��Ҫ�����޸�20160419 jh */
	{ "NorDig_V1Cfg",      0x03	       },
	{ "NorDig_V2Cfg",      0x04	       },

	{ "AUTO",	       0x0028	       },
	{ "PAL-BGHID",	       0x0029	       },
	{ "PAL-N",	       0x002A	       },
	{ "PAL-CN",	       0x001E	       },
	{ "PAL-M",	       0x001F	       },
	{ "NTSC-M",	       0x0020	       },
	{ "NTSC-J",	       0x002A	       },
	{ "NTSC-4.43",	       0x001E	       },


	{ "64_kbps",	       0x40	       },
	{ "96_kbps",	       0x60	       },
	{ "112_kbps",	       0x70	       },
	{ "128_kbps",	       0x80	       },
	{ "160_kbps",	       0xA0	       },
	{ "192_kbps",	       0xC0	       },
	{ "224_kbps",	       0xE0	       },
	{ "256_kbps",	       0x100	       },
	{ "320_kbps",	       0x140	       },
	{ "384_kbps",	       0x180	       },

	{ "MPEG-2",	       0x00	       },
	{ "MPEG-2-AAC",	       0x01	       },
	{ "MPEG-4-AAC",	       0x02	       },

	{ "HDMI_M",	       0x00	       },
	{ "CVBS_M",	       0x01	       },
	{ "YPbPr_M",	       0x02	       },

	{ "t5s",	       5	       },
	{ "t10s",	       10	       },
	{ "t30s",	       30	       },
	{ "t45s",	       45	       },
	{ "t60s",	       60	       },
	{ "t90s",	       90	       },
	{ "t120s",	       120	       },

	{ USB_SIG,	       USB_SINGLE      },
	{ USB_SIG_LOOP,	       USB_LOOP	       },
	{ USB_SIG_PLAY_ALL,    USB_SEGMENT     },

	{ USB_RECORD_SIG_MOD,  USB_RECORD_SIG  },
	{ USB_RECORD_SEG_MOD,  UAB_RECORD_SEG  },
	{ USB_RECORD_LOOP_MOD, USB_RECORD_LOOP },

/* AD9789CFG  AD9789MHZ */
};

cgf_stor_data* get_stor_data()
{
	return(cgf_data = stor_data);
}


static int get_nit_table_parm()
{
	int fd, res;

	fd = open( TABLE_NAME, O_RDWR );
	if ( fd < 0 )
	{
		DEBUG( "file open error" );
		return(-1);
	}
	usr_cfg_eit_t *cfg_eit_t;
	cfg_eit_t = calloc( 1, sizeof(usr_cfg_eit_t) );
	if ( !cfg_eit_t )
		return(-1);

	res = read( fd, cfg_eit_t, sizeof(usr_cfg_eit_t) );

	if ( res < 0 )
	{
		DEBUG( "eit file error" );
		close( fd );
		return(-1);
	}
	close( fd );
#if 0
	DEBUG( "read sizeof=%d", res );
	DEBUG( "= %s", cfg_eit_t->eit_now_startyear );
	int i;
	for ( i = 0; i < strlen( (char *) cfg_eit_t->eit_now_startyear ); i++ )
	{
		DEBUG( "  %c %d", cfg_eit_t->eit_now_startyear[i], cfg_eit_t->eit_now_startyear[i] );
	}

	DEBUG( "= %s", cfg_eit_t->eit_now_starttime );
	DEBUG( "= %s", cfg_eit_t->eit_now_duration );
	DEBUG( "= %s", cfg_eit_t->eit_now_eventname );

	DEBUG( "= %s", cfg_eit_t->eit_next_startyear );

	DEBUG( "= %s", cfg_eit_t->eit_next_starttime );
	DEBUG( "= %s", cfg_eit_t->eit_next_duration );
	DEBUG( "= %s", cfg_eit_t->eit_next_eventname );
#endif
	memset( (char *) config.scfg_Param.stream_nit_private_data, 0, 9 );
	sprintf( (char *) config.scfg_Param.stream_nit_private_data, "%s", cfg_eit_t->nit_private_data );

	memset( (char *) config.scfg_Param.encoder_program_number, 0, 10 );
	itoa_( cfg_eit_t->program_number, (char *) config.scfg_Param.encoder_program_number, 10 );

	memset( (char *) config.scfg_Param.stream_nit_network_id, 0, 10 );
	itoa_( (int) cfg_eit_t->network_id, (char *) config.scfg_Param.stream_nit_network_id, 10 );

	memset( (char *) config.scfg_Param.encoder_eit_now_startyear, 0, 9 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_startyear, 9, "%s", cfg_eit_t->eit_now_startyear );

	memset( (char *) config.scfg_Param.encoder_eit_now_starttime, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_starttime, 7, "%s", cfg_eit_t->eit_now_starttime );

	memset( (char *) config.scfg_Param.encoder_eit_now_duration, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_duration, 7, "%s", cfg_eit_t->eit_now_duration );

	int lenght = strlen( (char *) cfg_eit_t->eit_now_eventname ) + 1;
	memset( (char *) config.scfg_Param.encoder_eit_now_eventname, 0, 17 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_eventname, lenght, "%s", cfg_eit_t->eit_now_eventname );

	memset( (char *) config.scfg_Param.encoder_eit_next_startyear, 0, 9 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_startyear, 9, "%s", cfg_eit_t->eit_next_startyear );

	memset( (char *) config.scfg_Param.encoder_eit_next_starttime, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_starttime, 7, "%s", cfg_eit_t->eit_next_starttime );

	memset( (char *) config.scfg_Param.encoder_eit_next_duration, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_duration, 7, "%s", cfg_eit_t->eit_next_duration );
	lenght = strlen( (char *) cfg_eit_t->eit_next_eventname ) + 1;
	memset( (char *) config.scfg_Param.encoder_eit_next_eventname, 0, 17 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_eventname, lenght, "%s", cfg_eit_t->eit_next_eventname );

	config.scfg_Param.stream_nit_programifo_lcn = cfg_eit_t->program_ifo_lcn;


	free( cfg_eit_t );

	return(0);
}


int set_nit_table_parm()
{
	int fd, res;

	int testsize;

	fd = open( TABLE_NAME, O_RDWR | O_CREAT | O_TRUNC );
	if ( fd < 0 )
	{
		DEBUG( "file open error" );
		return(-1);
	}
	usr_cfg_eit_t *cfg_eit_t;
	cfg_eit_t = calloc( 1, sizeof(usr_cfg_eit_t) );
	if ( !cfg_eit_t )
		return(-1);


	testsize = strlen( (char *) config.scfg_Param.stream_nit_private_data );
	snprintf( (char *) cfg_eit_t->nit_private_data, testsize + 1, "%s", config.scfg_Param.stream_nit_private_data );

	cfg_eit_t->program_number	= (uint16_t) atoi( (char *) config.scfg_Param.encoder_program_number );
	cfg_eit_t->network_id		= (uint16_t) atoi( (char *) config.scfg_Param.stream_nit_network_id );

	cfg_eit_t->program_ifo_lcn = config.scfg_Param.stream_nit_programifo_lcn;

	testsize = strlen( (char *) config.scfg_Param.encoder_eit_now_startyear );
	snprintf( (char *) cfg_eit_t->eit_now_startyear, 9, "%s", config.scfg_Param.encoder_eit_now_startyear );

	snprintf( (char *) cfg_eit_t->eit_now_starttime, 7, "%s", config.scfg_Param.encoder_eit_now_starttime );


	snprintf( (char *) cfg_eit_t->eit_now_duration, 7, "%s", config.scfg_Param.encoder_eit_now_duration );

	int lenght = strlen( (char *) config.scfg_Param.encoder_eit_now_eventname ) + 1;
	snprintf( (char *) cfg_eit_t->eit_now_eventname, lenght, "%s", config.scfg_Param.encoder_eit_now_eventname );

	snprintf( (char *) cfg_eit_t->eit_next_startyear, 9, "%s", config.scfg_Param.encoder_eit_next_startyear );

	snprintf( (char *) cfg_eit_t->eit_next_starttime, 7, "%s", config.scfg_Param.encoder_eit_next_starttime );

	snprintf( (char *) cfg_eit_t->eit_next_duration, 7, "%s", config.scfg_Param.encoder_eit_next_duration );


	lenght = strlen( (char *) config.scfg_Param.encoder_eit_next_eventname ) + 1;
	snprintf( (char *) cfg_eit_t->eit_next_eventname, lenght, "%s", config.scfg_Param.encoder_eit_next_eventname );


	res = write( fd, cfg_eit_t, sizeof(usr_cfg_eit_t) );
	if ( res < 0 )
	{
		DEBUG( "eit file error" );
		close( fd );
		return(-1);
	}
	free( cfg_eit_t );
/*	DEBUG( "struct res=%d %d", res, sizeof(usr_cfg_eit_t) ); */
	close( fd );

	return(0);
}


static int parse_boolean_value( char *line )
{
	if ( strcasecmp( line, "yes" ) == 0 )
	{
		return(1);
	}
	if ( strcasecmp( line, "no" ) == 0 )
	{
		return(0);
	}
	if ( strcmp( line, "1" ) == 0 )
	{
		return(1);
	}
	if ( strcmp( line, "0" ) == 0 )
	{
		return(0);
	}

	return(-1);
}


static OpCodes config_parse_token( const char *cp, const char *filename, int linenum )
{
	int i;

	for ( i = 0; keywords[i].name; i++ )
		if ( strcasecmp( cp, keywords[i].name ) == 0 )
			return(keywords[i].opcode);

	DEBUG( "%s: line %d: Bad configuration option: %s\r\n", filename, linenum, cp );
	return(oBadOption);
}


static int store_parse_token( char *cp )
{
	int i;
	if ( cp == NULL )
		return(-1);
	for ( i = 0; stor_data[i].m_p_encode_name; i++ )
		if ( strcasecmp( cp, stor_data[i].m_p_encode_name ) == 0 )
			return(stor_data[i].m_encode_val);

	DEBUG( " Bad configuration option: %s\r\n", cp );
	return(-1);
}


static char* get_parse_str( uint8_t *cp )
{
	int i;
	if ( cp == NULL )
		return(NULL);

	for ( i = 0; stor_data[i].m_p_encode_name; i++ )
		if ( strcasecmp( (const char *) cp, stor_data[i].m_p_encode_name ) == 0 )
			return(stor_data[i].m_p_encode_name);

	DEBUG( " Bad configuration option: %s\r\n", cp );
	return(NULL);
}


int config_read( const char *filename )
{
	FILE *fd;

	char line[MAX_BUF], *s, *p1, *p2;

	int linenum = 0, opcode, value, len, ret = -1;

	/* DEBUG("Reading configuration file '%s'\r\n", filename); */

	get_nit_table_parm();

	if ( !(fd = fopen( filename, "r" ) ) )
	{
		DEBUG( "Could not open configuration file '%s', "
		       "exiting...\r\n", filename );
		exit( 1 );
	}


	while ( !feof( fd ) && fgets( line, MAX_BUF, fd ) )
	{
		linenum++;
		s = line;

		if ( s[strlen( s ) - 1] == '\n' )
			s[strlen( s ) - 1] = '\0';

		if ( (p1 = strchr( s, ' ' ) ) )
		{
			p1[0] = '\0';
		} else if ( (p1 = strchr( s, '\t' ) ) )
		{
			p1[0] = '\0';
		}

		if ( p1 )
		{
			p1++;

			/* Trim leading spaces */
			len = strlen( p1 );
			while ( *p1 && len )
			{
				if ( *p1 == ' ' )
					p1++;
				else
					break;
				len = strlen( p1 );
			}


			if ( (p2 = strchr( p1, ' ' ) ) )
			{
				p2[0] = '\0';
			} else if ( (p2 = strstr( p1, "\r\n" ) ) )
			{
				p2[0] = '\0';
			} else if ( (p2 = strchr( p1, '\n' ) ) )
			{
				p2[0] = '\0';
			}
		}

		if ( p1 && p1[0] != '\0' )
		{
			/* Strip trailing spaces */

			if ( (strncmp( s, "#", 1 ) ) != 0 )
			{
				/* DEBUG("Parsing token: %s, value: %s \r\n", s, p1); */
				opcode = config_parse_token( s, filename, linenum );

				switch ( opcode )
				{
				case oDaemon:
					if ( config.daemon == -1 && ( (value = parse_boolean_value( p1 ) ) != -1) )
					{
						config.daemon = value;
					}
					break;

				case oEncodeRate: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encode_rate_name	= safe_strdup( name );
						config.scfg_Param.encode_rate		= store_parse_token( p1 );
						/* config.configParam.encode_rate = safe_strdup(get_parse_str(replace_str)); */
						ret = 0;
					}
				}
				break;
				case oModulelateMode: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.modulelate_mode_name	= safe_strdup( name );
						config.scfg_Param.modulelate_mode	= store_parse_token( p1 );
						ret					= 0;
					}
				}

				break;
				case oFftMode: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.fft_mode_name = safe_strdup( name );
						config.scfg_Param.fft_mode	= store_parse_token( p1 );
						ret				= 0;
					}
				}

				break;
				case oDeltal: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.deltal_name	= safe_strdup( name );
						config.scfg_Param.deltal	= store_parse_token( p1 );
						ret				= 0;
					}
				}

				break;

				case oChannelWidth: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.channel_width_name	= safe_strdup( name );
						config.scfg_Param.channel_width		= store_parse_token( p1 );
						ret					= 0;
					}
				}
				break;

				case oModcode:
				{
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_video_interface_name	= safe_strdup( name );
						config.scfg_Param.encoder_video_interface	= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oTsFileName:
					memset( config.configParam.usb_tsfilename, 0, 17 );
					strncpy( config.configParam.usb_tsfilename, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oNetWorkName:
					memset( config.scfg_Param.stream_nit_network_name, 0, 16 );
					strncpy( (char *) config.scfg_Param.stream_nit_network_name, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oTsFileSize:
					config.configParam.usb_tsfilesize	= atoi( safe_strdup( p1 ) );
					ret					= 0;
					break;
				case oWriteRecord:
					config.scfg_Param.stream_usb_used_count = atoi( safe_strdup( p1 ) );
					ret					= 0;
					break;
				case oAd9789config: {
					config.localstatus.cfig_ad9789_ftw_bpf	= atof( safe_strdup( p1 ) );
					ret					= 0;
				}
				break;
				case oRFLevel: {
					config.scfg_Param.modulator_rf_level	= atof( safe_strdup( p1 ) );
					ret					= 0;
/*					DEBUG("modulator_rf_level %f" ,config.scfg_Param.modulator_rf_level); */
				}
				break;
				case oPMTPID:

					strncpy( (char *) &config.scfg_Param.encoder_pmt_pid, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oPRCPID:

					strncpy( (char *) &config.scfg_Param.encoder_pcr_pid, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oVideo_PID:

					strncpy( (char *) &config.scfg_Param.encoder_video_pid, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oAudioPID:
					strncpy( (char *) &config.scfg_Param.encoder_audio_pid, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oProgrNmu:
					memset( (char *) &config.scfg_Param.encoder_program_number, 0, 5 );
					strncpy( (char *) &config.scfg_Param.encoder_program_number, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oNETWORKID:        /* oNETWORKNUMB */
					memset( (char *) &config.scfg_Param.stream_nit_network_id, 0, 5 );
					strncpy( (char *) &config.scfg_Param.stream_nit_network_id, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oNETWORKNUMB:      /* oPRIVATEDATE */
					strncpy( (char *) &config.scfg_Param.stream_nit_version_number, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;
				case oEITISERTENABLE:   /* oProgramOutputEnable */
					memset( (char *) &config.scfg_Param.stream_eit_insert, 0, 8 );
					strncpy( (char *) &config.scfg_Param.stream_eit_insert, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;          /* oProgramName */

				case oNITISERTENABLE:   /* oProgramOutputEnable */
					memset( (char *) &config.scfg_Param.stream_nit_insert, 0, 8 );
					strncpy( (char *) &config.scfg_Param.stream_nit_insert, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;

				case oAutoModle:        /* oProgramOutputEnable */
					memset( (char *) &config.scfg_Param.stream_usb_record_auto, 0, 8 );
					strncpy( (char *) &config.scfg_Param.stream_usb_record_auto, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;

/*				case oLockPwdStu:      / * oProgramOutputEnable * / */
/*					memset( (char *) &config.scfg_Param.stream_lck_state, 0, 8 ); */
/*					strncpy( (char *) &config.scfg_Param.stream_lck_state, p1, strlen( p1 ) + 1 ); */
/*					ret = 0; */
/*					break;          / * oProgramName * / */

				case oRFENABLE:  {/* oProgramOutputEnable */
					/*
					 * memset( (char *) &config.scfg_Param.modulator_rf_on_name, 0, 8 );
					 * strncpy( (char *) &config.scfg_Param.modulator_rf_on_name, p1, strlen( p1 ) + 1 );
					 */

					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						memset( (char *) &config.scfg_Param.modulator_rf_on_name, 0, 8 );
						strncpy( (char *) &config.scfg_Param.modulator_rf_on_name, p1, strlen( p1 ) + 1 );
						config.scfg_Param.modulator_rf_on	= store_parse_token( p1 );
						ret					= 0;
					}
				}
				break;                  /* oProgramName */

				case oProgramOutput:    /*  */
					memset( (char *) &config.scfg_Param.encoder_program_output, 0, 8 );
					strncpy( (char *) &config.scfg_Param.encoder_program_output, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;          /* oProgramName */
				case oProgramName:      /*  */
					memset( (char *) &config.scfg_Param.encoder_program_name, 0, 16 );
					strncpy( (char *) &config.scfg_Param.encoder_program_name, p1, strlen( p1 ) + 1 );
					ret = 0;
					break;          /*  */
				case oServiceName:      /*  */
					memset( (char *) &config.scfg_Param.encoder_service_name, 0, 16 );
					strncpy( (char *) &config.scfg_Param.encoder_service_name, p1, strlen( p1 ) + 1 );

					ret = 0;
					break;          /*  */

				case oRateMode:
				{
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						memset( config.scfg_Param.encoder_video_ratemode, ' ', 3 );
						memcpy( config.scfg_Param.encoder_video_ratemode, name, 3 );
						config.scfg_Param.encoder_video_ratemode_fre	= store_parse_token( p1 );
						ret						= 0;
					}
				}
				break;
				case oVideoBitrate:
				{
					config.scfg_Param.encoder_video_bitrate = atoi( safe_strdup( p1 ) );
					ret					= 0;
				}
				break;

				case oProfileConfig: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_video_profile_name	= safe_strdup( name );
						config.scfg_Param.encoder_video_profile		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oLevelConfig: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_video_level_name	= safe_strdup( name );
						
						config.scfg_Param.encoder_video_level		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oPlayMode: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.stream_usb_play_mod	= safe_strdup( name );
						config.scfg_Param.stream_usb_mod	= store_parse_token( p1 );
						ret					= 0;
					}
				}

				break;

				case oRecordModle: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.stream_usb_record_save_mod	= safe_strdup( name );
						config.scfg_Param.stream_usb_record_mod		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oLcdTime: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.system_lcd_time_out_name	= safe_strdup( name );
						config.scfg_Param.system_lcd_time_out		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oAudioFormat: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_audio_format_name	= safe_strdup( name );
						config.scfg_Param.encoder_audio_format		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oVideoNorm: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_video_norm_name	= safe_strdup( name );
						config.scfg_Param.encoder_video_norm		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;

				case oNITLCNMOD: {
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.stream_nit_lcn_mode_name	= safe_strdup( name );
						config.scfg_Param.stream_nit_lcn_mode		= store_parse_token( p1 );
						ret						= 0;
					}
				}

				break;
				case oBrightness:
				{
					config.scfg_Param.encoder_video_brightness	= (short) atoi( safe_strdup( p1 ) );
					DEBUG("br :%d ",config.scfg_Param.encoder_video_brightness	);
					ret						= 0;
				}
				break;

				case oContrast:
				{
					config.scfg_Param.encoder_video_contrast	= (uint16_t) atoi( safe_strdup( p1 ) );
					ret						= 0;
				}
				break;

				case oCpBrightness:
				{
					config.scfg_Param.encoder_video_cp_brightness	= (uint8_t) atoi( safe_strdup( p1 ) );
					ret						= 0;
				}
				break;

				case oCpContrast:
				{
					config.scfg_Param.encoder_video_cp_contrast	= (uint8_t) atoi( safe_strdup( p1 ) );
					ret						= 0;
				}
				break;

				case oStreamTSID:
				{
					config.scfg_Param.stream_tsid	= atoi( safe_strdup( p1 ) );
					ret				= 0;
				}
				break;

				case oStreamONID:
				{
					config.scfg_Param.stream_onid	= atoi( safe_strdup( p1 ) );
					ret				= 0;
				}
				break;

				case oSaturation:
				{
					config.scfg_Param.encoder_video_saturation	= (uint16_t) atoi( safe_strdup( p1 ) );
					ret						= 0;
				}
				break;


				case oVideohue:
				{
					config.scfg_Param.encoder_video_hue	= (uint16_t) atoi( safe_strdup( p1 ) );
					ret					= 0;
				}
				break;

				case oCpSaturation:
				{
					config.scfg_Param.encoder_video_cp_saturation	= (uint8_t) atoi( safe_strdup( p1 ) );
					ret						= 0;
				}
				break;


				case oCpVideohue:
				{
					config.scfg_Param.encoder_video_cp_hue	= (int8_t) atoi( safe_strdup( p1 ) );
					ret					= 0;
				}
				break;

				case oAudioBitrate:
				{
					char *name;
					name = get_parse_str( (uint8_t *) p1 );
					if ( name )
					{
						config.scfg_Param.encoder_audio_bitrate_name	= safe_strdup( name );
						config.scfg_Param.encoder_audio_bitrate		= store_parse_token( p1 );
						ret						= 0;
					}
				}
				break;

				case oBadOption:
					DEBUG( "Bad option on line %d " "in %s. \r\n", linenum, filename );
					DEBUG( "Exiting...\r\n" );
					exit( -1 );
					break;
				}
			}
		}
	}


	fclose( fd );
	return(ret);
}


s_config *config_get_config()
{
	config.configParam.reset = SYS_BUS_SET;/* 系统总线复位 0x01 */

	return(&config);
}


void config_init( void )
{
	strncpy( config.configParam.usb_tsfilename, TSFILENAME, strlen( TSFILENAME ) + 1 );
	config.configParam.usb_tsfilesize = TSFILESIZE;

	/* 码率，带宽等默认设置 */
	config.scfg_Param.encode_rate		= ENCODE_RATE_8_7;      /* Encode rate:000->1/2,001->2/3,010->3/4,011->5/6,100->7/8;default->100 */
	config.scfg_Param.encode_rate_name	= "ENCODE_RATE_8_7";
	config.scfg_Param.modulelate_mode	= MODULATE_M_64QAM;     /* Modulate mode:000->QPSK,001->16QAM,010->64QAM;default->64QAM */
	config.scfg_Param.modulelate_mode_name	= "MODULATE_M_64QAM";
	config.scfg_Param.fft_mode		= FFT_2K;               /* 0->FFT 2K;1->FFT 8K; */
	config.scfg_Param.fft_mode_name		= "FFT_2K";
	config.scfg_Param.deltal		= F_DELTAL_32_1;        /* 00->1/32,01->1/16,10->1/8,11->1/4 */
	config.scfg_Param.deltal_name		= "F_DELTAL_32_1";
	config.scfg_Param.channel_width		= B_8MHz;               /* 00->8MHz,01->7MHz,10->6MH */
	config.scfg_Param.channel_width_name	= "B_8MHz";

	/* h.264运行速率 */
	memset( config.scfg_Param.encoder_video_ratemode, 0, 3 );
	memcpy( config.scfg_Param.encoder_video_ratemode, "CBR", 3 );
	config.scfg_Param.encoder_video_ratemode_fre = DEFAULT_CBR;

	config.localstatus.encoder_video_input_lock = VIDEO_INPUT_UNLOCK;
	memset( (char *) config.localstatus.encoder_video_resolution, ' ', 16 );
	memset( (char *) config.localstatus.encoder_video_shrot_resolution, ' ', 16 );
	memcpy( config.localstatus.encoder_video_shrot_resolution, "unknown", strlen( "unknown" ) );
	memcpy( config.localstatus.encoder_video_resolution, "unknown", strlen( "unknown" ) );
	config.localstatus.encoder_video_ident = ENCODE_IDENT_UNKONOWN;

	config.scfg_Param.encoder_video_bitrate = VIDEO_BITRATE;
	config.scfg_Param.stream_tsid		= STREAM_TSID;
	config.scfg_Param.stream_onid		= STREAM_ONID;

	config.scfg_Param.encoder_video_profile_name	= PROFILE_STR;
	config.scfg_Param.encoder_video_profile		= PROFILEDELETEC_P;

	config.scfg_Param.encoder_video_level		= LEVEL_;
	config.scfg_Param.encoder_video_level_name	= LEVEL_NAME;

	config.scfg_Param.stream_nit_lcn_mode		= LCN_MOD_VALUE;
	config.scfg_Param.stream_nit_lcn_mode_name	= NIT_LCN_MOD;
	config.scfg_Param.stream_usb_used_count		= 0;

	config.scfg_Param.encoder_video_norm		= VIDEO_NORM;
	config.scfg_Param.encoder_video_norm_name	= VIDEO_NORM_NAME;

	config.scfg_Param.encoder_video_brightness	= VIDEO_BRIGHT;
	config.scfg_Param.encoder_video_contrast	= VIDEO_CONTRAST;
	config.scfg_Param.encoder_video_saturation	= VIDEO_SATURATION;
	config.scfg_Param.encoder_video_hue		= VIDEO_HUE;

	config.scfg_Param.encoder_video_cp_brightness	= VIDEO_CP_BRIGHT;
	config.scfg_Param.encoder_video_cp_contrast	= VIDEO_CP_CONTRAST;
	config.scfg_Param.encoder_video_cp_saturation	= VIDEO_CP_SATURATION;
	config.scfg_Param.encoder_video_cp_hue		= VIDEO_CP_HUE;


	config.scfg_Param.encoder_audio_bitrate_name	= "64_kbps";
	config.scfg_Param.encoder_audio_bitrate		= AUDIO_RATE;
	config.scfg_Param.encoder_video_interface_name	= HDMI_M;
	config.scfg_Param.encoder_video_interface	= M_HDMI;

	config.scfg_Param.encoder_audio_format_name	= AUDIO_FORMAE_NAME;
	config.scfg_Param.encoder_audio_format		= AUDIO_FORMAE;

	config.scfg_Param.stream_nit_programifo_lcn = 1;

	config.key_statuus	= LOCK_OFF;
	config.lock_enter	= 0x03;

	memset( (char *) config.scfg_Param.stream_nit_private_data, 0, 9 );
	snprintf( (char *) config.scfg_Param.stream_nit_private_data, 9, "%s", PRIVATE_DATE );

	memset( (char *) config.scfg_Param.encoder_eit_now_startyear, 0, 9 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_startyear, 9, "%s", START_YEAR );

	memset( (char *) config.scfg_Param.encoder_eit_now_starttime, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_starttime, 7, "%s", START_TMIE );

	memset( (char *) config.scfg_Param.encoder_eit_now_duration, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_duration, 7, "%s", START_DURATION );

	int lenght = strlen( START_EVNETNAME ) + strlen( START_EVNETNAME2 ) + 2;
	memset( (char *) config.scfg_Param.encoder_eit_now_eventname, 0, 17 );
	snprintf( (char *) config.scfg_Param.encoder_eit_now_eventname, lenght, "%s\b%s", START_EVNETNAME, START_EVNETNAME2 );


	memset( (char *) config.scfg_Param.encoder_eit_next_startyear, 0, 9 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_startyear, 9, "%s", NEXT_YEAR );

	memset( (char *) config.scfg_Param.encoder_eit_next_starttime, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_starttime, 7, "%s", NEXT_TMIE );

	memset( (char *) config.scfg_Param.encoder_eit_next_duration, 0, 7 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_duration, 7, "%s", NEXT_DURATION );

	lenght = strlen( NEXT_EVNETNAME ) + strlen( NEXT_EVNETNAME2 ) + 2;
	memset( (char *) config.scfg_Param.encoder_eit_next_eventname, 0, 17 );
	snprintf( (char *) config.scfg_Param.encoder_eit_next_eventname, lenght, "%s\b%s", NEXT_EVNETNAME, NEXT_EVNETNAME2 );

	lenght = strlen( EIT_DESENABLE ) + 1;
	memset( (char *) config.scfg_Param.stream_eit_insert, 0, 8 );
	snprintf( (char *) config.scfg_Param.stream_eit_insert, lenght, "%s", EIT_DESENABLE );

	memset( (char *) config.scfg_Param.stream_eit_insert, 0, 8 );
	snprintf( (char *) config.scfg_Param.stream_eit_insert, lenght, "%s", NIT_DESENABLE );

	memset( (char *) config.scfg_Param.stream_usb_record_auto, 0, 8 );
	snprintf( (char *) config.scfg_Param.stream_usb_record_auto, lenght, "%s", EIT_DESENABLE );

	memset( (char *) config.scfg_Param.stream_lck_state, 0, 8 );
	snprintf( (char *) config.scfg_Param.stream_lck_state, lenght, "%s", DESENABLE );

	memset( (char *) config.scfg_Param.stream_lck_state, 0, 8 );
	snprintf( (char *) config.scfg_Param.stream_lck_state, lenght, "%s", DESENABLE );

	memset( (char *) config.scfg_Param.encoder_program_output, 0, 8 );
	snprintf( (char *) config.scfg_Param.encoder_program_output, lenght, "%s", EIT_DESENABLE );

	config.localstatus.cfig_ad9789_ftw_bpf	= 500.000;
	config.scfg_Param.modulator_rf_level	= -16.0;

	lenght = strlen( EIT_DESENABLE ) + 1;
	memset( (char *) config.scfg_Param.modulator_rf_on_name, 0, 8 );
	snprintf( (char *) config.scfg_Param.modulator_rf_on_name, lenght, "%s", EIT_DESENABLE );

	config.scfg_Param.system_lcd_backlighting	= LCD_LIGHT_ON;
	config.scfg_Param.system_gpio_mask		= 0;

	config.scfg_Param.system_lcd_time_out		= LCD_TIME;
	config.scfg_Param.system_lcd_time_out_name	= LCD_TIME_M;
	config.scfg_Param.stream_usb_play_mod		= USB_SIG;
	config.scfg_Param.stream_usb_mod		= USB_SINGLE;

	config.scfg_Param.stream_usb_record_save_mod	= USB_RECORD_SIG_MOD;
	config.scfg_Param.stream_usb_record_mod		= USB_RECORD_SIG;

	memset( (char *) config.scfg_Param.system_def_root_pwd, 0, 7 );
	snprintf( (char *) config.scfg_Param.system_def_root_pwd, 7, "%s", "005599" );

	memset( (char *) config.scfg_Param.system_root_pwd, 0, 7 );
	snprintf( (char *) config.scfg_Param.system_root_pwd, 7, "%s", "******" );

	config.scfg_Param.system_lck_st = NOTPWD;
}


s_config * get_profile()
{
	strncpy( config.script_configfile, SYS_ETC_CONF, sizeof(config.script_configfile) );
	return(&config);
}


static void print_c( char *buf, int len )
{
	int i;
	if ( !buf )
		return;

	for ( i = 0; i < len && buf[i] != '\0'; i++ )
	{
		printf( "%c ", buf[i] );
	}

	printf( "\n" );
}


int config_set_config( char *filename, const char *original_str, uint8_t *replace_str, char *firset_str )
{
	FILE	*fd;
	int	result;
	char	line[MAX_BUF], *s, *p1, *p2, tmpline[50];

	int linenum = 0, opcode, useropcode, value, len;

/*	DEBUG( "Reading configuration file '%s' original_str=%s replace_str=%s\r\n", filename, original_str, replace_str ); */

	if ( !(fd = fopen( filename, "r+" ) ) )
	{
		DEBUG( "Could not open configuration file '%s', "
		       "exiting...\r\n", filename );
		exit( 1 );
	}

	while ( !feof( fd ) && fgets( line, MAX_BUF, fd ) )
	{
		linenum++;
		s = line;

		if ( s[strlen( s ) - 1] == '\n' )
			s[strlen( s ) - 1] = '\0';

		if ( (p1 = strchr( s, ' ' ) ) )
		{
			p1[0] = '\0';
		} else if ( (p1 = strchr( s, '\t' ) ) )
		{
			p1[0] = '\0';
		}

		if ( p1 )
		{
			p1++;

			/* Trim leading spaces */
			len = strlen( p1 );
			while ( *p1 && len )
			{
				if ( *p1 == ' ' )
					p1++;
				else
					break;
				len = strlen( p1 );
			}


			if ( (p2 = strchr( p1, ' ' ) ) )
			{
				p2[0] = '\0';
			} else if ( (p2 = strstr( p1, "\r\n" ) ) )
			{
				p2[0] = '\0';
			} else if ( (p2 = strchr( p1, '\n' ) ) )
			{
				p2[0] = '\0';
			}
		}

		if ( p1 && p1[0] != '\0' )
		{
			if ( (strncmp( s, "#", 1 ) ) != 0 )
			{
				opcode = config_parse_token( s, filename, linenum );


				/*
				 * p1 为原配置文件中的字符�?
				 * original_str 为用户获得原配文件的字符�?
				 */
				if ( strncmp( p1, original_str, strlen( p1 ) - 1 ) == 0 )
				{
					useropcode = opcode;
				} else {
					useropcode = NOT;
				}

				switch ( useropcode )
				{
				case oDaemon:
					if ( config.daemon == -1 && ( (value = parse_boolean_value( p1 ) ) != -1) )
					{
						config.daemon = value;
					}
					break;
				case oEncodeRate: {
					char *name;

					name = get_parse_str( replace_str );
					if ( name )
					{
						config.scfg_Param.encode_rate_name = safe_strdup( name );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;
				case oModulelateMode:

				{
					char *name;
					name = get_parse_str( replace_str );
					if ( name )
					{
						config.scfg_Param.modulelate_mode_name = safe_strdup( name );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;
				case oFftMode: {
					char *name;
					name = get_parse_str( replace_str );
					if ( name )
					{
						config.scfg_Param.fft_mode_name = safe_strdup( name );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}


				break;
				case oDeltal:

				{
					char *name;
					name = get_parse_str( replace_str );
					if ( name )
					{
						config.scfg_Param.deltal_name = safe_strdup( name );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;

				case oChannelWidth:

				{
					char *name;
					name = get_parse_str( replace_str );
					if ( name )
					{
						config.scfg_Param.channel_width_name = safe_strdup( name );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;
				case oModcode: {
					if ( strncmp( firset_str, "videomod", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.encoder_video_interface_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}

				break;

				case oTsFileName: {
					char file_name[12];
					if ( CONFFILELEN <= strlen( (char *) replace_str ) )
					{
						snprintf( file_name, CONFFILELEN, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
					} else {
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
					}

					result = system( tmpline );
					if ( result != -1 )
						DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
				}

				break;
				case oNetWorkName: {
					if ( strncmp( firset_str, "NetWorkName", strlen( firset_str ) ) == 0 )
					{
						char file_name[12];
						if ( CONFFILELEN <= strlen( (char *) replace_str ) )
						{
							snprintf( file_name, CONFFILELEN, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						} else {
							sprintf( file_name, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						}

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oTsFileSize: {
					char file_name[12];
					if ( strncmp( firset_str, "tsfilesize", strlen( firset_str ) ) == 0 )
					{
						if ( 7 <= strlen( (char *) replace_str ) )
						{
							snprintf( file_name, 7, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						} else {
							sprintf( file_name, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						}

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;

				case oWriteRecord: {
					char file_name[12];
					if ( strncmp( firset_str, "WriteRecord", strlen( firset_str ) ) == 0 )
					{
						if ( 4 <= strlen( (char *) replace_str ) )
						{
							snprintf( file_name, 4, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						} else {
							sprintf( file_name, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						}

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}

				break;

				case oAd9789config: {
					char file_name[12];
					if ( strncmp( firset_str, "AD9789CFG", strlen( firset_str ) ) == 0 )
					{
						if ( 8 <= strlen( (char *) replace_str ) )
						{
							snprintf( file_name, 8, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						} else {
							sprintf( file_name, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						}

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oRFLevel: {
					char file_name[12];
					if ( strncmp( firset_str, "RFLevel", strlen( firset_str ) ) == 0 )
					{
						if ( 8 <= strlen( (char *) replace_str ) )
						{
							snprintf( file_name, 8, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						} else {
							sprintf( file_name, "%s", replace_str );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						}

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oPMTPID: {
					char file_name[12];

					if ( strncmp( firset_str, "PMTPID", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );
						print_c( buf, 5 );
						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oPRCPID: {
					char file_name[12];
					if ( strncmp( firset_str, "PRCPID", strlen( firset_str ) ) == 0 )
					{
						int trl_val = atoi( (char *) replace_str );

						char buf[5] = { 0 };
						itoa_( trl_val, buf, 10 );
						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oVideo_PID: { /* oProgrNmu */
					char	file_name[12];
					char	*str = "VidPID";
					if ( strncmp( firset_str, str, strlen( str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oAudioPID: {
					char file_name[12];
					if ( strncmp( firset_str, "AudioPID", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oAudioFormat: {
					char file_name[12];
					if ( strncmp( firset_str, "AudioFormat", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oProgrNmu: { /*  */
					char file_name[12];
					if ( strncmp( firset_str, "ProgrNmu", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oNETWORKID: { /* oNETWORKNUMB */
					char file_name[12];
					if ( strncmp( firset_str, "NETWORKID", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oNETWORKNUMB: { /* oNETWORKNUMB */
					char file_name[12];
					if ( strncmp( firset_str, "NETWORKNUMB", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oEITISERTENABLE: { /* oProgramOutputEnable */
					char file_name[12];
					if ( strncmp( firset_str, "EITISERTENABLE", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oNITISERTENABLE: { /* oProgramOutputEnable */
					char file_name[12];
					if ( strncmp( firset_str, "NITISERTENABLE", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oAutoModle: {
					char file_name[12];
					if ( strncmp( firset_str, "AutoModle", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

/*				case oLockPwdStu: { // */
/*					char file_name[12]; */
/*					if ( strncmp( firset_str, "LockPwdStu", strlen(firset_str) ) == 0 ) */
/*					{ */
/*						sprintf( file_name, "%s", replace_str ); */
/*						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum ); */

/*						result = system( tmpline ); */
/*						if ( result != -1 ) */
/*							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str ); */
/*					} */
/*				} */
/*				break; */

				case oRFENABLE: { /* oProgramOutputEnable */
					char file_name[12];
					if ( strncmp( firset_str, "RFENABLE", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oProgramOutput: { /*  */
					char file_name[12];
					if ( strncmp( firset_str, "ProgramOutput", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oProgramName: {
					char file_name[12];
					if ( strncmp( firset_str, "ProgramName", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oServiceName: {
					char file_name[12];
					if ( strncmp( firset_str, "ServiceName", strlen( firset_str ) ) == 0 )
					{
						sprintf( file_name, "%s", replace_str );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oRateMode:
				{
					char *name;
					if ( strncmp( firset_str, "RateMode", strlen( firset_str ) ) == 0 )
					{
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.channel_width_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oVideoBitrate:
				{
					char file_name[12];
					if ( strncmp( firset_str, "VideoBitrate", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oProfileConfig:
				{
					if ( strncmp( firset_str, "ProfileConfig", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.encoder_video_profile_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oLevelConfig:
				{
					if ( strncmp( firset_str, "LevelConfig", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.encoder_video_level_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oLcdTime:
				{
					if ( strncmp( firset_str, "LcdTime", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.system_lcd_time_out_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oPlayMode:
				{
					if ( strncmp( firset_str, "PlayModle", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.stream_usb_play_mod = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}

				break;

				case oRecordModle:
				{
					if ( strncmp( firset_str, "RecordModle", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.stream_usb_record_save_mod = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}

				break;

				case oVideoNorm:
				{
					if ( strncmp( firset_str, "VideoNorm", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.encoder_video_norm_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oNITLCNMOD:
				{
					if ( strncmp( firset_str, "NITLCNMOD", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.stream_nit_lcn_mode_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				case oBrightness:
				{
					char file_name[12];
					if ( strncmp( firset_str, "Brightness", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oContrast:
				{
					char file_name[12];
					if ( strncmp( firset_str, "Contrast", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oCpBrightness:
				{
					char file_name[12];
					if ( strncmp( firset_str, "CpBrightness", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oCpContrast:
				{
					char file_name[12];
					if ( strncmp( firset_str, "CpContrast", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oStreamTSID:
				{
					char file_name[12];
					if ( strncmp( firset_str, "StreamTSID", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oStreamONID:
				{
					char file_name[12];
					if ( strncmp( firset_str, "StreamONID", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oSaturation:
				{
					char file_name[12];
					if ( strncmp( firset_str, "Saturation", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oVideohue:
				{
					char file_name[12];
					if ( strncmp( firset_str, "Videohue", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oCpSaturation:
				{
					char file_name[12];
					if ( strncmp( firset_str, "CpSaturation", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;
				case oCpVideohue:
				{
					char file_name[12];
					if ( strncmp( firset_str, "CpVideohue", strlen( firset_str ) ) == 0 )
					{
						int	trl_val = atoi( (char *) replace_str );
						char	buf[5]	= { 0 };
						itoa_( trl_val, buf, 10 );

						sprintf( file_name, "%s", buf );
						sprintf( tmpline, "sysconfig.sh %s %s %d", p1, file_name, linenum );

						result = system( tmpline );
						if ( result != -1 )
							DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
					}
				}
				break;

				case oAudioBitrate: {
					if ( strncmp( firset_str, "AudioBitrate", strlen( firset_str ) ) == 0 )
					{
						char *name;
						name = get_parse_str( replace_str );
						if ( name )
						{
							config.scfg_Param.encoder_audio_bitrate_name = safe_strdup( name );
							sprintf( tmpline, "sysconfig.sh %s %s %d", p1, name, linenum );

							result = system( tmpline );
							if ( result != -1 )
								DEBUG( "%dline %s , %s write successfull !\r\n", linenum, p1, replace_str );
						}
					}
				}
				break;
				default:
					/* DEBUG( "not find object %d, s=%s p1=%s replace_str=%s original_str=%s", opcode, s, p1, replace_str, original_str ); */
					break;
				}
			}
		}
	}
	fclose( fd );
	set_nit_table_parm();
	return(linenum);
}


pid_t safe_fork( void )
{
	pid_t result;
	result = fork();

	if ( result == -1 )
	{
		DEBUG( "Failed to fork: %s.  Bailing out", strerror( errno ) );
		exit( 1 );
	} else if ( result == 0 )
	{
	}

	return(result);
}


void video_status_lock()
{
	if ( get_unlock_menu_exit() )
		return;

	if ( (M_HDMI == config.scfg_Param.encoder_video_interface) )
	{
		/* 设备配置 */
		int opcode, r_vic = get_vic_code();
		opcode = freq_parse_token( r_vic );
		if ( !opcode )
			opcode = VIDEO_INPUT_UNLOCK;
		else
			opcode = VIDEO_INPUT_LOCK;

		config.localstatus.encoder_video_input_lock = opcode;

		/* DEBUG("encoder_video_input_lock:%d  r_vic:%d ",config.localstatus.encoder_video_input_lock,r_vic); */
		hdmi_detection( r_vic );

		/* DEBUG( "encoder_video_input_lock = 0x%x", config.localstatus.encoder_video_input_lock ); */
	} else if ( (M_YPbPr == config.scfg_Param.encoder_video_interface) )
	{
		int optcode;


		memset( config.localstatus.encoder_video_resolution, ' ', 16 );
		memset( config.localstatus.encoder_video_shrot_resolution, ' ', 16 );
		optcode = get_vic_code();

		switch ( optcode )
		{
		case YPbPr1080Isign60Hz: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "1920x1080 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "1080i" );
		}
		break;
		case YPbPr1080Isign50Hz: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "1920x1080 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "1080i" );
		}
		break;
		case YPbPr720Psign50Hz: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "1280x720 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "720i" );
		}
		break;
		case YPbPr720Psign60Hz: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "1280x720 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "720i" );
		}
		break;
		default:
			DEBUG( "Not find user code..." );
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_UNLOCK;
			sprintf( config.localstatus.encoder_video_resolution, "unknown" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "unknown" );
			break;
		}

		DEBUG( "encoder_video_input_lock = 0x%02x", config.localstatus.encoder_video_input_lock );
		DEBUG( "%s  %p", config.localstatus.encoder_video_resolution, config.localstatus.encoder_video_resolution );
	} else if ( (M_CVBS == config.scfg_Param.encoder_video_interface) )
	{
		int optcode;
		memset( config.localstatus.encoder_video_resolution, ' ', 16 );
		memset( config.localstatus.encoder_video_shrot_resolution, ' ', 16 );
		optcode = get_vic_code();

		switch ( optcode )
		{
		case NTSC_60_ROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x480 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "480i" );
		}
		break;
		case NTSC_60_NOTROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x480 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "480i" );
		}
		break;
		case NTSC_50_NOTROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x576 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "576i" );
		}
		break;
		case NTSC_50_ROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x576 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "576i" );
		}
		break;

		case PAL_60_ROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x480 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "480i" );
		}
		break;
		case PAL_60_NOTROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x480 60i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "480i" );
		}
		break;
		case PAL_50_NOTROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x576 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "576i" );
		}
		break;
		case PAL_50_ROW: {
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_LOCK;
			sprintf( config.localstatus.encoder_video_resolution, "720x576 50i" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "576i" );
		}
		break;
		default:

		{
			config.localstatus.encoder_video_input_lock = VIDEO_INPUT_UNLOCK;
			sprintf( config.localstatus.encoder_video_resolution, "unknown" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "unknown" );
			DEBUG( "Not find user code..." );
		}
		break;
		}
	}
}


