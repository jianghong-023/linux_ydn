#ifndef __MENU_SET__H
#define __MENU_SET__H

struct MenuItem MainMenu[6];


/*
 * *-------------------------------
 * *���˵�����
 * *-------------------------------
 */

struct MenuItem Status[3];                      /* MainMenu��״̬�˵� */
struct MenuItem EncoderCfg[3];                  /* MainMenu�±������������ò˵� */
struct MenuItem ModulatorCfg[8];                /* MainMenu�µ������������ò˵� */
struct MenuItem StreamCfg[4];                   /* MainMenu��TS���������ò˵� */
struct MenuItem UsbDevCfg[3];                   /* MainMenu��USB�豸�������ò˵� */
struct MenuItem SystemCfg[4];                   /* MainMenu��ϵͳ���ò˵� */


/*
 * *-------------------------------
 * *һ���˵�����
 * *-------------------------------
 * *-1.1-
 */
struct  MenuItem	EncoderStatus[2];       /* Status�±�����״̬�˵� */
struct  MenuItem	ModulatorStatus[1];     /* Status�µ�����״̬�˵� */
struct  MenuItem	UsbDevStatus[3];        /* Status��USB�豸״̬�˵� */
struct  MenuItem	SystemStatus[2];        /* Status��ϵͳ״̬�˵� */
struct  MenuItem	AlarmStatus[3];         /* Status�¸澯״̬�˵� */

/* *-1.2- */
struct  MenuItem	VideoCfg[10];           /* EncoderCfg����Ƶ���ò˵� */
struct  MenuItem	AudioCfg [2];           /* EncoderCfg����Ƶ���ò˵� */
struct  MenuItem	ProgInfoCfg[10];        /* EncoderCfg�½�Ŀ��Ϣ���ò˵� */
/* *-1.3- */
struct  MenuItem	BandwidthCfg[1];        /* ModulatorCfg�´���������ò˵� */
struct  MenuItem	ConstellCfg [1];        /* ModulatorCfg�������������ò˵� */
struct  MenuItem	FFTCfg[1];              /* ModulatorCfg��FFT�������ò˵� */
struct  MenuItem	GuardIntvCfg[1];        /* ModulatorCfg�±�������������ò˵� */
struct  MenuItem	CoderateCfg[1];         /* ModulatorCfg�±������ʲ������ò˵� */
struct  MenuItem	RFFreqCfg[1];           /* ModulatorCfg����ƵƵ�ʲ������ò˵� */
struct  MenuItem	RFLevelCfg[1];          /* ModulatorCfg����Ƶ��ƽ�������ò˵� */
struct  MenuItem	RFOutCfg[1];            /* ModulatorCfg����Ƶ�������ò˵� */
/* struct  MenuItem	Bitrate[1]; */
/* *-1.4- */
struct  MenuItem	TsidCfg[1];             /* StreamCfg��TS ID�������ò˵� */
struct  MenuItem	OnidCfg[1];             /* StreamCfg��ON ID�������ò˵� */
struct  MenuItem	NitCfg[7];              /* StreamCfg��NIT�������ò˵� */
struct  MenuItem	EitCfg[2];              /* StreamCfg��EIT�������ò˵� */
/* *-1.5- */
struct  MenuItem	RecordTsCfg[2];         /* UsbDevCfg��TS¼�Ʋ������ò˵� */
struct  MenuItem	PlayTsCfg[2];           /* UsbDevCfg��TS���Ų������ò˵� */
/* *-1.6- */
struct  MenuItem	SaveCfg[1];             /* SystemCfg�±������ò˵� */
struct  MenuItem	LoadSaveCfg[1];         /* SystemCfg�¼������ò˵� */
struct  MenuItem	FactoryRstCfg[1];       /* SystemCfg�³������ò˵� */
struct  MenuItem	LCDTimeoutCfg[1];       /* SystemCfg��LCD���Ᵽ���������ò˵� */
struct  MenuItem	KeyPasswordCfg[1];      /* SystemCfg�°������������ò˵� */
struct  MenuItem	LockKeyCfg[1];          /* SystemCfg�°�������ʹ�����ò˵� */
struct  MenuItem	ProjectTestCfg[1];      /* SystemCfg�¹��̲���ʹ�����ò˵� */


/*
 * *-------------------------------
 * *�����˵�����
 * *-------------------------------
 * *-1.1.1-
 */
