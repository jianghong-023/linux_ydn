#define __LCD1602_H__

#include <assert.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <usb_signal.h>
#include <adv7842_config.h>
#include <readbus.h>

#include <usbhotplug.h>
#include <sys/time.h>
#include <writebus.h>
#include <menu_foun.h>

#include <debug.h>
#include <key-ctl.h>
#include <usbinfo.h>
#include <common_conf.h>

#include <lcd1602.h>
#include <dev-info.h>
#include <main_call.h>
#include <config_menu_opt.h>
#include <gpio_config.h>


extern struct cache_t	*cache_pr;
extern struct  MenuItem * MenuPoint;
extern uint8_t		t_date[16];
extern struct cache_t	*cachep;
extern int		progr_bar;
extern DSPLAY_SIGNAL_SET_T* get_DSPLAY_SIGNAL_SET_T();


extern void gpiocfg( struct h64_gpio_et_cfg_t *pcfg );


extern void gpio_mask( uint8_t setval );


menu_return_arr_t menu_return_arr[8];

static time_t	lasttime;
static int	start_machine, progr_bar_flag = 0;


uint8_t scan_key_resvale( uint8_t choose );


static void sighandler_str( int sgn );


static void monitor_opt( int coder, int flags );


static void sys_start_prompt( void );


static void set_rtr_arr( signed char userChoose, signed char displaystart, uint8_t subscripts );


static void loop_hand( void );


static void esc_rtr_arr( uint8_t subscripts );


static void mod_rtr_arr( signed char displaystart, uint8_t subscripts );


static int dsplay_append_bstr( dis_contrl * dis_contrl, char *lstr );


static void append_str_dsplat_proc( dis_contrl * dis_contrl, int, int );


static int dsplay_append_lstr( dis_contrl * dis_contrl, char *bstr );


static char* display_input( double cfg_data, char * org_str, int *lenth );


static char* display_input_str( char *str, int size, int *lenth );


static void menu_selected_cfg( char **arr, int size, char * );


static int check_pwrd( char *pwd );


static int access_cheak();


static int receive_ds_str( char* str_, char *str, int size );


static void receive_ds_menu( int );


static void curos_dsp( int optcode );


static int index_char_dsp( int *ch_count, int *bakcount, int *count, char*str_ );


static int write_char( int size, int interval, int change, char *data );


static int course_move( int move_count );


static void lcd_dsplay_and_cursor_move( int *count, int* bakcount, int pre_count, char *bakarr, int pos );


static void data_process_and_cursor_move( int bakcount, int pre_count, char date, int pos );


static int system_script_config( dis_contrl *_contrl, char *, int affirmRecode );


static char* get_input_buffer_str( char *str, char * in_buffer, int size_t );


static int change_cfg_menu( signed char keynumber );


static void function_call();


static int menu_browse_change( int keynumber );


static void ShowMenu();


static void recode_menu( void );


static void change_stats( int enrt, int frist_f, int cursor_s, int input_s, int affirm );


static int lock_enter_input_pwd( int dsplay_status, int keynumber );


static void lock_enter_input_prompt( int dsplay_status );


static void m_backlighting( uint8_t _backlighting );


static int m_timers( int scend );


static void last_time();


static void cl_time();


static int pwd_state();


static struct timeval tpstart;

static int	sysstart = 0;
dis_contrl	discontrl;


extern s_config config;

enum ctrl_comm {
	PUT_CHAR	= 0x0,
	CLEAR		= 0x1,
	COURSOR_POST	= 0x3,
};

enum {
	up	= 0x7e,
	lift	= 0x7d,
	rigth	= 0x7b,
	down	= 0x77,
	esc	= 0x6f,
	lock	= 0x5f,
	enter	= 0x3f,
	post	= 0x00,
	post2	= 0x40,
};

enum usr_che {
	nullfunation	= 0x00,
	tsfilename	= 0x01,
};

void  init_lcddefault_donfig()
{
	discontrl.DisplayStart		= DEFAULTE;
	discontrl.UserChoose		= DEFAULTE;
	discontrl.DisplayPoint		= DEFAULTE;
	discontrl.DisplayLine		= DEFAULTELINE;
	discontrl.MaxItems		= DEFAULTE;
	discontrl.Option		= DEFAULTE;
	discontrl.keyValue		= DEFAULTKEY;
	discontrl.Modulator		= DEFAULTE;
	discontrl.recoredFrist		= DEFAULTE;
	discontrl.write_size		= DEFAULTE;
	discontrl.restimeuse		= DEFAULTE;
	discontrl.baktimeuse		= DEFAULTE;
	discontrl.lcdfd			= DEFAULTE;
	discontrl.keyfd			= DEFAULTE;
	discontrl.record_auto_flag	= DEFAULTE;
	discontrl.keyoff		= DEFAULTE;
	discontrl.changeflag		= DEFAULTE;
	discontrl.changemenuflag	= DEFAULTE;
	discontrl.rightliftmove		= DEFAULTE;
	discontrl.updownchoose		= DEFAULTE;
	discontrl.lock_dspaly_status	= DEFAULTE;
	discontrl.enter_status		= DEFAULTE;
	discontrl.read_status		= 0x00;
	discontrl.rtr_count		= 0;
}


/*****************************************************************************/
uint8_t scan_code[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'n', 'm', 'l', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
			'E', 'F', 'G', 'H', 'I', 'J',
			'K', 'N', 'M', 'L', 'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z',
			'-', '*', '&', '@', ' ', ':' };

uint8_t key_scan_code[] = { up, down, lift, rigth, enter, esc };
/*****************************************************************************/


dis_contrl *discontrl_t()
{
	return(&discontrl);
}


/* all sys config */
void all_config_s()
{
	flush_conf();
	system_conf();
	sys_ect_conf();
}


/* 时间戳 */
void res_time()
{
	gettimeofday( &tpstart, NULL );
	discontrl.restimeuse = 1000000 * (tpstart.tv_sec) + tpstart.tv_usec;
}


/* 按键索引值 */
uint8_t scan_key_resvale( uint8_t choose )
{
	return(scan_code[choose]);
}


/* 父菜单 */
void paren_menu( void )
{
	recode_menu();
	ShowMenu();
}


/* 当前 菜单 */
void current_menu( void )
{
	signed char count = discontrl.rtr_count;
	if ( count < 0 )
		count = 0;

	esc_rtr_arr( count );

	if ( MenuPoint[discontrl.DisplayStart].ChildrenMenus != NULL )
		MenuPoint = MenuPoint[discontrl.DisplayStart].ChildrenMenus;

	discontrl.changemenuflag	= DEFAULTE;
	discontrl.rightliftmove		= DEFAULTE;
	discontrl.updownchoose		= DEFAULTE;

	ShowMenu();
}


/*
 * lcd 背光打开/关闭
 *
 */

static void m_backlighting( uint8_t _backlighting )
{
	lcd_bkl_offon( _backlighting );
}


/*
 * 上一次开始时间
 *
 */
static void last_time()
{
	time( &lasttime );
}


/*
 * 定时器
 * scend        定时时间，如2毫秒 scend = 2
 *
 * 超时返回1，否则返回0
 *
 */
static int  m_timers( int scend )
{
	int	retime = 0;
	time_t	s_nowtime;
	time( &s_nowtime );

	if ( (s_nowtime - lasttime) > scend )
		retime = 1;


	return(retime);
}


/*
 * 清定时器
 */
static void cl_time()
{
	time_t s_nowtime;
	time( &s_nowtime );
	lasttime = s_nowtime;
}


/*
 * 记录菜单选择路径
 *
 */
static void  recode_menu()
{
	signed char count = --discontrl.rtr_count;
	if ( count < 0 )
		count = 0;

	esc_rtr_arr( count );

	if ( MenuPoint[discontrl.DisplayStart].ParentMenus != NULL )
		MenuPoint = MenuPoint[discontrl.DisplayStart].ParentMenus;      /* 指向父菜单 */

	discontrl.changemenuflag	= DEFAULTE;                             /* 置0 */
	discontrl.rightliftmove		= DEFAULTE;
	discontrl.updownchoose		= DEFAULTE;
}


int null_fun()
{
	return(0);
}


signed char resval()
{
	/* 读文件记录位置 */
	MenuPoint[discontrl.DisplayPoint].item = discontrl.UserChoose;
	return(MenuPoint[discontrl.DisplayPoint].item);
}


static int write_char( int size, int interval, int change, char *data )
{
	int i;

	for ( i = 0; i < size; i++ )
		key_input_char( i + interval, data[i], change, discontrl.lcdfd );
	return(i);
}


static int course_move( int move_count )
{
	int i;
	for ( i = 0; i < move_count; i++ )
		key_input_char( i, ' ', CHANGE_G, discontrl.lcdfd );

	return(i);
}


/* pre_count 预增数 */
static void lcd_dsplay_and_cursor_move( int *count, int* bakcount,
					int pre_count, char *bakarr, int pos )
{
	key_input_char( *count + pre_count, bakarr[*bakcount = *count], CHANGE_G, discontrl.lcdfd );
	cursor_move( discontrl.lcdfd, LCD_SUROS_MOVE, pos );
}


static void data_process_and_cursor_move( int bakcount, int pre_count, char date, int pos )
{
	key_input_char( bakcount + pre_count, date, CHANGE_G, discontrl.lcdfd );
	cursor_move( discontrl.lcdfd, LCD_SUROS_MOVE, pos );
}


static int system_script_config( dis_contrl *_contrl, char *org_str_, int affirmRecode )
{
	int res = -1;
	DEBUG( "%s \n", _contrl->cechebuf );

	if ( _contrl->wd_configfile != &null_Subfun )
	{
		_contrl->wd_configfile( org_str_, _contrl->cechebuf );
		_contrl->affirmRecode	= affirmRecode;
		res			= 0;
	} else
		res = -1;

	return(res);
}


