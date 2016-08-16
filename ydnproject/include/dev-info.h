/*
 * Format of Version 2 AVI InfoFrame
 * The format of the Version 2 AVI InfoFrame is backward compatible with Version 1. All of the fields that
 * were contained in the Version 1 AVI InfoFrame are also contained in the Version 2 AVI InfoFrame. Their
 * purpose and use remain unchanged. All fields of the Version 2 AVI are described here.
 *
 * Format of Version 2 AVI InfoFrame:show
 * ___________________________________________________________________________________
 | InfoFrame Type Code     | InfoFrame Type = 0216                                 |
 ||__________________________|_______________________________________________________|
 | InfoFrame Version Number | Version = 0216                                        |
 ||__________________________|_______________________________________________________|
 | Length of AVI InfoFrame  | Length of AVI InfoFrame (13)                          |
 ||__________________________|_______________________________________________________|
 | Data Byte 1                     | F17=0 | Y1  | Y0  | A0  | B1  | B0  | S1  | S0        |
 ||__________________________|_______|_____|_____|_____|_____|_____|_____|___________|
 | Data Byte 2                     | C1    | C0  | M1  | M0  | R3  | R2  | R1  | R0        |
 ||__________________________|_______|_____|_____|_____|_____|_____|_____|___________|
 | Data Byte 3                     | ITC   |EC2  | EC1 | EC0 | Q1  | Q0  | SC1 | SC0       |
 | Data Byte 4                     | F47=0 |VIC6 | VIC5|VIC4 |VIC3 |VIC2 |VIC1 |VIC0       |
 | Data Byte 5                     | F57=0 |F56=0|F55=0|F54=0| PR3 | PR2 | PR1 | PR0       |
 ||__________________________|_______|_____|_____|_____|_____|_____|_____|___________|
 | Data Byte 6                     | Line  Number of End of Top Bar (lower 8 bits)         |
 ||__________________________|_______________________________________________________|
 | Data Byte 7                     | Line  Number of End of Top Bar (upper 8 bits)         |
 ||__________________________|_______________________________________________________|
 | Data Byte 8                     | Line  Number of Start of Bottom Bar (lower 8 bits)    |
 ||__________________________|_______________________________________________________|
 | Data Byte 9                     | Line  Number of Start of Bottom Bar (upper 8 bits)    |
 ||__________________________|_______________________________________________________|
 | Data Byte 10                            | Pixel Number of End of Left Bar (lower 8 bits)        |
 ||__________________________|_______________________________________________________|
 | Data Byte 11                            | Pixel Number of End of Left Bar (upper 8 bits)        |
 ||__________________________|_______________________________________________________|
 |
 */

#ifndef __DEV_INFO__H
#define __DEV_INFO__H

#include <stdint.h>


/*
 *
 * 信息源视频ID码
 *
 *
 */
/*
 * "d" 表示 "."
 * "or" 选择其一
 * 
 */
