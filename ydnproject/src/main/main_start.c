#include <pthread.h>
#include <bits/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <main_start.h>
#include <string.h>
#include <errno.h>
#define __DEBUG__


#include <init-configuration.h>
#include <key-ctl.h>
#include <freememscript.h>
#include <lcd1602.h>
#include <common_conf.h>
#include <menu_foun.h>
#include <dev-info.h>
#include <usbhotplug.h>
#include <main_call.h>
#include <usb_signal.h>
#include <debug.h>

static void init_mod_code();


static void init_mode_recognition();


static pthread_t p_lcd, p_dsplay, p_usb, p_mem;

pthread_t action_usbop, usb_sig;

static volatile int adv7842_fd;


static void set_file_fd( int fd )
{
	adv7842_fd = fd;
}


int get__file_fd( void )
{
	return(adv7842_fd);
}


static void init_mod_code()
{
	s_config *dconfig = config_get_config();

	if ( strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name,
		      strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		dconfig->scfg_Param.encoder_video_interface = M_HDMI;
	} else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name,
			     strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		dconfig->scfg_Param.encoder_video_interface = M_CVBS;
	} else if ( strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name,
			     strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		dconfig->scfg_Param.encoder_video_interface = M_YPbPr;
	}
}


/* 避免僵尸进程 */
static void sigchld_handler( int s )
{
	int status;

	pid_t rc;

	rc = waitpid( -1, &status, WNOHANG );
	if ( rc != -1 )
		;
}


static void ctrl_handler( int s )
{
	DEBUG( "CTRL + C " );

	exit( 1 );
}


static void mem_manage()
{
	struct sysinfo info;


	for (;; )
	{
		sysinfo( &info );
		if ( info.freeram / 1024 <= LIMITMEM )
		{
			mem_look_clear();
		}
		nano_sleep( 2, 0 );
	}
}


static void time_handler( int s )
{
	static pthread_mutex_t sigterm_mutex = PTHREAD_MUTEX_INITIALIZER;

	if ( pthread_mutex_trylock( &sigterm_mutex ) )
	{
		DEBUG( "Another thread already began global termination handler. I'm exiting" );
		pthread_exit( NULL );
	} else {
		DEBUG( "Cleaning up and exiting" );
	}

	exit( s == 0 ? 1 : 0 );
}


void interrupt_signals_mask( int opcode )
{
	sigset_t sig;
	sigemptyset( &sig );
	sigaddset( &sig, SIGIO );


	switch ( opcode )
	{
	case 1:

		if ( sigprocmask( SIG_BLOCK, &sig, NULL ) == -1 )/* 屏蔽该信号 */
		{
			perror( "fail to set signal mask" );
			return;
		}
		DEBUG( "close signal" );
		break;
	case 0:

		if ( sigprocmask( SIG_UNBLOCK, &sig, NULL ) == -1 )/* 恢复屏蔽的信号 */
		{
			perror( "fail to set signal mask" );
			return;
		}
		DEBUG( "revive signal" );
		break;
	}
}


static void init_signals( void )
{
	struct sigaction sig;
	sig.sa_handler = sigchld_handler;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = SA_RESTART;

	if ( sigaction( SIGCHLD, &sig, NULL ) == -1 )
	{
		DEBUG( "error sigaction(): %s", strerror( errno ) );
		exit( 1 );
	}

	sig.sa_handler = time_handler;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = SA_RESTART;

	/* Trap SIGTERM */
	if ( sigaction( SIGTERM, &sig, NULL ) == -1 )
	{
		DEBUG( "sigaction(): %s", strerror( errno ) );
		exit( 1 );
	}

	sig.sa_handler = ctrl_handler;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = 0;

	/* Trap SIGINT */
	if ( sigaction( SIGINT, &sig, NULL ) == -1 )
	{
		DEBUG( "sigaction(): %s", strerror( errno ) );
		exit( 1 );
	}

	sig.sa_handler = ChangeMenu;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = SA_RESTART;

	/* trap SIGIO [key] */
	if ( sigaction( SIGIO, &sig, NULL ) == -1 )
	{
		DEBUG( "signal(): %s", strerror( errno ) );
		exit( 1 );
	}
}


/*
 *  只在开机时有效，用于检测上次记录
 *
 */
static void init_mode_recognition()
{
	s_config *dconfig = config_get_config();
	if ( (M_HDMI == dconfig->scfg_Param.encoder_video_interface) )
		adv_defualt_set();
	else if ( (M_YPbPr == dconfig->scfg_Param.encoder_video_interface) )
		YPbPr_def();
	else if ( M_CVBS == (dconfig->scfg_Param.encoder_video_interface) )
		cvbs_def();
}


