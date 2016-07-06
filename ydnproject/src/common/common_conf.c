#define __LCD1602_H__

#include <init-configuration.h>

#include <freememscript.h>
#include <cy22393-set.h>
#include <cy22393_config.h>
#include <adv7842-set.h>
#include <math_instance.h>
#include <defineident.h>
#include <common_conf.h>
#include <lcd1602.h>
#include <dev-info.h>
#include <menu_foun.h>
#include <menu-set.h>
#include <readbus.h>
#include <usbinfo.h>


struct cache_t		*cache_pr;
struct  MenuItem	*MenuPoint;
uint8_t			t_date[17];
struct cache_t		*cachep;


struct MenuItem MainMenu[6] = {                                                 /* 主菜单 */
	{
		6, " Status         ", &NullSubs, sysStatus, NULL               /* modulater alarm not doing 20160420 */
	},
	{ 6, " Encoder        ", &NullSubs, EncoderCfg,	  NULL },               /* ok 20160420 */
	{ 6, " Modulator      ", &NullSubs, ModulatorCfg, NULL },               /* ok 20160420 */
	{ 6, " Stream         ", &NullSubs, StreamCfg,	  NULL },               /* ok 20160420 */
	{ 6, " USB Device     ", &NullSubs, UsbDevCfg,	  NULL },               /* no ,only ts lock 20160420 */
	{ 6, " System         ", &NullSubs, SystemCfg,	  NULL },               /* ok 20160420 */
};


struct MenuItem sysStatus[4] = {
	{ 4, " Encoder Stat   ", &NullSubs,	  EncoderStatus, MainMenu },
	{ 4, " Modulator Stat ", &mbitrate,	  NULL,		 MainMenu },
	{ 4, " Alarm stat     ", &NullSubs,	  Status,	 MainMenu },    /* ok */
	{ 4, " System Stat    ", &systemstat_cfg, NULL,		 MainMenu },    /* ok */
};

struct  MenuItem *MenuPoint = MainMenu;

struct  MenuItem Status[3] = {                                                  /* Uptime                 / * 状态菜单 * / */
	{ 3, " Uptime         ", &run_time,    NULL, sysStatus },               /* ok */
	{ 3, " Serial Number  ", &sys_serial,  NULL, sysStatus },               /* Serial ok */
	{ 3, " Version        ", &sys_Version, NULL, sysStatus },               /* ok */
};


struct  MenuItem EncoderCfg[3] = {                                              /* 编码器参数配置菜单 */
	{
		3, " Video CFG      ", &NullSubs, VideoCfg, MainMenu
	},
	{ 3, " Audio CFG      ", &NullSubs, AudioCfg,	 MainMenu },
	{ 3, " Program Info   ", &NullSubs, ProgInfoCfg, MainMenu },
};

struct  MenuItem ModulatorCfg[8] = {
	/* 调制器参数配置菜单 */
	{
		8, " Bandwidth CFG  ", &Bandwidth_Cfg, BandwidthCfg, MainMenu
	},
	{ 8, " Constell CFG   ", &Constell_cfg,	       ConstellCfg,  MainMenu },
	{ 8, " FFT CFG        ", &fft_NK_cfg,	       FFTCfg,	     MainMenu },
	{ 8, " Guard interval ", &guardIntv_dalay_cfg, GuardIntvCfg, MainMenu }, /* ? */
	{ 8, " Code rate CFG  ", &coderate_cfg,	       CoderateCfg,  MainMenu },
	{ 8, " RF frequency   ", &RF_frequency_status, RFFreqCfg,    MainMenu },
	{ 8, " RF level       ", &rf_level,	       RFLevelCfg,   MainMenu },
	{ 8, " RF out         ", &rf_out,	       RFOutCfg,     MainMenu },
};


struct  MenuItem usbbitrat[1] = {
	{
		1, "                ", &NullSubs, NULL, FileBrowse
	},
};

struct  MenuItem StreamCfg[4] = {                                                       /* TS流参数配置菜单 */
	{
		4, " TSID           ", &streamtsid, TsidCfg, MainMenu
	},
	{ 4, " ONID           ", &streamonid, OnidCfg, MainMenu },
	{ 4, " NIT            ", &NullSubs,   NitCfg,  MainMenu },
	{ 4, " EIT            ", &NullSubs,   EitCfg,  MainMenu },
};


