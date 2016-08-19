#define _GNU_SOURCE
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include <usb_signal.h>
#include <reset-oparetion.h>
#include <common_conf.h>
#include <menu-set.h>
#include <usbhotplug.h>
#include <debug.h>
#include <key-ctl.h>
#include <math_instance.h>
#include <menu_foun.h>
#include <readbus.h>
#include <freememscript.h>
#include <io_rw.h>

#include <pares_psi.h>
#include <usbinfo.h>
#include <common_interface.h>


#define STOP_OP		0
#define STAR_OP		1
#define PATHLENGTH	150
#define NAMESIZE	30
#define FILE_COUNT	999

extern
struct parse_ts_id parse_ts_id;

extern pthread_t usb_sig;

struct param {
	int	flag;
	off_t	offset;
};


struct read_bus {
	uint8_t		* map_base;
	uint8_t		* tmp_basefpga;
	bus_init	bus_inttt;
	pthread_t	clientw;
	int		fdr;
};

struct index_t {
	int	count;
	char	path[PATHLENGTH];
	char	name[NAMESIZE];
	uint8_t filepathpos;
};

struct recode_file {
	char name[NAMESIZE];
};

struct filter_file_story_t {
	int			count;
	char			path[PATHLENGTH];
	struct recode_file	recode_t[FILE_COUNT];
};

struct filter_file_story_t	story_t;
struct param			_param;
struct read_bus			* bus_r		= NULL;
static int			r_time_out_flag = 0;

static int		itemcount = 0;
static struct index_t	index_math[100];


extern CHANNEL_INFO_STRUCT	* program_info;
static uint8_t			* sharemem_map_base	= NULL;
static int			item_pos		= 0;
static double			ts_lenth		= 1.0, ts_add_toal = 0.0;
static int			play_count		= 0;

static struct read_bus init_bus_r = {
	.bus_inttt.reset		= 0x01,
	.bus_inttt.rest_usb_op		= 0x01,
	.bus_inttt.usb_dir		= 0x01, /* 改0时为读 */
	.bus_inttt.usb_rdaddr_b3124	= 0x01, /* 地址读时为0x6400000 */
	.bus_inttt.usb_rdaddr_b2316	= 0x40,
	.bus_inttt.usb_rdaddr_b158	= 0x00,
	.bus_inttt.usb_rdaddr_b70	= 0x00,
	.bus_inttt.usb_op		= 0x01, /* 启动读时操作 */
	.map_base			= NULL,
	.tmp_basefpga			= NULL,
	.clientw			= 0,
	.fdr				= -1,
};


static void modulator_audio_video_set( uint8_t* map_base )
{
	map_base[(BUS_OFFSET_ADDR + 0x19) / sizeof(uint8_t)]	= 0x01;
	map_base[(BUS_OFFSET_ADDR + 0x19) / sizeof(uint8_t)]	= 0x00;

	reset_dvb_t( 0x01, map_base );

	s_config *dconfig = config_get_config();
	modulator_set( map_base, dconfig );
	usleep( 5000 );
	map_base[(BUS_OFFSET_ADDR + 0x14) / sizeof(uint8_t)] = 0x00;
	usleep( 5000 );
	audio_video_pid( map_base, dconfig );

	reset_dvb_t( 0x00, map_base );
}


static int r_time_out( struct timeval tpstart )
{
	float		timeuse;
	int		timer;
	struct timeval	tpend;
	gettimeofday( &tpend, NULL );
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);
	timer	= timeuse /= 1000;
	return(timer);
}


/*
 * 写中断
 */
static void inter_signal( uint8_t* map_addr, uint8_t *post )
{
	struct timeval	tpstart;
	char		tmp;
	gettimeofday( &tpstart, NULL );

	while ( ( (tmp = map_addr[(BUS_OFFSET_ADDR + 0x13) / sizeof(uint8_t)]) & 0x1) != 1 )
	{
		usleep( 0 );
		if ( r_time_out( tpstart ) >= DTS_STREAM_TIME )
		{
			r_time_out_flag = 1;

			break;
		}
		r_time_out_flag = 0;
	}

	*post = ( (tmp >> 1) & 0x1);

	map_addr[(BUS_OFFSET_ADDR + 0x13) / sizeof(uint8_t)] = tmp & 0xFE;


/*
 * struct timeval tpstart;
 * gettimeofday( &tpstart, NULL );
 */


/*
 * while ( readb( &map_addr[BUS_OFFSET_ADDR + 0x13] ) != 1 )
 * {
 *      usleep( 0 );
 *      if ( r_time_out( tpstart ) >= 2000 )
 *      {
 *              r_time_out_flag = 1;
 *              break;
 *      }
 *      r_time_out_flag = 0;
 * }
 * if ( 1 == readb( &map_addr[BUS_OFFSET_ADDR + 0x13] ) )
 * {
 *      writeb( 0x00, &map_addr[BUS_OFFSET_ADDR + 0x13] );
 * }
 */
}