static char* get_input_buffer_str( char *str, char * in_buffer, int size_t )
{
	char *ret = NULL;
	if ( !str )
		return(ret);

	if ( in_buffer )
	{
		snprintf( in_buffer, size_t + 1, "%s", str );

		return(ret = in_buffer);
	}

	return(ret);
}


static int receive_ds_str( char* str_, char *str, int size )
{
	lcd_clear( discontrl.lcdfd );
	int ch_count = sizeof(scan_code) / sizeof(scan_code[0]);


	/*
	 * 接受选择项
	 * 显示选择项字符在LCD的第-行
	 */
	if ( discontrl.lcd_time_key == LCD_LOOP_ON && discontrl.call_time_enter == 0 )
	{
		if ( discontrl.lcd_loopdsplay != NULL )
			discontrl.lcd_loopdsplay();
		return(ch_count);
	}else
		receive_ds_menu( 16 );

	/* 接受配置数据 */
	if ( str_ )
	{
		memset( str_, ' ', MAXWIDE );
		snprintf( str_, size + 1, "%s", str );

		str_[size] = ' ';
		char *clean_null = "                ";

		write_char( MAXWIDE, 0, CHANGE_G, clean_null );

		/* 显示配置数据在LCD的第二行 */

		write_char( size, 2, CHANGE_G, str_ );
	}


	return(ch_count);
}


static void receive_ds_menu( int lenth )
{
	int i = 0;


	lcd_clear( discontrl.lcdfd );


	/*
	 * 接受选择项
	 * 显示选择项字符在LCD的第-行
	 */
	char *clean_null = "                ";
	write_char( MAXWIDE, 0, CHANGE_G, clean_null );

	if ( discontrl.lcd_locknote[0] != '\0' )
	{
		for (; i < lenth; i++ )
		{
			key_input_char( i, discontrl.lcd_locknote[i], CHANGE_N, discontrl.lcdfd );
		}
	}else{
		for (; i < lenth; i++ )
		{
			key_input_char( i, t_date[i], CHANGE_N, discontrl.lcdfd );
		}
		memset( t_date, 0, MAXWIDE );
	}
	discontrl.recoredFrist = 0x2;
}


static int index_char_dsp( int *ch_count, int *bakcount,
			   int *count, char*str_ )
{
	/* 第先找出当前字符在字库中的位置 */
	int i = 0, pos = -1;
	for ( i = 0; i < *ch_count; i++ )
	{
		if ( str_[*bakcount = *count] == scan_code[i] )
		{
			pos = i;
			break;
			/* return ; */
		}
	}
	return(pos);
}


static void curos_dsp( int optcode )
{
	/* 光标状态 */
	switch ( optcode )
	{
	case LCD_CHAR_DOT:
		key_input_char( 1, ' ', CHANGE_G, discontrl.lcdfd );
		ctl_cursor( discontrl.lcdfd, LCD_CHAR_DOT, 0x40 + 1 );
		break;
	case LCD_CHAR_ARR:
		key_input_char( 1, ' ', CHANGE_G, discontrl.lcdfd );
		ctl_cursor( discontrl.lcdfd, LCD_CHAR_ARR, 0x40 + 1 );
		break;
	}
}


static int  dsplay_append_lstr( dis_contrl * dis_contrl, char *lstr )
{
	int ret = -1;
	memset( lstr, 0, 6 );
	if ( dis_contrl->lcd_b_last_dsp.l_dsp )
	{
		int length = dis_contrl->lcd_b_last_dsp.i_ldsp_length + 1;
		snprintf( lstr, length, "%s", dis_contrl->lcd_b_last_dsp.l_dsp );
		ret = length - 1;
	}
	return(ret);
}


static int  dsplay_append_bstr( dis_contrl * dis_contrl, char *bstr )
{
	int ret = -1;
	memset( bstr, 0, 6 );
	if ( dis_contrl->lcd_b_last_dsp.b_dsp )
	{
		int length = dis_contrl->lcd_b_last_dsp.i_bdsp_length + 1;
		snprintf( bstr, length, "%s", dis_contrl->lcd_b_last_dsp.b_dsp );
		ret = length - 1;
	}

	return(ret);
}


/*
 * 循环显示处理
 */

static void loop_hand( void )
{
	if ( discontrl.lcd_time_key == LCD_LOOP_ON )
	{
		discontrl.lcd_time_key = LCD_LOOP_OFF;

		discontrl.call_loop_enter	= 0;
		discontrl.call_stop_enter	= 1;

		alarm( 0 );
	}
}


static void sighandler_str( int sgn )
{
	if ( SIGALRM == sgn )
	{
		if ( discontrl.lcd_loopdsplay != NULL )
			discontrl.lcd_loopdsplay();


		alarm( 1 );
	}
}


/*
 * lcd 动态显示字符
 *
 * dynamicflgs    0x1循环显示，0x00 非循环显示;
 * keyflage		  enter 键
 */

static void dynamic_string( int dynamicflgs, int keyflage )
{
	static int count = 0;


	if ( discontrl.call_stop_enter == 1 )
	{
		alarm( 0 );
		count				= 0;
		discontrl.call_stop_enter	= 0;
	}


	cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* 关 */

	if ( dynamicflgs == 1 )
	{
		++count;

		if ( count == 1 )
		{
			signal( SIGALRM, sighandler_str );
			alarm( 1 );
		}else if ( count == 2 )
		{
			alarm( 0 );
			count				= 0;
			discontrl.call_loop_enter	= enter;
		}
	}
}


static void append_str_dsplat_proc( dis_contrl * dis_contrl, int bpos, int lpos )
{
	char b_apend[6] = "", l_apend[6] = "";

	int relength = dsplay_append_lstr( &discontrl, l_apend );
	if ( relength > 0 )
	{
		int n;
		for ( n = 0; n < relength; n++ )
		{
			key_input_char( n + lpos, l_apend[n], CHANGE_G, discontrl.lcdfd );
		}
	}
	relength = dsplay_append_bstr( &discontrl, b_apend );
	if ( relength > 0 )
	{
		int n;
		for ( n = 0; n < relength; n++ )
		{
			key_input_char( n + bpos, b_apend[n], CHANGE_G, discontrl.lcdfd );
		}
	}
}


