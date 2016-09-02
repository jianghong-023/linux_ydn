#ifndef __DEFINEIDENT__H_

#define __DEFINEIDENT__H_
#include <stdint.h>

/* 视频输入是否锁定 */
#define  VIDEO_INPUT_LOCK	(0x01)
#define  VIDEO_INPUT_UNLOCK	(0x02)

/* 视频频率标示 */
#define YPbPr1080Isign60Hz	(0x01)
#define YPbPr1080Isign50Hz	(0x02)
#define YPbPr720Psign50Hz	(0x03)
#define YPbPr720Psign60Hz	(0x04)

/* 视频逐行，非逐行扫描 */
#define NTSC_60_ROW	(0x08)
#define NTSC_60_NOTROW	(0x01)
#define NTSC_50_NOTROW	(0x02)
#define NTSC_50_ROW	(0x03)

#define PAL_60_ROW	(0x04)
#define PAL_60_NOTROW	(0x05)
#define PAL_50_NOTROW	(0x06)
#define PAL_50_ROW	(0x07)


#define VIDEO_LOCK (0X01)
#define VIDEO_UNLOCK (0X02)


/* 系统配置 */
#define STRINGS			"freemem.sh"
#define RULER			"/usr/sbin/"STRINGS
#define RULER_STRING		RULER
#define RULER_STRING_ARG	"/usr/sbin/freemem.sh 2"
#define SYS_CONF		"/usr/sbin/sysconfig.sh"
#define SYS_ETC_CONF		"/etc/dvb.conf"
#define RES_ETC_CONF		"/tmp/reset.sh"
#define LIMITMEM		(80000)

/* usb file */
#define TSFILENAME	"tsfiledefault"
#define TSFILESIZE	(512)
#define DEFAULT		"default"
#define MAX_BUF		(4096)
#define NOT		(0xFF)
#define CONFFILELEN	(11)
#define M_HDMI		(0x00)
#define M_CVBS		(0x01)
#define M_YPbPr		(0x02)

/* usb */

#define  ADD		(0)
#define  REMOVE		(1)
#define  USBWRITESET	(0x00)
#define  USBWRITESTART	(0x01)
#define  USBWRITECTL	(0x02)


#define FILE_DEV_CHECK		"/proc/scsi/scsi"                       /* 用来检测设备数和类型 */
#define FILE_MOUNT_CHECK	"/proc/mounts"                          /* 用来检测设备是否被mount */
#define FILE_DISC_PARTS_CHECK	"/proc/partitions"                      /* 用来检测设备的分区情况 */
#define FILE_DEV_PART_TEMPL	"/dev/scsi/host%d/bus0/target0/lun0/"   /* 具体的设备 */
#define USB_CDROM_MP		"/tmp/cdrom"
#define USB_DISK_MP		"/tmp/usbdisk"
#define MAX_NAME_LEN		(100)
#define MAX_PART_NUM		(6)                                     /* 最多运行6个分区 */
#define KK_FS_OK		(1)
#define KK_FS_FAIL		(0)
#define TRUE			(1)
#define FALSE			(0)
#define DEVROMV			(0)
#define DEVACTT			(1)
#define OTHER			(3)

#define DELAY_ON		(5)
#define DELAY_OFF			(6)

#define  STORE_FILE	(1)
#define  PALAY_FILE	(2)

#define  FILE_Y (1)
#define  FILE_N (0)

#define DTS_STREAM_TIME (37000)

/*
 * 带宽
 * 00->8MHz,01->7MHz,10->6MH
 */
#define B_8MHz (0x00)


/*
 * 帧保护间隔
 * 00->1/32,01->1/16,10->1/8,11->1/4
 */
#define F_DELTAL_32_1	(0x00)
#define F_DELTAL_16_1	(0x01)
#define F_DELTAL_8_1	(0x02)
#define F_DELTAL_4_1	(0x03)


/*
 * 调制器FFT模式
 * 0->FFT 2K;1->FFT 8K;
 */
#define FFT_2K	(0x00)
#define FFT_8K	(0x01)


/*
 * 调制器星座
 * Modulate mode:000->QPSK,001->16QAM,010->64QAM;default->64QAM
 */