/*
 * 写总线操作 2M
 */
static void readusb( int fd, uint8_t*  map_addr, int copy_size )
{
	off_t	size	= 0;
	ssize_t cntr	= 0;
	int	segment = 0;


	uint8_t* stor_addr = calloc( 1, FRISTSIZE + 0xf );
	if ( !stor_addr )
	{
		DEBUG( "calloc error" );
		return;
	}

	while ( (cntr = read( fd, stor_addr, copy_size ) ) != 0 )
	{
		if ( recv_usb_notify() <= 0 )
			break;

		if ( cntr < 0 )
		{
			perror( "read error" );
			break;
		}else if ( cntr > 0 )
		{
			uint8_t post;

			inter_signal( map_addr, &post );

			if ( is_usb_online() != DEVACTT )
				r_time_out_flag = 1;

			if ( r_time_out_flag == 1 )
				break;

			DEBUG( "post :%d  cntr:%x", post, cntr );
			if ( segment == 0 )
			{
				memcpy( sharemem_map_base, stor_addr, cntr );
				segment = 1;
			}else if ( segment == 1 )
			{
				memcpy( sharemem_map_base + FRISTSIZE, stor_addr, cntr );
				segment = 0;
			}
			size		+= cntr;
			ts_add_toal	+= cntr;
		}
	}
	ts_add_toal += cntr;
	memset( stor_addr, 0, FRISTSIZE + 0xf );
	free( stor_addr );
	stor_addr = NULL;
}


/*
 * 初始化总线写操作 2M
 */
static int init_bus_wrop( int fd )
{
	int	ret	= -1;
	ssize_t cntr	= 0;

	uint8_t* stor_addr = calloc( 1, MEMSIZE_2M + 0xf );
	if ( !stor_addr )
	{
		DEBUG( "calloc error" );
		return(ret);
	}


	while ( (cntr = read( fd, stor_addr, MEMSIZE_2M ) ) != 0 )              /* 读文件 */
	{
		if ( cntr < 0 )                                                 /* 文件读取是否出错 */
		{
			perror( "read error" );
			free( stor_addr );
			break;
		} else if ( cntr == MEMSIZE_2M )                                /* 写入字节和读取字节相同，数据全部写入 */

		{
			memcpy( sharemem_map_base, stor_addr, MEMSIZE_2M );     /* 将数据写入内存 */

			ret = cntr;
			break;
		}
	}
	ts_add_toal = cntr;
	free( stor_addr );

	DEBUG( "cntr :%x ", cntr );
	return(ret);
}


#define PATHSIZE 1024


static int path_noloop( const char *path )
{
	char *pos;
	pos = strrchr( path, '/' );

	if ( pos == NULL )
		exit( 1 );

	if ( strcmp( pos + 1, "." ) == 0 || strcmp( pos + 1, ".." ) == 0 )
		return(0);

	return(1);
}


/*
 * 消息设置
 */
static void  send_usb_message( pthread_t usb_action_t, int signum, void *dcfig, usb_handler usb_hand, int is_start )
{
	union sigval		usbsigval;
	static usb_operation_t	usb_cation;
	s_config		*dconfig = (s_config *) dcfig;
	usb_cation.op_mod	= dconfig->scfg_Param.stream_usb_mod;/* USB_SINGLE; */
	usb_cation.ts_size	= dconfig->configParam.usb_tsfilesize;
	usb_cation.is_start	= is_start;

	usb_wr_handler( usb_hand );
	usbsigval.sival_ptr = &usb_cation;

	if ( pthread_sigqueue( usb_action_t, signum, usbsigval ) != 0 )
		perror( "send error\n" );
}