/* 数值及字符处理，用于处理整数，浮点型 */
static char * display_input( double cfg_data, char * org_str, int *lenth )
{
	/*
	 * 第一次需要做清屏工作
	 * 然后在第一行写入选择项
	 * 若不是第一次操作，就无需做清屏操作，更无需写入选着项
	 */
	static char	bak_arr[16]	= "", bb[16] = "", *orgbff = NULL, *bakarr = NULL;
	static char	cmp_arr[16]	= "";
	static int	count		= 0, bakcount, bakneg = -1;/* bakcount 是要记录自加之前的数据，之后数据会造成数据位的加1或者减1 */
	char		*re_orfbuf	= NULL;
	static int	l_r_rocde	= 0, Neg_baklen = 0, in_length = 0;


	/*
	 * lcd 字符的循环显示/或者非循环显示
	 */
	if ( discontrl.recoredFrist == DEFAULTE )
	{
		lcd_clear( discontrl.lcdfd );
		/* 接受配置数据菜单 */
		receive_ds_menu( 16 );

		int i;
		memset( bb, 0, 16 );
		gcvt_char( cfg_data, 3, bb );
		orgbff = strdup( bb ); /* 保留原数据不被修改 */
		/* float */
		if ( discontrl.f_status == INPUT_FLOAT_STATUS )
		{
			memset( bak_arr, ' ', 16 );
			in_length = discontrl.input_lenth;
			int org_length = get_count( bb );
			get_char_filter_to_flaot( org_length, discontrl.input_lenth, bak_arr, bb );
			bakarr = strdup( bak_arr );

			/* 显示配置数据在LCD的第二行 */
			int in_length = discontrl.lcd_b_last_dsp.contian_decimal_length;

			if ( in_length == 0 )
				in_length = strlen( bb );


			write_char( in_length, 2, CHANGE_G, bb );

			append_str_dsplat_proc( &discontrl, discontrl.lcd_b_last_dsp.desplay_b_pos,
						discontrl.lcd_b_last_dsp.desplay_l_pos );
			course_move( 1 );
		} else if ( discontrl.f_status == INPUT_DIGIT_STATUS )
		{
			memset( bak_arr, ' ', 16 );
			int	b_pos, w_pos = 0;
			int	org_length = get_count( bb );
			get_char_filter_to_digit( org_length, discontrl.input_lenth, bak_arr, bb );
			in_length	= discontrl.input_lenth;
			bakarr		= strdup( bak_arr );

			if ( bakarr[0] == '-' )
			{
				if ( (discontrl.input_lenth + 1) > get_count( bakarr ) )
				{
					Neg_baklen				= discontrl.input_lenth;
					bakarr[discontrl.input_lenth]		= '0';
					bakarr[discontrl.input_lenth + 1]	= '\0';
					discontrl.input_lenth			= get_count( bakarr );
					in_length				= Neg_baklen;
				}else{
					Neg_baklen = discontrl.input_lenth;

					discontrl.input_lenth	= get_count( bakarr );
					in_length		= discontrl.input_lenth - 1;
				}
			}

			if ( discontrl.lcd_b_last_dsp.desplay_b_pos == 0 )
				b_pos = 2;
			else
				b_pos = discontrl.lcd_b_last_dsp.desplay_b_pos + 1;

			if ( bakarr[0] == '-' )
				w_pos = b_pos;
			else
				w_pos = b_pos + 1;

			append_str_dsplat_proc( &discontrl, b_pos,
						discontrl.lcd_b_last_dsp.desplay_l_pos );
			/* 显示配置数据在LCD的第二行 */
			i = 0;

			if ( bb[0] == '-' )
			{
				while ( bb[i] != '.' )
				{
					key_input_char( i + w_pos, bb[i], CHANGE_G, discontrl.lcdfd );
					i++;
				}
				bakneg = atoi( bb );
			}else{
				while ( bb[i] != '.' )
				{
					key_input_char( i + w_pos, bb[i], CHANGE_G, discontrl.lcdfd );
					i++;
				}
			}
			course_move( 1 );
		}

		discontrl.affirmRecode = 0; /* 进入时此值已经为1了，所以要清掉 */
	}

	switch ( discontrl.affirmRecode )
	{
	case 0: {
		l_r_rocde			= 0;
		count				= 0;
		discontrl.rig_lifet_status	= LIFTRIGHT_DEF;
		discontrl.add_sub_status	= UPDOWN_DEF;
	}

	break;
	case 1: {
		/* 光标状态 */
		if ( discontrl.curos_stat == CURSOR_ON || discontrl.curos_stat == CURSOR_OFF )
		{
			cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x3 ); /* 开 */
			discontrl.curos_stat		= CURSOR_NOUSER;
			discontrl.rig_lifet_status	= LIFTRIGHT_DEF;

			char *clean_null = "                ";
			write_char( MAXWIDE, 0, CHANGE_G, clean_null );


			int bpos = discontrl.lcd_b_last_dsp.desplay_b_pos;
			if ( bpos == 0 )
				bpos = 2;
			else
				bpos += 1;


			if ( discontrl.f_status == INPUT_FLOAT_STATUS )
			{
				write_char( discontrl.input_lenth, bpos, CHANGE_G, bakarr );
			} else if ( discontrl.f_status == INPUT_DIGIT_STATUS )
			{
				int	len	= get_count( bakarr );
				int	b_pos	= bpos;
				int	w_pos;
				if ( bakarr[0] == '-' )
				{
					b_pos	= bpos;
					w_pos	= bpos;
				}else{
					b_pos	= bpos;
					w_pos	= bpos + 1;
				}
				append_str_dsplat_proc( &discontrl, b_pos,
							discontrl.lcd_b_last_dsp.desplay_l_pos );

				write_char( len, w_pos, CHANGE_G, bakarr );
			}

			count = 0;
		}

		/* 数据显示部分 */
		if ( (discontrl.rig_lifet_status == RIGHT_ADD) || (discontrl.curos_stat == CURSOR_NOUSER) ) /* 右行动时不改变数据 */

		{
			int pos, w_pos;

			if ( l_r_rocde == LIFT_SUB )
				count += 2;


			if ( discontrl.curos_stat == CURSOR_NOUSER )
				discontrl.curos_stat = CURSOR_DEF;

			if ( count >= discontrl.input_lenth || count < 0 )
				count = 0;

			if ( bakarr[count] == '-' )
				count++;        /* 跳过'-'的位置 */


			if ( bakarr[count] == '.' )
				count++;        /* 跳过小数点的位置 */


			l_r_rocde = RIGHT_ADD;

			{
				if ( discontrl.lcd_b_last_dsp.desplay_b_pos == 0 )
					pos = 2;
				else
					pos = discontrl.lcd_b_last_dsp.desplay_b_pos + 1;

				if ( discontrl.f_status == INPUT_DIGIT_STATUS )
				{
					if ( bakarr[0] == '-' )
					{
						char t_buf[2] = "";
						snprintf( t_buf, 2, "%s", discontrl.lcd_b_last_dsp.b_dsp );
						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", "-" );
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );

						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", t_buf );
						discontrl.input_lenth = get_count( bakarr );

						w_pos = pos;
					}else{
						discontrl.input_lenth = in_length;
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );
						w_pos = pos + 1;
					}
				}else{
					append_str_dsplat_proc( &discontrl, pos, discontrl.lcd_b_last_dsp.desplay_l_pos );
					w_pos = pos;
				}

				lcd_dsplay_and_cursor_move( &count, &bakcount, w_pos, bakarr, 0x10 );
			}
			count++;

			/* 并且要清出加左右状态 */
			discontrl.rig_lifet_status = LIFTRIGHT_DEF;
		} else if ( (discontrl.rig_lifet_status == LIFT_SUB) ) /* 左行动时不改变数据 */

		{
			int pos, w_pos;

			if ( l_r_rocde == RIGHT_ADD )
				count -= 2;


			if ( count < 0 || count >= discontrl.input_lenth )
				count = discontrl.input_lenth - 1;

			if ( bakarr[count] == '-' )
				count = discontrl.input_lenth - 1;


			if ( bakarr[count] == '.' )
				--count; /* 跳过小数点的位置 */


			l_r_rocde = LIFT_SUB;

			{
				if ( discontrl.lcd_b_last_dsp.desplay_b_pos == 0 )
					pos = 2;
				else
					pos = discontrl.lcd_b_last_dsp.desplay_b_pos + 1;

				if ( discontrl.f_status == INPUT_DIGIT_STATUS )
				{
					if ( bakarr[0] == '-' )
					{
						char t_buf[2] = "";
						snprintf( t_buf, 2, "%s", discontrl.lcd_b_last_dsp.b_dsp );
						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", "-" );
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );
						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", t_buf );
						discontrl.input_lenth	= get_count( bakarr );
						w_pos			= pos;
					}else{
						discontrl.input_lenth = in_length;
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );
						w_pos = pos + 1;
					}
				}else{
					append_str_dsplat_proc( &discontrl, pos,
								discontrl.lcd_b_last_dsp.desplay_l_pos );
					w_pos = pos;
				}

				lcd_dsplay_and_cursor_move( &count, &bakcount, w_pos, bakarr, 0x10 );
			}

			count--;

			/* 并且要清出加左右状态 */
			discontrl.rig_lifet_status = LIFTRIGHT_DEF;
		}

		/* 数据处理部分 */

		if ( (discontrl.add_sub_status == UP_ADD) || (discontrl.add_sub_status == DOWN_SUB) )
		{
			int	in_len		= discontrl.input_lenth;
			int	inputlength	= in_len;
			if ( discontrl.f_status == INPUT_DIGIT_STATUS )
			{
				if ( bakarr[0] == '-' )
					inputlength = get_count( bakarr ) - 1;
				else
					inputlength = in_length;
			}

			switch ( discontrl.add_sub_status )
			{
			case UP_ADD:
			{
				if ( bakarr[bakcount] == '-' )
					bakcount++;

				if ( bakarr[bakcount] == '.' )
					bakcount++; /* 跳过小数点的位置 */


				if ( discontrl.f_status == INPUT_DIGIT_STATUS )
				{
					double number = atoi( bakarr );

					double	max = discontrl.max_regin;
					int	pos, t_pos = discontrl.input_lenth - bakcount;
					double	temp;
					char	b_b[16] = "";

					pos = t_pos;

					temp = add_sub( &number, pos - 1, TRUE, pos - 1 );

					if ( temp < 0 )
					{
						if ( temp > bakneg )
							temp = discontrl.min_region;
					}else{
						if ( temp > max )
							temp = discontrl.min_region;
					}
					gcvt_char( temp, 3, b_b );
					memset( bak_arr, ' ', 16 );
					int org_length = get_count( b_b );

					get_char_filter_to_digit( org_length, inputlength, bak_arr, b_b );

					bakarr = strdup( bak_arr );
				}else{
					double	number	= atof( bakarr );
					double	max	= discontrl.max_regin;

					int	pos = discontrl.input_lenth - bakcount;
					double	temp;
					char	b_b[16]		= "";
					int	i_power		= discontrl.lcd_b_last_dsp._power;
					int	decimal_length	= i_power;
					/* 小数点判断 */
					char float_bb[16] = "";
					gcvt_char( number, decimal_length, float_bb );
					int i_count = 0, b_true = -1;
					while ( float_bb[i_count++] == '.' && float_bb[i_count] != '\0' )
						b_true = 1;

					if ( pos - 1 < 4 )
					{
						if ( i_power == 0 )
							i_power = 4;
						else
							i_power -= 1;

						int i_pos;

						i_pos = pos - 2;

						if ( b_true )
						{
							if ( i_pos > 0 )
								i_pos -= 1;
						}


						temp = add_sub( &number, i_pos - 1, TRUE, i_pos - i_power ); /* max 0.001 */
					}else{
						if ( i_power == 0 )
							i_power = 5;
						else
							i_power -= 1;

						int i_pos;

						i_pos = pos - 2;

						if ( b_true )
						{
							if ( i_pos > 0 )
								i_pos -= 1;
						}

						temp = add_sub( &number, i_pos - 1, TRUE, i_pos - i_power ); /* 10^2 */
					}


					if ( temp < 0 )
					{
						if ( temp > discontrl.min_region )
							temp = discontrl.min_region;
					}else{
						if ( temp > max )
							temp = discontrl.min_region;
					}


					int org_length;
					if ( decimal_length == 0 )
					{
						decimal_length = 3;
						gcvt_char( temp, decimal_length, b_b );
						org_length = get_count( b_b );
					}else{
						gcvt_char( temp, decimal_length, b_b );
						org_length = get_count( b_b ) + decimal_length;
					}


					memset( bak_arr, ' ', 16 );


					get_char_filter_to_flaot( org_length, inputlength, bak_arr, b_b );

					bakarr = strdup( bak_arr );
				}
				/* 并且要清出加减状态 */
				discontrl.add_sub_status = UPDOWN_DEF;
			}
			break;
			case DOWN_SUB:
			{
				if ( bakarr[bakcount] == '.' )
				{
					/* 跳过小数点的位置 */
					bakcount++;
				}

				if ( discontrl.f_status == INPUT_DIGIT_STATUS )
				{
					double	number	= atoi( bakarr );
					double	max	= discontrl.max_regin;

					int pos = discontrl.input_lenth - bakcount;

					double	temp;
					char	b_b[16] = "";
					temp = add_sub( &number, pos - 1, FALSE, pos - 1 );

					if ( temp < 0 )
					{
						if ( temp < discontrl.min_region )
							temp = max;
					}else{
						if ( temp < discontrl.min_region )
							temp = max;
					}
					gcvt_char( temp, 3, b_b );
					memset( bak_arr, ' ', 16 );
					int org_length = get_count( b_b );

					get_char_filter_to_digit( org_length, inputlength, bak_arr, b_b );

					bakarr = strdup( bak_arr );
				}else{
					double	number	= atof( bakarr );
					double	max	= discontrl.max_regin;
					int	i_power = discontrl.lcd_b_last_dsp._power;

					int	decimal_length	= i_power;
					int	pos		= discontrl.input_lenth - bakcount;
					double	temp;
					char	b_b[16] = "";

					/* 小数点判断 */
					char float_bb[16] = "";
					gcvt_char( number, decimal_length, float_bb );
					int i_count = 0, b_true = -1;
					while ( float_bb[i_count++] == '.' && float_bb[i_count] != '\0' )
						b_true = 1;


					if ( pos - 1 < 4 )
					{
						if ( i_power == 0 )
							i_power = 4;
						else
							i_power -= 1;

						int i_pos;

						i_pos = pos - 2;

						if ( b_true )
						{
							if ( i_pos > 0 )
								i_pos -= 1;
						}

						temp = add_sub( &number, i_pos - 1, FALSE, i_pos - i_power ); /* max 0.001 */
					}else{
						if ( i_power == 0 )
							i_power = 5;
						else
							i_power -= 1;

						int i_pos;

						i_pos = pos - 2;


						if ( b_true )
						{
							if ( i_pos > 0 )
								i_pos -= 1;
						}

						temp = add_sub( &number, i_pos - 1, FALSE, i_pos - i_power ); /* 10^2 */
					}

					if ( temp < 0 )
					{
						if ( temp < max )
							temp = max;
					}else{
						if ( temp < discontrl.min_region )
							temp = max;
					}

					int org_length;


					if ( decimal_length == 0 )
					{
						decimal_length = 3;
						gcvt_char( temp, decimal_length, b_b );
						org_length = get_count( b_b );
					}else{
						gcvt_char( temp, decimal_length, b_b );
						org_length = get_count( b_b ) + decimal_length;
					}


					memset( bak_arr, ' ', 16 );


					get_char_filter_to_flaot( org_length, inputlength, bak_arr, b_b );

					bakarr = strdup( bak_arr );
				}

				discontrl.add_sub_status = UPDOWN_DEF;
			}
			break;
			}

			int ii = 0, pos, tmpos = 0, length = strlen( bakarr );

			if ( discontrl.lcd_b_last_dsp.desplay_b_pos == 0 )
				pos = 2;
			else
				pos = discontrl.lcd_b_last_dsp.desplay_b_pos + 1;


			int w_pos = pos;
			if ( discontrl.f_status == INPUT_DIGIT_STATUS )
			{
				if ( bakarr[0] == '-' )
				{
					w_pos = pos;
				}else{
					w_pos = pos + 1;
				}
			}

			{
				if ( discontrl.f_status == INPUT_DIGIT_STATUS )
				{
					if ( bakarr[0] == '-' )
					{
						char t_buf[2] = "";
						snprintf( t_buf, 2, "%s", discontrl.lcd_b_last_dsp.b_dsp );
						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", "-" );
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );
						snprintf( discontrl.lcd_b_last_dsp.b_dsp, 2, "%s", t_buf );
					}else{
						append_str_dsplat_proc( &discontrl, pos,
									discontrl.lcd_b_last_dsp.desplay_l_pos );
					}
				}else{
					append_str_dsplat_proc( &discontrl, pos,
								discontrl.lcd_b_last_dsp.desplay_l_pos );
				}

				if ( length <= bakcount )
				{
					tmpos = bakcount - 1;
					if ( bakarr[tmpos] == '-' )
						tmpos++;
				}else{
					tmpos = bakcount;
					if ( bakarr[tmpos] == '-' )
						tmpos++;
				}

				for (; ii < length; ii++ )
				{
					if ( cmp_arr[ii] != bakarr[ii] )
					{
						data_process_and_cursor_move( ii, w_pos, bakarr[ii], 0x10 );
					}
				}
			}

			/* 这个操作没有实质性的意义，只是为了防止光标随意跳动 */
			{
				data_process_and_cursor_move( tmpos, w_pos, bakarr[tmpos], 0x10 ); /*  */

				snprintf( cmp_arr, strlen( bakarr ), "%s", bakarr );
			}
		}
	}

	break;
	case 2: {
		/*
		 * 确定将数据写入文件
		 * 光标状态
		 */
		if ( discontrl.curos_stat == CURSOR_DEF )
		{
			cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* 关 */
			discontrl.curos_stat = CURSOR_OFF;                      /* 状态锁定 */
		}

		if ( bakarr[0] == 48 )
		{
			int n = 0;

			while ( bakarr[n] == 48 )                               /* 过滤0 */
			{
				n++;
			}
			char *buff = strchr( (const char *) bakarr, bakarr[n] );
			snprintf( (char *) discontrl.cechebuf, strlen( buff ) + 1, "%s", buff );

			char *clean_null = "                ";
			write_char( MAXWIDE, 0, CHANGE_G, clean_null );
			write_char( strlen( buff ), 2, CHANGE_G, buff ); /* 4->2 */
		} else {
			/* 文件预保存处理 */
			snprintf( (char *) discontrl.cechebuf, strlen( bakarr ) + 1, "%s", bakarr );

			char *clean_null = "                ";
			write_char( MAXWIDE, 0, CHANGE_G, clean_null );
			write_char( strlen( bakarr ), 2, CHANGE_G, bakarr );
		}
		*lenth		= strlen( orgbff );
		re_orfbuf	= (char *) calloc( 16, sizeof(char) );
		if ( re_orfbuf )
		{
			get_input_buffer_str( orgbff, re_orfbuf, *lenth );
			orgbff = strdup( discontrl.cechebuf );
		}
		discontrl.changemenuflag = ~CHAR_INPUT_ON;

		return(re_orfbuf);
	}
	break;
	}


	return(re_orfbuf);
}


