#ifndef __INIT_CONFIGURATION__H
#define __INIT_CONFIGURATION__H

#include <unistd.h>
#include <stdint.h>

#include "freememscript.h"
#include "common.h"
#include "math_instance.h"
#include "dev-info.h"
#include "defineident.h"


typedef enum {
	oBadOption,
	oDaemon,
	oProgramName,
	oServiceName,
	oNITLCNMOD,
	oProgramOutput,
	oVideoNorm,
	oStreamTSID,
	oStreamONID,
	oTsFileName,
	oTsFileSize,
	oModcode,
	oAuthServSSLPort,
	oLockPwdStu,

	oRateMode,
	oVideoBitrate,

	oProfileConfig,
	oLevelConfig,
	oBrightness,
	oContrast,
	oSaturation,
	oVideohue,

	oCpBrightness,
	oCpContrast,
	oCpSaturation,
	oCpVideohue,

	oAudioBitrate,
	oProgrNmu,
	oWriteRecord,

	oEncodeRate,
	oModulelateMode,
	oFftMode,
	oDeltal,
	oChannelWidth,

	oAd9789config,
	oRFLevel,
	oLcdTime,
	oPlayMode,
	oRecordModle,
	oPMTPID,
	oPRCPID,
	oVideoPID,
	oAudioPID,

	oNETWORKID,
	oNetWorkName,
	oNETWORKNUMB,
	oEITISERTENABLE,
	oRFENABLE,
	oAudioFormat,
} OpCodes;


/* 设备初始化配置结构体 */
typedef struct {
	/* system script */
	char script_configfile[60];
	/* key passwrd status */
	uint8_t		key_statuus; /* 0x01 开启，0x02 锁定 */
	uint8_t		lock_enter;
	uint32_t	daemon;

	pthread_mutex_t gpiolock;
	pthread_mutex_t set_gpiolock;
	pthread_mutex_t set_usb_glob_info_lock;

	/* 码率，带宽，星座等设置 */
	bus_init	configParam;
	localStatus	localstatus;
	scfgParam	scfg_Param;
	_passwrd	p_wrd_t;
} s_config;

/* 配置存储 */
typedef struct {
	char	* m_p_encode_name;
	short	m_encode_val;
} cgf_stor_data;

typedef struct cfgt {
	unsigned long	flags;
	uint8_t		rf_power;
	uint8_t		FSC_H;
	uint8_t		FSC_L;
}cfgt_t;

typedef struct {
	char	* optmenu;
	char	* cfgmenu;
}menu_cfg_t;

cgf_stor_data *cgf_data;

pid_t safe_fork( void );


s_config * get_profile( void );


s_config *config_get_config( void );


cgf_stor_data* get_stor_data( void );


void config_init( void );


int config_read( const char *filename );


int config_set_config( char *filename, const char *original_str, uint8_t *replace_str, char *firset_str );


int set_nit_table_parm();


#endif