/*
 * 消息设置
 */

static uint8_t  send_usb_read_message()
{
	extern pthread_t action_usbop;

	s_config *dconfig = config_get_config();

	send_usb_message( action_usbop, SIGUSR1, dconfig, read_usb, START_R );

	nano_sleep( 0, 1000 );

	send_usb_stop_message( usb_sig, SIGUSR2, dconfig, START_R );

	return(1);
}


/*
 * 动态菜单创建
 */
static int get_ts_name( char *tsname, int item, int item_count )
{
	int ret = -1;
	if ( tsname != NULL && item_count != 0 )
	{
		struct MenuItem mitem[item_count];
		mitem[item].DisplayString = calloc( 1, sizeof(char) * 30 );
		if ( mitem[item].DisplayString == NULL )
			return(ret);
		char *tmp = rindex( tsname, '/' );
		snprintf( mitem[item].DisplayString, strlen( tmp ) + 1, " %s", tmp + 1 );
		mitem[item].MenuCount		= item_count;
		mitem[item].Subs		= &send_usb_read_message;
		mitem[item].ChildrenMenus	= NULL;
		mitem[item].ParentMenus		= PlayTsCfg;
		mitem[item].offon		= 0;
		mitem[item].filepathpos		= item;
		FileBrowse[item]		= mitem[item];

		int	length, pathlent;
		char	string[NAMESIZE]	= "";
		char	path[PATHLENGTH]	= "";

		/* 备份数据 */
		snprintf( string, strlen( mitem[item].DisplayString ), "%s", mitem[item].DisplayString + 1 );
		length		= strlen( string );
		pathlent	= strlen( tsname );
		int subcunt = pathlent - length;
		snprintf( path, subcunt + 1, "%s", tsname );
		memset( index_math[item].path, 0, PATHLENGTH );
		memset( index_math[item].name, 0, NAMESIZE );
		index_math[item].count		= item_count;
		index_math[item].filepathpos	= item;
		snprintf( index_math[item].path, subcunt + 1, "%s", path );
		snprintf( index_math[item].name, length + 1, "%s", string );

		ret = item;
	}else if ( item_count == 0 )
	{
		struct MenuItem mitem[1];
		mitem[0].DisplayString = calloc( 1, sizeof(char) * 16 );
		if ( mitem[0].DisplayString == NULL )
			return(ret = -1);

		sprintf( mitem[0].DisplayString, " %s", "  No TS file   " );
		mitem[0].MenuCount	= 1;
		mitem[0].Subs		= &NullSubs;
		mitem[0].ChildrenMenus	= NULL;
		mitem[0].ParentMenus	= UsbDevCfg;
		mitem[0].offon		= START_R;

		FileBrowse[0]	= mitem[0];
		ret		= item;
	}

	return(ret);
}


/*
 * 在设备中找文件
 */
static int i_next = 0;
static int64_t  seach_fts( const char *path, int flag )
{
	int64_t sum;
	int	i, isdir_count = 0, isdir_falg = 0;

	static struct stat statres, statr;

	static char	nextpath[PATHSIZE];
	glob_t		globres;

	if ( lstat( path, &statres ) < 0 )
	{
		perror( "lstat()" );
		exit( 1 );
	}

	if ( !S_ISDIR( statres.st_mode ) )
		return(statres.st_blksize);

	/* aa/bb//cc/ee/ff */
	strncpy( nextpath, path, PATHSIZE );
	strncat( nextpath, "/*", PATHSIZE );
	if ( glob( nextpath, 0, NULL, &globres ) == GLOB_NOMATCH )
		fprintf( stderr, "for no found matches\n" );


	strncpy( nextpath, path, PATHSIZE );
	strncat( nextpath, "*.ts", PATHSIZE );
	if ( glob( nextpath, GLOB_APPEND, NULL, &globres ) == GLOB_NOMATCH )
		fprintf( stderr, "for no found matches\n" );

	sum = statres.st_blksize;


	for ( i = 0; i < globres.gl_pathc; i++ )
	{
		if ( path_noloop( globres.gl_pathv[i] ) )
			sum += seach_fts( globres.gl_pathv[i], flag );

		if ( lstat( globres.gl_pathv[i], &statr ) < 0 )
		{
			perror( "lstat()" );
			exit( 1 );
		}
		if ( S_ISDIR( statr.st_mode ) )
		{
			++isdir_count;
			isdir_falg = 1;
		}else
			isdir_falg = 0;

		if ( flag != 0 && isdir_falg != 1 )
		{
			if ( get_ts_name( globres.gl_pathv[i], i_next, itemcount ) < 0 )
				goto __exit;
			++i_next;
		}
	}

	i_next = 0;

	if ( globres.gl_pathc == 0 )
	{
		char *notfile = "  No TS file   ";
		get_ts_name( notfile, 0, itemcount );
	}

	if ( flag == 0 )
		itemcount += globres.gl_pathc - isdir_count;


__exit:
	globfree( &globres );

	return(sum);
}