static void auto_and_man_send_message( void )
{
	send_usb_writ_message();
	char		ch[20]		= "";
	s_config	*dconfig	= config_t();
	snprintf( ch, 16, "DVB-T %.3fM", dconfig->localstatus.cfig_ad9789_ftw_bpf );
	lcd_Write_String( 0, ch );
	lcd_Write_String( 1, " Record...      " );
}


char * string_cheack( char * opstr, char *str, int size )
{
	int i;

	if ( size != 16 )
	{
		snprintf( opstr, size + 1, "%s", str );
		i = size;

		for (; i < 16; i++ )
			opstr[i] = ' ';
		DEBUG( "%s  %d", opstr, i );
	} else {
		if ( size > 16 )
			size = 16;

		snprintf( opstr, size + 1, "%s", str );
	}

	return(opstr);
}


/* 文件及字符串显示处理，用于处理文件名称及其它字符串处理 */
static char* display_input_str( char *str, int size, int *lenth )
{
	/*
	 * 第一次需要做清屏工作
	 * 然后在第一行写入选择项
	 * 若不是第一次操作，就无需做清屏操作，更无需写入选着项
	 */
	static char	*bak_str, str_[17];
	static int	count		= 0, bakcount;/* bakcount 是要记录自加之前的数据，之后数据会造成数据位的加1或者减1 */
	static int	pos		= 0, ch_count = 0;
	char		*re_orfbuf	= NULL;
	static int	l_r_rocde	= 0;

	if ( discontrl.recoredFrist == DEFAULTE )
	{
		char tmpbuf[17] = "";
		string_cheack( tmpbuf, str, size );
		ch_count		= receive_ds_str( str_, tmpbuf, size );
		bak_str			= strdup( str );
		discontrl.affirmRecode	= 0; /* 进入时此值已经为1了，所以要清掉 */
	}


	/*
	 * lcd 字符的循环显示/或者非循环显示
	 */
	if ( discontrl.lcd_time_key == LCD_LOOP_ON )
	{
		dynamic_string( 1, 0 );
	}else if ( discontrl.lcd_time_key == LCD_LOOP_OFF )
	{
		switch ( discontrl.affirmRecode )
		{
		case 0: {
			discontrl.rig_lifet_status	= LIFTRIGHT_DEF;
			discontrl.add_sub_status	= UPDOWN_DEF;
			l_r_rocde			= 0;
		}
		break;

		case 1: {
			/* 光标状态 */
			if ( discontrl.curos_stat == CURSOR_ON || discontrl.curos_stat == CURSOR_OFF )
			{
				cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x3 ); /* 开 */
				discontrl.curos_stat	= CURSOR_NOUSER;
				count			= 0;

				cursor_move( discontrl.lcdfd, LCD_SUROS_MOVE, 0x10 );
			}

			/* 数据显示部分 */
			if ( (discontrl.rig_lifet_status == RIGHT_ADD) || (discontrl.curos_stat == CURSOR_NOUSER) ) /* 左行动时不改变数据 */

			{
				if ( discontrl.curos_stat == CURSOR_NOUSER )
					discontrl.curos_stat = CURSOR_DEF;

				if ( l_r_rocde == LIFT_SUB )
					count += 2;

				if ( count >= size || count < 0 )
					count = 0;
				l_r_rocde = RIGHT_ADD;


				pos = index_char_dsp( &ch_count, &bakcount, &count, str_ );
				char date;
				date = scan_key_resvale( pos );


				if ( 0x3A == date ) /* jmp ':' */
				{
					count++;
					pos = index_char_dsp( &ch_count, &bakcount, &count, str_ );
				}


				data_process_and_cursor_move( count, 2, scan_key_resvale( pos ), 0x10 );
				count++;

				/* 并且要清出加左右状态 */
				discontrl.rig_lifet_status = LIFTRIGHT_DEF;
			} else if ( (discontrl.rig_lifet_status == LIFT_SUB) ) /* 右行动时不改变数据 */

			{
				if ( l_r_rocde == RIGHT_ADD )
					count -= 2;

				if ( count < 0 || count >= size )
					count = size - 1;
				l_r_rocde = LIFT_SUB;

				pos = index_char_dsp( &ch_count, &bakcount, &count, str_ );
				char date;
				date = scan_key_resvale( pos );
				if ( 0x3A == date ) /* jmp':' */
				{
					count--;
					pos = index_char_dsp( &ch_count, &bakcount, &count, str_ );
				}


				data_process_and_cursor_move( count, 2, scan_key_resvale( pos ), 0x10 );
				count--;

				discontrl.rig_lifet_status = LIFTRIGHT_DEF;
			}

			/* 数据处理部分 */
			if ( (discontrl.add_sub_status == UP_ADD) )
			{
				pos++;

				if ( ch_count <= pos || pos < 0 )
					pos = 0;

				/* 从当前字库的位置进行索引 */
				char date = scan_key_resvale( pos );

				if ( pwd_state() < 0 )
				{
					if ( (pos > 9) || (date == 42) )
						pos = 0;
					date = scan_key_resvale( pos );
				}

				data_process_and_cursor_move( bakcount, 2, date, 0x10 );

				str_[bakcount] = date;
				/* 并且要清出加减状态 */
				discontrl.add_sub_status = UPDOWN_DEF;
			} else if ( (discontrl.add_sub_status == DOWN_SUB) )
			{
				pos--;

				if ( ch_count <= pos || pos < 0 )
					pos = ch_count - 1;
				/* 从当前字库的位置进行索引 */
				char date = scan_key_resvale( pos );

				if ( pwd_state() < 0 )
				{
					if ( (pos > 9) || (date == 42) )
						pos = 9;
					date = scan_key_resvale( pos );
				}

				data_process_and_cursor_move( bakcount, 2, date, 0x10 );
				str_[bakcount]			= date;
				discontrl.add_sub_status	= UPDOWN_DEF;
			}
		}
		break;
		case 2: {
			/* 光标状态 */
			if ( discontrl.curos_stat == CURSOR_DEF )
			{
				cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* 关 */
				discontrl.curos_stat = CURSOR_OFF;                      /* 状态锁定 */
			}
			/* 文件预保存处理 */
			memset( (char *) discontrl.cechebuf, 0, 17 );
			snprintf( (char *) discontrl.cechebuf, strlen( str_ ) + 1, "%s", str_ );


			*lenth		= strlen( bak_str );
			re_orfbuf	= (char *) calloc( 17, sizeof(char) );
			if ( re_orfbuf )
			{
				get_input_buffer_str( bak_str, re_orfbuf, *lenth );
				bak_str = strdup( discontrl.cechebuf );
				char *clean_null = "                ";
				write_char( MAXWIDE, 0, CHANGE_G, clean_null );
				write_char( strlen( bak_str ), 2, CHANGE_G, bak_str );
			}
			discontrl.enter_status		= KEY_AFFRIM_ENTER_Y;
			discontrl.changemenuflag	= ~CHAR_INPUT_ON;


			return(re_orfbuf);
			break;
		}
		}
	}
	return(re_orfbuf);
}


