#ifndef __MENU_SET__H
#define __MENU_SET__H

struct MenuItem MainMenu[6];


/*
 * *-------------------------------
 * *主菜单定义
 * *-------------------------------
 */

struct MenuItem Status[3];                      /* MainMenu下状态菜单 */
struct MenuItem EncoderCfg[3];                  /* MainMenu下编码器参数配置菜单 */
struct MenuItem ModulatorCfg[8];                /* MainMenu下调制器参数配置菜单 */
struct MenuItem StreamCfg[4];                   /* MainMenu下TS流参数配置菜单 */
struct MenuItem UsbDevCfg[3];                   /* MainMenu下USB设备参数配置菜单 */
struct MenuItem SystemCfg[4];                   /* MainMenu下系统配置菜单 */


/*
 * *-------------------------------
 * *一级菜单定义
 * *-------------------------------
 * *-1.1-
 */
struct  MenuItem	EncoderStatus[2];       /* Status下编码器状态菜单 */
struct  MenuItem	ModulatorStatus[1];     /* Status下调制器状态菜单 */
struct  MenuItem	UsbDevStatus[3];        /* Status下USB设备状态菜单 */
struct  MenuItem	SystemStatus[2];        /* Status下系统状态菜单 */
struct  MenuItem	AlarmStatus[3];         /* Status下告警状态菜单 */

/* *-1.2- */
struct  MenuItem	VideoCfg[10];           /* EncoderCfg下视频配置菜单 */
struct  MenuItem	AudioCfg [2];           /* EncoderCfg下音频配置菜单 */
struct  MenuItem	ProgInfoCfg[10];        /* EncoderCfg下节目信息配置菜单 */
/* *-1.3- */
struct  MenuItem	BandwidthCfg[1];        /* ModulatorCfg下带宽参数配置菜单 */
struct  MenuItem	ConstellCfg [1];        /* ModulatorCfg下星座参数配置菜单 */
struct  MenuItem	FFTCfg[1];              /* ModulatorCfg下FFT参数配置菜单 */
struct  MenuItem	GuardIntvCfg[1];        /* ModulatorCfg下保护间隔参数配置菜单 */
struct  MenuItem	CoderateCfg[1];         /* ModulatorCfg下编码码率参数配置菜单 */
struct  MenuItem	RFFreqCfg[1];           /* ModulatorCfg下射频频率参数配置菜单 */
struct  MenuItem	RFLevelCfg[1];          /* ModulatorCfg下射频电平参数配置菜单 */
struct  MenuItem	RFOutCfg[1];            /* ModulatorCfg下射频开关配置菜单 */
/* struct  MenuItem	Bitrate[1]; */
/* *-1.4- */
struct  MenuItem	TsidCfg[1];             /* StreamCfg下TS ID参数配置菜单 */
struct  MenuItem	OnidCfg[1];             /* StreamCfg下ON ID参数配置菜单 */
struct  MenuItem	NitCfg[7];              /* StreamCfg下NIT参数配置菜单 */
struct  MenuItem	EitCfg[2];              /* StreamCfg下EIT参数配置菜单 */
/* *-1.5- */
struct  MenuItem	RecordTsCfg[2];         /* UsbDevCfg下TS录制参数配置菜单 */
struct  MenuItem	PlayTsCfg[2];           /* UsbDevCfg下TS播放参数配置菜单 */
/* *-1.6- */
struct  MenuItem	SaveCfg[1];             /* SystemCfg下保存配置菜单 */
struct  MenuItem	LoadSaveCfg[1];         /* SystemCfg下加载配置菜单 */
struct  MenuItem	FactoryRstCfg[1];       /* SystemCfg下出工配置菜单 */
struct  MenuItem	LCDTimeoutCfg[1];       /* SystemCfg下LCD背光保护参数配置菜单 */
struct  MenuItem	KeyPasswordCfg[1];      /* SystemCfg下按键锁密码配置菜单 */
struct  MenuItem	LockKeyCfg[1];          /* SystemCfg下按键锁屏使能配置菜单 */
struct  MenuItem	ProjectTestCfg[1];      /* SystemCfg下工程测试使能配置菜单 */


/*
 * *-------------------------------
 * *二级菜单定义
 * *-------------------------------
 * *-1.1.1-
 */