/*
 * 文件路径节点
 *
 */
static int index_path_node( const char *src_name, uint8_t pathpos )
{
	int i, ret = -1, length = 0;

	if ( src_name == NULL )
		return(ret);

	length = strlen( src_name );

	for ( i = 0; i < itemcount; i++ )
	{
		if ( (strncasecmp( src_name, index_math[i].name, length ) == 0) &&
		     (index_math[i].filepathpos == pathpos) )
		{
			ret		= i;
			item_pos	= i;
			break;
		}
	}
	return(ret);
}


/*
 * 获取文件路径
 */
static int  get_file_path( int item, char *ts_path )
{
	int	ret = -1;
	int	length;

	if ( 0 > item )
		return(ret);

	char *path = index_math[item].path;

	length	= strlen( path );
	ret	= index_math[item].count;

	snprintf( ts_path, length + 1, "%s", path );

	return(ret);
}


/*
 * 归档文件路径节点
 *
 */
static int archives_index_path_node( const char *src_name )
{
	int i, node = -1, length = 0;

	if ( src_name == NULL )
		return(node);

	length = strlen( src_name );

	for ( i = 0; i < story_t.count; i++ )
	{
		if ( strncasecmp( src_name, story_t.recode_t[i].name, length ) == 0 )
		{
			node = i;

			break;
		}
	}
	return(node);
}


/*
 * 获取归档文件,路径
 */
static int  get_archives_index_file( int item, char *ts_name, char *path )
{
	int	ret = -1;
	int	length, p_length;

	if ( 0 > item )
		return(ret);

	char	*name		= story_t.recode_t[item].name;
	char	* p_name	= story_t.path;
	length		= strlen( name );
	p_length	= strlen( p_name );
	ret		= story_t.count;

	snprintf( ts_name, length + 1, "%s", name );
	snprintf( path, p_length + 1, "%s", p_name );

	return(ret);
}


/*
 * 写流时信息
 */
static void chech_message()
{
	static int	count		= 0;
	char		ch[20]		= "";
	char		ch_1[20]	= "";
	s_config	*dconfig	= config_t();

	snprintf( ch, 16, "DVB-T %.3fM", dconfig->localstatus.cfig_ad9789_ftw_bpf );

	switch ( count )
	{
	case 0:
	{
		lcd_Write_String( 0, ch );
		lcd_Write_String( 1, " Play...        " );
		count++;
	}
	break;
	case 1:
	{
		char	*name	= story_t.recode_t[play_count].name;
		int	len	= strlen( name );
		snprintf( ch_1, len + 1, "%s", name );
		lcd_Write_String( 0, ch );
		lcd_Write_String( 1, "               " );
		lcd_Write_String( 1, ch_1 );
		count++;
	}
	break;
	case 2:
	{
		float usbrbitrate;

		double progressbar = ts_add_toal / ts_lenth * 100;

		usb_read_bitrate( &usbrbitrate );

		snprintf( ch_1, 16, "%.1f\%% %.2fM", progressbar, usbrbitrate );
		lcd_Write_String( 0, ch );
		lcd_Write_String( 1, "               " );
		lcd_Write_String( 1, ch_1 );
		count++;
	}
	break;
	}


	if ( count >= 3 )
		count = 1;
}


/*
 *
 * 显示
 */
uint8_t read_stream_info( void )
{
	signal_open( USB_READ );
	start_alarm();
	set_state_desplay( MONITOER_TRUE, USB_READ_SIGNAL_START, USB_READ_SIGNAL_FLAG );
	function_inter( chech_message );

	return(0);
}