struct  MenuItem UsbDevCfg[3] = {                                       /* usb_rec_enable     yes no                        / * TS流参数配置菜单 * / */
	{
		3, " Record TS      ", &NullSubs, RecordTsCfg, MainMenu
	},
	{ 3, " Play TS        ", &NullSubs, PlayTsCfg,	  MainMenu },
	{ 3, " USB Status     ", &NullSubs, UsbDevStatus, MainMenu },   /* 20160412 update jh */
};

struct  MenuItem SystemCfg[4] = {                                       /* 系统参数配置菜单 */
	{ 4, " Factory reset  ", &factory_reset,   FactoryRstCfg,  MainMenu },
	{ 4, " LCD time out   ", &lcd_timeout,	   LCDTimeoutCfg,  MainMenu },
	{ 4, " Key password   ", &key_passwrd_cfg, KeyPasswordCfg, MainMenu },
	{ 4, " Lock key       ", &key_lock,	   LockKeyCfg,	   MainMenu },
};


struct  MenuItem EncoderStatus[2] = {                                   /* RecordTsCfg                               / * 编码器状态菜单 * / */
	{
		2, " Video in lock  ", &video_lock_status_Cfg, NULL, sysStatus
	},
	{ 2, " Video resolution", &video_resolution, NULL, sysStatus }, /* VideoResolution */
};


struct  MenuItem LockKeyCfg[1] = {
	{ 1, " Lock key       ", &NullSubs, NULL, SystemCfg },
};


struct  MenuItem UsbDevStatus[3] = {    /* USB设备状态菜单 UsbPlay*/
	{
		3, " TS lock        ", &NullSubs, NULL, UsbDevCfg
	},
	{ 3, " Disk usage     ", &get_usb_usrinfo,   NULL, UsbDevCfg },
	{ 3, " Remove device  ", &manu_do_mount_dev, NULL, UsbDevCfg },
};


struct  MenuItem SystemStatus[2] = {                            /* 系统状态菜单 */
	{
		2, " Device status  ", &NullSubs, DevStatus, Status
	},
	{ 2, " Uptime         ", &NullSubs, NULL, Status },
};

struct  MenuItem AlarmStatus[3] = {                             /* 系统告警菜单 */
	{
		3, " Device alarm   ", &NullSubs, NULL, Status
	},
	{ 3, " Video unlock   ", &NullSubs, NULL, Status },
	{ 3, " TS unlock      ", &NullSubs, NULL, Status },     /* 由fpga进行设置 */
};


/*
 * struct  MenuItem ProjectStatus[6] = {  / * 工程测试状态菜单 * /
 * {
 *      6, " ADV7441A status", &NullSubs, Adv7441aStatus, Status
 * },
 * { 6, " MB86H46 status ", &NullSubs, Mb86h46Status, Status },
 * { 6, " AD9789 status  ", &NullSubs, Ad9789Status,  Status },
 * { 6, " ADF4350 status ", &NullSubs, Adf4350Status, Status },
 * { 6, " USB3300 status ", &NullSubs, Usb3300Status, Status },
 * { 6, " FPGA status    ", &NullSubs, FpgaStatus,	   Status },
 * };
 */

struct  MenuItem VideoCfg[10] = {                               /* 视频编码配置菜单 */
	{
		10, " Video interface", &video_inerface_cfg, VideoIntfCfg, EncoderCfg
	},
	{ 10, " Video bitrate  ", &video_bitrate,    VideoBitrateCfg, EncoderCfg },
	{ 10, " Video rate mode", &video_rate_mode,  RateModeCfg,     EncoderCfg },
	{ 10, " H.264 profile  ", &h264_profile_cfg, ProfileCfg,      EncoderCfg },
	{ 10, " H.264 level    ", &h264_level,	     LevelCfg,	      EncoderCfg },
	{ 10, " Video brightnes", &vedeio_bright,    BrightnessCfg,   EncoderCfg },
	{ 10, " Video contrast ", &videocontrast,    ContrastCfg,     EncoderCfg },
	{ 10, " Video saturatio", &videosaturation,  SaturationCfg,   EncoderCfg },
	{ 10, " Video hue      ", &videohue,	     HueCfg,	      EncoderCfg },
	{ 10, " Video Norm     ", &videonorm,	     StandardCfg,     EncoderCfg },
};