struct MenuItem		VideoInLock[1];         /* EncoderStatus下的视频输入锁定状态菜单 */
struct  MenuItem	VideoResolution[1];     /* EncoderStatus下的视频分辨率状态菜单 */
struct  MenuItem	HDMI1080ix50[1];
struct  MenuItem	InternelEDID_[1];
struct  MenuItem	CVBS576i480ix50_[1];
struct  MenuItem	HDMI_GR_480px60_[1];
struct  MenuItem	YPbPr_720px60_[1];
struct  MenuItem	YPbPr_1080px50_[1];
struct  MenuItem	YPbPr_1080ix50_[1];
struct  MenuItem	SD_576i_480ix50_[1];
struct  MenuItem	CP_YPbPr576ix50_[1];
struct  MenuItem	CP_YPbPr576px50_[1];
struct  MenuItem	HDMI_1080px50_[1];
struct  MenuItem	HDMI_720px60_[1];
struct  MenuItem	HDMI_576ix50_[1];
/* *-1.1.2- */
struct MenuItem		BitrateAct[1];  /* ModulatorStatus下的实际比特率状态菜单 */
struct  MenuItem	BitrateMax[1];  /* ModulatorStatus下的最大比特率状态菜单 */
/* *-1.1.3- */
struct MenuItem		UsbPlay[1];     /* UsbDevStatus下的TS流播放锁定状态菜单 */
struct  MenuItem	DiskUsage[1];   /* UsbDevStatus下U盘使用状态菜单 */
/* *-1.1.4- */
struct MenuItem DevStatus[1];           /* SystemStatus下外设工作状态菜单 */
struct MenuItem sysStatus[4];

struct  MenuItem recird_mod[1];

struct  MenuItem usbbitrat[1];

/* *-1.1.5- */
/* struct MenuItem		DevAlarm[1];    / * AlarmStatus下外设告警菜单 * / */
struct  MenuItem	VideoUnlock[1]; /* AlarmStatus下视频输入失锁告警菜单 */
struct  MenuItem	TsUnlock[1];    /* AlarmStatus下USB播放TS流失锁告警菜单 */
/* *-1.1.6- */
struct MenuItem Adv7441aStatus[1];      /* ProjectStatus下ADV7441A工程测试状态菜单 */
struct MenuItem Mb86h46Status[1];       /* ProjectStatus下MB86H46工程测试状态菜单 */
struct MenuItem Ad9789Status[1];        /* ProjectStatus下AD9789工程测试状态菜单 */
struct MenuItem Adf4350Status[1];       /* ProjectStatus下ADF4350工程测试状态菜单 */
struct MenuItem Usb3300Status[1];       /* ProjectStatus下USB3300工程测试状态菜单 */
struct MenuItem FpgaStatus[1];          /* ProjectStatus下FPGA功能模块工程测试状态菜单 */


/*
 * *-------------------------------
 * *-1.2.1-
 */
struct MenuItem VideoIntfCfg[1];        /* VideoCfg下视频输入接口配置菜单 */
struct MenuItem VideoBitrateCfg[1];     /* VideoCfg下视频编码比特率配置菜单 */
struct MenuItem RateModeCfg[1];         /* VideoCfg下视频编码率模式配置菜单 */
struct MenuItem ProfileCfg[1];          /* VideoCfg下H.264 profile配置菜单 */
struct MenuItem LevelCfg[1];            /* VideoCfg下H.264 level配置菜单 */
struct MenuItem BrightnessCfg[1];       /* VideoCfg下视频亮度配置菜单 */
struct MenuItem ContrastCfg[1];         /* VideoCfg下视频对比度配置菜单 */
struct MenuItem SaturationCfg[1];       /* VideoCfg下视频饱和度配置菜单 */
struct MenuItem HueCfg[1];              /* VideoCfg下视频色度配置菜单 */
struct MenuItem StandardCfg[1];         /* VideoCfg下视频制式配置菜单 */
/* *-1.2.2- */
struct MenuItem AudioBitrateCfg[1];     /* AudioCfg下音频编码采样率配置菜单 */
struct MenuItem AudioFormatCfg[1];      /* AudioCfg下音频格式配置菜单 */

