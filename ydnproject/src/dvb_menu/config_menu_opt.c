#include <config_menu_opt.h>
#include <debug.h>


menu_cfg_t cfg_level[] = {
	{
		.optmenu	= "Level   4",
		.cfgmenu	=  "Level4",
	},
	{
		.optmenu	= "Level 4.1",
		.cfgmenu	= "Level4_1",
	},
	{
		.optmenu	=  "Level 4.2",
		.cfgmenu	=  "Level4_2",
	},
	{
		.optmenu	= "Level   3",
		.cfgmenu	=  "Level3",
	},
	{
		.optmenu	="Level 3.1",
		.cfgmenu	= "Level3_1",
	},
	{
		.optmenu	= "Level 3.2",
		.cfgmenu	= "Level3_2",
	},
	{
		.optmenu	="Default  ",
		.cfgmenu	= "Leveldefu",
	},
};


menu_cfg_t cfg_videonorm[] = {
	{
		.optmenu	= "AUTO     ",
		.cfgmenu	= "AUTO",
	},
	{
		.optmenu	= "PAL-BGHID",
		.cfgmenu	= "PAL-BGHID",
	},
	{
		.optmenu	= "PAL-N    ",
		.cfgmenu	= "PAL-N",
	},
	{
		.optmenu	= "PAL-CN   ",
		.cfgmenu	= "PAL-CN",
	},
	{
		.optmenu	= "PAL-M    ",
		.cfgmenu	= "PAL-M",
	},
	{
		.optmenu	= "NTSC-M   ",
		.cfgmenu	= "NTSC-M",
	},
	{
		.optmenu	= "NTSC-J   ",
		.cfgmenu	= "NTSC-J",
	},
	{
		.optmenu	= "NTSC-4.43",
		.cfgmenu	= "NTSC-4.43",
	},
};

menu_cfg_t cfg_lcnmod[] = {
	{
		.optmenu	= "European ",
		.cfgmenu	= "European_Cfg",
	},
	{
		.optmenu	= "NorDig V1",
		.cfgmenu	= "NorDig_V1Cfg",
	},
	{
		.optmenu	= "NorDig V1",
		.cfgmenu	= "NorDig_V2Cfg",
	},
};

	
menu_cfg_t cfg_profile[] = {
	{
		.optmenu	= "High Profile",
		.cfgmenu	= "HIGH_CFG",
	},
	{
		.optmenu	= "Main Profile",
		.cfgmenu	= "MAIN_CFG",
	},
	{
		.optmenu	= "Base Profile",
		.cfgmenu	= "BASE_CFG",
	},
	{
		.optmenu	= "Default       ",
		.cfgmenu	= "DEFAULT_CFG",
	},
};

menu_cfg_t cfg_video_bitrate[] = {
	{
		.optmenu	= "64  kbps",
		.cfgmenu	= "64_kbps",
	},
	{
		.optmenu	= "96  kbps",
		.cfgmenu	= "96_kbps",
	},
	{
		.optmenu	= "112 kbps",
		.cfgmenu	= "112_kbps",
	},
	{
		.optmenu	= "128 kbps",
		.cfgmenu	= "128_kbps",
	},
	{
		.optmenu	= "160 kbps",
		.cfgmenu	= "160_kbps",
	},
	{
		.optmenu	= "192 kbps",
		.cfgmenu	= "192_kbps",
	},
	{
		.optmenu	= "224 kbps",
		.cfgmenu	= "224_kbps",
	},
	{
		.optmenu	= "256 kbps",
		.cfgmenu	= "256_kbps",
	},
	{
		.optmenu	= "320 kbps",
		.cfgmenu	= "320_kbps",
	},
	{
		.optmenu	= "384 kbps",
		.cfgmenu	= "384_kbps",
	},
};

menu_cfg_t cfg_video_rate_mode[] = {
	{
		.optmenu	= "CBR",
		.cfgmenu	= "CBR",
	},
	{
		.optmenu	= "VBR",
		.cfgmenu	= "VBR",
	},
};

menu_cfg_t cfg_Constell[] = {
	{
		.optmenu	= "64 QAM",
		.cfgmenu	= "MODULATE_M_64QAM",
	},
	{
		.optmenu	= "QPSK  ",
		.cfgmenu	= "MODULATE_M_QPSK",
	},
	{
		.optmenu	= "16 QAM",
		.cfgmenu	= "MODULATE_M_16QAM",
	},
};

menu_cfg_t cfg_eitinsert[] = {
	{
		.optmenu	= "Yes",
		.cfgmenu	= EIT_ENABLE,
	},
	{
		.optmenu	= "No ",
		.cfgmenu	= EIT_DESENABLE,
	},
};