/*
 * 菜单选择配置项
 */
static void menu_selected_cfg( char **arr, int size, char *cfgmenu )
{
	/*
	 * 第一次需要做清屏工作
	 * 然后在第一行写入选择项
	 * 若不是第一次操作，就无需做清屏操作，更无需写入选着项
	 */
	static int retid;
	if ( discontrl.recoredFrist == DEFAULTE )
	{
		receive_ds_menu( 16 );
		retid = 0;
		int index = 0;
		/* 获取设置的菜单 */
		if ( !cfgmenu )
			return;
		int /*i,*/	arr_size;
		char		*arr_cfg;
		retid = index_cfg_menu( arr[index], cfgmenu );

		arr_size	= strlen( arr[retid] );
		arr_cfg		= arr[retid];
		write_char( arr_size, 2, CHANGE_G, arr_cfg );

		discontrl.affirmRecode = 0;                             /* 进入时此值已经为1了，所以要清掉 */
	}

	switch ( discontrl.affirmRecode )
	{
	case 0: {
		discontrl.updownchoose = retid;                         /* 防止提前按下而改变选择项 */
		cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* off */
		curos_dsp( LCD_CHAR_DOT );
	}
	break;

	case 1: {
		/* 菜单上下选择 */

		if ( discontrl.updownchoose >= size ) /* 不能超过本数据大小 */
			discontrl.updownchoose = 0;
		/* 光标状态 */
		curos_dsp( LCD_CHAR_ARR );

		/* 接受配置数据 */
		int /*i,*/	arr_size;
		char		*arr_cfg = arr[discontrl.updownchoose];
		arr_size = strlen( arr[discontrl.updownchoose] );
		/* 获取设置的菜单 */
		write_char( arr_size, 2, CHANGE_G, arr_cfg );
	}
	break;

	case 2: {
		/* 文件预保存处理 */
		snprintf( (char *) discontrl.cechebuf, strlen( arr[discontrl.updownchoose] ) + 1, "%s", arr[discontrl.updownchoose] );

		if ( discontrl.usb_wr_flag == USBWRITESTART )
		{
			if ( strncasecmp( discontrl.cechebuf, "Yes", strlen( discontrl.cechebuf ) - 1 ) == 0 )
				auto_and_man_send_message();
			discontrl.usb_wr_flag = USBWRITECTL;
		}else if ( discontrl.menu_cfg_fun != &null_Subcfg )
		{
			int res;
			res = discontrl.menu_cfg_fun( discontrl.cechebuf );
			if ( res < 0 )
				DEBUG( "[%s] svae error", discontrl.cechebuf );
			discontrl.affirmRecode = 3;
			char *clean_null = "                ";
			write_char( MAXWIDE, 0, CHANGE_G, clean_null );
			write_char( strlen( arr[discontrl.updownchoose] ), 2, CHANGE_G, discontrl.cechebuf );
			discontrl.changemenuflag = ~CHAR_INPUT_ON;
		}

		s_config *dconfig = config_t();
		if ( dconfig->scfg_Param.system_restart_flag == SYS_RESTART )
		{
			lcd_Write_String( 0, "The system is b-" );
			lcd_Write_String( 1, "eing restarted.." );
			nano_sleep( 1, 0 );
			cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 );
			stop_alarm();
			lcd_clear( discontrl.lcdfd );
			lcd_bkl_offon( LCD_LIGHT_OF );

			while ( 1 )
				;
		}
	}

	break;
	}
}


static void set_rtr_arr( signed char userChoose, signed char displaystart, uint8_t subscripts )
{
	menu_return_arr[subscripts].rtr_option		= userChoose;
	menu_return_arr[subscripts].rtr_displaystart	= displaystart;
}


static void mod_rtr_arr( signed char displaystart, uint8_t subscripts )
{
	menu_return_arr[subscripts].rtr_displaystart = displaystart;
}


static void esc_rtr_arr( uint8_t subscripts )
{
	discontrl.UserChoose	= menu_return_arr[subscripts].rtr_option;
	discontrl.DisplayStart	= menu_return_arr[subscripts].rtr_displaystart;
}


void enter_code()
{
	if ( MenuPoint[discontrl.UserChoose].Subs != &NullSubs )
	{
		(*MenuPoint[discontrl.UserChoose].Subs)();
	}

	if ( MenuPoint[discontrl.UserChoose].ChildrenMenus != NULL )
	{
		MenuPoint		= MenuPoint[discontrl.UserChoose].ChildrenMenus; /* 指向子菜单 */
		discontrl.Option	= discontrl.UserChoose;
		/* 在此记录 */
		signed char count = discontrl.rtr_count;
		set_rtr_arr( discontrl.UserChoose, DEFAULTE, count );
		discontrl.rtr_count += 1;

		discontrl.UserChoose	= DEFAULTE;
		discontrl.DisplayStart	= DEFAULTE;
	}
}


/* 菜单显示 */
static void ShowMenu()
{
	uint8_t i;

	discontrl.MaxItems = MenuPoint[DEFAULTE].MenuCount;     /* 获取当前菜单的条目数 */

	discontrl.DisplayPoint = discontrl.DisplayStart;

	if ( discontrl.MaxItems >= LCD1602MaxDisplayLine )      /* 如果大于能显示的菜单数做处理 */

	{
		for ( i = 0; i < LCD1602MaxDisplayLine; i++ )
		{
			lcd_Write_String( i, MenuPoint[discontrl.DisplayPoint].DisplayString );

			if ( discontrl.DisplayPoint == discontrl.UserChoose )
			{
				if ( MenuPoint[discontrl.UserChoose].offon == START_R )
				{
					if ( !discontrl.read_status )
					{
						resval();
						discontrl.read_status = 0x01;
					}
				}
				ctl_fun( discontrl.lcdfd, 0x40 );
			} else {
				ctl_fun( discontrl.lcdfd, 0x00 );
			}

			discontrl.DisplayPoint += 1;
			if ( discontrl.DisplayPoint == discontrl.MaxItems )
			{
				discontrl.DisplayPoint = DEFAULTE;
			}
		} /* end for */
	} else {
		for ( i = 0; i < LCD1602MaxDisplayLine; i++ )
		{
			if ( discontrl.DisplayPoint < discontrl.MaxItems )
			{
				lcd_Write_String( i, MenuPoint[discontrl.DisplayPoint].DisplayString );

				if ( discontrl.DisplayPoint == discontrl.UserChoose )
				{
					if ( MenuPoint[discontrl.UserChoose].offon == START_R )
					{
						if ( !discontrl.read_status )
						{
							resval();

							discontrl.read_status = 0x01;
						}
					}

					ctl_fun( discontrl.lcdfd, 0x40 );
				} else {
					ctl_fun( discontrl.lcdfd, 0x00 );
				}

				discontrl.DisplayPoint += 1;
			} else {
				lcd_Write_String( i, "                " );
				if ( discontrl.DisplayPoint == discontrl.MaxItems )
				{
					discontrl.DisplayPoint = DEFAULTE;
				}
			}
		}
	}
}


