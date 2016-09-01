#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <menu_foun.h>
#include <dev-info.h>
#include <key-ctl.h>
#include <cy22393_config.h>
#include <cy22393-set.h>
#include <adv7842_config.h>
#include <adv7842-set.h>
#include <gpio_config.h>
#include <h46-set.h>
#include <h46-comctrl.h>
#include <i2c-wr.h>
#include <debug.h>
#include <gpio_config.h>
#include <defineident.h>
#include <init-configuration.h>


extern s_config config;


/* hdmi */
struct dvb_peripheral	hdmi_input_50Hz_mod[11];
struct dvb_peripheral	hdmi_input_59d94Hz_mod[11];
struct dvb_peripheral	hdmi_input_60_3Hz_mod[11];
struct dvb_peripheral	hdmi_input_100Hz_mod[4];
struct dvb_peripheral	hdmi_input_119d88Hz_mod[4];
struct dvb_peripheral	hdmi_input_120Hz_mod[4];
struct dvb_peripheral	hdmi_input_200Hz_mod[2];
struct dvb_peripheral	hdmi_input_239Hz_mod[2];
struct dvb_peripheral	hdmi_input_240_3Hz_mod[2];
struct dvb_peripheral	hdmi_input_lowfieldrate_mod[3];

/* cvbs */
struct dvb_peripheral	cvbs_input_50Hz_mod[2];
struct dvb_peripheral	cvbs_input_60_3Hz_mod[2];


/* YPbPr */

struct dvb_peripheral	ypbpr_input_50Hz_mod[3];
struct dvb_peripheral	ypbpr_input_60_3Hz_mod[2];

static struct dvb_peripheral	*peripheral;
static struct h64_gpio_et_cfg_t h64_gpio_et_cfg;

static void hdmi_config( int opcode, int );


static int calc_v( int ypbpr_hdmi );


#define CLEAR		0x1
#define CVBS		(0x08)  /* YPrPb/CVBS input */
#define HDMI		(0x10)  /* G2=1,G1=0	//HDMI input */
#define nRST_H46_LO	(0x00)
#define nRST_H46_HI	(0x01)


#define Full_sampling_point1080I50Hz	2640
#define Full_sampling_point1080I60Hz	2200
#define Full_sampling_point720P50Hz	1980
#define Full_sampling_point720P60Hz	1650
#define ch1lcf1080i			562
#define ch1720p				745
/*  */
#define ntsc		0x01
#define pal		0x02
#define error		0x00
#define linebyline	0x00
#define linebylineno	0x01
#define hz60		0x00
#define hz50		0x01


static const struct {
	sourcecodeID	codeid;
	sorcefreq	video_hz;
} auto_code[] = {                                               /* 50hZ (17~31,37~39) */
	{ v720x576p_50Hz,		       v50Hz, },        /* 17 strat */
	{ v720x576p_50Hz18,		       v50Hz, },        /* 17 strat */
	{ v1280x720p_50Hz,		       v50Hz },
	{ v1920x1080i_50Hz,		       v50Hz },
	{ v720_or_1440x576i_50Hz,	       v50Hz },
	{ v720_or_1440x576i_50Hz22,	       v50Hz },
	{ v720_or_1440x288p_50Hz,	       v50Hz },
	{ v720_or_1440x288p_50Hz24,	       v50Hz },
	{ v2880x576i_50Hz,		       v50Hz },
	{ v2880x576i_50Hz26,		       v50Hz },
	{ v2880x288p_50Hz,		       v50Hz },
	{ v2880x288p_50Hz28,		       v50Hz },
	{ v1440x576p_50Hz,		       v50Hz },
	{ v1440x576p_50Hz30,		       v50Hz },
	{ v1920x1080p_50Hz,		       v50Hz },
	{ v2880x576p_50Hz,		       v50Hz },
	{ v2880x576p_50Hz38,		       v50Hz },
	{ v1920x1080i_or_1250_50Hz,	       v50Hz },
	/* 59.94Hz (1~16,35,36) */
#if 0
	{ v640x480p_59d94_or_60Hz,	       v59_94Hz },
	{ v720x480p_59d94_or_60Hz,	       v59_94Hz },
	{ v720x480p_59d94_or_60Hz3,	       v59_94Hz },
	{ v2880x240p_59d94_or_60Hz13,	       v59_94Hz },
	{ v1280x720p_59d94_or_60Hz,	       v59_94Hz },
	{ v1440x480p_59d94_or_60Hz15,	       v59_94Hz },
	{ v1920x1080i_59d94_or_60Hz,	       v59_94Hz },
	{ v720_or_1440x480i_59d94_or_60Hz,     v59_94Hz },
	{ v720_or_1440x480i_59d94_or_60Hz7,    v59_94Hz },
	{ v720_or_1440x240p_59d94_or_60Hz,     v59_94Hz },
	{ v720_or_1440x240p_59d94_or_60Hz9,    v59_94Hz },
	{ v2880x480i_59d94_or_60Hz,	       v59_94Hz },
	{ v2880x480i_59d94_or_60Hz11,	       v59_94Hz },
	{ v2880x240p_59d94_or_60Hz,	       v59_94Hz },
	{ v1440x480p_59d94_or_60Hz,	       v59_94Hz },
	{ v1920x1080p_59d94_or_60Hz,	       v59_94Hz },
	{ v2880x480p_59d94_or_60Hz,	       v59_94Hz },
	{ v2880x480p_59d94_or_60Hz36,	       v59_94Hz },
#endif
	/* 60Hz3 (1~16,35,36) */
	{ v640x480p_59d94_or_60Hz,	       v60Hz },
	{ v720x480p_59d94_or_60Hz,	       v60Hz },
	{ v720x480p_59d94_or_60Hz3,	       v60Hz },
	{ v2880x240p_59d94_or_60Hz13,	       v60Hz },
	{ v1280x720p_59d94_or_60Hz,	       v60Hz },
	{ v1440x480p_59d94_or_60Hz15,	       v60Hz },
	{ v1920x1080i_59d94_or_60Hz,	       v60Hz },
	{ v720_or_1440x480i_59d94_or_60Hz,     v60Hz },
	{ v720_or_1440x480i_59d94_or_60Hz7,    v60Hz },
	{ v720_or_1440x240p_59d94_or_60Hz,     v60Hz },
	{ v720_or_1440x240p_59d94_or_60Hz9,    v60Hz },
	{ v2880x480i_59d94_or_60Hz,	       v60Hz },
	{ v2880x480i_59d94_or_60Hz11,	       v60Hz },
	{ v2880x240p_59d94_or_60Hz,	       v60Hz },
	{ v1440x480p_59d94_or_60Hz,	       v60Hz },
	{ v1920x1080p_59d94_or_60Hz,	       v60Hz },
	{ v2880x480p_59d94_or_60Hz,	       v60Hz },
	{ v2880x480p_59d94_or_60Hz36,	       v60Hz },
	/* 100 Hz (40~45) */
	{ v1920x1080i_100Hz,		       v100Hz },
	{ v1280x720p_100Hz,		       v100Hz },
	{ v720x576p_100Hz,		       v100Hz },
	{ v720x576p_100Hz43,		       v100Hz },
	{ v720_or_1440x576i_100Hz,	       v100Hz },
	{ v720_or_1440x576i_100Hz45,	       v100Hz },
	/* 119.88 Hz (46~51) */
	{ v1920x1080i_119d88_or_120Hz,	       v119_88Hz },
	{ v1280x720p_119d88_or_120Hz,	       v119_88Hz },
	{ v720x480p_119d88_or_120Hz,	       v119_88Hz },
	{ v720x480p_119d88_or_120Hz49,	       v119_88Hz },
	{ v720_or_1440x480i_119d88_or_120Hz,   v119_88Hz },
	{ v720_or_1440x480i_119d88_or_120Hz51, v119_88Hz },
	/* 120 Hz3 (46~51) */
	{ v1920x1080i_119d88_or_120Hz,	       v120Hz },
	{ v1280x720p_119d88_or_120Hz,	       v120Hz },
	{ v720x480p_119d88_or_120Hz,	       v120Hz },
	{ v720x480p_119d88_or_120Hz49,	       v120Hz },
	{ v720_or_1440x480i_119d88_or_120Hz,   v120Hz },
	{ v720_or_1440x480i_119d88_or_120Hz51, v120Hz },
	/* 200 Hz (52~55) */
	{ v720x576p_200Hz,		       v200Hz },
	{ v720x576p_200Hz53,		       v200Hz },
	{ v720_or_1440x576i_200Hz,	       v200Hz },
	{ v720_or_1440x576i_200Hz55,	       v200Hz },
	/* 239 Hz (56~59) */
	{ v720x480p_239d76_or_240Hz,	       v239Hz },
	{ v720x480p_239d76_or_240Hz57,	       v239Hz },
	{ v720_1440x480i_239d76_or_240Hz,      v239Hz },
	{ v720_1440x480i_239d76_or_240Hz59,    v239Hz },
	/* 240 Hz3(56~59) */
	{ v720x480p_239d76_or_240Hz,	       v240Hz },
	{ v720x480p_239d76_or_240Hz57,	       v240Hz },
	{ v720_1440x480i_239d76_or_240Hz,      v240Hz },
	{ v720_1440x480i_239d76_or_240Hz59,    v240Hz },


	/*
	 * Low Field Rate (32~34)
	 * 24Hz,25Hz,30Hz
	 */
	{ v1920x1080p_23d98_or_24Hz,	       vlowfieldrate },
	{ v1920x1080p_25Hz,		       vlowfieldrate },
	{ v1920x1080p_29d98_or_30Hz,	       vlowfieldrate },
};