menu_cfg_t cfg_play_mod[] = {
	{
		.optmenu	= "Single file",
		.cfgmenu	= USB_SIG,
	},
	{
		.optmenu	= "Single loop",
		.cfgmenu	= USB_SIG_LOOP,
	},
	{
		.optmenu	= "Play all   ",
		.cfgmenu	= USB_SIG_PLAY_ALL,
	},
};


menu_cfg_t cfg_record_mod[] = {
	{
		.optmenu	= "Single file   ",
		.cfgmenu	= USB_RECORD_SIG_MOD,
	},
	{
		.optmenu	= "Segmented file",
		.cfgmenu	= USB_RECORD_SEG_MOD,
	},
	{
		.optmenu	= "loop record   ",
		.cfgmenu	= USB_RECORD_LOOP_MOD,
	},
};

menu_cfg_t cfg_rfout[] = {
	{
		.optmenu	= "Yes",
		.cfgmenu	= EIT_ENABLE,
	},
	{
		.optmenu	= "No ",
		.cfgmenu	= EIT_DESENABLE,
	},
};

menu_cfg_t cfg_programoutput[] = {
	{
		.optmenu	= "Enable ",
		.cfgmenu	= EIT_ENABLE,
	},
	{
		.optmenu	= "Disable",
		.cfgmenu	= EIT_DESENABLE,
	},
};

menu_cfg_t cfg_video_inerface[] = {
	{
		.optmenu	= "HDMI ",
		.cfgmenu	= HDMI_M,
	},
	{
		.optmenu	= "YPbPr",
		.cfgmenu	= YPbPr_M,
	},
	{
		.optmenu	= "CVBS ",
		.cfgmenu	= CVBS_M,
	},
};

menu_cfg_t cfg_audio_format[] = {
	{
		.optmenu	= "MPEG-2    ",
		.cfgmenu	= "MPEG-2",
	},
	{
		.optmenu	= "MPEG-2 AAC",
		.cfgmenu	= "MPEG-2-AAC",
	},
	{
		.optmenu	= "MPEG-4 AAC",
		.cfgmenu	= "MPEG-4-AAC",
	},
};

menu_cfg_t	cfg_Bandwidth[] = {
	{
		.optmenu	= "8MHz",
		.cfgmenu	= "8MHz",
	},
	{
		.optmenu	= "7MHz",
		.cfgmenu	= "7MHz",
	},
	{
		.optmenu	= "6MHz",
		.cfgmenu	= "6MHz",
	},
};
menu_cfg_t	cfg_ftt[] = {
	{
		.optmenu	= "FFT 2K",
		.cfgmenu	= "FFT_2K",
	},
	{
		.optmenu	= "FFT 8K",
		.cfgmenu	= "FFT_8K",
	},
};

menu_cfg_t cfg_coderate[] = {
	{
		.optmenu	= "1/2",
		.cfgmenu	= "ENCODE_RATE_2_1",
	},
	{
		.optmenu	= "2/3",
		.cfgmenu	= "ENCODE_RATE_3_2",
	},
	{
		.optmenu	= "3/4",
		.cfgmenu	= "ENCODE_RATE_4_3",
	},
	{
		.optmenu	= "5/6",
		.cfgmenu	= "ENCODE_RATE_6_5",
	},
	{
		.optmenu	= "7/8",
		.cfgmenu	= "ENCODE_RATE_8_7",
	},
};

menu_cfg_t cfg_guardIntv_dalay[] = {
	{
		.optmenu	= "1/32",
		.cfgmenu	= "F_DELTAL_32_1",
	},
	{
		.optmenu	= "1/16",
		.cfgmenu	= "F_DELTAL_16_1",
	},
	{
		.optmenu	= "1/8 ",
		.cfgmenu	= "F_DELTAL_8_1",
	},
	{
		.optmenu	= "1/4 ",
		.cfgmenu	= "F_DELTAL_4_1",
	},
};

menu_cfg_t cfg_lcd_time[] = {
	{
		.optmenu	= "5s  ",
		.cfgmenu	= "t5s",
	},
	{
		.optmenu	= "10s ",
		.cfgmenu	= "t10s",
	},
	{
		.optmenu	= "30s ",
		.cfgmenu	= "t30s",
	},
	{
		.optmenu	= "45s ",
		.cfgmenu	= "t45s",
	},
	{
		.optmenu	= "60s ",
		.cfgmenu	= "t60s",
	},
	{
		.optmenu	= "90s ",
		.cfgmenu	= "t90s",
	},
	{
		.optmenu	= "120s",
		.cfgmenu	= "t120s",
	},
};