struct  MenuItem AudioCfg[2] = {                /* 音频编码配置菜单 */
	{
		2, " Audio bitrate  ", &audio_bitrate, AudioBitrateCfg, EncoderCfg
	},
	{ 2, " Audio format   ", &audio_format, AudioFormatCfg, EncoderCfg },
};

struct  MenuItem ProgInfoCfg[10] = {            /* 视频编码配置菜单 */
	{
		10, " Program Output ", &program_ouput, ProgOutCfg, EncoderCfg
	},
	{ 10, " Program name   ", &program_name,    ProgNameCfg,    EncoderCfg },
	{ 10, " Service name   ", &Service_name,    ServiceNameCfg, EncoderCfg },
	{ 10, " Program number ", &program_num,	    ProgNumCfg,	    EncoderCfg },
	{ 10, " PMT PID        ", &pmt_pid,	    PmtPidCfg,	    EncoderCfg },
	{ 10, " PCR PID        ", &prc_pid,	    PcrPidCfg,	    EncoderCfg },
	{ 10, " Video PID      ", &video_pid,	    VideoPidCfg,    EncoderCfg },
	{ 10, " Audio PID      ", &audio_pid,	    AudioPidCfg,    EncoderCfg },
	{ 10, " LCN            ", &programinfo_lcn, LcnCfg,	    EncoderCfg },
	{ 10, " EIT event      ", &NullSubs,	    EitEventCfg,    EncoderCfg },
};

struct MenuItem AudioFormatCfg[1] = {
	{ 2, " Audio format   ", &NullSubs, NULL, AudioCfg },
};

struct  MenuItem	TsidCfg[1] = {
	{
		1, " TSID           ", &NullSubs, NULL, StreamCfg
	},
};
struct  MenuItem	OnidCfg[1] = {
	{ 1, " ONID           ", &NullSubs, NULL, StreamCfg },
};