/* USB 设备信息 */
uint8_t usb_ts_inf()
{
	int	ret		= -1, i;
	char	path[100]	= "";
	if ( usb_probe() < 0 )
	{
		paren_menu();
		return(ret);
	}

	itemcount = 0;

	for ( i = 0; i < get_stata_path()->part_num; i++ )
	{
		if ( get_stata_path()->mount_path[i] != NULL )
		{
			snprintf( path, strlen( get_stata_path()->mount_path[i] ) + 1, "%s",
				  get_stata_path()->mount_path[i] );
			ret = 0;
			break;
		}
	}

	if ( ret < 0 )
		return(ret);

	ret	= seach_fts( path, 0 );
	ret	= seach_fts( path, 1 );

	return(ret);
}


/*
 * 解析pat并产生表
 */
static void pat_parse_gener_table( char* streamfilename )
{
	pat_parse_enter( streamfilename );
	s_config *dconfig = config_get_config();

	pmt_parse_enter( streamfilename, parse_ts_id.i_number, parse_ts_id.i_pmt_pid );

	memset( dconfig->scfg_Param.encoder_pcr_pid, 0, 4 );
	memset( dconfig->scfg_Param.encoder_pmt_pid, 0, 4 );
	memset( dconfig->scfg_Param.encoder_video_pid, 0, 4 );
	memset( dconfig->scfg_Param.encoder_audio_pid, 0, 4 );
	memset( dconfig->scfg_Param.encoder_program_number, 0, 5 );

	char buf[6];
	itoa_( hextodec( parse_ts_id.i_number, buf ), (char *) dconfig->scfg_Param.encoder_program_number, 16 );

	itoa_( hextodec( parse_ts_id.i_pmt_pid, buf ), (char *) dconfig->scfg_Param.encoder_pmt_pid, 16 );
	itoa_( hextodec( parse_ts_id.i_pcr_pid, buf ), (char *) dconfig->scfg_Param.encoder_pcr_pid, 16 );
	itoa_( hextodec( parse_ts_id.i_video_pid, buf ), (char *) dconfig->scfg_Param.encoder_video_pid, 16 );
	itoa_( hextodec( parse_ts_id.i_audieo_pid, buf ), (char *) dconfig->scfg_Param.encoder_audio_pid, 16 );

	gener_table();

	config_read( get_profile()->script_configfile );
}


/*
 *  依选择次序归档
 */
static int64_t  ts_archives( const char *path, const char* _name )
{
	int sum = 0;

	int		i, ret = -1;
	char		ts_name[NAMESIZE]	= "";
	int		count			= 0;
	struct stat	statres;

	char	nextpath[PATHSIZE];
	glob_t	globres;

	if ( lstat( path, &statres ) < 0 )
	{
		perror( "lstat()" );
		return(ret);
	}

	if ( !S_ISDIR( statres.st_mode ) )
		return(statres.st_blksize);

	strncpy( nextpath, path, PATHSIZE );
	strncat( nextpath, "/*.ts", PATHSIZE );

	if ( glob( nextpath, 0, NULL, &globres ) == GLOB_NOMATCH )
		fprintf( stderr, "for no found matches\n" );


	for ( i = 0; i < globres.gl_pathc; i++ )
	{
		/*
		 * 下一个文件
		 * 查看是否同一序列
		 * 相同存储
		 * 不同忽略
		 */
		char	tmp_ts_name[NAMESIZE];
		char	not_digit_name[NAMESIZE];
		char	dig_buf[10];
		int	cpy_vale;
		char	*tmp	= rindex( globres.gl_pathv[i], '/' );
		size_t	tmplen	= strlen( tmp );


/* 不是循环ts */
		if ( !get_string_last_digit_str( dig_buf, tmp, 3 ) )
		{
			snprintf( not_digit_name, tmplen, "%s", tmp + 1 );

			if ( strncasecmp( not_digit_name, _name, tmplen ) == 0 )
			{
				story_t.count = 1;
				memset( story_t.path, 0, PATHLENGTH );
				memset( story_t.recode_t[0].name, 0, NAMESIZE );
				int len = strlen( path );
				snprintf( story_t.path, len + 1, "%s", path );
				snprintf( story_t.recode_t[0].name, tmplen + 1, "%s", tmp + 1 );

				snprintf( ts_name, tmplen + 1, "%s", tmp + 1 );
				ret = 1;
				return(ret);
			}else
				continue;
		}

		if ( 0 == count )
		{
			snprintf( tmp_ts_name, tmplen, "%s", tmp + 1 );
			snprintf( ts_name, tmplen, "%s", tmp_ts_name );
			sum		= atoi( dig_buf );
			cpy_vale	= sum;
		}else{
			char	append_dig[10];
			char	append_str[10];
			snprintf( tmp_ts_name, tmplen, "%s", tmp + 1 );
			cpy_vale	= atoi( dig_buf );
			sum		+= 1;
			gcvt_char( sum, 1, append_dig );
			int org_length = get_count( append_dig );
			get_char_filter_to_digit( org_length, 3, append_str, append_dig );

			strncat( append_str, ".ts", 3 );
			memset( ts_name, '\0', NAMESIZE );
			append_string_last_digit_str( ts_name, tmp_ts_name, append_str, strlen( append_str ) );
		}

		if ( cpy_vale != sum )
			continue;

		if ( strncasecmp( tmp_ts_name, ts_name, tmplen ) == 0 )
		{
			story_t.count = i + 1;

			if ( story_t.count >= FILE_COUNT )
			{
				story_t.count = FILE_COUNT;
				break;
			}

			memset( story_t.path, 0, PATHLENGTH );
			memset( story_t.recode_t[i].name, 0, NAMESIZE );
			int len = strlen( path );
			snprintf( story_t.path, len + 1, "%s", path );
			snprintf( story_t.recode_t[i].name, tmplen + 1, "%s", tmp + 1 );

			snprintf( ts_name, tmplen + 1, "%s", tmp + 1 );
			ret = count = i;
		}
	}

	globfree( &globres );

	return(ret);
}