static int  check_pwrd( char *pwd )
{
	/*
	 * 如果第一次检查、或者是上锁不用提示
	 *
	 * 是否设置密码
	 */

	_passwrd t_wrd[2];

	int fd, res = -1;

	fd = open( ACSSECC_PATH, O_RDWR );
	if ( fd < 0 )
	{
		fd = open( ACSSECC_PATH, O_RDWR | O_CREAT | O_TRUNC );
		if ( fd < 0 )
		{
			DEBUG( "file open error" );
			return(res);
		}
		DEBUG( "open file fail " );
	}
	memset( t_wrd, 0, sizeof(t_wrd) );

	if ( read( fd, t_wrd, sizeof(t_wrd) ) != sizeof(t_wrd) )
	{
		perror( "check_pwrd() " );
	}


	close( fd );


	int i;

	for ( i = 0; i < 2; i++ )
	{
		if ( strncmp( t_wrd[i].pwrd, pwd, 6 ) == 0 )
		{
			config.lock_enter		= 0x03;
			discontrl.changemenuflag	= 0x00 & CHAR_INPUT_ON;/* 菜单显示开启 */
			DEBUG( "pwrd successfull" );
			res = 0;
			break;
		}
	}

	if ( res < 0 )
	{
		/* 密码不正确 */
		char *bb = "   error        ";
		write_char( 16, 1, CHANGE_G, bb );
	}

	return(res);
}


static void function_call()
{
	if ( discontrl_t()->write_char_dig_status == DIGIT_STATUS )
	{
		char	*org_str	= NULL;
		int	lenth		= 0;

		org_str = display_input( discontrl_t()->cfg_size, org_str, &lenth );
		if ( org_str && lenth )
		{
			if ( discontrl.wd_configfile != NULL )
			{
				int ret = system_script_config( &discontrl, org_str, 3 );
				if ( !ret )
				{
					DEBUG( "%s %d\n", org_str, lenth );
					free( org_str );
				}
			}
		}
	} else if ( discontrl_t()->write_char_dig_status == CHAR_STATUS )                                       /*  */
	{
		char	*org_str	= NULL;
		int	lenth		= 0;

		org_str = display_input_str( discontrl_t()->filename, discontrl_t()->write_size, &lenth );      /* 参数:名称及名称长度; */
		if ( org_str && lenth )
		{
			if ( discontrl.wd_configfile != NULL )
			{
				int ret = system_script_config( &discontrl, org_str, 3 );
				if ( !ret )
				{
					/* DEBUG( "%s %d\n", org_str, lenth ); */
					free( org_str );
				}
			}
		}
	} else if ( discontrl_t()->write_char_dig_status == MENU_STATUS ) /* menu */

	{
		menu_selected_cfg( discontrl_t()->arr, discontrl_t()->write_size, discontrl_t()->cfg_menu );
	}
}


static int access_cheak( char *pwd )
{
	int rest = check_pwrd( pwd );
	if ( rest < 0 )
	{
		DEBUG( "check_pwrd failed" );
	}else{
		int		lenght		= strlen( EIT_DESENABLE ) + 1;
		s_config	*dconfig	= config_t(); /*  */
		memset( (char *) dconfig->scfg_Param.stream_lck_state, 0, 8 );
		snprintf( (char *) dconfig->scfg_Param.stream_lck_state, lenght, "%s", EIT_DESENABLE );
	}

	return(rest);
}


static int change_cfg_menu( signed char keynumber )
{
	int ret = -1;

	switch ( keynumber )
	{
	case up:                                /* 上 */
		discontrl.updownchoose--;       /* 用户字符选择 */
		if ( discontrl.updownchoose < DEFAULTE )
		{
			discontrl.updownchoose = KEYMAXSZIE;
		}
		ret				= 0;
		discontrl.add_sub_status	= UP_ADD;

		break;
	case down:                              /* 下 */
		discontrl.updownchoose++;       /* 用户字符选择 */
		if ( discontrl.updownchoose == KEYMAXSZIE )
		{
			discontrl.updownchoose = DEFAULTE;
		}
		ret				= 0;
		discontrl.add_sub_status	= DOWN_SUB;

		break;

	case lift:                              /* 左移动 */
		discontrl.rightliftmove--;      /* 用户字符选择 */
		if ( discontrl.rightliftmove < DEFAULTE )
		{
			discontrl.rightliftmove = DEFAULTE;
		}

		ret				= 0;
		discontrl.rig_lifet_status	= LIFT_SUB;
		break;

	case rigth:                             /* 右移动 */
		discontrl.rightliftmove++;      /* 用户字符选择 */
		if ( discontrl.rightliftmove == MAXWIDE - 5 )
		{
			discontrl.rightliftmove = DEFAULTE;
		}
		ret				= 0;
		discontrl.rig_lifet_status	= RIGHT_ADD;
		break;

	case enter: {                                           /* 确认 */
		/*
		 * 第一次被按下时作为普通显示状态，光标程“*”状态，这时用于提示用户，当前只是
		 * 显示已经被选中的状态
		 * 第二次被按下时作为修改状态，光标程"->"状态,修改
		 * 第三次按下保存文件
		 */

		ret = 0;
		discontrl.affirmRecode++;
		if ( 2 < discontrl.affirmRecode )               /* 记数清零  == 4*/
			discontrl.affirmRecode = 0;


		if ( discontrl.changeflag )
		{
			discontrl.changeflag	= ENTER_SET;
			discontrl.recoredFrist	= DEFAULTE;     /* 置0 */
		}
	}

	break;
	case esc:
		/* 退出 */
		stop_alarm();
		signal_close();
		cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* 关 */
		if ( discontrl.affirmRecode <= 2 )                      /* 的在配置时，中途退出时要做如下设置 */
		{
			discontrl.curos_stat		= CURSOR_OFF;
			discontrl.enter_status		= KEY_AFFRIM_ENTER_Y;
			discontrl.changemenuflag	= DEFAULTE;     /* 置0 */
			discontrl.rightliftmove		= DEFAULTE;
			discontrl.write_size		= DEFAULTE;
			discontrl.updownchoose		= DEFAULTE;
			discontrl.changemenuflag	= ~CHAR_INPUT_ON;


			break;
		}
		ret = 0; /* 必须放在这 */

		break;


	case lock:
	{
		ret = 0;
	}
	break;
	}


	return(ret);
}


static int menu_browse_change( int keynumber )
{
	int ret = -1;

	switch ( keynumber )
	{
	case up:

		discontrl.UserChoose--; /* 用户菜单的选择 */
		if ( discontrl.UserChoose < DEFAULTE )
		{
			discontrl.UserChoose = discontrl.MaxItems - 1;
		}
		ret = 0;
		break;
	case down:

		discontrl.UserChoose++;
		if ( discontrl.UserChoose == discontrl.MaxItems )
		{
			discontrl.UserChoose = DEFAULTE;
		}
		ret = 0;
		break;
	case enter:

		enter_code();

		ret = 0;
		break;

	case esc:
		discontrl.escOption = 0x01;
		stop_alarm();
		signal_close();

		if ( MenuPoint[discontrl.DisplayStart].ParentMenus != NULL )
		{
			MenuPoint = MenuPoint[discontrl.DisplayStart].ParentMenus; /* 指向父菜单 */
			signed char count = --discontrl.rtr_count;
			esc_rtr_arr( count );
			ret = 0;
			break;
		}

		if ( MenuPoint[discontrl.DisplayStart].ParentMenus == NULL )
		{
			ret			= 0;
			discontrl.UserChoose	= DEFAULTE;
			discontrl.DisplayStart	= DEFAULTE;

			break;
		}
	}


	/*
	 * *--------------------------------------------------------------------
	 * *菜单循环处理
	 */

	if ( (discontrl.UserChoose < discontrl.DisplayStart) ||
	     (discontrl.UserChoose >= (discontrl.DisplayStart + 2) ) )
	{
		discontrl.DisplayStart = discontrl.UserChoose;

		signed char count = discontrl.rtr_count - 1;
		if ( count < 0 )
		{
			count = 0;
			mod_rtr_arr( discontrl.DisplayStart, count );
		}
	}

	return(ret);
}


static void change_stats( int enrt, int frist_f, int cursor_s, int input_s, int affirm )
{
	config.lock_enter		= enrt;
	discontrl.recoredFrist		= frist_f;
	discontrl.curos_stat		= input_s;
	discontrl.write_char_dig_status = cursor_s;
	discontrl.affirmRecode		= affirm;
}


static int pwd_state()
{
	int rs = 0;

	if ( config.scfg_Param.system_lck_st != NOTPWD )
	{
		rs = -1;
	}
	return(rs);
}


static void dsplay_cfg( int dsplay_status )
{
	int		lenth;
	s_config	*dconfig = config_get_config();
	discontrl.write_size	= 6;
	discontrl.input_lenth	= 6;
	discontrl.enter_status	= KEY_AFFRIM_ENTER_N;
	display_input_str( (char *) dconfig->scfg_Param.system_root_pwd, 6, &lenth );
}


static int lock_enter_input_pwd( int dsplay_status, int keynumber )
{
	int ret = -1;
	discontrl.lock_dspaly_status = dsplay_status;
	change_cfg_menu( keynumber );
	dsplay_cfg( dsplay_status );

	if ( discontrl.enter_status == KEY_AFFRIM_ENTER_Y )
	{
		char tmp[16] = { 0 };
		sprintf( tmp, discontrl.cechebuf );
		if ( access_cheak( tmp ) == 0 )
			ret = 0;
		else{
			discontrl.pwd_error_flag = TRUE;

			nano_sleep( 1, 0 );
		}
	}

	return(ret);
}


static void lck_stut()
{
	s_config *dconfig = config_get_config();
	if ( strncmp( (char *) dconfig->scfg_Param.stream_lck_state, ENABLE,
		      sizeof( (char *) dconfig->scfg_Param.stream_lck_state) ) == 0 )
	{
		dconfig->scfg_Param.system_lck_st = YESPWD;
	}else{
		dconfig->scfg_Param.system_lck_st = NOTPWD;
	}
}