/* *-1.2.3- */
struct MenuItem ProgOutCfg[1];          /* ProgInfoCfg下视频输入接口配置菜单 */
struct MenuItem ProgNameCfg[1];         /* ProgInfoCfg下视频编码比特率配置菜单 */
struct MenuItem ServiceNameCfg[1];      /* ProgInfoCfg下视频编码率模式配置菜单 */
struct MenuItem ProgNumCfg[1];          /* ProgInfoCfg下H.264 profile配置菜单 */
struct MenuItem PmtPidCfg[1];           /* ProgInfoCfg下H.264 level配置菜单 */
struct MenuItem PcrPidCfg[1];           /* ProgInfoCfg下视频亮度配置菜单 */
struct MenuItem VideoPidCfg[1];         /* ProgInfoCfg下视频对比度配置菜单 */
struct MenuItem AudioPidCfg[1];         /* ProgInfoCfg下视频饱和度配置菜单 */
struct MenuItem LcnCfg[1];              /* ProgInfoCfg下视频色度配置菜单 */
struct MenuItem EitEventCfg[2];         /* ProgInfoCfg下视频制式配置菜单 */
/* *-1.2.4- */
struct MenuItem EitNowCfg[4];           /* eit now时间周期等配置菜单 */
struct MenuItem EitNextCfg[4];          /*      eit next时间周期等配置菜单 */

/* *-1.2.5- */
struct MenuItem StartYearCfg[1];
struct MenuItem StartTimeCfg[1];
struct MenuItem StartDurationCfg[1];
struct MenuItem StartEventNameCfg[1];

struct MenuItem NextYearCfg[1];
struct MenuItem NextTimeCfg[1];
struct MenuItem NextDurationCfg[1];
struct MenuItem NextEventNameCfg[1];

struct MenuItem GuardIntv_1_32[1];      /* GuardIntvCfg下1/32配置菜单 */
struct MenuItem GuardIntv_1_16[1];      /* GuardIntvCfg下1/16配置菜单 */
struct MenuItem GuardIntv_1_8[1];       /* GuardIntvCfg下1/8配置菜单 */
struct MenuItem GuardIntv_1_4[1];       /* GuardIntvCfg下1/4配置菜单 */


struct MenuItem RFLevelEdit[1];         /* RFLevelCfg下射频电平编辑菜单 */
/* *-1.3.8- */
struct MenuItem RFOutSel[1];            /* RFOutCfg下射频输出选择菜单 */


/*
 * *-------------------------------
 * *-1.4.1-
 */
struct MenuItem TsidEdit[1];            /* TsidCfg下TS ID编辑菜单 */
/* *-1.4.2- */
struct MenuItem OnidEdit[1];            /* OnidCfg下ON ID编辑菜单 */
/* *-1.4.3- */
struct MenuItem NetworkIdCfg[1];        /* NitCfg下网络ID配置菜单 */
struct MenuItem NetworkNameCfg[1];      /* NitCfg下网络name配置菜单 */
struct MenuItem VersionMode[2];         /* NitCfg下版本模式配置菜单 */
struct MenuItem VersionNumberCfg[1];    /* NitCfg下版本号配置菜单 */
struct MenuItem LcnMode[1];             /* NitCfg下LCN模式配置菜单 */
struct MenuItem PrivateDataCfg[1];      /* NitCfg下私有数据配置菜单 */
struct MenuItem NitInsert[1];           /* NitCfg下NIT插入使能配置菜单 */
/* *-1.4.4- */
struct MenuItem LanguageCfg[1];         /* EitCfg下语言编码配置菜单 */
struct MenuItem EitInsert[1];           /* EitCfg下EIT插入配置菜单 */


/*
 * *-------------------------------
 * *-1.5.1-
 */
struct MenuItem StartRecord[1];         /* RecordTsCfg下启动码流录制菜单 */
struct MenuItem AdvancedCfg[4];         /* RecordTsCfg下录制参数配置菜单 */
/* *-1.5.2- */
struct MenuItem FileBrowse[999];        /* PlayTsCfg下文件浏览菜单 */
struct MenuItem PlayMode[1];            /* PlayTsCfg下播放模式配置菜单 */


/*
 * *-------------------------------
 * *-1.6.1-
 */
struct MenuItem SaveEnable[1];          /* SaveCfg下保存使能菜单 */
/* *-1.6.2- */
struct MenuItem LoadSaveEable[1];       /* LoadSaveCfg下加载保存使能菜单 */
/* *-1.6.3- */
struct MenuItem FactoryRstEnable[1];    /* FactoryRstCfg下恢复出厂配置使能菜单 */

/* *-1.6.5- */
struct MenuItem KeyPasswordEdit[1];     /* KeyPasswordCfg下按键密码编辑菜单 */
/* *-1.6.6- */
struct MenuItem LockKeyEnable[1];       /* LockKeyCfg下按键锁使能菜单 */
/* *-1.6.7- */
struct MenuItem ProjectTestEnable[1];   /* ProjectTestCfg下按键锁使能菜单 */


/* 写文件缓存 */
struct cache_t	cache_null[1];
struct cache_t	ts_file_name[1];
struct cache_t	filesize[1];

/* *------------------------------- */

#endif