static const struct {
	int	codeid;
	char	long_video_resolution[25];
	char	short_video_resolution[17];
} auto_resolution[] = {
	{ v720x576p_50Hz,		       "720x576 50p",	    "576p"  },
	{ v720x576p_50Hz18,		       "720x576 50p",	    "576p"  },
	{ v1280x720p_50Hz,		       "1280x720 50p",	    "720p"  },
	{ v1920x1080i_50Hz,		       "1920x1080 50i",	    "1080i" },
	{ v720_or_1440x576i_50Hz,	       "720x576 50i",	    "576i"  },
	{ v720_or_1440x576i_50Hz22,	       "720x576 50i",	    "576i"  },
	{ v720_or_1440x288p_50Hz,	       "720x288 50p",	    "288p"  },
	{ v720_or_1440x288p_50Hz24,	       "720x288 50p",	    "288p"  },
	{ v2880x576i_50Hz,		       "2880x576 50i",	    "576i"  },
	{ v2880x576i_50Hz26,		       "2880x576 50i",	    "576i"  },
	{ v2880x288p_50Hz,		       "2880x288 50p",	    "288p"  },
	{ v2880x288p_50Hz28,		       "2880x288 50p",	    "288p"  },
	{ v1440x576p_50Hz,		       "1440x576 50p",	    "576p"  },
	{ v1440x576p_50Hz30,		       "1440x576 50p",	    "576p"  },
	{ v1920x1080p_50Hz,		       "1920x1080 50p",	    "1080p"  },
	{ v2880x576p_50Hz,		       "2880x576 50p",	    "576p"  },
	{ v2880x576p_50Hz38,		       "2880x576 50p",	    "576p"  },
	{ v1920x1080i_or_1250_50Hz,	       "1920x1080 50i",	    "1080i" },
	/* 59.94Hz (1~16,35,36) */
#if 0
	{ v640x480p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v720x480p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v720x480p_59d94_or_60Hz3,	       ,		    "", ""  },
	{ v2880x240p_59d94_or_60Hz13,	       ,		    "", ""  },
	{ v1280x720p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v1440x480p_59d94_or_60Hz15,	       ,		    "", ""  },
	{ v1920x1080i_59d94_or_60Hz,	       ,		    "", ""  },
	{ v720_or_1440x480i_59d94_or_60Hz,     ,		    "", ""  },
	{ v720_or_1440x480i_59d94_or_60Hz7,    ,		    "", ""  },
	{ v720_or_1440x240p_59d94_or_60Hz,     ,		    "", ""  },
	{ v720_or_1440x240p_59d94_or_60Hz9,    ,		    "", ""  },
	{ v2880x480i_59d94_or_60Hz,	       ,		    "", ""  },
	{ v2880x480i_59d94_or_60Hz11,	       ,		    "", ""  },
	{ v2880x240p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v1440x480p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v1920x1080p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v2880x480p_59d94_or_60Hz,	       ,		    "", ""  },
	{ v2880x480p_59d94_or_60Hz36,	       ,		    "", ""  },
#endif
	/* 60Hz3 (1~16,35,36) */
	{ v640x480p_59d94_or_60Hz,	       "640x480 60p",	    "480p"  },
	{ v720x480p_59d94_or_60Hz,	       "720x480 60p",	    "480p"  },
	{ v720x480p_59d94_or_60Hz3,	       "720x480 60p",	    "480p"  },
	{ v2880x240p_59d94_or_60Hz13,	       "2880x240 60p",	    "240p"  },
	{ v1280x720p_59d94_or_60Hz,	       "1280x720 60p",	    "720p"  },
	{ v1440x480p_59d94_or_60Hz15,	       "1440x480 60p",	    "480p"  },
	{ v1920x1080i_59d94_or_60Hz,	       "1920x1080 60i",	    "1080i" },
	{ v720_or_1440x480i_59d94_or_60Hz,     "720x480 60i",	    "480i"  },
	{ v720_or_1440x480i_59d94_or_60Hz7,    "720x480 60i",	    "480i"  },
	{ v720_or_1440x240p_59d94_or_60Hz,     "720x240 60p",	    "240p"  },
	{ v720_or_1440x240p_59d94_or_60Hz9,    "720x240 60p",	    "240p"  },
	{ v2880x480i_59d94_or_60Hz,	       "2880x480 60i",	    "480i"  },
	{ v2880x480i_59d94_or_60Hz11,	       "2880x480 60i",	    "480i"  },
	{ v2880x240p_59d94_or_60Hz,	       "2880x240 60p",	    "240p"  },
	{ v1440x480p_59d94_or_60Hz,	       "1440x480 60p",	    "480p"  },
	{ v1920x1080p_59d94_or_60Hz,	       "1920x1080 60p",	    "1080p" },
	{ v2880x480p_59d94_or_60Hz,	       "2880x480 60p",	    "480p"  },
	{ v2880x480p_59d94_or_60Hz36,	       "2880x480 60p",	    "480p"  },
	/* 100 Hz (40~45) */
	{ v1920x1080i_100Hz,		       "1920x1080 100i",    "1080i" },
	{ v1280x720p_100Hz,		       "1280x720 100p",	    "720p"  },
	{ v720x576p_100Hz,		       "720x576 100p",	    "576p"  },
	{ v720x576p_100Hz43,		       "720x576 100p",	    "576p"  },
	{ v720_or_1440x576i_100Hz,	       "720x576 100i",	    "576i"  },
	{ v720_or_1440x576i_100Hz45,	       "720x576 100i",	    "576i"  },
#if 0
	/* 119.88 Hz (46~51) */
	{ v1920x1080i_119d88_or_120Hz,	       "1920x1080 119.88i", "1080i" },
	{ v1280x720p_119d88_or_120Hz,	       "1280x720 119.88p",  "720p"  },
	{ v720x480p_119d88_or_120Hz,	       "720x480 119.88p",   "480p"  },
	{ v720x480p_119d88_or_120Hz49,	       "720x480 119.88p",   "480p"  },
	{ v720_or_1440x480i_119d88_or_120Hz,   "720x480 119.88i",   "480i"  },
	{ v720_or_1440x480i_119d88_or_120Hz51, "720x480 119.88i",   "480i"  },
#endif
	/* 120 Hz3 (46~51) */
	{ v1920x1080i_119d88_or_120Hz,	       "1920x1080 120i",    "1080i" },
	{ v1280x720p_119d88_or_120Hz,	       "1280x720 120p",	    "720p"  },
	{ v720x480p_119d88_or_120Hz,	       "720x480 120p",	    "480p"  },
	{ v720x480p_119d88_or_120Hz49,	       "720x480 120p",	    "480p"  },
	{ v720_or_1440x480i_119d88_or_120Hz,   "720x480 120i",	    "480i"  },
	{ v720_or_1440x480i_119d88_or_120Hz51, "720x480 120i",	    "480i"  },
	/* 200 Hz (52~55) */
	{ v720x576p_200Hz,		       "720x576 200p",	    "576p"  },
	{ v720x576p_200Hz53,		       "720x576 200p",	    "576p"  },
	{ v720_or_1440x576i_200Hz,	       "720x576 200i",	    "576i"  },
	{ v720_or_1440x576i_200Hz55,	       "720x576 200i",	    "576i"  },
	/* 239 Hz (56~59) */
	{ v720x480p_239d76_or_240Hz,	       "720x480 239p",	    "480p"  },
	{ v720x480p_239d76_or_240Hz57,	       "720x480 239p",	    "480p"  },
	{ v720_1440x480i_239d76_or_240Hz,      "720x480 239i",	    "480i"  },
	{ v720_1440x480i_239d76_or_240Hz59,    "720x480 239i",	    "480i"  },
	/* 240 Hz3(56~59) */
	{ v720x480p_239d76_or_240Hz,	       "720x480 240p",	    "480p"  },
	{ v720x480p_239d76_or_240Hz57,	       "720x480 240p",	    "480p"  },
	{ v720_1440x480i_239d76_or_240Hz,      "720x480 240i",	    "480i"  },
	{ v720_1440x480i_239d76_or_240Hz59,    "720x480 240i",	    "480i"  },


	/*
	 * Low Field Rate (32~34)
	 * 24Hz,25Hz,30Hz
	 */
	{ v1920x1080p_23d98_or_24Hz,	       "1920x1080 24p",	    "1080p" },
	{ v1920x1080p_25Hz,		       "1920x1080 25p",	    "1080p" },
	{ v1920x1080p_29d98_or_30Hz,	       "1920x1080 30p",	    "1080p" },
};