static void pwd_cursor_key_set()
{
	s_config *dconfig = config_get_config();
	dconfig->key_statuus	= LOCK_ON;
	dconfig->lock_enter	= LOCKKEY_DOWN_F;
	cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 ); /* 关，防止退出时 */
}


static void button_tow_enetr()
{
	discontrl.call_loop_enter	= 0;
	discontrl.enter_status		= KEY_AFFRIM_ENTER_Y;
	discontrl.changemenuflag	= ~CHAR_INPUT_ON;
	discontrl.call_stop_enter	= 0;
	discontrl.lcd_time_key		= LCD_LOOP_OFF;
}


static struct itimerval *set_alrm( int ctv_sec, int64_t ctv_usec, int ntv_sec, int64_t ntv_usec )
{
	static struct itimerval itv;
	itv.it_interval.tv_sec	= ntv_sec;
	itv.it_interval.tv_usec = ntv_usec;
	itv.it_value.tv_sec	= ctv_sec;
	itv.it_value.tv_usec	= ctv_usec;

	return(&itv);
}


/*
 * 定时器停止
 */
void stop_alarm( void )
{
	if ( setitimer( ITIMER_REAL, set_alrm( 0, 0, 0, 0 ), NULL ) < 0 )
	{
		fprintf( stderr, "%s: %d: %s\n", __FUNCTION__, __LINE__, strerror( errno ) );
		exit( 1 );
	}

	reset_desplay();
}


/*
 * 开启定时器，1秒级
 */
void start_alarm( void )
{
	if ( setitimer( ITIMER_REAL, set_alrm( 0, 100000, 1, 0 ), NULL ) < 0 )
	{
		fprintf( stderr, "%s: %d: %s\n", __FUNCTION__, __LINE__, strerror( errno ) );
		exit( 1 );
	}
}


static void alrm_hander( int s )
{
	int monitor_flag, coder, flags;

	get_state_desplay( &monitor_flag, &coder, &flags );

	if ( monitor_flag )
		monitor_opt( coder, flags );
	else
		;
}


static void usb_wr_hander( int signum, siginfo_t *siginfo, void *not_used )
{
	if ( SIGUSR1 == signum )
	{
		usb_operation_t *usb_action = siginfo->si_value.sival_ptr;
		usb_wroperator( usb_action );
	}
}


static int start_time_out( struct timeval tpstart )
{
	float		timeuse;
	int		timer;
	struct timeval	tpend;
	gettimeofday( &tpend, NULL );
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);
	timer	= timeuse /= 1000;
	return(timer);
}


static void lock_enter_input_prompt( int dsplay_status )
{
	lcd_clear( discontrl.lcdfd );

	discontrl.lock_dspaly_status = dsplay_status;

	memset( discontrl.cechebuf, '\0', 17 );
	memset( t_date, '\0', 17 );
	memset( discontrl.filename, '\0', 17 );

	snprintf( (char *) t_date, 17, " Enter password " );

	change_stats( LOCKKEY_WOWN_S, 0, CURSOR_ON, CHAR_STATUS, 0 );
}


static void sys_start_timer( void )
{
	struct timeval tpstart;

	sys_start_prompt();

	gettimeofday( &tpstart, NULL );

	while ( progr_bar != 0x40 )
	{
		if ( start_time_out( tpstart ) >= 4000 )
		{
			progr_bar = 0x40;
			break;
		}
	}

	progr_bar_flag = 1;

	if ( sysstart == 0 )
		sysstart = 1;
}


static void stream_info( void )
{
	stop_alarm();
	start_alarm();
	set_state_desplay( MONITOER_TRUE, NORMAL_DESPLAY_START, NORMAL_DESPLAY_FLAG );
	function_inter( lock_dspbitraet );
}


static void close_info( void )
{
	set_state_desplay( MONITOER_TRUE, NORMAL_DESPLAY_STOP, NORMAL_DESPLAY_FLAG );
	function_inter( lock_dspbitraet );
	reset_desplay();
	stop_alarm();
	signal_close();
}


/* 按键检测 */
void ChangeMenu( int keySigNum )
{
	signed char	keynumber;
	static int	lock_count	= 0;
	s_config	*dconfig	= config_t();

	if ( !progr_bar_flag )
		return;


/* 清理定时器 */
	cl_time();
	last_time();

/* 唤醒信号 */
	if ( discontrl.lcd_time_key == LCD_LOOP_ON )
		alarm( 1 );

/* 背光 */
	m_backlighting( LCD_LIGHT_ON );

/* 密码， */
	if ( dconfig->scfg_Param.system_pwd_count == 1 )
		keynumber = (char ) keySigNum;
	else
		read( discontrl.keyfd, &keynumber, 1 );

/* 开机启动时生效 */
	if ( start_machine == 1 )
	{
		if ( keynumber != lock )
		{
			stream_info();
			return;
		}
	}


/* 过滤不相干中断 */
	if ( keynumber < 0 )
		return;

/* 信号测试 */
	switch ( test_signal() )
	{
	case USB_WRITE:
	case USB_READ: {
		if ( keynumber != esc )
		{
			stop_alarm();
			desplay_stream_info();
			return;
		}else{
			extern pthread_t	usb_sig;
			s_config		*dconfig = config_get_config();
			send_usb_stop_message( usb_sig, SIGUSR2, dconfig, STARTSTOP );

			return;
		}
	}
	break;

	case NORMAL_SIG: {/* 状态类 */
		if ( keynumber != enter )
		{
#if 1

			if ( keynumber == esc )
				keynumber = enter;
			else
#endif
			{ return; }
		}
	}
	break;
	}

/* 密码检测 */
	lck_stut();

#if 1
/* pwd  设置密码时生效*/
	if ( keynumber != 0x7f )
	{
		if ( pwd_state() < 0 )
		{
			if ( dconfig->scfg_Param.system_pwd_count == 1 )
			{
				keynumber				= lock;
				dconfig->scfg_Param.system_pwd_count	= 2;
			}

			if ( keynumber != 0x7f )
			{
				switch ( keynumber )
				{
				case lock: {
					lock_count++;

					pwd_cursor_key_set();
					loop_hand();
				}
				}
			}
__agin:
			if ( lock_count > 1 )
			{
				lock_count = 0;
			}

			if ( lock_count == 1 )
			{
				static int t_count = 0;

				if ( t_count == 0 )
				{
					lock_enter_input_prompt( LOCK_ON );

					t_count = 1;
				}
				int ret;
				if ( (keynumber != esc) && (keynumber != 0x7f) )
				{
					ret = lock_enter_input_pwd( LOCK_OFF, keynumber );
				}else {
					lock_count	= 0;
					t_count		= 0;
					cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 );
				}

				if ( 0 == ret )
				{
					discontrl.enter_status			= KEY_AFFRIM_ENTER_N;
					lock_count				= 0;
					t_count					= 0;
					dconfig->scfg_Param.system_pwd_count	= 0;
					paren_menu();
					return;
				}else if ( ret < 0 )
				{
					if ( discontrl.pwd_error_flag )
					{
						lock_count			= 1;
						t_count				= 0;
						discontrl.pwd_error_flag	= FALSE;

						goto __agin;
					}
					return;
				}
			}else{
				return;
			}
		}
	}

#endif


#if 1
	/*  lock 按下lock生效*/
	if ( keynumber != 0x7f )
	{
		static int lockcount = 0;
		switch ( keynumber )
		{
		case lock: {
			lock_count++;
			lockcount = 1;
			if ( lock_count > 1 )
			{
				lock_count = 0;
				close_info();
				paren_menu();
				lockcount = 0;
				
			}
			if ( lock_count == 1 )
			{
				stream_info();
				start_machine = 2;
				return;
				break;
			}
		}
		}

		if ( lockcount == 1 )
		{
			stream_info();
			if ( keynumber != lock )
			{
				return;
			}
		}
	}


#endif

/*      //菜单显示 */
	if ( keynumber != 0x7f && discontrl.changemenuflag != CHAR_INPUT_ON
	     && config.lock_enter == 0x03 )
	{
		if ( !menu_browse_change( keynumber ) )
		{
			ShowMenu();

			discontrl.read_status = 0x00;
		}
	}

/* 用于字符,数字的输入 */
	if ( keynumber != 0x7f && discontrl.changemenuflag == CHAR_INPUT_ON
	     && config.lock_enter == 0x03 )
	{
		if ( !change_cfg_menu( keynumber ) )
		{
			function_call();


			/*
			 * 按两次enetr时返回
			 */
			if ( discontrl.call_loop_enter == enter )
			{
				keynumber = enter;
				button_tow_enetr();
				DEBUG( "call_loop_enter" );
			}
		}

		switch ( keynumber )
		{
		case esc:
		{
			/*
			 * 按esc键时返回
			 */
			loop_hand();
		}
		case enter:
		{
			if ( discontrl.changemenuflag != CHAR_INPUT_ON )
			{
				discontrl.escOption = 0x01;

				if ( MenuPoint[discontrl.DisplayPoint].ParentMenus != NULL )
				{
					MenuPoint		= MenuPoint[discontrl.DisplayPoint].ParentMenus;        /* 指向父菜单 */
					discontrl.UserChoose	= DEFAULTE;
					discontrl.DisplayStart	= DEFAULTE;
					discontrl.UserChoose	= discontrl.Option;                                     /* 恢复先前的菜单 */
				}
				if ( (discontrl.UserChoose < discontrl.DisplayStart) ||
				     (discontrl.UserChoose >= (discontrl.DisplayStart + 2) ) )
				{
					discontrl.DisplayStart = discontrl.UserChoose;
				}
				ShowMenu();

				signed char count = --discontrl.rtr_count;
				if ( count < 0 )
					count = 0;
				esc_rtr_arr( count );
				keynumber = 0x7f;
			}
		}
		}
	}
}