void send_filter_ect( uint8_t *mem )
{
	s_config	*dconfig	= config_get_config();
	uint16_t	pmtpid		= (uint16_t) atoi( (char *) dconfig->scfg_Param.encoder_pmt_pid );
	uint8_t		pmt_pid_a[2];
	pmt_pid_a[0]	= GET_HEX( pmtpid, 2 );
	pmt_pid_a[1]	= GET_HEX( pmtpid, 1 );
	filter_ts_pmt( mem, pmt_pid_a[0], pmt_pid_a[1] );
	usb_start_rwop( 0x00, mem );
	usbr_interrupt_enable( 0x00, mem );     /* 关中断 */
	usb_dir( 0x00, mem );
	fpga_w_addr( &init_bus_r.bus_inttt, mem );
	reset_usb_op( 0x01, mem );

	usleep( 5000 );
	reset_usb_op( 0x01, mem );
	usb_start_rwop( 0x01, mem );            /* 启动读时操作 */
	reset_dvb_t( 0x01, mem );
	reset_dvb_t( 0x00, mem );
	modulator_audio_video_set( mem );
}


/*
 * 分段读
 * ts_name      文件名xxx.ts
 * pos			在数组中的存储位置
 * count_item	归档总文件数
 * ts_fd		文件描述符
 * mem			写内存地址
 *
 ***************************************************/
static int32_t segment_read_ts_strem( const char *src_name, uint8_t pos, int count_item, int ts_fd, uint8_t*  mem )
{
	int	item, i, ret = -1;
	char	ts_path[100] = "";
	char	f_name[NAMESIZE];
	char	path[PATHLENGTH] = "";

/* 第一个文件读完 */
	readusb( ts_fd, mem, FRISTSIZE );
	close_stream_file( ts_fd );

	if ( r_time_out_flag == 1 )
		return(ret);
/* 第二个文件的开始...直到<count_item 结束*/

	item = archives_index_path_node( src_name );

	for ( i = item + 1; i < count_item; i++ )
	{
		ts_lenth	= 1;
		ts_add_toal	= 0;
		play_count	= i;
		struct stat st;

		memset( f_name, '\0', NAMESIZE );
		memset( ts_path, '\0', PATHLENGTH );
		memset( path, '\0', PATHLENGTH );

		if ( get_archives_index_file( i, f_name, ts_path ) < 0 )
			return(ret);

		sprintf( path, "%s%s", ts_path, f_name );

		if ( stat( path, &st ) < 0 )
		{
			perror( "read_usb \n" );
			return(ret);
		}

		ts_lenth = st.st_size;
		if ( r_time_out_flag == 1 )
			break;

		ts_fd = open_stream_file( path );
		if ( ts_fd < 0 )
		{
			destory_mem( sharemem_map_base, MEMSIZE_2M );
			destory_mem( mem, MAP_SIZE );

			return(ret);
		}

		readusb( ts_fd, mem, FRISTSIZE );
		close_stream_file( ts_fd );
	}


	return(ret);
}