#define MODULATE_M_QPSK		(0x00)
#define MODULATE_M_16QAM	(0x01)
#define MODULATE_M_64QAM	(0x02)


/*
 * 调制器编码码率
 * Encode rate:000->1/2,001->2/3,010->3/4,011->5/6,100->7/8;default->100
 */
#define ENCODE_RATE_2_1 (0x00)
#define ENCODE_RATE_3_2 (0x01)
#define ENCODE_RATE_4_3 (0x02)
#define ENCODE_RATE_6_5 (0x03)
#define ENCODE_RATE_8_7 (0x04)

/* 总线复位 */
#define SYS_BUS_SET (0x01)

/* add sub */
#define UP_ADD		(0x01)
#define DOWN_SUB	(0x02)
#define UPDOWN_DEF	(0x00)

#define LIFT_SUB	(0x01)
#define RIGHT_ADD	(0x02)
#define LIFTRIGHT_DEF	(0x00)

#define CURSOR_ON	(0x01)
#define CURSOR_OFF	(0x02)
#define CURSOR_DEF	(0x00)
#define CURSOR_NOUSER	(0x03)


/*
 * ���ַ�����ʱ��lcdѭ��/��ѭ����ʾ ����
 */
#define LCD_LOOP_ON	(0x1)
#define LCD_LOOP_OFF	(0x0)


/*  */

#define MEMDEV_IOC_MAGIC 'G'
#define LCD_CHAR_INPUT \
	_IO( MEMDEV_IOC_MAGIC, 0x61 )

#define  CHAR_S		('s')
#define  CHANGE_G	(0x30)
#define  CHANGE_N	('N')

#define  START_R	(0x01)
#define  STARTSTOP	(0x02)
#define  USB_READ	(0x40)
#define  USB_WRITE	(0x50)

#define  NORMAL_SIG (0x60)

#define CHAR_INPUT_ON	(0x26)
#define READWRITEOFF	(1)

#define HDMI_M	"HDMI_M"
#define CVBS_M	"CVBS_M"
#define YPbPr_M "YPbPr_M"


/*  */
#define ADV7842ADDR	(0x20)
#define I2C_NR_1	(1)
#define CY22393ADDR	(0x69)
#define I2C_NR_0	(0)

/* 文件及字符信号 */
#define IDENT_CH	('A')
#define MAX_FILE_LIMIT	(10)
#define DIG_SZIE	(1)
#define SIGL_C		(2)
#define sig_dig_error	(-1)
#define SIG_FLAG	(8)
#define DIG_FLAG	(7)
#define SIG_STOP	(9)


/*
 * 外设地址及范围
 * #define MAP_BASE_ADDR       (0xFF200000)//0xFF230000
 */
#define MAP_BASE_ADDR (0xFF230000)
/* #define MAP_SIZE            (0x100) */
#define MAP_SIZE	(0x10000)
#define BUS_OFFSET_ADDR (0x00000000)
/* #define BUS_OFFSET_ADDR       (0x00000000) */


/* 共享内存 */
#define MEMSIZE_4M	(0x400160) //0x2000B0 by 2016-8-17 jh
#define MEMSIZE_2M	(0x2000B0) //0x2000B0 by 2016-8-17 jh
#define MEMSTARTADD_2	(0x1400000)     /* 100MB */
#define MISCMODRST	(0x20)
#define RSTMGR		(0XFFD05000)
#define FRISTSIZE	(sizeof(uint8_t) * 5578 * 188)
#define __DEBUG__


#define MEMSTARTADD	(0x1400000)     /* (0x6400000) // 100MB */
#define MISCMODRST	(0x20)
#define RSTMGR		(0XFFD05000)
#define size_rstmgr	(32 + MISCMODRST)

#define BUS_INIT_BASE (0x00000000)

#define ERR		(-1)
#define TOTA_CAPACITY	(0)
#define USED_CAPACITY	(1)
#define FREE_CAPACITY	(2)

/* gpio */
#define CVBS		(0x08)  /* YPrPb/CVBS input */
#define HDMI		(0x10)  /* G2=1,G1=0	//HDMI input */
#define nRST_H46_LO	(0x00)
#define nRST_H46_HI	(0x01)


