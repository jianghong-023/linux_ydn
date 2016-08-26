#ifndef __COMMON_CONF__H
#define __COMMON_CONF__H

#include <stdint.h>


struct MenuItem {                               /* 结构体名称 */
	uint8_t			MenuCount;      /* 当前层节点数、每层菜单能显示的条目数 */
	char			*DisplayString; /* 指向菜单标题的字符串 */
	uint8_t			(*Subs)();      /* 指向当前状态应该执行的功能函数 */
	struct   MenuItem	*ChildrenMenus; /* 指向当前菜单的下级菜单 */
	struct   MenuItem	*ParentMenus;   /* 指向当前菜单的上级菜单 */
	uint8_t			item;           /* ��¼�ļ��ĵ�ǰλ�ã�����usb��дʱ����Ҫ��ȡDesplayPoint��ֵ */
	uint8_t			filepathpos;    /* ��¼�ļ�·��λ�� */
	uint8_t			offon;
} /*Null*/;


struct cache_t {
	void	(*w_cahebuf)();
	int8_t	use_item;
};


typedef struct {
	char	i_bdsp_length;          /* ��ʾָʾ�ַ����� */
	char	i_ldsp_length;          /* ��ʾָʾ�ַ����� */
	char	desplay_b_pos;          /* ��ʾָʾ�ַ�ǰλ�� */
	char	desplay_l_pos;          /* ��ʾָʾ�ַ�ǰλ�� */
	char	b_dsp[3];               /* �ַ� */
	char	l_dsp[5];               /* �ַ� */
	int	contian_decimal_length; /* �����ַ��ܳ��� */
	int	_power;                 /* ���� n */
}lcd_b_last_dsp_t;

typedef struct dis_contrl_t dis_contrl;

struct dis_contrl_t {
	int8_t	DisplayStart;           /* 显示菜单的开始位置 */
	int8_t	UserChoose;             /* 用户的选择 */
	int8_t	DisplayPoint;           /* 显示菜单的编号 */
	int8_t	DisplayLine;
	int8_t	MaxItems;               /* 菜单的数量 */
	int8_t	Option;                 /* 保存用户的菜单编号 */
	int8_t	escOption;              /* ���˷�֧���� enert = 0x01,esc= 0x00 */
	int8_t	keyValue;
	int8_t	record_auto_flag;      /* �Զ�������־ */
	int8_t	Modulator;
	int8_t	recoredFrist;
	int8_t	write_size;             /* 字符串长度 */
	int8_t	write_char_dig_status;  /* 0x01 为数值状态，0x02为字符状态, */
	/* 0x03为菜单配置 ,0x04 usb读写,0x05 视频锁状态 ,0x06 密码锁状态 */
	float		cfg_size;       /* 配置数值 */
	uint32_t	f_status;       /* 0x02 整数，0x01浮点 */

	float	restimeuse;             /* 记录当前时间 */
	float	baktimeuse;             /* 备份当前时间 */
	int8_t	changemenuflag;         /* 按键输入字符时改变标志 */
	int8_t	rightliftmove;          /* 左右移动位置 */
	int8_t	updownchoose;           /* 上下字符选择 */
	int8_t	affirmRecode;
	int8_t	add_sub_status;         /* 0x01加，0x02减 */
	int8_t	rig_lifet_status;       /* 0x01左，0x02右 */
	int8_t	curos_stat;             /* 0x01 开，0x00 关,0x02 锁定,0x03 进入显示预览开启 */
	int8_t	nomor_status;           /* 一般性无输入选择状态 0x01 开启，0x02关闭 */
	int8_t	run_status;             /* 一般性无输入选择状态 0x01 开启，0x02关闭 */
	int8_t	aar_dot_stat;
	int8_t	lock_dspaly_status;
	int8_t	lcd_time_key;           /* 0x1 loop ,0x00 not */
	uint8_t call_loop_enter;        /* ģ��enter */
	uint8_t call_stop_enter;        /* ������esc.enter��ʱ */
	uint8_t call_time_enter;        /* 1 ��Ч ��0 ��Ч */
	uint8_t pwd_error_flag;

	/* 按键/lcd 描述符 changeflag为0x01时，表示进入时是enter键 */
	int	lcdfd, keyfd, changeflag;
	int	keyoff;                 /* 记录按下时读写usb,不管是读还是在写时 */
	/* 同一时间只能是读或者是写，不可即读又写 */

	/* 实时缓存 */
	char	cechebuf[17];
	char	filename[17];           /* 文件名称 */
	char	cfg_menu[17];
	char	lcd_locknote[17];
	int	input_lenth;
	double	max_regin;              /* ���ֵ�ķ�Χ */
	double	min_region;

	/* 菜单的配置项 */
	char	**arr;
	uint8_t usb_wr_flag;
	int	enter_status;
	/* pid状态 */
	uint8_t pid_status; /* 0x01 开启,0x02关闭 */
	/* 通用函数 */
	void	( * wd_configfile)( char *orgbuf, char *repbuf );
	int	( * menu_cfg_fun)( char *str );
	uint8_t (*WR_usb)();
	void	( * lcd_loopdsplay)();

	lcd_b_last_dsp_t lcd_b_last_dsp;

	int8_t read_status; /* ֻ��� usb ��ʱ��Ч�������˱�־λ���� */
	int8_t delay_statusl;

	int8_t rtr_count;
};


typedef struct {
	int8_t	rtr_option;
	int8_t	rtr_displaystart;
	int8_t	rtr_arrlenght;
	struct  MenuItem *p_menupoint;
	struct  MenuItem *p_chaild_menupoint;
}menu_return_arr_t;


void paren_menu( void );


void current_menu( void );


int lcd_main( void );


void user_desplay_monitor( void );


void user_operation_usb( void );


void ChangeMenu( int keySigNum );


void init_lcddefault_donfig( void );


void all_config_s( void );


dis_contrl *discontrl_t();


uint8_t NullSubs( void );


void null_Subfun( char *orgbuf, char *repbuf );


int null_Subcfg( char *cfgbuf );


void res_time();


void null_cahebuf( void );


void NullInput( char length );


void dev_config_printf();


void change_seach();


void gcvt_char( double change_num, int precision, char *change_array );


void video_status_lock( void );


void gener_table();


int set_system_time( char *strdt );


void get_system_time( int *day, int *hours, int *min, int *sec );


#endif