/*
 * 循环读
 * ts_name      文件名xxx.ts
 * pos			在数组中的存储位置
 * count_item	归档总文件数
 * ts_fd		文件描述符
 * mem			写内存地址
 *
 ***************************************************/
static int32_t loop_read_ts_strem( const char *src_name, uint8_t pos, int count_item, int ts_fd, uint8_t*  mem )
{
	int	item, i, ret = -1, bak_item = 0;
	char	ts_path[100] = "";
	char	f_name[NAMESIZE];
	char	path[PATHLENGTH] = "";

/* 第一个文件读完 */
	readusb( ts_fd, mem, FRISTSIZE );
	close_stream_file( ts_fd );

	if ( r_time_out_flag == 1 )
		return(ret);

/* 第二个文件的开始...直到<count_item 结束*/

	bak_item = item = archives_index_path_node( src_name );
	item++;

	while ( 1 )
	{
		if ( recv_usb_notify() <= 0 )
			break;

		for ( i = item; i < count_item; i++ )
		{
			play_count	= i;
			ts_lenth	= 1;
			ts_add_toal	= 0;
			struct stat st;

			memset( f_name, '\0', NAMESIZE );
			memset( ts_path, '\0', PATHLENGTH );
			memset( path, '\0', PATHLENGTH );

			if ( get_archives_index_file( i, f_name, ts_path ) < 0 )
				return(ret);

			sprintf( path, "%s%s", ts_path, f_name );

			if ( stat( path, &st ) < 0 )
			{
				perror( "read_usb \n" );
				return(ret);
			}

			ts_lenth = st.st_size;

			if ( r_time_out_flag == 1 )
				break;

			ts_fd = open_stream_file( path );
			if ( ts_fd < 0 )
			{
				destory_mem( sharemem_map_base, MEMSIZE_2M );
				destory_mem( mem, MAP_SIZE );

				return(ret);
			}

			readusb( ts_fd, mem, FRISTSIZE );
			close_stream_file( ts_fd );
		}

		if ( count_item <= i )
			item = bak_item;
	}
	return(ret);
}


static void usb_sop_notify()
{
	lcd_Write_String( 1, "                " );
	lcd_Write_String( 1, " stop...        " );
	nano_sleep( 1, 0 );
}


/*
 * usb单文件读,分段读，循环读
 * path_name    路径+xxx.ts
 * path			路径
 * ts_name      文件名xxx.ts
 * pos			在数组中的存储位置
 * size			当前文件大小
 * play_mod		播放模式
 *
 ***************************************************/