menu_opt_t menu_opt[] = {
	{
		.first_menu	= "European ",
		.cfg_t		= cfg_lcnmod,
		.lenth		= sizeof(cfg_lcnmod) / sizeof(cfg_lcnmod[0]),
	},
	{
		.first_menu	= "Level   4",
		.cfg_t		= cfg_level,
		.lenth		= sizeof(cfg_level) / sizeof(cfg_level[0]),
	},
	{
		.first_menu	= "AUTO     ",
		.cfg_t		= cfg_videonorm,
		.lenth		= sizeof(cfg_videonorm) / sizeof(cfg_videonorm[0]),
	},
	{
		.first_menu	= "High Profile",
		.cfg_t		= cfg_profile,
		.lenth		= sizeof(cfg_profile) / sizeof(cfg_profile[0]),
	},
	{
		.first_menu	= "64  kbps",
		.cfg_t		= cfg_video_bitrate,
		.lenth		= sizeof(cfg_video_bitrate) / sizeof(cfg_video_bitrate[0]),
	},
	{
		.first_menu	= "CBR",
		.cfg_t		= cfg_video_rate_mode,
		.lenth		= sizeof(cfg_video_rate_mode) / sizeof(cfg_video_rate_mode[0]),
	},
	{
		.first_menu	= "64 QAM",
		.cfg_t		= cfg_Constell,
		.lenth		= sizeof(cfg_Constell) / sizeof(cfg_Constell[0]),
	},
	{
		.first_menu	= "Yes",
		.cfg_t		= cfg_eitinsert,
		.lenth		= sizeof(cfg_eitinsert) / sizeof(cfg_eitinsert[0]),
	},
	{
		.first_menu	= "Yes",
		.cfg_t		= cfg_rfout,
		.lenth		= sizeof(cfg_rfout) / sizeof(cfg_rfout[0]),
	},
	{
		.first_menu	= "Enable ",
		.cfg_t		= cfg_programoutput,
		.lenth		= sizeof(cfg_programoutput) / sizeof(cfg_programoutput[0]),
	},
	{
		.first_menu	= "HDMI ",
		.cfg_t		= cfg_video_inerface,
		.lenth		= sizeof(cfg_video_inerface) / sizeof(cfg_video_inerface[0]),
	},
	{
		.first_menu	= "8MHz",
		.cfg_t		= cfg_Bandwidth,
		.lenth		= sizeof(cfg_Bandwidth) / sizeof(cfg_Bandwidth[0]),
	},
	{
		.first_menu	= "MPEG-2    ",
		.cfg_t		= cfg_audio_format,
		.lenth		= sizeof(cfg_audio_format) / sizeof(cfg_audio_format[0]),
	},
	{
		.first_menu	= "FFT 2K",
		.cfg_t		= cfg_ftt,
		.lenth		= sizeof(cfg_ftt) / sizeof(cfg_ftt[0]),
	},
	{
		.first_menu	= "1/2",
		.cfg_t		= cfg_coderate,
		.lenth		= sizeof(cfg_coderate) / sizeof(cfg_coderate[0]),
	},
	{
		.first_menu	= "1/32",
		.cfg_t		= cfg_guardIntv_dalay,
		.lenth		= sizeof(cfg_guardIntv_dalay) / sizeof(cfg_guardIntv_dalay[0]),
	},
	{
		.first_menu	= "5s  ",
		.cfg_t		= cfg_lcd_time,
		.lenth		= sizeof(cfg_lcd_time) / sizeof(cfg_lcd_time[0]),
	},
	{
		.first_menu	= "Single file",
		.cfg_t		= cfg_play_mod,
		.lenth		= sizeof(cfg_play_mod) / sizeof(cfg_play_mod[0]),
	},
	{
		.first_menu	= "Single file   ",
		.cfg_t		= cfg_record_mod,
		.lenth		= sizeof(cfg_record_mod) / sizeof(cfg_record_mod[0]),
	},
};

menu_opt_t *p_menu_opt_arr()
{
	return(menu_opt);
}


/*
 * 菜单配置项索引
 *
 * firstitemmenu 菜单第一项firstitemmenu[0]
 * cfgmenu       配置菜单项
 *
 * res                  返回值
 */
int index_cfg_menu( const char *firstitemmenu, const char *cfgmenu )
{
	int i, y = 0, res = 0;
	
	for ( i = 0; i < sizeof(menu_opt) / sizeof(menu_opt[0]); i++ )
	{
		if ( strncasecmp( firstitemmenu, (const char *) menu_opt[i].first_menu,
				  strlen( (const char *) menu_opt[i].first_menu ) - 1 ) == 0 )
		{
			for ( y = 0; y < menu_opt[i].lenth; y++ )
			{
				if ( strncasecmp( menu_opt[i].cfg_t[y].cfgmenu, cfgmenu,strlen(cfgmenu))== 0 )
						
				{
					res = y;
					
					break;
				}
			}
		}
	}

	return(res);
}