/* 定时器 */
#define TMIER_OUT (800000 * 9)

/* lcd */
#define  LCD1602MaxDisplayLine	(2)
#define   ENTER_KEY		(0x01)
#define  ENTER_SET		(0X00)
#define  WRITELIMIT		(0x02)          /* lcd 仅数字输入 */
#define  DIGLIMIT		(6)             /* 可输入最大位数 */
#define  MAXFILESIZE		(106496)        /* usb 最大存储值 */
#define  LCD_LIGHT_ON		(0x01)
#define  LCD_LIGHT_OF		(0x00)
#define  LCD_TIME		(10)
#define  LCD_TIME_M		"t10s"

#define  NOTPWD (0)
#define  YESPWD (1)

/* 字符输入，数字输入 */
#define  CHAR_INPUT	(0x01)
#define  DIGT_INPUT	(0x02)

/* lcd */
#define KEYMAXSZIE	(sizeof(scan_code) / sizeof(char) )     /* 可选择最大长度 */
#define MAXWIDE		(16)                                    /* lcd屏宽 */
#define TMIER_LIMT	(800000)                                /* 自动选择默认定时 */
#define DEFAULTE	(0)                                     /* 默认值 */
#define DEFAULTELINE	(2)                                     /* lcd屏行 */
#define DEFAULTKEY	(0xFF)                                  /* key 默认值 */
#define TMPBUFFETTSIZE	(40)                                    /* 临时缓存大小 */
#define USB_READ_WRITE	(1)

#define LCD_CHAR_ARR \
	_IO( MEMDEV_IOC_MAGIC, 0x62 )
#define LCD_CHAR_DOT \
	_IO( MEMDEV_IOC_MAGIC, 0x63 )
#define LCD_SUROS_ONOFF \
	_IO( MEMDEV_IOC_MAGIC, 0x64 ) /* 0x00 off  0x0x on */
#define LCD_SUROS_MOVE \
	_IO( MEMDEV_IOC_MAGIC, 0x60 )



#define  ACSSECC_PATH "/project/pass.dat"

/* use read */
#define F_R_TIME	(0)
#define N_R_TIME	(1)

/* passwrd init */
#define INIT_PWRD	"******"
#define LOCK_ON		0x02
#define LOCK_OFF	0x01
#define STOP_DSP	0x03

/* usb write */
#define FAIL (0)

#define  ENABLE_W1	(1)
#define  ENABLE_W2	(1)

#define  DSENABLE_W1	(0)
#define  DSENABLE_W2	(0)
#define  UN_LIMIT	(30000)
#define  DEV		(1)
#define  DEV2		(2)
#define  MU_C		(1)

#define ENCODE_IDENT_LOCK	(0x01)
#define ENCODE_IDENT_RES	(0x02)
#define ENCODE_IDENT_UNKONOWN	(0x00)


/* h.264 */
#define DEFAULT_CBR	(0x0030)
#define VIDEO_BITRATE	(8000)

#define STREAM_TSID	(1)
#define STREAM_ONID	(1)

#define CYCOFPAT_MS	(90)
#define CYCOFPMT_MS	(90)
#define CYCOFSIT_MS	(900)
#define CYCOFPCR_MS	(30)

#define PROFILEDELETEC_P	(0x0000)
#define PROFILE_STR		"DEFAULT_CFG"

#define LEVEL_		(0x0000)
#define LEVEL_NAME	"Leveldefu"

#define VIDEO_BRIGHT		(128)
#define VIDEO_CONTRAST		(128)
#define VIDEO_SATURATION	(128)
#define VIDEO_HUE		(128)

#define VIDEO_CP_BRIGHT		(127)
#define VIDEO_CP_CONTRAST	(127)
#define VIDEO_CP_SATURATION	(127)
#define VIDEO_CP_HUE		(127)

#define VIDEO_NORM	(100)
#define VIDEO_NORM_NAME "AUTO"
#define AUDIO_RATE	(0x40)

#define AUDIO_FORMAE		(0x00)
#define AUDIO_FORMAE_NAME	(0x00)