struct MenuItem		VideoInLock[1];         /* EncoderStatus�µ���Ƶ��������״̬�˵� */
struct  MenuItem	VideoResolution[1];     /* EncoderStatus�µ���Ƶ�ֱ���״̬�˵� */
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
struct MenuItem		BitrateAct[1];  /* ModulatorStatus�µ�ʵ�ʱ�����״̬�˵� */
struct  MenuItem	BitrateMax[1];  /* ModulatorStatus�µ���������״̬�˵� */
/* *-1.1.3- */
struct MenuItem		UsbPlay[1];     /* UsbDevStatus�µ�TS����������״̬�˵� */
struct  MenuItem	DiskUsage[1];   /* UsbDevStatus��U��ʹ��״̬�˵� */
/* *-1.1.4- */
struct MenuItem DevStatus[1];           /* SystemStatus�����蹤��״̬�˵� */
struct MenuItem sysStatus[4];

struct  MenuItem recird_mod[1];

struct  MenuItem usbbitrat[1];

/* *-1.1.5- */
/* struct MenuItem		DevAlarm[1];    / * AlarmStatus������澯�˵� * / */
struct  MenuItem	VideoUnlock[1]; /* AlarmStatus����Ƶ����ʧ���澯�˵� */
struct  MenuItem	TsUnlock[1];    /* AlarmStatus��USB����TS��ʧ���澯�˵� */
/* *-1.1.6- */
struct MenuItem Adv7441aStatus[1];      /* ProjectStatus��ADV7441A���̲���״̬�˵� */
struct MenuItem Mb86h46Status[1];       /* ProjectStatus��MB86H46���̲���״̬�˵� */
struct MenuItem Ad9789Status[1];        /* ProjectStatus��AD9789���̲���״̬�˵� */
struct MenuItem Adf4350Status[1];       /* ProjectStatus��ADF4350���̲���״̬�˵� */
struct MenuItem Usb3300Status[1];       /* ProjectStatus��USB3300���̲���״̬�˵� */
struct MenuItem FpgaStatus[1];          /* ProjectStatus��FPGA����ģ�鹤�̲���״̬�˵� */


/*
 * *-------------------------------
 * *-1.2.1-
 */
struct MenuItem VideoIntfCfg[1];        /* VideoCfg����Ƶ����ӿ����ò˵� */
struct MenuItem VideoBitrateCfg[1];     /* VideoCfg����Ƶ������������ò˵� */
struct MenuItem RateModeCfg[1];         /* VideoCfg����Ƶ������ģʽ���ò˵� */
struct MenuItem ProfileCfg[1];          /* VideoCfg��H.264 profile���ò˵� */
struct MenuItem LevelCfg[1];            /* VideoCfg��H.264 level���ò˵� */
struct MenuItem BrightnessCfg[1];       /* VideoCfg����Ƶ�������ò˵� */
struct MenuItem ContrastCfg[1];         /* VideoCfg����Ƶ�Աȶ����ò˵� */
struct MenuItem SaturationCfg[1];       /* VideoCfg����Ƶ���Ͷ����ò˵� */
struct MenuItem HueCfg[1];              /* VideoCfg����Ƶɫ�����ò˵� */
struct MenuItem StandardCfg[1];         /* VideoCfg����Ƶ��ʽ���ò˵� */
/* *-1.2.2- */
struct MenuItem AudioBitrateCfg[1];     /* AudioCfg����Ƶ������������ò˵� */
struct MenuItem AudioFormatCfg[1];      /* AudioCfg����Ƶ��ʽ���ò˵� */

/* *-1.2.3- */
struct MenuItem ProgOutCfg[1];          /* ProgInfoCfg����Ƶ����ӿ����ò˵� */
struct MenuItem ProgNameCfg[1];         /* ProgInfoCfg����Ƶ������������ò˵� */
struct MenuItem ServiceNameCfg[1];      /* ProgInfoCfg����Ƶ������ģʽ���ò˵� */
struct MenuItem ProgNumCfg[1];          /* ProgInfoCfg��H.264 profile���ò˵� */
struct MenuItem PmtPidCfg[1];           /* ProgInfoCfg��H.264 level���ò˵� */
struct MenuItem PcrPidCfg[1];           /* ProgInfoCfg����Ƶ�������ò˵� */
struct MenuItem VideoPidCfg[1];         /* ProgInfoCfg����Ƶ�Աȶ����ò˵� */
struct MenuItem AudioPidCfg[1];         /* ProgInfoCfg����Ƶ���Ͷ����ò˵� */
struct MenuItem LcnCfg[1];              /* ProgInfoCfg����Ƶɫ�����ò˵� */
struct MenuItem EitEventCfg[2];         /* ProgInfoCfg����Ƶ��ʽ���ò˵� */
/* *-1.2.4- */
struct MenuItem EitNowCfg[4];           /* eit nowʱ�����ڵ����ò˵� */
struct MenuItem EitNextCfg[4];          /*      eit nextʱ�����ڵ����ò˵� */