static void  monitor_opt( int coder, int flags )
{
	switch ( flags )
	{
	case NORMAL_DESPLAY_FLAG:
		comm_handler( coder );
		break;
	case ENCODE_BITRTE_FLAG:
		comm_handler( coder );
		break;
	case SYS_RUN_TIME_FLAG:
		comm_handler( coder );
		break;
	case DEVICE_SER_FLAG:
		comm_handler( coder );
		break;
	case DEVICE_NUM_FLAG:
		comm_handler( coder );
		break;
	case ENCODE_FORMAT_FLAG:
		comm_handler( coder );
		break;
	case USER_PWD_FLAG:
		comm_handler( coder );
		break;
	case USB_WRITE_SIGNAL_FLAG:
		comm_handler( coder );
		break;
	case USB_READ_SIGNAL_FLAG:
		comm_handler( coder );
		break;
	default:
		break;
	}
}


/*
 * 监控一些lcd显示操作
 */
void user_desplay_monitor( void )
{
	struct sigaction sig;


	function_inter( null_handler );
	sig.sa_handler = alrm_hander;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = 0;

	if ( sigaction( SIGALRM, &sig, NULL ) < 0 )
	{
		fprintf( stderr, "%s: %d:%s\n", __FUNCTION__, __LINE__, strerror( errno ) );
		exit( 1 );
	}


	if ( !start_machine )
	{
		while ( sysstart == 0 )
			nano_sleep( 0, 100 );

		sysstart = 2;

		/* 100% */
		while ( !progr_bar_flag )
			nano_sleep( 0, 100 );

		m_backlighting( LCD_LIGHT_ON );
		stop_alarm();
		start_alarm();
		set_state_desplay( MONITOER_TRUE, NORMAL_DESPLAY_START, NORMAL_DESPLAY_FLAG );
		function_inter( lock_dspbitraet );
		start_machine = 1;
	}

	while ( 1 )
		nano_sleep( 0, 500000000 );
}


/*
 * 监控usb读写行为
 *
 */
void user_operation_usb( void )
{
	struct sigaction sig;

	sig.sa_sigaction = usb_wr_hander;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = SA_SIGINFO;

	if ( sigaction( SIGUSR1, &sig, NULL ) < 0 )
	{
		fprintf( stderr, "%s: %d:%s\n", __FUNCTION__, __LINE__, strerror( errno ) );
		exit( 1 );
	}

	while ( 1 )
		nano_sleep( 0, 500000000 );
}


static void sys_start_prompt( void )
{
	m_backlighting( LCD_LIGHT_ON );
	lcd_Write_String( 0, " Start up      " );
	lcd_Write_String( 1, " Please wait..." );
}


/* #define debug_call */
int lcd_main( void )
{
	int flag = 0;

/* debug */
#if debug_call
	dev_config_printf();
#endif

	discontrl.lcdfd = lcd_open();
	lcd_clear( discontrl.lcdfd ); /* 并清除内存 */

	/* set key Asynchronous parameter */
	fcntl( discontrl.keyfd, F_SETOWN, getpid() );
	flag = fcntl( discontrl.keyfd, F_GETFL );
	fcntl( discontrl.keyfd, F_SETFL, flag | FASYNC );


	/* 检查是否启动完成，是否超时 */
	sys_start_timer();

	cursor_onoff( discontrl.lcdfd, LCD_SUROS_ONOFF, 0x00 );

	last_time();

	while ( 1 )
	{
		if ( m_timers( config.scfg_Param.system_lcd_time_out ) )
		{
			cl_time();

			if ( discontrl.lcd_time_key == LCD_LOOP_ON )
				alarm( 0 );

			m_backlighting( LCD_LIGHT_OF );
			stop_alarm();
		}

		s_config *dconfig = config_t();
		if ( dconfig->scfg_Param.system_pwd_count == 1 )
		{
			ChangeMenu( up );
		}

		nano_sleep( 1, 0 );

		/* 自动 处理usb 写 */

		if ( discontrl.record_auto_flag == USB_AUTO_HAND )
		{
			nano_sleep( 1, 0 );
			auto_and_man_send_message();
			start_machine			= 2;            /* 自动录制时，打开esc键 */
			discontrl.record_auto_flag	= USB_OFF_HAND; /* 移出usb时再次有效 */
		}
	}

	return(1);
}


/* dev config info */
void dev_config_printf()
{
	/* config_read( get_profile()->script_configfile ); */
#if 0
	s_config *dconfig = config_get_config();


	DEBUG( "config->script_configfile=%s \r\n", dconfig->script_configfile );
	DEBUG( "config->usb_tsfilename=%s \r\n", dconfig->usb_tsfilename );
	DEBUG( "config->usb_tsfilesize=%d \r\n", dconfig->usb_tsfilesize );
	DEBUG( "encoder_video_interface_name =%s \r\n", dconfig->scfg_Param.encoder_video_interface_name );

	DEBUG( "config.configParam.reset=0x%02x \r\n", config.configParam.reset );
	DEBUG( "config.configParam.encode_rate_name =%s \r\n", config.configParam.encode_rate_name );
	DEBUG( "config.configParam.encode_rate=0x%02x \r\n", config.configParam.encode_rate );

	DEBUG( "config.configParam.modulelate_mode_name=%s \r\n", config.configParam.modulelate_mode_name );
	DEBUG( "config.configParam.modulelate_mode=0x%02x \r\n", config.configParam.modulelate_mode );

	DEBUG( "config.configParam.fft_mode_name=%s \r\n", config.configParam.fft_mode_name );
	DEBUG( "config.configParam.fft_mode=0x%02x \r\n", config.configParam.fft_mode );

	DEBUG( "config.configParam.deltal_name=%s \r\n", config.configParam.deltal_name );
	DEBUG( "config.configParam.deltal=0x%02x \r\n", config.configParam.deltal );

	DEBUG( "config.configParam.channel_width_name=%s \r\n", config.configParam.channel_width_name );
	DEBUG( "config.configParam.channel_width=0x%02x \r\n", config.configParam.channel_width );

	DEBUG( "config.localstatus.cfig_ad9789_ftw_bpf=%0.3f \r\n", (double) config.localstatus.cfig_ad9789_ftw_bpf );


	DEBUG( "discontrl_t()->encoder_video_interface=%d \r\n", config.scfg_Param.encoder_video_interface );

	DEBUG( "scfg_Param.encoder_pmt_pid=%s \r\n", config.scfg_Param.encoder_pmt_pid );
	DEBUG( "config.scfg_Param.encoder_pcr_pid=%s \r\n", config.scfg_Param.encoder_pcr_pid );
	DEBUG( "scfg_Param.encoder_video_pid=%s \r\n", config.scfg_Param.encoder_video_pid );
	DEBUG( "scfg_Param.encoder_audio_pid=%s \r\n", config.scfg_Param.encoder_audio_pid );
	DEBUG( "scfg_Param.encoder_program_number=%s \r\n", config.scfg_Param.encoder_program_number );

	DEBUG( "encoder_video_ratemode_fre = 0x%04x", config.scfg_Param.encoder_video_ratemode_fre );
	DEBUG( "encoder_video_ratemode = %s", config.scfg_Param.encoder_video_ratemode );
	DEBUG( "encoder_video_bitrate = %d", config.scfg_Param.encoder_video_bitrate );

	DEBUG( "encoder_video_profile_name = %s", config.scfg_Param.encoder_video_profile_name );
	DEBUG( "encoder_video_profile = 0x%04x", config.scfg_Param.encoder_video_profile );

	DEBUG( "encoder_video_level = %s", config.scfg_Param.encoder_video_level_name );
	DEBUG( "encoder_video_level_name = 0x%04x", config.scfg_Param.encoder_video_level );
	DEBUG( "encoder_video_brightness = %d", config.scfg_Param.encoder_video_brightness );


	DEBUG( "encoder_audio_bitrate_name = %s", config.scfg_Param.encoder_audio_bitrate_name );
	DEBUG( "encoder_audio_bitrate = %d", config.scfg_Param.encoder_audio_bitrate );

	DEBUG( "passwrd status = 0x%02x", config.key_statuus );
	DEBUG( "stream_nit_network_id = %s", config.scfg_Param.stream_nit_network_id );
	DEBUG( "stream_nit_network_name =%s ", config.scfg_Param.stream_nit_network_name );
	DEBUG( "encoder_eit_now_startyear =%s", config.scfg_Param.encoder_eit_now_startyear );
	DEBUG( "encoder_eit_now_starttime =%s", config.scfg_Param.encoder_eit_now_starttime );
	DEBUG( "encoder_eit_now_duration  =%s", config.scfg_Param.encoder_eit_now_duration );
	DEBUG( "encoder_eit_now_eventname  =%s", config.scfg_Param.encoder_eit_now_eventname );

	DEBUG( "encoder_eit_next_startyear  =%s", config.scfg_Param.encoder_eit_next_startyear );
	DEBUG( "encoder_eit_next_starttime  =%s", config.scfg_Param.encoder_eit_next_starttime );
	DEBUG( "encoder_eit_next_duration  =%s", config.scfg_Param.encoder_eit_next_duration );
	DEBUG( "encoder_eit_next_eventname  =%s", config.scfg_Param.encoder_eit_next_eventname );


/*
 * int i;
 * for(i=0;i<strlen((char *)config.scfg_Param.encoder_eit_next_eventname);i++)
 *      {
 *              DEBUG("%c",config.scfg_Param.encoder_eit_next_eventname[i]);
 * }
 */
	DEBUG( "stream_nit_private_data  =%s", dconfig->scfg_Param.stream_nit_private_data );

	DEBUG( "stream_eit_insert  =%s", dconfig->scfg_Param.stream_eit_insert );
	DEBUG( "stream_nit_insert  =%s", dconfig->scfg_Param.stream_nit_insert );
#endif
}