struct dvb_peripheral * peripheral_config()
{
	return(peripheral);
}


struct h64_gpio_et_cfg_t * h64gpio_et_cfg()
{
	return(&h64_gpio_et_cfg);
}


/*gpio 配置
 *
 * // 以后看情况而定[由用户选择还是系统自选配置]
 *
 ****************/

void input_mod_gpio( uint8_t i_mod )
{
	gpio_input_mod( i_mod );
}


/*
 *
 *
 *
 */

/* adv7842/cy22393 配置 */


/* 类型选择	 cy22393			实际类型
 * 0	refclk=13.5MHz                  refclk_13_5M
 * 1	refclk=27MHz			refclk_27M
 * 2    refclk=27/1.001MHz		refclk_27M_1_001M
 * 3	refclk=74.25MHz			refclk_74_25M
 * 4	refclk=74.25MHz/1.001	refclk_74_25M_1_001M
 * 5    refclk=148.5MHz			refclk_148_5M
 * -	-error
 *************/


/* 类型选择	 adv7842		模式
 * 10						Internel_EDID
 * 11						CVBS_576i_480ix50
 * 12						HDMI_GR_480px60
 * 13						YPbPr_720px60
 * 14						YPbPr_1080px50
 * 15						YPbPr_1080ix50
 * 16						SD_YPbPr_576i_480ix50
 * 17						CP_YPbPr_576ix50
 * 18						CP_YPbPr_576px50
 * 19						HDMI_COMP_1080ix50
 * 20						HDMI_COMP_1080px50
 * 21						HDMI_COMP_720px60
 * 22						HDMI_COMP_576ix50
 * -						-error
 ****************************/
int cy22393_open()
{
	int fd;
	/* int adapter_nr = 0; */
	char filename[20];
	snprintf( filename, 19, "/dev/i2c-%d", I2C_NR_0 );
	fd = open( filename, O_RDWR );
	if ( fd < 0 )
	{
		DEBUG( "open [%s] error...", filename );
		return(fd);
	}
	return(fd);
}


int adv7842_open()
{
	int	fd;
	char	filename[20];
	/* int adapter_nr = 1; */
	snprintf( filename, 19, "/dev/i2c-%d", I2C_NR_1 );
	fd = open( filename, O_RDWR );
	if ( fd < 0 )
	{
		DEBUG( "open [%s] error...", filename );
		return(fd);
	}

	return(fd);
}


static int h46_open()
{
	int fd = 0, i = -1;

	if ( (fd = uart_open( fd, 1 ) ) < 0 )
	{
		perror( "uart_open error\n" );
		return(fd);
	}

	if ( (i = set_uart( fd, 115200, 8, 'N', 1 ) ) < 0 )
	{
		perror( "set_uart error \n" );
		return(fd);
	}
	return(fd);
}


/* 自动选择code码 */
sorcefreq freq_parse_token( int resolution_ratio )
{
	int i;

	for ( i = 0; auto_code[i].codeid; i++ )
		if ( auto_code[i].codeid == resolution_ratio )
			return(auto_code[i].video_hz);

	DEBUG( "resolution ratio %d: \r\n", resolution_ratio );

	return(0);
}


struct dvb_peripheral ypbpr_input_50Hz_mod[3] = {
	{
		/* ok */
		.codeid			= v1280x720p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &YPbPr_720px50_mod,
		.modopcodeset		= &v72050p_mod,
	},
	{
		.codeid			= v1920x1080p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_148_5MHz,
		.pixefreq		= &YPbPr_1080px50_mod,
		.modopcodeset		= &v108050p_mod,
	},
	{
		.codeid			= v1920x1080i_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &YPbPr_1080ix50_mod,
		.modopcodeset		= &v108050i_mod,
	},
};

struct dvb_peripheral ypbpr_input_60_3Hz_mod[2] = {
	{
		.codeid			= v720_or_1440x240p_59d94_or_60Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v640x480p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

struct dvb_peripheral cvbs_input_50Hz_mod[2] = {
	{
		.codeid			= v720_or_1440x576i_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_27MHz,
		.pixefreq		= &CVBS_576ix50_mod,
		.modopcodeset		= &v57650i_mod,
	},
#if 0
	{
		.codeid			= v720_or_1440x288p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
#endif
};

struct dvb_peripheral cvbs_input_60_3Hz_mod[2] = {
	{
		.codeid			= v720_or_1440x480i_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_27MHz,
		.pixefreq		= &CVBS_480ix60_mod,
		.modopcodeset		= &v48060i_mod,
	},
#if 0
	{
		.codeid			= v720_or_1440x240p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
#endif
};


/*
 * 视频输入为50Hz
 * 50hZ (17~31,37~39)
 */
struct dvb_peripheral hdmi_input_50Hz_mod[11] = {
	{
		/* ok */
		.codeid			= v1280x720p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &HDMI_COMP_720px50_mod,
		.modopcodeset		= &v72050p_mod,
	},
	{
		/* ok 20160129 */
		.codeid			= v1920x1080i_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &HDMI_COMP_1080ix50_mod,
		.modopcodeset		= &v108050i_mod,
	},
	{
		.codeid			= v720_or_1440x576i_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_27MHz,
		.pixefreq		= &HDMI_COMP_576ix50_mod,
		.modopcodeset		= &v57650i_mod,
	},
	{
		/* ok */
		.codeid			= v1920x1080p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_148_5MHz,
		.pixefreq		= &HDMI_COMP_1080px50_mod,
		.modopcodeset		= &v108050p_mod,
	},
#if 0
	{
		.codeid			= v720x576p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL, /* &refclk_74_25MHz, */
		.pixefreq		= NULL, /* &HDMI_COMP_1080ix50_mod, */
		.modopcodeset		= NULL, /* &v108050i_mod, */
	},