/*
 * status
 * 0x01 Ϊ��ֵ״̬��0x02Ϊ�ַ�״̬,
 * 0x03Ϊ�˵����� ,0x04 usb��д,0x05 ��Ƶ��״̬ ,0x06 ������״̬
 */
#define USB_SINGLE	(0x01)
#define USB_SEGMENT	(0x10)
#define USB_LOOP	(0x11)
#define DIGIT_STATUS	0x01
#define CHAR_STATUS	0x02
#define MENU_STATUS	0x03
#define USB_STATUS	0x04
#define VIDEO_STATUS	0x05
#define PWRD_STATUS	0x06
#define USB_AUTO_HAND	(0x10)
#define USB_OFF_HAND	(0x20)

#define USB_SIG			"SINGALMENT"
#define USB_SIG_LOOP		"SINGALLOOP"
#define USB_SIG_PLAY_ALL	"PLAYALL"

#define USB_RECORD_SIG_MOD	"RECORDSIG"
#define USB_RECORD_SEG_MOD	"RECORDSEGMENT"
#define USB_RECORD_LOOP_MOD	"RECORDLOOP"
#define USB_RECORD_AUTO_MOD	"RECORDAUTO"

#define USB_RECORD_SIG	(0x20)
#define UAB_RECORD_SEG	(0x30)
#define USB_RECORD_LOOP (0x40)

/* ���������� */
#define INPUT_DIGIT_STATUS	(0x02)
#define INPUT_FLOAT_STATUS	(0x01)
/*  */
#define KEY_AFFRIM_ENTER_Y	(0x01)
#define KEY_AFFRIM_ENTER_N	(0x02)

#define LOCKKEY_DOWN_F	(0x01)          /* ��һ�� */
#define LOCKKEY_WOWN_S	(0x02)          /* ���� */
#define LOCKKEY_WOWN_T	(0x03)          /* ���� */


/* table */
#define PACK_SIZE_NOR	(188)
#define PACK_SIZE_OTH	(204)
#define TABLE_NAME	"/etc/table.dat"

/* nit */
#define USER_DEFINE_DESC_TAG	(0x83)
#define  NIT_CURRENT_NEXT	(1)
#define NETWORK_NAME_TAG	(0x40)
#define NIT_EXTENSION		(1)

/* sdt */
#define SDT_DESCRIPTOR_TAG	(0x48)
#define SDT_FREE_CA		(0)
#define SDT_EIT_SCHEDULE	(0)
#define SDT_EXTENSION		(0x0001)
#define SDT_RUNNING_STATUS	(0x04)
#define SDT_EIT_PRESENT		(0)
#define SDT_CURRENT_NEXT	(1)
#define NETWORK_ID		(0x0001)
#define SDT_TABLE_ID		(0x42)
#define SDT_VERSION		(30)

#define SDT_STOP_START_BIT	(0x81)
#define SDT_OFFSET		(0x400)
#define SDT_STOP		(0x0)
#define SDT_START		(0x1)


/* pmt */
#define VIDEO_TYPE		(0x1B)
#define AUDIO_TYPE		(0x03)
#define PMT_CURRENT_NEXT	(1)
#define PMT_VERSION		(3)

#define PMT_STOP_START_BIT	(0x80)
#define PMT_OFFSET		(0x200)
#define PMT_STOP		(0x0)
#define PMT_START		(0x1)

/* pat */
#define MAX_NUM_PROGRAM		(1)
#define PAT_CURRENT_NEXT	(1)
#define PAT_PID			(0x0064)
#define PAT_VAERSION		(30)
#define TS_ID			(0x0001)

#define PAT_STOP_START_BIT	(0x7f)
#define PAT_OFFSET		(0x100)
#define PAT_STOP		(0x0)
#define PAT_START		(0x1)

/* nit */
#define CODE_RAT_LP_STREAM	(0)
#define NIT_TABLE_ID		(0x40)
#define NIT_VERSION		(31)
#define NIT_STOP_START_BIT	(0x82)
#define NIT_OFFSET		(0x800)
#define NIT_STOP		(0x0)
#define NIT_START		(0x1)
#define NIT_LCN_MOD		"European_Cfg"
#define LCN_MOD_VALUE		(0x02)