static int  usb_read_handler( char *path_name, const char *path, const char *ts_name, int pos, off_t size, int play_mod )
{
	int	ret	= -1;
	uint8_t * mem	= NULL;
	int	fd, s_fd, ts_fd;

	if ( (fd = open_mem_fd() ) < 0 )
		return(-1);

	mem = init_mem( MAP_SIZE, fd, MAP_BASE_ADDR );
	if ( !mem )
		close_mem_fd( fd );


	if ( (s_fd = open_mem_fd() ) < 0 )
	{
		destory_mem( mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	sharemem_map_base = init_mem( MEMSIZE_2M, s_fd, MEMSTARTADD_2 );
	if ( !sharemem_map_base )
	{
		close_mem_fd( s_fd );
		destory_mem( mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	system( RULER_STRING_ARG );
/* 定位文件 */
	memset( story_t.recode_t[0].name, 0, NAMESIZE );
	snprintf( story_t.recode_t[0].name, strlen( ts_name ) + 1, "%s", ts_name );

	read_stream_info();

	/* pat */
	pat_parse_gener_table( path_name );

	comom_stream_info( read_stream_info );
	/* init fpga */
	usbr_interrupt_enable( 0x00, mem );             /* 关中断 */
	usb_start_rwop( 0x00, mem );                    /* stop   */
	usb_dir( 0x01, mem );                           /* stop   */
	reset_usb_op( 0x01, mem );                      /*  stop  */

	ts_fd = open_stream_file( path_name );
	if ( ts_fd < 0 )
	{
		destory_mem( sharemem_map_base, MEMSIZE_2M );
		close_mem_fd( s_fd );
		destory_mem( mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	/* fpga最初的2M */
	if ( init_bus_wrop( ts_fd ) < 0 )
	{
		DEBUG( "init_bus_wrop" );
		destory_mem( sharemem_map_base, MEMSIZE_2M );
		close_mem_fd( s_fd );
		destory_mem( mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	send_filter_ect( mem );


	switch ( play_mod )
	{
	case USB_SINGLE: {
		readusb( ts_fd, mem, FRISTSIZE );
		ts_lenth	= 1;
		ts_add_toal	= 0;
	}
	break;
	case USB_SEGMENT: {
		if ( ts_archives( path, ts_name ) != -1 )
		{
			int node;
			if ( (node = archives_index_path_node( ts_name ) ) < 0 )
				return(ret);

			play_count = node;
			segment_read_ts_strem( ts_name, node, story_t.count, ts_fd, mem );
			ts_lenth	= 1;
			ts_add_toal	= 0;
			play_count	= 0;
		}
	}
	break;
	case USB_LOOP: {
		if ( ts_archives( path, ts_name ) != -1 )
		{
			int node;
			if ( (node = archives_index_path_node( ts_name ) ) < 0 )
				return(ret);

			play_count = node;

			loop_read_ts_strem( ts_name, node, story_t.count, ts_fd, mem );
			ts_lenth	= 1;
			ts_add_toal	= 0;
			play_count	= 0;
		}
	}
	break;
	}


	usbr_interrupt_enable( 0x00, mem );             /* 关中断 */
	usb_start_rwop( 0x00, mem );                    /* stop */
	usb_dir( 0x01, mem );                           /* stop */
	reset_usb_op( 0x01, mem );

	memset( sharemem_map_base, 0, MEMSIZE_2M );

	close_stream_file( ts_fd );
	destory_mem( sharemem_map_base, MEMSIZE_2M );
	sharemem_map_base = NULL;
	destory_mem( mem, MAP_SIZE );
	close_mem_fd( s_fd );
	close_mem_fd( fd );

	init_bus();

	DEBUG( "file szie : %lld ", (uint64_t) size );
	return(0);
}


int32_t read_usb( void *usb_hand )
{
	int32_t ret = -1, pos = 0, length;
	int	item;
	r_time_out_flag = 0;
	char		tmpname[NAMESIZE]	= "";
	char		path[PATHLENGTH]	= "";
	struct stat	st;
	usb_operation_t *usb_action = (usb_operation_t *) usb_hand;

	if ( is_usb_online() != DEVACTT )
		return(ret);

	if ( usb_action->ts_size < 0 )
		return(ret);

	if ( FileBrowse[resval()].DisplayString != NULL )
	{
		length = strlen( FileBrowse[resval()].DisplayString );
		snprintf( tmpname, length + 1, "%s", FileBrowse[resval()].DisplayString + 1 );
		pos = FileBrowse[resval()].filepathpos;
	}else
		return(ret);


	item = index_path_node( tmpname, pos );
	if ( item < 0 )
		return(ret);

	char ts_path[100] = "";

	if ( get_file_path( item, ts_path ) < 0 )
		return(ret);

	sprintf( path, "%s%s", ts_path, tmpname );


	if ( stat( path, &st ) < 0 )
	{
		perror( "read_usb \n" );
		return(ret);
	}

	ts_lenth = st.st_size;

	DEBUG( "file size : %lld\n", st.st_size );

	if ( usb_action->is_start == STARTSTOP )
		return(ret);

	usb_read_handler( path, ts_path, tmpname, pos, st.st_size, usb_action->op_mod );

	stop_alarm();
	signal_close();

	s_config *dconfig = config_get_config();
	send_usb_stop_message( usb_sig, SIGUSR2, dconfig, STARTSTOP );
	loop_cl_cah();

	usb_sop_notify();

	paren_menu();

	return(ret);
}