typedef enum  {
	/* 640x480p @ 59.94/60Hz */
	v640x480p_59d94_or_60Hz = 1,
	v720x480p_59d94_or_60Hz,
	v720x480p_59d94_or_60Hz3,
	v1280x720p_59d94_or_60Hz = 4,
	v1920x1080i_59d94_or_60Hz,
	/* 720(1440)x480i @ 59.94/60Hz */
	v720_or_1440x480i_59d94_or_60Hz,
	v720_or_1440x480i_59d94_or_60Hz7,
	/* 720(1440)x240p @ 59.94/60Hz */
	v720_or_1440x240p_59d94_or_60Hz = 8,
	v720_or_1440x240p_59d94_or_60Hz9,
	/* 59.94/60Hz */
	v2880x480i_59d94_or_60Hz = 10,
	v2880x480i_59d94_or_60Hz11,
	v2880x240p_59d94_or_60Hz = 12,
	v2880x240p_59d94_or_60Hz13,
	v1440x480p_59d94_or_60Hz = 14,
	v1440x480p_59d94_or_60Hz15,
	v1920x1080p_59d94_or_60Hz = 16,
	/* 50Hz */
	v720x576p_50Hz,
	v720x576p_50Hz18,
	v1280x720p_50Hz = 19,
	v1920x1080i_50Hz,
	v720_or_1440x576i_50Hz,
	v720_or_1440x576i_50Hz22,
	v720_or_1440x288p_50Hz = 23,
	v720_or_1440x288p_50Hz24,
	v2880x576i_50Hz = 25,
	v2880x576i_50Hz26,
	v2880x288p_50Hz = 27,
	v2880x288p_50Hz28,
	v1440x576p_50Hz = 29,
	v1440x576p_50Hz30,
	v1920x1080p_50Hz = 31,
	/* 23.98/24Hz */
	v1920x1080p_23d98_or_24Hz,
	/* 25Hz */
	v1920x1080p_25Hz,
	/* 29.98/30Hz */
	v1920x1080p_29d98_or_30Hz,
	/* 59.94/60Hz */
	v2880x480p_59d94_or_60Hz,
	v2880x480p_59d94_or_60Hz36,
	/* 50Hz */
	v2880x576p_50Hz = 37,
	v2880x576p_50Hz38,
	v1920x1080i_or_1250_50Hz = 39,
	/* 100Hz */
	v1920x1080i_100Hz,
	v1280x720p_100Hz,
	v720x576p_100Hz,
	v720x576p_100Hz43,
	v720_or_1440x576i_100Hz = 44,
	v720_or_1440x576i_100Hz45,
	/* 119.88/120Hz */
	v1920x1080i_119d88_or_120Hz = 46,
	v1280x720p_119d88_or_120Hz,
	v720x480p_119d88_or_120Hz,
	v720x480p_119d88_or_120Hz49,
	v720_or_1440x480i_119d88_or_120Hz = 50,
	v720_or_1440x480i_119d88_or_120Hz51,
	/* 200Hz */
	v720x576p_200Hz = 52,
	v720x576p_200Hz53,
	v720_or_1440x576i_200Hz = 54,
	v720_or_1440x576i_200Hz55,
	/* 239.76/240Hz */
	v720x480p_239d76_or_240Hz = 56,
	v720x480p_239d76_or_240Hz57,
	v720_1440x480i_239d76_or_240Hz = 58,
	v720_1440x480i_239d76_or_240Hz59,
} sourcecodeID;

/*
 * 视频源信息频率
 * "_" 表示 "."
 */
typedef enum {
	v50Hz = 1,
	/* v59_94Hz, */
	v60Hz = 3,
	v100Hz,
	v119_88Hz,
	v120Hz,
	v200Hz,
	v239Hz,
	v240Hz,
	vlowfieldrate,
} sorcefreq;

/* ������״̬�� */
struct h64_gpio_et_cfg_t {
	int (*ad9789_cfg)( void );

	int	(*adv7842_cfg)( void );
	void	(*gpio_cfg)( void );

	void	(*h46_cfg)( int fd );
	int	(*open_h46dev)( void );

	int	(*open_9789dev)( void );
	int	(*open_7842dev)( void );
};


struct dvb_peripheral {
	sourcecodeID	codeid;
	sorcefreq	video_hz;
	unsigned int	i2c_0addr;
	unsigned int	mutex_falg; /* 用于区分带小数的hz，还是不带小数的hz */
	/*
	 * 这个暂时用于保留位，后期根据需求是否
	 * 在设定
	 */

	int	i2c_0ordernumber;
	int	i2c_1ordernumber;

	int	(*open_clockdev)( void );
	int	(*open_freqdev)( void );
	int	(*open_uartdev)( void );

	void	(*clockset)( int clockfd, unsigned int slave_addr );    /* 22393 */
	void	(*pixefreq)( int freqfd );                              /* 7842 */
	void	(*modopcodeset)( int fd );                              /* h46 */
};
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

int key_open( void );


int lcd_open( void );


/* 7842 默认设置 */
void default_7842();


void cvbs_def();


void YPbPr_def();


void usercode_mod( int modcod, int ypbpr_cvbs, int);


void adv_defualt_set();


int read_vic();


int cy22393_open();


int adv7842_open();


/* 分量 */
int pare_YPbPr_HDMI();


int pare_cvbs();


void ypbpr_cvbs_cfg();


void hdmi_gpio_dfg();


void lcd_bkl_offon( uint8_t on_off );


struct dvb_peripheral * peripheral_config();


struct h64_gpio_et_cfg_t * h64gpio_et_cfg();


int hdmi_detection( int r_ratio );


sorcefreq freq_parse_token( int resolution_ratio );


#endif