void change_seach()
{
	if ( peripheral_config() )
	{
		peripheral_dev_config( peripheral_config() );
	}
}


void exception_handling( int status, void* arg )
{
	int		ret;
	pthread_attr_t	*attr = (pthread_attr_t *) arg;

	close( get__file_fd() );

	ret = pthread_attr_destroy( attr );

	if ( ret != 0 )
	{
		DEBUG( "pthread_attr_init :%s", strerror( errno ) );
	}
}


static int sig_vic( signed int vic )
{
	static signed int first_vic;

	if ( first_vic != vic )
	{
		first_vic = vic;
		return(-1);
	}else
		return(vic);
}


static void main_loop( void )
{
	int	result;
	int	fd;
	/* int error; */

	signed int	b_val = -1, r_vic = -1;
	int		bakcode, bakcodecvbs, ret;

	dis_contrl *p_discontrl = discontrl_t();

	size_t		stacksize = 20480;
	pthread_attr_t	attr;

	ret = pthread_attr_init( &attr );
	if ( ret != 0 )
	{
		DEBUG( "pthread_attr_init :%s", strerror( errno ) );
	}

	ret = pthread_attr_setstacksize( &attr, stacksize );


	/* sub */
	result = pthread_create( &p_usb, &attr, (void *) thread_for_usb, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (p_usb) - exiting" );
		time_handler( 0 );
	}
	pthread_detach( p_usb );

	/* lcd */
	result += pthread_create( &p_lcd, &attr, (void *) lcd_main, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (p_lcd)  - exiting" );
		time_handler( 0 );
	}
	pthread_detach( p_lcd );

	/* desplay */
	result += pthread_create( &p_dsplay, &attr, (void *) user_desplay_monitor, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (p_dsplay)  - exiting" );
		time_handler( 0 );
	}
	pthread_detach( p_dsplay );


	/* mem */
	result += pthread_create( &p_mem, &attr, (void *) mem_manage, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (p_mem)  - exiting" );
		time_handler( 0 );
	}
	pthread_detach( p_mem );


	/* monitor usb action */
	result += pthread_create( &action_usbop, &attr, (void *) user_operation_usb, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (action_usbop)  - exiting" );
		time_handler( 0 );
	}
	pthread_detach( action_usbop );

	sigset_t set;
	sigemptyset( &set );
	sigaddset( &set, SIGUSR1 );
	if ( pthread_sigmask( SIG_BLOCK, &set, NULL ) != 0 )
		DEBUG( "!! Set pthread mask failed\n" );
	/* monitor usb siganl */
	result += pthread_create( &usb_sig, &attr, (void *) usb_op_signal, NULL );
	if ( result != 0 )
	{
		DEBUG( "FATAL: Failed to create a new thread (usb_sig)  - exiting" );
		time_handler( 0 );
	}
	pthread_detach( usb_sig );

	on_exit( exception_handling, &attr );


	/*
	 * 开机前默认设置，使其生效，方可进行读写配置
	 *
	 */

	/* 开机时的初始模式 */
	init_mod_code();

	init_mode_recognition();

	fd = adv7842_open();
	set_file_fd( fd );

	while ( 1 )
	{
		nano_sleep( 1, 0 );
		result = -1;
		s_config *dconfig = config_get_config();


		/*
		 * 文件配置
		 * cof_flag 此值需要修改
		 * HDMI mode
		 */
		if ( (M_HDMI == dconfig->scfg_Param.encoder_video_interface)
		     || (p_discontrl->pid_status == 0x01) )
		{
			if ( p_discontrl->pid_status == 0x01 )
			{
				bakcodecvbs		= -1;
				bakcode			= -1;
				b_val			= -1;
				p_discontrl->pid_status = 0x02;
			}else{
				bakcodecvbs	= -1;
				bakcode		= -1;
			}


			/*
			 * 设备配置
			 *
			 */
			r_vic = read_vic( get__file_fd() );
			int ret = sig_vic( r_vic );
			if ( ret < 0 )
				continue;

			if ( b_val != ret )
			{
				b_val = ret;
				set_vic_code( ret );

				if ( ret <= 0 )
				{
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, 0, v1920x1080i_50Hz );       /* 0 没用此位 */

					change_seach();
				}else{
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, 0, ret );                    /* 0 没用此位 */

					change_seach();
				}
			}


			/*
			 *
			 *  YPbPr
			 */
		} else if ( (M_YPbPr == dconfig->scfg_Param.encoder_video_interface)
			    || (p_discontrl->pid_status == 0x01) )
		{
			int optcode;

			if ( p_discontrl->pid_status == 0x01 )
			{
				bakcodecvbs		= -1;
				b_val			= -1;
				bakcode			= -1;
				p_discontrl->pid_status = 0x02;
			}else{
				bakcodecvbs	= -1;
				b_val		= -1;
			}


			/*
			 * 分量信号
			 * YPbPr
			 * DEBUG(" pare YPbPr mod %d ",pare_YPbPr_HDMI());
			 * 分量信号设定之前需要预先初始化YPbPr模式
			 * 否则将无法修其模式
			 */

			optcode = pare_YPbPr_HDMI( get__file_fd() );
			int ret = sig_vic( optcode );
			if ( ret < 0 )
				continue;
			if ( bakcode != ret )
			{
				bakcode = ret;  /* 防止重复性 */
				set_vic_code( ret );

				switch ( ret )  /* 智能识别其模式 */
				{
				case YPbPr1080Isign60Hz:
					DEBUG( " pare YPbPr mod %d ", ret );
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					result = 0;
					break;
				case YPbPr1080Isign50Hz:
					DEBUG( " pare YPbPr mod %d ", ret );
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					result = 0;
					break;
				case YPbPr720Psign50Hz:
					DEBUG( " pare YPbPr mod %d ", ret );
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					result = 0;
					break;
				case YPbPr720Psign60Hz:
					DEBUG( " pare YPbPr mod %d ", ret );
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					result = 0;
					break;
				default:
					DEBUG( "Not find user code..." );
					break;
				}
				if ( result != -1 )
					change_seach();
				else{
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, YPbPr1080Isign50Hz, 0 );
					change_seach();
				}
			}
		} else if ( (M_CVBS == dconfig->scfg_Param.encoder_video_interface)
			    || (p_discontrl->pid_status == 0x01) )
		{
			int optcode;

			if ( p_discontrl->pid_status == 0x01 )
			{
				bakcode			= -1;
				b_val			= -1;
				bakcodecvbs		= -1;
				p_discontrl->pid_status = 0x02;
			}else{
				bakcode = -1;
				b_val	= -1;
			}


			/*
			 * CVBS 信号
			 * CVBS  模式下只能识别两种hz，50hz,60hz
			 */

			optcode = pare_cvbs( get__file_fd() );
			int ret = sig_vic( optcode );
			if ( ret < 0 )
				continue;
			if ( bakcodecvbs != ret )
			{
				bakcodecvbs = ret;      /* 防止重复性 */
				set_vic_code( ret );

				switch ( ret )          /* 智能识别其模式 */
				{
				case NTSC_60_ROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "NTSC_60_ROW" );
					result = 0;
					break;
				case NTSC_60_NOTROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "NTSC_60_NOTROW" );
					result = 0;
					break;
				case NTSC_50_NOTROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "NTSC_50_NOTROW" );
					result = 0;
					break;
				case NTSC_50_ROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "NTSC_50_ROW" );
					result = 0;
					break;

				case PAL_60_ROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "PAL_60_ROW" );
					result = 0;
					break;
				case PAL_60_NOTROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "PAL_60_NOTROW" );
					result = 0;
					break;
				case PAL_50_NOTROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "PAL_50_NOTROW" );
					break;
				case PAL_50_ROW:
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, ret, 0 );
					DEBUG( "PAL_50_ROW" );
					result = 0;
					break;
				default:
					DEBUG( "Not find user code..." );
					break;
				}
				if ( result != -1 )
					change_seach();
				else{
					usercode_mod( dconfig->scfg_Param.encoder_video_interface, PAL_50_NOTROW, 0 );
					change_seach();
				}
			}
		}
	}
	;
}


int main( int argc, char **argv )
{
	char *strdt = SYS_SET_TIME;
	set_system_time( strdt );

	all_config_s();

	init_lcddefault_donfig();
	/* 文件配置 */
	config_init();
	config_read( get_profile()->script_configfile );


	pthread_mutex_init( &get_profile()->set_usb_glob_info_lock, NULL );
	usleep( 30000 );
	/* 开机前默认设置，使其生效，方可进行读写配置 */

	init_signals();
	dis_contrl *p_discontrl = discontrl_t();

	/* 按键处理 */
	p_discontrl->keyfd = key_open();
	if ( p_discontrl->keyfd < 0 )
	{
		DEBUG( "key open error..." );
		exit( 0 );
	}


	switch ( safe_fork() )
	{
	case 0:         /* child */
		setsid();
		main_loop();
		break;

	default:        /* parent */
		exit( 0 );
		break;
	}

	return(0);
}