	{
		.codeid			= v720_or_1440x288p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
	{
		.codeid			= v2880x576i_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
	{
		.codeid			= v2880x288p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
	{
		.codeid			= v1440x576p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
	{
		.codeid			= v2880x576p_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},
	{
		.codeid			= v1920x1080i_or_1250_50Hz,
		.video_hz		= v50Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.modopcodeset		= NULL,
		.pixefreq		= NULL,
	},

#endif
};

#if 0
/* 59.94Hz (1~16,35,36) */
struct dvb_peripheral hdmi_input_59d94Hz_mod[11] = {
	{
		.codeid			= v640x480p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		/* 20160128 jh */
		.codeid			= v720x480p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &,
		.pixefreq		= &HDMI_GR_480px60_mod,
		.modopcodeset		= NULL, /* 这个对应的h46没有配置吗? jh */
	},
	{
		.codeid			= v1280x720p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1920x1080i_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		/* 20160128 jh */
		.codeid			= v720_or_1440x480i_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x240p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v2880x480i_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v2880x240p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1440x480p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1920x1080p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v2880x480p_59d94_or_60Hz,
		.video_hz		= v59_94Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};
#endif
/* 60Hz3 (1~16,35,36) */
struct dvb_peripheral hdmi_input_60_3Hz_mod[11] = {
	{
		.codeid			= v720x480p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_27MHz_1_001MHz,
		.pixefreq		= &YPbPr_720px60_mod,
		.modopcodeset		= &v72060p_mod,
	},
	{
		/* 这里vic 收不到 */
		.codeid			= v1280x720p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &HDMI_COMP_720px60_mod,
		.modopcodeset		= &v72060p_mod,
	},
	{
		.codeid			= v1920x1080i_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_74_25MHz,
		.pixefreq		= &HDMI_COMP_1080ix60_mod,
		.modopcodeset		= &v108060i_mod,
	},
	{
		.codeid			= v1920x1080p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= &refclk_148_5MHz,
		.pixefreq		= &HDMI_COMP_1080px60_mod,
		.modopcodeset		= &v108060p_mod,
	},
#if 0
	{
		.codeid			= v640x480p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL, /* &refclk_74_25MHz, */
		.pixefreq		= NULL, /* &HDMI_COMP_1080ix50_mod, */
		.modopcodeset		= NULL, /* &v108050i_mod, */
	},
	{
		/* 20160128 jh ,蓝屏 9999 */
		.codeid			= v720_or_1440x480i_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x240p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v2880x480i_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v2880x240p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1440x480p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},

	{
		.codeid			= v2880x480p_59d94_or_60Hz,
		.video_hz		= v60Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
#endif
};

/* 100 Hz (40~45) */
struct dvb_peripheral hdmi_input_100Hz_mod[4] = {
	{
		.codeid			= v1920x1080i_100Hz,
		.video_hz		= v100Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1280x720p_100Hz,
		.video_hz		= v100Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720x576p_100Hz,
		.video_hz		= v100Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x576i_100Hz,
		.video_hz		= v100Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 119.88 Hz (46~51) */
struct dvb_peripheral hdmi_input_119d88Hz_mod[4] = {
	{
		.codeid			= v1920x1080i_119d88_or_120Hz,
		.video_hz		= v119_88Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1280x720p_119d88_or_120Hz,
		.video_hz		= v119_88Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720x480p_119d88_or_120Hz,
		.video_hz		= v119_88Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x480i_119d88_or_120Hz,
		.video_hz		= v119_88Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 120 Hz3 (46~51) */
struct dvb_peripheral hdmi_input_120Hz_mod[4] = {
	{
		.codeid			= v1920x1080i_119d88_or_120Hz,
		.video_hz		= v120Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1280x720p_119d88_or_120Hz,
		.video_hz		= v120Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720x480p_119d88_or_120Hz,
		.video_hz		= v120Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x480i_119d88_or_120Hz,
		.video_hz		= v120Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 200 Hz (52~55) */
struct dvb_peripheral hdmi_input_200Hz_mod[2] = {
	{
		.codeid			= v720x576p_200Hz,
		.video_hz		= v200Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_or_1440x576i_200Hz,
		.video_hz		= v200Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 239 Hz (56~59) */
struct dvb_peripheral hdmi_input_239Hz_mod[2] = {
	{
		.codeid			= v720x480p_239d76_or_240Hz,
		.video_hz		= v239Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_1440x480i_239d76_or_240Hz,
		.video_hz		= v239Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 240 Hz3 */
struct dvb_peripheral hdmi_input_240_3Hz_mod[2] = {
	{
		.codeid			= v720x480p_239d76_or_240Hz,
		.video_hz		= v240Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v720_1440x480i_239d76_or_240Hz,
		.video_hz		= v240Hz,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};

/* 24Hz,25Hz,30Hz */
struct dvb_peripheral hdmi_input_lowfieldrate_mod[3] = {
	{
		.codeid			= v1920x1080p_23d98_or_24Hz,
		.video_hz		= vlowfieldrate,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1920x1080p_25Hz,
		.video_hz		= vlowfieldrate,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
	{
		.codeid			= v1920x1080p_29d98_or_30Hz,
		.video_hz		= vlowfieldrate,
		.i2c_0addr		= CY22393ADDR,
		.open_clockdev		= &cy22393_open,
		.open_freqdev		= &adv7842_open,
		.i2c_0ordernumber	= I2C_NR_0,
		.i2c_1ordernumber	= I2C_NR_1,
		.open_uartdev		= &h46_open,
		.clockset		= NULL,
		.pixefreq		= NULL,
		.modopcodeset		= NULL,
	},
};


/* #define open_text */

int read_vic()
{

	
	int	r_ratio;


	if(get_unlock_menu_exit())
		return 0;
	
	int	fd = adv7842_open();

	/* PRIM_MODE */
	uint8_t addr;

	addr = 0x20;

	/* 由PRIM_MODE[3:0] should be used with VID_STD[5:0] to set the desired video mode */
	usleep( 10000 );
#ifdef open_testt
	DEBUG( "mode=0x%x\n", get_i2c_register_value( fd, addr, 0x01 ) );
	DEBUG( "VID_STD=0x%x\n", get_i2c_register_value( fd, addr, 0x00 ) );

	DEBUG( "PRIM_MODE[3:0]=0x%x should be used with VID_STD[5:0]=0x%x to set the desired video mode HZ=%x \r\n",
	       0xF & get_i2c_register_value( fd, addr, 0x01 ),
	       get_i2c_register_value( fd, addr, 0x00 ) & 0x1F,
	       ( (get_i2c_register_value( fd, addr, 0x01 ) & (~(0xF) ) ) & 0x7F) >> 4 );
#endif
	usleep( 10000 );
	addr = 0x7C / 2;

	/* 下是vic 编码号 */
#ifdef open_test
	DEBUG( "DATA Byta 3=0x%x\n", get_i2c_register_value( fd, addr, 0x03 ) );
#endif
	r_ratio = get_i2c_register_value( fd, addr, 0x04 );
#ifdef open_test
	DEBUG( "DATA Byta 4,Video Description VIC=0x%x\n", r_ratio );

	/* 看是否有需要重复的像素值 */
	DEBUG( "DATA Byta 5,Valid Pixel Repeat Values , pixel data sent VIC5=0x%x times\n",
	       get_i2c_register_value( fd, addr, 0x05 ) + 1 );
#endif
	close( fd );
	return(r_ratio);
}


/* HDMI detection */

int hdmi_detection(  int r_ratio )
{
	int rest = -1,  i;
	static int item;

#if 1
	static int bak_r_opcode = 0;

	

	if(bak_r_opcode == r_ratio)
	{
		//DEBUG("r_ratio :%d",r_ratio);
		return(r_ratio);
	}else if(bak_r_opcode != r_ratio)
		bak_r_opcode = r_ratio;

	//DEBUG("r_ratio :%d",r_ratio);
#endif

	memset( config.localstatus.encoder_video_resolution, ' ', 16 );
	memset( config.localstatus.encoder_video_shrot_resolution, ' ', 16 );

	item = sizeof(auto_resolution) / sizeof(auto_resolution[0]);

	for ( i = 0; i < item; i++ )
	{
		if ( auto_resolution[i].codeid == r_ratio )
		{
			snprintf( config.localstatus.encoder_video_resolution, strlen( auto_resolution[i].long_video_resolution ) + 1,
				  "%s ", auto_resolution[i].long_video_resolution );
			snprintf( config.localstatus.encoder_video_shrot_resolution, strlen( auto_resolution[i].short_video_resolution ) + 1
				  , "%s", auto_resolution[i].short_video_resolution );
			rest = r_ratio;
			DEBUG( "%s   %s  %x ", config.localstatus.encoder_video_resolution, config.localstatus.encoder_video_resolution ,r_ratio);

			break;
		}else{
			rest = VIDEO_INPUT_UNLOCK;
			sprintf( config.localstatus.encoder_video_resolution, "unknown" );
			sprintf( config.localstatus.encoder_video_shrot_resolution, "unknown" );
		}
	}

	
	return(rest);
}


/* HDMI */
static void hdmi_config( int opcode, int r_ratio )
{
	int i;

	switch ( opcode )
	{
	/*
	 * 自动匹配时一定会有重叠
	 * 选项，这个需要后期进行细化
	 * 目前还不知道项目最终所需要配置
	 * 所以眼前只能大略通过
	 */
	case v50Hz:
		/* 17,18 */
		if ( (r_ratio == v720x576p_50Hz) || (r_ratio == v720x576p_50Hz18) )
		{
			r_ratio = v720x576p_50Hz;
		}

		if ( r_ratio == v1280x720p_50Hz )
		{
			r_ratio = v1280x720p_50Hz;
		}

		if ( r_ratio == v1920x1080i_50Hz )
		{
			r_ratio = v1920x1080i_50Hz;
		}
		/* 19,20 */

		/* 21,22 */
		if ( (r_ratio == v720_or_1440x576i_50Hz) || (r_ratio == v720_or_1440x576i_50Hz22) )
		{
			r_ratio = v720_or_1440x576i_50Hz;
		}
		/* 23,24 */
		if ( (r_ratio == v720_or_1440x288p_50Hz) || (r_ratio == v720_or_1440x288p_50Hz24) )
		{
			r_ratio = v720_or_1440x288p_50Hz;
		}

		/* 25,26 */
		if ( (r_ratio == v2880x576i_50Hz) || (r_ratio == v2880x576i_50Hz26) )
		{
			r_ratio = v2880x576i_50Hz;
		}

		/* 27,28 */
		if ( (r_ratio == v2880x288p_50Hz) || (r_ratio == v2880x288p_50Hz28) )
		{
			r_ratio = v2880x288p_50Hz;
		}

		/* 29，30 */
		if ( (r_ratio == v1440x576p_50Hz) || (r_ratio == v1440x576p_50Hz30) )
		{
			r_ratio = v1440x576p_50Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_50Hz_mod) / sizeof(hdmi_input_50Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_50Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_50Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_50Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_50Hz_mod[i].modopcodeset;
				DEBUG( "...v50Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
#if 0
	case v59_94Hz:
		/* 10,11 */
		if ( (r_ratio == v2880x480i_59d94_or_60Hz) || (r_ratio == v2880x480i_59d94_or_60Hz11) )
		{
			r_ratio = v2880x480i_59d94_or_60Hz;
		}

		/* 12,13 */
		if ( (r_ratio == v2880x240p_59d94_or_60Hz) || (r_ratio == v2880x240p_59d94_or_60Hz13) )
		{
			r_ratio = v2880x240p_59d94_or_60Hz;
		}

		/* 14,15 */
		if ( (r_ratio == v1440x480p_59d94_or_60Hz) || (r_ratio == v1440x480p_59d94_or_60Hz15) )
		{
			r_ratio = v1440x480p_59d94_or_60Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_59d94Hz_mod) / sizeof(hdmi_input_59d94Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_59d94Hz_mod[i].codeid )
			{
				peripheral = &hdmi_input_59d94Hz_mod[i];
				DEBUG( "...v59_94Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
#endif
	case v60Hz:
		/* 10,11 */
		if ( (r_ratio == v2880x480i_59d94_or_60Hz) || (r_ratio == v2880x480i_59d94_or_60Hz11) )
		{
			r_ratio = v2880x480i_59d94_or_60Hz;
		}

		/* 12,13 */
		if ( (r_ratio == v2880x240p_59d94_or_60Hz) || (r_ratio == v2880x240p_59d94_or_60Hz13) )
		{
			r_ratio = v2880x240p_59d94_or_60Hz;
		}

		/* 14,15 */
		if ( (r_ratio == v1440x480p_59d94_or_60Hz) || (r_ratio == v1440x480p_59d94_or_60Hz15) )
		{
			r_ratio = v1440x480p_59d94_or_60Hz;
		}


		for ( i = 0; i < sizeof(hdmi_input_60_3Hz_mod) / sizeof(hdmi_input_60_3Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_60_3Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_60_3Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_60_3Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_60_3Hz_mod[i].modopcodeset;
				DEBUG( "...v60Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;

	case v100Hz:
		/* 42,43 */
		if ( (r_ratio == v720x576p_100Hz) || (r_ratio == v720x576p_100Hz43) )
		{
			r_ratio = v720x576p_100Hz;
		}

		/* 44,45 */
		if ( (r_ratio == v720_or_1440x576i_100Hz) || (r_ratio == v720_or_1440x576i_100Hz45) )
		{
			r_ratio = v720_or_1440x576i_100Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_100Hz_mod) / sizeof(hdmi_input_100Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_100Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_100Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_100Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_100Hz_mod[i].modopcodeset;
				DEBUG( "...v100Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case v119_88Hz:
		/* 50,51 */
		if ( (r_ratio == v720_or_1440x480i_119d88_or_120Hz) || (r_ratio == v720_or_1440x480i_119d88_or_120Hz) )
		{
			r_ratio = v720_or_1440x480i_119d88_or_120Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_119d88Hz_mod) / sizeof(hdmi_input_119d88Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_119d88Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_119d88Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_119d88Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_119d88Hz_mod[i].modopcodeset;
				DEBUG( "...v119_88Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case v120Hz:

		/* 50,51 */
		if ( (r_ratio == v720_or_1440x480i_119d88_or_120Hz) || (r_ratio == v720_or_1440x480i_119d88_or_120Hz) )
		{
			r_ratio = v720_or_1440x480i_119d88_or_120Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_120Hz_mod) / sizeof(hdmi_input_120Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_120Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_120Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_120Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_120Hz_mod[i].modopcodeset;
				DEBUG( "...v120Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case v200Hz:
		/* 52,53 */
		if ( (r_ratio == v720x576p_200Hz) || (r_ratio == v720x576p_200Hz53) )
		{
			r_ratio = v720x576p_200Hz;
		}

		/* 54,55 */
		if ( (r_ratio == v720_or_1440x576i_200Hz) || (r_ratio == v720_or_1440x576i_200Hz55) )
		{
			r_ratio = v720_or_1440x576i_200Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_200Hz_mod) / sizeof(hdmi_input_200Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_200Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_200Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_200Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_200Hz_mod[i].modopcodeset;
				DEBUG( "...v200Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case v239Hz:
		/* 56,57 */
		if ( (r_ratio == v720x480p_239d76_or_240Hz) || (r_ratio == v720x480p_239d76_or_240Hz57) )
		{
			r_ratio = v720x480p_239d76_or_240Hz;
		}

		/* 58,59 */
		if ( (r_ratio == v720_1440x480i_239d76_or_240Hz) || (r_ratio == v720_1440x480i_239d76_or_240Hz59) )
		{
			r_ratio = v720_1440x480i_239d76_or_240Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_239Hz_mod) / sizeof(hdmi_input_239Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_239Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_239Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_239Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_239Hz_mod[i].modopcodeset;
				DEBUG( "...v239Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case v240Hz:
		/* 56,57 */
		if ( (r_ratio == v720x480p_239d76_or_240Hz) || (r_ratio == v720x480p_239d76_or_240Hz57) )
		{
			r_ratio = v720x480p_239d76_or_240Hz;
		}

		/* 58,59 */
		if ( (r_ratio == v720_1440x480i_239d76_or_240Hz) || (r_ratio == v720_1440x480i_239d76_or_240Hz59) )
		{
			r_ratio = v720_1440x480i_239d76_or_240Hz;
		}

		for ( i = 0; i < sizeof(hdmi_input_240_3Hz_mod) / sizeof(hdmi_input_240_3Hz_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_240_3Hz_mod[i].codeid )
			{
				peripheral			= &hdmi_input_240_3Hz_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_240_3Hz_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_240_3Hz_mod[i].modopcodeset;
				DEBUG( "...v240Hz.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;
	case vlowfieldrate:

		for ( i = 0; i < sizeof(hdmi_input_lowfieldrate_mod) / sizeof(hdmi_input_lowfieldrate_mod[0]); i++ )
		{
			if ( r_ratio == hdmi_input_lowfieldrate_mod[i].codeid )
			{
				peripheral			= &hdmi_input_lowfieldrate_mod[i];
				h64_gpio_et_cfg.open_h46dev	= hdmi_input_lowfieldrate_mod[i].open_uartdev;
				h64_gpio_et_cfg.h46_cfg		= hdmi_input_lowfieldrate_mod[i].modopcodeset;
				DEBUG( "...vlowfieldrate.....%d..%d......", opcode, r_ratio );
				break;
			}
		}

		break;

	default:
		DEBUG( "Error..HDMI...%d..%d......", opcode, r_ratio );
		break;
	}
}


/*
 * gpio掩码状态
 */
void gpio_mask( uint8_t setval )
{
	pthread_mutex_lock( &config.gpiolock );
	config.scfg_Param.system_gpio_mask = setval;
	pthread_mutex_unlock( &config.gpiolock );
}


/* HDMI 模式下的gpio */

void hdmi_gpio_dfg( void )
{
	/* gpio */

	input_mod_gpio( HDMI | nRST_H46_HI );
	input_mod_gpio( HDMI | nRST_H46_LO );
	usleep( 200000 );
	input_mod_gpio( HDMI | nRST_H46_HI );
}


/* ypbpr,cvbs 模式下的gpio */
void ypbpr_cvbs_cfg()
{
	/* gpio */
	DEBUG("----------------------ypbpr_cvbs gpio");
	input_mod_gpio( CVBS | nRST_H46_HI );
	input_mod_gpio( CVBS | nRST_H46_LO );
	usleep( 200000 );
	input_mod_gpio( CVBS | nRST_H46_HI );
}


/*
 * lcd 背光开关
 */
void lcd_bkl_offon( uint8_t on_off )
{
	lcd_bkl_enable( on_off );
}


/*
 *  希望可以从寄存器中读出7842的code ID 码来
 *  确定输入的是何种模式，输入的频率是多少。
 *  以此来自动确定在什么频率下，运行的辨率是多少，而
 *  自动设置运行。
 */
void usercode_mod( int modcod, int ypbpr_cvbs, int r_ratio )
{
	int opcode, i;

	if ( modcod == -1 )
		return;

	opcode = freq_parse_token( r_ratio );
	/* opcode = v50Hz; //测试用。指定一个时钟频率 */
	DEBUG( "........opcode=%d..r_ratio=%d..modcod=%d....", opcode, r_ratio, modcod );


	/*
	 * 有这几种模式,HDMI,CVBS,YPbPr
	 * 每一种模式都重复的ID,所以需要进行分类化处理
	 */

	switch ( modcod )
	{
	case M_HDMI: {
		DEBUG( "HDMI module" );

		hdmi_gpio_dfg();
		h64_gpio_et_cfg.gpio_cfg = &hdmi_gpio_dfg;

		hdmi_config( opcode, r_ratio );
	}

	break;
	case M_CVBS:

	{
		DEBUG( "CVBS module" );

		ypbpr_cvbs_cfg();
		h64_gpio_et_cfg.gpio_cfg = &ypbpr_cvbs_cfg;
		usleep( 1000 );


		/*
		 * cvbs_config(opcode,r_ratio);
		 * 逐行与非逐行
		 */
		switch ( ypbpr_cvbs )
		{
		case NTSC_60_ROW: {
			for ( i = 0; i < sizeof(cvbs_input_60_3Hz_mod) / sizeof(cvbs_input_60_3Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x480i_59d94_or_60Hz == cvbs_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".cvbs..v60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "NTSC_60_ROW" );
			break;
		case NTSC_60_NOTROW: {
			for ( i = 0; i < sizeof(cvbs_input_60_3Hz_mod) / sizeof(cvbs_input_60_3Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x480i_59d94_or_60Hz == cvbs_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".cvbs..v60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "NTSC_60_NOTROW" );
			break;
		case NTSC_50_NOTROW: {
			for ( i = 0; i < sizeof(cvbs_input_50Hz_mod) / sizeof(cvbs_input_50Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x576i_50Hz == cvbs_input_50Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_50Hz_mod[i].modopcodeset;
					DEBUG( "..cvbs.v50Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "NTSC_50_NOTROW" );
			break;
		case NTSC_50_ROW: {
			for ( i = 0; i < sizeof(cvbs_input_50Hz_mod) / sizeof(cvbs_input_50Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x576i_50Hz == cvbs_input_50Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_50Hz_mod[i].modopcodeset;
					DEBUG( "..cvbs.v50Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "NTSC_50_ROW" );
			break;
		case PAL_60_ROW: {
			for ( i = 0; i < sizeof(cvbs_input_60_3Hz_mod) / sizeof(cvbs_input_60_3Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x480i_59d94_or_60Hz == cvbs_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".cvbs..v60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "PAL_60_ROW" );
			break;
		case PAL_60_NOTROW: {
			for ( i = 0; i < sizeof(cvbs_input_60_3Hz_mod) / sizeof(cvbs_input_60_3Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x480i_59d94_or_60Hz == cvbs_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".cvbs..v60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "PAL_60_NOTROW" );
			break;
		case PAL_50_NOTROW: {
			for ( i = 0; i < sizeof(cvbs_input_50Hz_mod) / sizeof(cvbs_input_50Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x576i_50Hz == cvbs_input_50Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_50Hz_mod[i].modopcodeset;
					DEBUG( "..cvbs.v50Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "PAL_50_NOTROW" );
			break;
		case PAL_50_ROW: {
			for ( i = 0; i < sizeof(cvbs_input_50Hz_mod) / sizeof(cvbs_input_50Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x576i_50Hz == cvbs_input_50Hz_mod[i].codeid )
				{
					peripheral			= &cvbs_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= cvbs_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= cvbs_input_50Hz_mod[i].modopcodeset;
					DEBUG( "..cvbs.v50Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
			DEBUG( "PAL_50_ROW" );
			break;
		default:
			break;
		}
	}

	break;
	case M_YPbPr: {
		DEBUG( "YPbPr module" );
		/* gpio */
		ypbpr_cvbs_cfg();
		h64_gpio_et_cfg.gpio_cfg = &ypbpr_cvbs_cfg;
		usleep( 1000 );
		switch ( ypbpr_cvbs )
		{
		case YPbPr1080Isign60Hz: {
			for ( i = 0; i < sizeof(ypbpr_input_60_3Hz_mod) / sizeof(ypbpr_input_60_3Hz_mod[0]); i++ )
			{
				if ( v640x480p_59d94_or_60Hz == ypbpr_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &ypbpr_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= ypbpr_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= ypbpr_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".YPbPr1080Isign60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
		break;
		case YPbPr1080Isign50Hz: {
			for ( i = 0; i < sizeof(ypbpr_input_50Hz_mod) / sizeof(ypbpr_input_50Hz_mod[0]); i++ )
			{
				if ( v1920x1080i_50Hz == ypbpr_input_50Hz_mod[i].codeid )
				{
					peripheral			= &ypbpr_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= ypbpr_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= ypbpr_input_50Hz_mod[i].modopcodeset;
					DEBUG( ".YPbPr1080Isign50Hz......%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}

		break;
		case YPbPr720Psign50Hz: {
			for ( i = 0; i < sizeof(ypbpr_input_50Hz_mod) / sizeof(ypbpr_input_50Hz_mod[0]); i++ )
			{
				if ( v1280x720p_50Hz == ypbpr_input_50Hz_mod[i].codeid )
				{
					peripheral			= &ypbpr_input_50Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= ypbpr_input_50Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= ypbpr_input_50Hz_mod[i].modopcodeset;
					DEBUG( ".YPbPr720Psign50Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
		break;
		case YPbPr720Psign60Hz: {
			for ( i = 0; i < sizeof(ypbpr_input_60_3Hz_mod) / sizeof(ypbpr_input_60_3Hz_mod[0]); i++ )
			{
				if ( v720_or_1440x240p_59d94_or_60Hz == ypbpr_input_60_3Hz_mod[i].codeid )
				{
					peripheral			= &ypbpr_input_60_3Hz_mod[i];
					h64_gpio_et_cfg.open_h46dev	= ypbpr_input_60_3Hz_mod[i].open_uartdev;
					h64_gpio_et_cfg.h46_cfg		= ypbpr_input_60_3Hz_mod[i].modopcodeset;
					DEBUG( ".YPbPr720Psign60Hz.....%d..%d......", opcode, r_ratio );
					break;
				}
			}
		}
		break;
		default:
			DEBUG( "YPbPr code error" );
			break;
		}

		/* ypbpr_config(opcode,r_ratio); */
	}

	break;
	default:
		break;
	}
}


/* adv7842 默认设置
 * 只在开机启动时有效
 * 因要根据7842的状态值来配置22393,h46,所以在开机前必须对adv7842进行一次
 * 默认设置，使adv7842生效后，方可读出有效ID
 */
void adv_defualt_set()
{
	int fd;
	fd = cy22393_open();
	if ( fd < 0 )
	{
		DEBUG( "22393 device open fail..." );
		exit( -1 );
	}
	refclk_74_25MHz( fd, CY22393ADDR );
	close( fd );
	default_7842();
}


/*
 * 默认设置 7842
 */
void default_7842()
{
	int fd = adv7842_open();
	if ( fd < 0 )
	{
		DEBUG( "7842 device open fail..." );
		exit( -1 );
	} else {
		DEBUG( "adv7842 starting configuartion..." );
		HDMI_COMP_1080ix50_mod( fd );
	}
	close( fd );
}


void YPbPr_def()
{
	int fd;
	fd = cy22393_open();
	if ( fd < 0 )
	{
		DEBUG( "22393 device open fail..." );
		exit( -1 );
	}
	refclk_27MHz( fd, CY22393ADDR );
	close( fd );

	fd = adv7842_open();
	if ( fd < 0 )
	{
		DEBUG( "7842 device open fail..." );
		exit( -1 );
	} else {
		/* YPbPr */
		YPbPr_720px60_mod( fd );
		close( fd );
	}
}


void cvbs_def()
{
	int fd;
	fd = cy22393_open();
	if ( fd < 0 )
	{
		DEBUG( "22393 device open fail..." );
		exit( -1 );
	}

	refclk_74_25MHz( fd, CY22393ADDR );
	close( fd );

	fd = adv7842_open();
	if ( fd < 0 )
	{
		DEBUG( "7842 device open fail..." );
		exit( -1 );
	} else {
		CVBS_576ix50_mod( fd );
		close( fd );
	}
}


/*
 * 在手册中使用的频率与开发板使用的晶振频率不同
 * 手册中使用的是28.63636MHz
 * 开发板使用的是74.25MHz
 * 但都是8行扫描
 * 只是做了个公式换算
 *
 */
static int calc_v( int ypbpr_hdmi )
{
	int ret;
	ret = -1;

	if ( !ypbpr_hdmi )
	{
		DEBUG( "ypbpr_hdmi input error %d", ypbpr_hdmi );
		return(ret);
	}

	ret = ypbpr_hdmi * 74.25 / 28.63636 / 8;

	return(ret);
}


/* YPbPr 和 HDMI 格式解析 */
int pare_YPbPr_HDMI()
{
	int		result, fd;
	uint8_t		addr, ch1bl_h, ch1bl_l;
	uint8_t		ch1lcf_h, ch1lcf_l;
	unsigned short	bl_, bl_back, bl_calc, lcf;

	result = -1;

	if(get_unlock_menu_exit())
		return 0;

	fd = adv7842_open();
	if ( fd < 0 )
	{
		DEBUG( "7842 device open fail..." );
		return(result);
	} else {
		/*
		 * DEBUG("adv7842 starting configuartion...");
		 * common_set(fd);// 初始化7842
		 */

		addr = 0x44 / 2;


		/*
		 * CH1_BL[13:0], Addr 44 (CP), Address 0xB1[5:0]; Address 0xB2[7:0] (Read Only)
		 * A readback for the Block Length for sync channel 1. Number of crystal clock cycles in a block of
		 * eight lines of incoming video. This readback is valid if CH1_STDI_DVALID is high
		 * CH1_BL
		 * while ( (ch1bl_h = get_i2c_register_value(fd,addr,0xB1))&0x08 != 1);
		 * do{
		 * ch1bl_h = get_i2c_register_value(fd,addr,0xB1); //& 0x80;
		 * ch1bl_h = ch1bl_h >> 3;
		 * DEBUG("ch1bl_h[7]=%d",ch1bl_h);
		 * usleep(100);
		 * }while(!ch1bl_h);
		 * bl 最高位必须为1
		 */
		ch1bl_h = get_i2c_register_value( fd, addr, 0xB1 );
		ch1bl_l = get_i2c_register_value( fd, addr, 0xB2 );


		/*
		 * DEBUG("STDI  CH1_BL =0x%x    0xB1[5:0] \n", ch1bl_h);
		 * DEBUG("STDI  CH1_BL =0x%x    0xB2[7:0] \n", ch1bl_l);
		 */
		bl_back = ( (ch1bl_h << 8) | ch1bl_l);
		/* DEBUG("bl_back %x",(bl_back )); */
		bl_ = bl_back & 0x3fff;
		/* DEBUG("bl_ %d",bl_ ); */
		bl_calc = calc_v( bl_ );
		/* DEBUG("calc_v =%d ",bl_calc); */


		/*
		 * CH1_LCF[10:0], Addr 44 (CP), Address 0xB3[2:0]; Address 0xB4[7:0] (Read Only)
		 * A readback for the sync channel 1 Line Count in a Field
		 * Number of lines between two VSyncs measured on sync channel 1. The readback from this field is valid if CH1_STDI_DVALID is high.
		 * CH1_LCF
		 */
		ch1lcf_h	= get_i2c_register_value( fd, addr, 0xB3 );
		ch1lcf_l	= get_i2c_register_value( fd, addr, 0xB4 );

		close( fd );


		/* lcf */
		lcf	= (ch1lcf_h << 8) | ch1lcf_l;
		lcf	= lcf & 0x7ff;
		/* DEBUG("lcf =%d  %x",lcf,lcf); */

		if ( (bl_calc == Full_sampling_point1080I50Hz) || (bl_calc >= Full_sampling_point1080I50Hz + 20) || /* YPbPr 1080I 50Hz [2640] */
		     (bl_calc >= Full_sampling_point1080I50Hz - 20) )
		{
			/* ch1lcf 562 */
			if ( (lcf == ch1lcf1080i) || (lcf >= ch1lcf1080i + 20) ||
			     (lcf >= ch1lcf1080i - 20) )
			{
				result = YPbPr1080Isign50Hz;
			} else {
				result = -1;
			}
		} else if ( (bl_calc == Full_sampling_point1080I60Hz) || (bl_calc >= Full_sampling_point1080I60Hz + 20) || /* YPbPr 1080I  60Hz */
			    (bl_calc >= Full_sampling_point1080I60Hz - 20) )
		{
			/* ch1lcf 562 */
			if ( (lcf == ch1lcf1080i) || (lcf >= ch1lcf1080i + 20) ||
			     (lcf >= ch1lcf1080i - 20) )
			{
				result = YPbPr1080Isign60Hz;
			} else {
				result = -1;
			}
		} else if ( (bl_calc == Full_sampling_point720P50Hz) || (bl_calc >= Full_sampling_point720P50Hz + 20) ||/* YPbPr 720P 50Hz */
			    (bl_calc >= Full_sampling_point720P50Hz - 20) )
		{
			/* ch1lcf 562 */
			if ( (lcf == ch1720p) || (lcf >= ch1720p + 20) ||
			     (lcf >= ch1720p - 20) )
			{
				result = YPbPr720Psign50Hz;
			} else {
				result = -1;
			}
		} else if ( (bl_calc == Full_sampling_point720P60Hz) || (bl_calc >= Full_sampling_point720P60Hz + 20) || /* YPbPr 720P 60Hz */
			    (bl_calc >= Full_sampling_point720P60Hz - 20) )
		{
			/* ch1lcf 562 */
			if ( (lcf == ch1720p) || (lcf >= ch1720p + 20) ||
			     (lcf >= ch1720p - 20) )
			{
				result = YPbPr720Psign60Hz;
			} else {
				result = -1;
			}
		} else {
			/*
			 * DEBUG("bl_back %x",(bl_back ));
			 * DEBUG("bl_ %x",(bl_ ));
			 * DEBUG("calc_v =%d ",bl_calc);
			 * DEBUG("lcf =%d  %x",lcf,lcf);
			 */
			result = -1;
		}
	}


	return(result);
}


/* CVBS */
int pare_cvbs()
{
	int		result, fd;
	uint8_t		addr, sdp_std, sdp_interlaced;
	uint8_t		sdp_ad_50_60_hz, hz_50_60;
	unsigned short	bl_, Line_by_line = -1;
	result = -1;


	/*
	 * 时钟设定
	 * 74.25mhz
	 */

	fd = cy22393_open();
	if ( fd < 0 )
	{
		DEBUG( "22393 device open fail..." );
		return(result);
	}

	refclk_27MHz( fd, CY22393ADDR );
	close( fd );

	sdp_ad_50_60_hz = -1;
	hz_50_60	= -1;


	addr = 0x90 / 2;

	fd = adv7842_open();
	if ( fd < 0 )
	{
		DEBUG( "7842 device open fail..." );
		return(result);
	} else {
		/* common_set(fd);// 初始化7842 */

		sdp_std		= get_i2c_register_value( fd, addr, 0x52 );
		sdp_interlaced	= (get_i2c_register_value( fd, addr, 0x57 ) & 0x08) >> 3;
		sdp_ad_50_60_hz = (get_i2c_register_value( fd, addr, 0x59 ) & 0x08) >> 3;
		close( fd );


		/*
		 * DEBUG("STDI SDP_STD =0x%x                            0x52[3:0] \n", sdp_std);
		 * DEBUG("STDI SDP_INTERLACED =0x%x             0x57[3] \n", sdp_interlaced);
		 * DEBUG("STDI SDP_AD_50_60_HZ =0x%x            0x59[3] \n", sdp_ad_50_60_hz);
		 * NTSC_60_ROW 0x00
		 * NTSC_60_NOTROW 0x01
		 * NTSC_50_NOTROW 0x02
		 * NTSC_50_ROW 0x03
		 */


		/*
		 * PAL_60_ROW 0x04
		 * PAL_60_NOTROW 0x05
		 * PAL_50_NOTROW 0x06
		 * PAL_50_ROW 0x07
		 */
		switch ( sdp_std & 0x0F )
		{
		case 0x04:      /* ntsc */
		case 0x06:
		case 0x0c:
		case 0x0E:
			/* do ... */
			bl_ = ntsc;
			break;
		case 0x00:      /* pal */
		case 0x02:
			/* do ... */
			bl_ = pal;
			break;
		default:
			bl_ = error;
			DEBUG( "sdp_std detection error %d", sdp_std & 0x0F );
			break;
		}

		/* 是逐行还是非逐行 */
		if ( 0 == sdp_interlaced )              /* 逐行 */
		{
			Line_by_line = linebyline;
		} else if ( 1 == sdp_interlaced )       /* 非逐行 */
		{
			Line_by_line = linebylineno;
		}

		/* 60Hz and 50hz */
		if ( hz60 == sdp_ad_50_60_hz )          /* 逐行 */
		{
			hz_50_60 = hz60;
		} else if ( hz50 == sdp_ad_50_60_hz )   /* 非逐行 */
		{
			hz_50_60 = hz50;
		}

		/* 以下对8种可能模式进行设定 */
		if ( (bl_ == ntsc) && (Line_by_line == linebyline)
		     && (hz_50_60 == hz60) )
			result = NTSC_60_ROW;
		else if ( (bl_ == ntsc) && (Line_by_line == linebylineno)
			  && (hz_50_60 == hz60) )
			result = NTSC_60_NOTROW;
		else if ( (bl_ == ntsc) && (Line_by_line == linebylineno)
			  && (hz_50_60 == hz50) )
			result = NTSC_50_NOTROW;
		else if ( (bl_ == ntsc) && (Line_by_line == linebyline)
			  && (hz_50_60 == hz50) )
			result = NTSC_50_ROW;
		else if ( (bl_ == pal) && (Line_by_line == linebyline)
			  && (hz_50_60 == hz60) )
			result = PAL_60_ROW;
		else if ( (bl_ == pal) && (Line_by_line == linebylineno)
			  && (hz_50_60 == hz60) )
			result = PAL_60_NOTROW;
		else if ( (bl_ == pal) && (Line_by_line == linebylineno)
			  && (hz_50_60 == hz50) )
			result = PAL_50_NOTROW;
		else if ( (bl_ == pal) && (Line_by_line == linebyline)
			  && (hz_50_60 == hz50) )
			result = PAL_50_ROW;
	}

	return(result);
}


/* lcd open */
int lcd_open()
{
	int lcdfd;
	lcdfd = open( "/dev/lcd16207", O_RDWR | O_SYNC );
	if ( lcdfd < 0 )
	{
		DEBUG( "lcd open error\n" );
	}

	ioctl( lcdfd, CLEAR );

	return(lcdfd);
}


/* key open */
int key_open()
{
	int keyfd;
	keyfd = open( "/dev/dvbkey", O_RDWR );
	if ( keyfd < 0 )
	{
		DEBUG( "key open error\n" );
	}

	return(keyfd);
}