struct MenuItem StandardCfg[1] = {
	{ 10, " Video Norm     ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem HueCfg[1] = {
	{ 1, " hue            ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem SaturationCfg[1] = {
	{ 1, " saturation     ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem ContrastCfg[1] = {
	{ 1, " Video contrast ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem ProgOutCfg[1] = {
	{
		1, " Program Output ", &NullSubs, NULL, ProgInfoCfg
	},
};

struct MenuItem LcnCfg[1] = {
	{ 1, " LCN            ", &NullSubs, NULL, ProgInfoCfg }
};

struct MenuItem EitEventCfg[2] = {
	{ 2, " Now            ", &NullSubs, EitNowCfg,	ProgInfoCfg },
	{ 2, " Next           ", &NullSubs, EitNextCfg, ProgInfoCfg },
};

struct MenuItem EitNowCfg[4] = {
	{ 4, " Start year     ", &start_year,	       StartYearCfg,	  EitEventCfg },
	{ 4, " Start time     ", &start_time,	       StartTimeCfg,	  EitEventCfg },
	{ 4, " Duration       ", &start_duration,      StartDurationCfg,  EitEventCfg },
	{ 4, " Event name     ", &start_eventname_Cfg, StartEventNameCfg, EitEventCfg },
};
struct MenuItem EitNextCfg[4] = {
	{ 4, " Next start year", &next_year,	      NextYearCfg,	EitEventCfg },
	{ 4, " Next start time", &next_time,	      NextTimeCfg,	EitEventCfg },
	{ 4, " Next duration  ", &next_duration,      NextDurationCfg,	EitEventCfg },
	{ 4, " Next event name", &next_eventname_Cfg, NextEventNameCfg, EitEventCfg },
};

struct  MenuItem EitCfg[2] = {                                          /* EIT配置菜单 */
	{
		2, " Language code  ", &language_Cfg, LanguageCfg, StreamCfg
	},
	{ 2, " EIT insert     ", &eit_insert_Cfg, EitInsert, StreamCfg },
};
/*  */
struct  MenuItem RecordTsCfg[2] = {                                     /* send_usb_writ_message                                / * usb_recode                                       / * TS流录制配置菜单 * / * / */
	{
		2, " Start record   ", &usb_rec_enable, NULL, UsbDevCfg /*, .offon = START_W_R, */
	},
	{ 2, " Advanced config", &NullSubs, AdvancedCfg, UsbDevCfg },   /* 预设输入改变功能 */
};


struct  MenuItem NitCfg[7] = {                                          /* NIT配置菜单 */
	{ 7, " Network ID     ", &network_id,	    NetworkIdCfg,     StreamCfg },
	{ 7, " Network name   ", &network_name,	    NetworkNameCfg,   StreamCfg },
	{ 7, " Version mode   ", &NullSubs,	    NULL,	      StreamCfg },
	{ 7, " Version number ", &nit_version_numb, VersionNumberCfg, StreamCfg },
	{ 7, " LCN mode       ", &nit_lcn_mod_cfg,  LcnMode,	      StreamCfg },
	{ 7, " Private data   ", &PrivateData,	    PrivateDataCfg,   StreamCfg },
	{ 7, " NIT insert     ", &NullSubs,	    NULL,	      StreamCfg },
};

struct MenuItem LcnMode[1] = {
	{ 1, " LCN mode       ", &NullSubs, NULL, NitCfg },
};
/* usb配置 */
struct MenuItem AdvancedCfg[4] = {
	{ 4, " File name      ", &ts,		   NULL,       RecordTsCfg },
	{ 4, " Automatic recor", &NullSubs,	   NULL,       RecordTsCfg },
	{ 4, " File Size      ", &file_size,	   NULL,       RecordTsCfg },
	{ 4, " File save Mode ", &record_save_mod, recird_mod, RecordTsCfg }
};

struct  MenuItem recird_mod[1] = {
	{ 1, " File save Mode ", &NullSubs, NULL, AdvancedCfg }
};

struct  MenuItem LCDTimeoutCfg[1] = {   /* LCD背光超时保护配置菜单 */
	{ 1, " LCD time out   ", &NullSubs, NULL, SystemCfg },
};


struct  MenuItem PlayTsCfg[2] = {       /* TS流播放配置菜单 */
	{
		2, " File browse    ", &usb_ts_inf, FileBrowse, UsbDevCfg
	},
	{ 2, " Play mode      ", &play_mod, PlayMode, UsbDevCfg },
};

struct MenuItem PlayMode[1] = {
	{ 1, " Play mode      ", &NullSubs, NULL, PlayTsCfg },
};

struct MenuItem LevelCfg[1] = {
	{ 1, " H.264 level    ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem ProfileCfg[1] = {
	{ 1, " H.264 profile  ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem VideoBitrateCfg[1] = {
	{ 1, " Video bitrate  ", &NullSubs, NULL, VideoCfg },
};

struct MenuItem BrightnessCfg[1] = {
	{ 1, " Video brightnes", &NullSubs, NULL, VideoCfg },
};

struct MenuItem RateModeCfg[1] = {
	{ 1, " Video rate mode", &NullSubs, NULL, VideoCfg }
};

struct MenuItem VideoIntfCfg[1] = {
	{ 1, " Video interface", &NullSubs, NULL, VideoCfg },
};

struct MenuItem StartYearCfg[1] = {
	{ 1, " Start year     ", &NullSubs, NULL, EitNowCfg },
};
struct MenuItem StartTimeCfg[1] = {
	{ 1, " Start time     ", &NullSubs, NULL, EitNowCfg },
};
struct MenuItem StartDurationCfg[1] = {
	{ 1, " Duration       ", &NullSubs, NULL, EitNowCfg },
};
struct MenuItem StartEventNameCfg[1] = {
	{ 1, " Event name     ", &NullSubs, NULL, EitNowCfg },
};

struct MenuItem NextYearCfg[1] = {
	{ 1, " Next start year", &NullSubs, NULL, EitNextCfg },
};
struct MenuItem NextTimeCfg[1] = {
	{ 1, " Next start time", &NullSubs, NULL, EitNextCfg },
};
struct MenuItem NextDurationCfg[1] = {
	{ 1, " Next duration  ", &NullSubs, NULL, EitNextCfg },
};
struct MenuItem NextEventNameCfg[1] = {
	{ 1, " Next event name", &NullSubs, NULL, EitNextCfg },
};


struct MenuItem ServiceNameCfg[1] = {
	{ 1, " Service name   ", &NullSubs, NULL, ProgInfoCfg },
};

struct MenuItem ProgNameCfg[1] = {
	{ 1, " Program name   ", &NullSubs, NULL, ProgInfoCfg },
};

struct MenuItem ProgNumCfg[1] = {
	{ 1, " Program number ", &NullSubs, NULL, ProgInfoCfg },
};

struct MenuItem AudioBitrateCfg[1] = {
	{ 1, " Audio bitrate  ", &NullSubs, NULL, AudioCfg }
};

struct MenuItem AudioPidCfg[1] = {
	{ 10, " Audio PID      ", &NullSubs, NULL, ProgInfoCfg },
};

struct MenuItem VideoPidCfg[1] = {
	{ 1, " Video PID      ", &NullSubs, NULL, ProgInfoCfg }
};

struct MenuItem PcrPidCfg[1] = {
	{ 1, " PCR PID        ", &NullSubs, NULL, ProgInfoCfg },
};

struct MenuItem PmtPidCfg[1] = {
	{ 1, " PMT PID        ", &NullSubs, NULL, ProgInfoCfg },
};


struct  MenuItem BandwidthCfg[1] = {    /* 调制器带宽配置菜单 */
	{
		1, " Bandwidth      ", &NullSubs, NULL, ModulatorCfg
	},
};


struct  MenuItem ConstellCfg[1] = {     /* 调制器星座配置菜单 */
	{
		1, " Constell       ", &NullSubs, NULL, ModulatorCfg
	},
};

struct  MenuItem FFTCfg[1] = {          /* 调制器FFT配置菜单 */
	{
		1, " FFT            ", &NullSubs, NULL, ModulatorCfg
	},
};

struct  MenuItem GuardIntvCfg[1] = {    /* 调制器OFDM帧保护间隔长度配置菜单 */
	{
		1, " Guard interval ", &NullSubs, NULL, ModulatorCfg
	},
};


struct  MenuItem CoderateCfg[1] = {     /* 调制器编码码率配置菜单 */
	{ 1, " Code rate      ", &NullSubs, NULL, ModulatorCfg },
};

struct  MenuItem RFFreqCfg[1] = {       /* 调制器射频频率配置菜单 */
	{
		1, " RF frequency   ", &NullSubs, NULL, ModulatorCfg
	},
};

struct  MenuItem RFLevelCfg[1] = {      /* 调制器射频电平配置菜单 */
	{
		1, " RF level       ", &NullSubs, RFLevelEdit, ModulatorCfg
	},
};

struct  MenuItem RFOutCfg[1] = {        /* 调制器射频电平配置菜单 */
	{
		1, " RF Out         ", &NullSubs, RFOutSel, ModulatorCfg
	},
};


struct MenuItem PrivateDataCfg[1] = {
	{ 7, " Private data   ", &NullSubs, NULL, NitCfg },
};

struct MenuItem VersionNumberCfg[1] = {
	{ 1, " Version number ", &NullSubs, NULL, NitCfg },
};

struct MenuItem NetworkNameCfg[1] = {
	{ 1, " Network name   ", &NullSubs, NULL, NitCfg },
};

struct MenuItem NetworkIdCfg[1] = {
	{ 1, " Network ID     ", &NullSubs, NULL, NitCfg },
};


struct MenuItem EitInsert[1] = {
	{ 1, " EIT insert     ", &NullSubs, NULL, EitCfg },
};

struct MenuItem LanguageCfg[1] = {
	{
		1, " Language code  ", &NullSubs, NULL, EitCfg
	},
};

struct MenuItem StartRecord[2] = {      /* usb_rec_enable */
	{ 2, " Ready to record", &NullSubs, NULL, RecordTsCfg },
	{ 2, " please wait... ", &NullSubs, NULL, RecordTsCfg },
};


struct  MenuItem SaveCfg[1] = {         /* 保存配置菜单 */
	{
		1, " Save config    ", &NullSubs, SaveEnable, SystemCfg
	},
};

struct  MenuItem LoadSaveCfg[1] = {     /* 加载保存配置菜单 */
	{
		1, " Load save confi", &NullSubs, LoadSaveEable, SystemCfg
	},
};

struct  MenuItem FactoryRstCfg[1] = {   /* 恢复出厂配置菜单 */
	{
		1, " Factory reset  ", &NullSubs, FactoryRstEnable, SystemCfg
	},
};


#if 1
struct  MenuItem KeyPasswordCfg[1] = {  /* 按键密码配置菜单 */
	{
		1, " Key password   ", &NullSubs, NULL, SystemCfg
	},
};
#endif


struct  MenuItem ProjectTestCfg[1] = {  /* 工程测试配置菜单 */
	{
		1, " Project test   ", &NullSubs, ProjectTestEnable, SystemCfg
	},
};