/* eit */
#define EIT_STOP_START_BIT	(0x83)
#define EIT_OFFSET		(0x1000)
#define EIT_ENABLE		"Enable"
#define EIT_DESENABLE		"Denable"
#define NIT_DESENABLE  EIT_DESENABLE
#define NIT_ENABLE 	EIT_ENABLE

#define PROGRAM_OUT  EIT_ENABLE

#define ENABLE		"Enable"
#define DESENABLE	"Denable"


#define NEXT_YEAR	"20130101"              /* mod */
#define NEXT_TMIE	"010000"                /* mod */
#define NEXT_DURATION	"003000"                /* mod */
#define NEXT_EVNETNAME	"eng"
#define NEXT_EVNETNAME2 "program2"              /* mod */

#define START_YEAR		"20130101"      /* mod */
#define START_TMIE		"000000"        /* mod */
#define START_DURATION		"003000"        /* mod */
#define START_EVNETNAME		"eng"
#define START_EVNETNAME2	"program1"      /* mod */

#define PRIVATE_DATE "01fc6500"

/* dvt */
#define DVB_DESCRIPTOR_TAG	(0x5a)
#define DVB_SCRIPTOR_LENGHT	(11)


/* table op */
#define NIT_TABLE	(0x010)
#define SDT_TABLE	(0x011)
#define PAT_TABLE	(0x012)
#define PMT_TABLE	(0x013)
#define EIT_TABLE	(0x014)


/* parse */


#define FILTERMODE	(0x00)
#define PID		(0x00)
#define PIDE		(0x00)
#define START		(0x00)

#define PARE_ADDR_0	(0x9000)
#define PARE_ADDR_1	(0x9001)
#define PARE_ADDR_2	(0x9002)
#define PARE_ADDR_3	(0x9003)
#define PARE_ADDR_4	(0x9004)
#define PARE_ADDR_5	(0x9005)
#define PARE_ADDR_6	(0x9006)
#define PARE_ADDR_7	(0x9007)
#define PARE_ADDR_8	(0x9008)
#define PARE_ADDR_9	(0x9009)

/* notify error */
#define ERR_PMT (0x01)
#define ERR_PAT (0x02)
#define ERR_SDT (0x03)
#define ERR_NIT (0x04)
#define PAR_SUC (0x05)


/* signal set */

#define MONITOER_TRUE	1
#define MONITOER_FALSE	0
#define MONITOER_STAGE	3

#define ENCODE_BITRTE_START	0x00000010
#define ENCODE_BITRTE_STOP	0x00000011
#define ENCODE_BITRTE_FLAG	0x00000012

#define SYS_RUN_TIME_START	0x00000020
#define SYS_RUN_TIME_STOP	0x00000021
#define SYS_RUN_TIME_FLAG	0x00000022

#define DEVICE_SER_START	0x00000030
#define DEVICE_SER_STOP		0x00000031
#define DEVICE_SER_FLAG		0x00000032

#define DEVICE_NUM_START	0x00000040
#define DEVICE_NUM_STOP		0x00000041
#define DEVICE_NUM_FLAG		0x00000042

#define ENCODE_FORMAT_START	0x00000050
#define ENCODE_FORMAT_STOP	0x00000051
#define ENCODE_FORMAT_FLAG	0x00000052

#define USER_PWD_START	0x00000060
#define USER_PWD_STOP	0x00000061
#define USER_PWD_FLAG	0x00000062

#define NORMAL_DESPLAY_START	0x00000070
#define NORMAL_DESPLAY_STOP	0x00000071
#define NORMAL_DESPLAY_FLAG	0x00000072

#define SIGNAL_NONE	0x00000000
#define NONE		SIGNAL_NONE

#define USB_WRITE_SIGNAL_START	0x00000080
#define USB_WRITE_SIGNAL_STOP	0x00000081
#define USB_WRITE_SIGNAL_FLAG	0x00000082

#define USB_READ_SIGNAL_START	0x00000090
#define USB_READ_SIGNAL_STOP	0x00000091
#define USB_READ_SIGNAL_FLAG	0x00000092

#define SYS_RESTART   (0x00000100)
#define SYS_CLEAN   (0x00000101)


#define SYS_SET_TIME "2016-6-8 00:00:00"

#endif