/* *-1.2.5- */
struct MenuItem StartYearCfg[1];
struct MenuItem StartTimeCfg[1];
struct MenuItem StartDurationCfg[1];
struct MenuItem StartEventNameCfg[1];

struct MenuItem NextYearCfg[1];
struct MenuItem NextTimeCfg[1];
struct MenuItem NextDurationCfg[1];
struct MenuItem NextEventNameCfg[1];

struct MenuItem GuardIntv_1_32[1];      /* GuardIntvCfg��1/32���ò˵� */
struct MenuItem GuardIntv_1_16[1];      /* GuardIntvCfg��1/16���ò˵� */
struct MenuItem GuardIntv_1_8[1];       /* GuardIntvCfg��1/8���ò˵� */
struct MenuItem GuardIntv_1_4[1];       /* GuardIntvCfg��1/4���ò˵� */


struct MenuItem RFLevelEdit[1];         /* RFLevelCfg����Ƶ��ƽ�༭�˵� */
/* *-1.3.8- */
struct MenuItem RFOutSel[1];            /* RFOutCfg����Ƶ���ѡ��˵� */


/*
 * *-------------------------------
 * *-1.4.1-
 */
struct MenuItem TsidEdit[1];            /* TsidCfg��TS ID�༭�˵� */
/* *-1.4.2- */
struct MenuItem OnidEdit[1];            /* OnidCfg��ON ID�༭�˵� */
/* *-1.4.3- */
struct MenuItem NetworkIdCfg[1];        /* NitCfg������ID���ò˵� */
struct MenuItem NetworkNameCfg[1];      /* NitCfg������name���ò˵� */
struct MenuItem VersionMode[2];         /* NitCfg�°汾ģʽ���ò˵� */
struct MenuItem VersionNumberCfg[1];    /* NitCfg�°汾�����ò˵� */
struct MenuItem LcnMode[1];             /* NitCfg��LCNģʽ���ò˵� */
struct MenuItem PrivateDataCfg[1];      /* NitCfg��˽���������ò˵� */
struct MenuItem NitInsert[1];           /* NitCfg��NIT����ʹ�����ò˵� */
/* *-1.4.4- */
struct MenuItem LanguageCfg[1];         /* EitCfg�����Ա������ò˵� */
struct MenuItem EitInsert[1];           /* EitCfg��EIT�������ò˵� */


/*
 * *-------------------------------
 * *-1.5.1-
 */
struct MenuItem StartRecord[1];         /* RecordTsCfg����������¼�Ʋ˵� */
struct MenuItem AdvancedCfg[4];         /* RecordTsCfg��¼�Ʋ������ò˵� */
/* *-1.5.2- */
struct MenuItem FileBrowse[999];        /* PlayTsCfg���ļ�����˵� */
struct MenuItem PlayMode[1];            /* PlayTsCfg�²���ģʽ���ò˵� */


/*
 * *-------------------------------
 * *-1.6.1-
 */
struct MenuItem SaveEnable[1];          /* SaveCfg�±���ʹ�ܲ˵� */
/* *-1.6.2- */
struct MenuItem LoadSaveEable[1];       /* LoadSaveCfg�¼��ر���ʹ�ܲ˵� */
/* *-1.6.3- */
struct MenuItem FactoryRstEnable[1];    /* FactoryRstCfg�»ָ���������ʹ�ܲ˵� */

/* *-1.6.5- */
struct MenuItem KeyPasswordEdit[1];     /* KeyPasswordCfg�°�������༭�˵� */
/* *-1.6.6- */
struct MenuItem LockKeyEnable[1];       /* LockKeyCfg�°�����ʹ�ܲ˵� */
/* *-1.6.7- */
struct MenuItem ProjectTestEnable[1];   /* ProjectTestCfg�°�����ʹ�ܲ˵� */


/* д�ļ����� */
struct cache_t	cache_null[1];
struct cache_t	ts_file_name[1];
struct cache_t	filesize[1];

/* *------------------------------- */

#endif


