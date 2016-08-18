#define _GNU_SOURCE
#include <pthread.h>
#include <execinfo.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <writebus.h>
#include <freememscript.h>

#include <debug.h>
#include <devcapacitylook.h>
#include <init-configuration.h>
#include <usb_signal.h>
#include <usbhotplug.h>
#include <common_conf.h>
#include <common_interface.h>
#include <key-ctl.h>
#include <c-file.h>
#include <menu_foun.h>
#include <reset-oparetion.h>
#include <io_rw.h>
#include <stdlib.h>


#define STOP_OP		0
#define STAR_OP		1
#define TMIEROUT	800000 * 9
#define MAX_FREE	(2048)
#define MYTPF_MAX	100
#define PATHLENGTH	150
#define NAMESIZE	30
#define PATHSIZE	1024
#define DTS_STREAM_TIME (12000)

/* #define  BUFFER_SIZE_1M (0x100058) */
#define  BUFFER_SIZE_1M (0x2000b0) /* 2016-8-17 */
#define  BUFFER_SIZE_2M (0x2000b0)


extern s_config		config;
extern pthread_t	usb_sig;
static uint8_t		* sharemem_base = NULL;
static int8_t		stop		= 1;
static int8_t		wait_stop	= 0;
static int		r_time_out_flag = 0;
static double		ts_lenth	= 0.0, ts_add_toal = 0.0;
#define CPS		(1)
#define BURST		(50)
#define SEG_FLAG	(0x20)
#define SEG_SFLAG	(0x30)
#define SEG_GFLAG	(0x00)
#define DISC_FREE_MAX	(0x3c00000)

typedef struct init_bus_ {
	bus_init	bus_t;
	pthread_t	cache_copy, write_u;
	int		mod;
	uint8_t		* mem;
	char		path[PATHLENGTH];
}init_bus_t;


typedef struct usb_token_t {
	int32_t cps;
	int32_t burst;
	int32_t token;
	int32_t pos;
	uint8_t *ts_date;
}usb_token;

typedef struct job_squeue_t {
	uint8_t flag;
	uint8_t node;
}job_squeue_t;

job_squeue_t squeue_t;

static usb_token	*job[MYTPF_MAX];
static usb_token	*token		= NULL;
static char		*tmp_dsplay	= NULL;
static uint8_t		*jobsqueue_date[MYTPF_MAX];

static int32_t writ_usb( void * );


static void *read_cache( void * );


static void inter_signal( uint8_t *, uint8_t * );


static int disk_check( void );


static int descriptor_generation( void *init_b, int incom_count, int * );


init_bus_t init_t = {
	.bus_t.usb_dir		= 0x01,
	.bus_t.usb_wraddr_b3124 = 0x01,
	.bus_t.usb_wraddr_b2316 = 0x40,
	.bus_t.usb_wraddr_b158	= 0x00,
	.bus_t.usb_wraddr_b70	= 0x00,
	.bus_t.usb_op		= 0x01,
};

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
 * 文件管理
 */
static int management_document( char* i_path, char *file_name, uint32_t order, char *new_file )
{
	int	ret = -1;
	char	chang_dig[10];
	char	get_append_str[10];
	char	path[PATHSIZE];
	char	ts_name[NAMESIZE] = "";

	if ( (!i_path) && (!file_name) )
		return(ret);

	if ( 1000 <= order )
		order = 1;

	size_t p_len = strlen( i_path );
	snprintf( path, p_len + 1, "%s", i_path );

	gcvt_char( order, 1, chang_dig );
	int org_length = get_count( chang_dig );
	get_char_filter_to_digit( org_length, 3, get_append_str, chang_dig );

	snprintf( ts_name, strlen( file_name ) + 1, "%s", file_name );
	strncat( ts_name, get_append_str, 3 );
	strncat( ts_name, ".ts", 3 );
	sprintf( new_file, "%s/%s", path, ts_name );

	return(ret = 0);
}


/*
 * 写模式
 * 返回值 : 0  单模式 ，1 分段模式，2 循环模式,4 无条件结束,5 小空间循环
 */
static int mod_manage( int play_mod, int *next_incom_count )
{
	int ret = -1, dick_is_full;


	dick_is_full = disk_check();

	switch ( play_mod )
	{
	case USB_RECORD_SIG: {
		DEBUG( "USB_RECORD_SIG" );
		if ( dick_is_full < 0 )
			ret = 4;
		else{
			ret = 0;
			++(*next_incom_count);
		}
		DEBUG( "USB_RECORD_SIG" );
	}
	break;
	case UAB_RECORD_SEG: {
		DEBUG( "UAB_RECORD_SEG" );
		if ( dick_is_full < 0 )
			ret = 4;

		else{
			ret = 1;
			++(*next_incom_count);
		}
		DEBUG( "UAB_RECORD_SEG" );
	}
	break;
	case USB_RECORD_LOOP: {
		DEBUG( "USB_RECORD_LOOP" );
		if ( dick_is_full < 0 )
		{
			ret = 5;
		}else{
			ret = 2;
			++(*next_incom_count);
		}
		DEBUG( "USB_RECORD_LOOP" );
	}
	break;
	}

	return(ret);
}


static void  initialize_bus( void *init_b, uint8_t* map_base )
{
	init_bus_t* init = (init_bus_t *) init_b;


	map_base[(BUS_OFFSET_ADDR + 0x07) / sizeof(uint8_t)] = 0x00;


	/*
	 * map_base[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)]	= 0x00;
	 * map_base[(BUS_OFFSET_ADDR + 6) / sizeof(uint8_t)]	= init->bus_t.usb_dir;
	 */

	while ( 0x01 == map_base[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)] )
	{
		map_base[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)] = 0x00;
		nano_sleep( 0, 1000 );
	}

	map_base[(BUS_OFFSET_ADDR + 6) / sizeof(uint8_t)]	= init->bus_t.usb_dir;
	map_base[(BUS_OFFSET_ADDR + 0x0a) / sizeof(uint8_t)]	= init->bus_t.usb_wraddr_b3124;
	map_base[(BUS_OFFSET_ADDR + 0x0b) / sizeof(uint8_t)]	= init->bus_t.usb_wraddr_b2316;
	map_base[(BUS_OFFSET_ADDR + 0x0c) / sizeof(uint8_t)]	= init->bus_t.usb_wraddr_b158;
	map_base[(BUS_OFFSET_ADDR + 0x0d) / sizeof(uint8_t)]	= init->bus_t.usb_wraddr_b70;

	map_base[(BUS_OFFSET_ADDR + 0x19) / sizeof(uint8_t)]	= 0x01;
	map_base[(BUS_OFFSET_ADDR + 0x19) / sizeof(uint8_t)]	= 0x00;
	map_base[(BUS_OFFSET_ADDR + 0x07) / sizeof(uint8_t)]	= init->bus_t.usb_op;
}


/*
 * 获得有效位
 */
static int  get_free_pos( void )
{
	int i;
	for ( i = 0; i < MYTPF_MAX; i++ )
	{
		if ( job[i] == NULL )
			return(i);
	}

	return(-1);
}


/*
 * 获取ts流写入的位置
 */
/*
 * static int get_sharemem_read_pos(uint8_t* map_base)
 * {
 * int result;
 * char w_pos;
 * w_pos = map_base[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)];
 * result = ((w_pos >> 1) & 0x1);
 * return (result);
 * }
 */


/*
 * 令牌管理
 */
/* #define text */
static void token_hander( uint8_t post )
{
	int	i, backcps;
	uint8_t flag = post;

	DEBUG("post:%d",flag);
	for ( i = 0; i < MYTPF_MAX; i++ )
	{
		if ( job[i] != NULL )
		{
			backcps = job[i]->cps;

			job[i]->ts_date = calloc( 1, BUFFER_SIZE_1M );

			if ( !job[i]->ts_date )
			{
				job[i]->token = backcps;
				DEBUG( "calloc()" );
				return;
			}

			if ( job[i]->token > job[i]->burst )
				job[i]->token = job[i]->burst;

#if 0
			if ( flag == 0 )
			{
				memcpy( job[i]->ts_date, sharemem_base, BUFFER_SIZE_1M );

				flag = 1;
			}else if ( flag == 1 )
			{
				memcpy( job[i]->ts_date, sharemem_base + BUFFER_SIZE_1M, BUFFER_SIZE_1M );
				flag = 0;
			}
#else

			if ( flag == 0 )
			{
				memcpy( job[i]->ts_date, sharemem_base, BUFFER_SIZE_1M );
			}else if ( flag == 1 )
			{
				memcpy( job[i]->ts_date, sharemem_base + BUFFER_SIZE_1M, BUFFER_SIZE_1M );
			}
#endif


			if ( squeue_t.flag == SEG_FLAG )
			{
				jobsqueue_date[squeue_t.node] = job[i]->ts_date;
			}else if ( squeue_t.flag == SEG_SFLAG )
			{
				jobsqueue_date[squeue_t.node] = job[i]->ts_date;
			}else{
				free( job[i]->ts_date );
				return;
			}

			++squeue_t.node;

			job[i]->token += backcps;
		}
	}
}


/*
 * 销毁数据
 */
static void destory_date( int count, int node )
{
	int i;

	for ( i = 0; i < count; i++ )
	{
		free( jobsqueue_date[node + i] );
		jobsqueue_date[node + i] = NULL;
	}
//	DEBUG( "clean date :%d", count );
}


/*
 * 初始化
 */
usb_token *tokens_init( int cps, int burst )
{
	int pos;

	usb_token *mem = NULL;

	pos = get_free_pos();
	if ( pos < 0 )
		return(NULL);

	mem = calloc( 1, sizeof(*mem) );
	if ( !mem )
		return(NULL);
	mem->token	= 0;
	mem->cps	= cps;
	mem->burst	= burst;
	mem->pos	= pos;

	job[pos] = mem;

	squeue_t.node	= 0;
	squeue_t.flag	= SEG_FLAG;

	return(mem);
}


static int min( int a, int b )
{
	if ( a < b )
		return(a);

	return(b);
}


/*
 * 获得令牌
 */
int fetch_token( usb_token *ptr, int size )
{
	int n;

	usb_token	*mem = ptr;
	struct timeval	tpstart;
	if ( size <= 0 )
		return(-EINVAL);
	gettimeofday( &tpstart, NULL );

	while ( mem->token <= 0 )
	{
		nano_sleep( 0, 10 );

		if ( r_time_out( tpstart ) >= DTS_STREAM_TIME )
		{
			break;
		}
	}


	n = min( mem->token, size );

	mem->token -= n;

	return(n);
}


/*
 * 返回令牌
 */
int return_token( usb_token *ptr, int size )
{
	usb_token *mem = ptr;

	if ( size <= 0 )
		return(-EINVAL);

	mem->token += size;

	if ( mem->token > mem->burst )
		mem->token = mem->burst;

	return(size);
}


int _destroy( usb_token *ptr )
{
	usb_token *mem = ptr;

	job[mem->pos] = NULL;
	free( mem );

	return(0);
}


/*
 * 节点
 */
static int node_seach()
{
	int node = -1;

	if ( squeue_t.flag == SEG_FLAG )
	{
		node		= 0;
		squeue_t.flag	= SEG_SFLAG;
		squeue_t.node	= 50;
	}else if ( squeue_t.flag == SEG_SFLAG )
	{
		node		= 50;
		squeue_t.flag	= SEG_FLAG;
		squeue_t.node	= 0;
	}

	return(node);
}


/*
 * 清理
 */

static void clean_date()
{
	int count, node;

	count = fetch_token( token, BURST );
	if ( count < 0 )
	{
		fprintf( stderr, "fetch_token()\n" );
		return;
	}

	if ( (node = node_seach() ) < 0 )
		return;

	if ( count > 0 )
		destory_date( count, node );
//	DEBUG( "clean :%d ", count );
}


/*
 * 写设备
 * sfd 文件描述符
 * node_count 写入的节点总数
 * node 节点的位置
 * wr_size 写入总大小(M)
 * incom_count  记录开始录制时的参数
 * off   循环时需要的开关
 */
static int  wr_usb( int *sfd, int node_count, int node, size_t *wr_size, void *init_b, int incom_count, int *off, int *start_stop )
{
	int	i_count = 0;
	ssize_t ret	= -1;

	s_config	* dconfig	= config_t();
	size_t		i_size		= dconfig->configParam.usb_tsfilesize;

//	DEBUG( "current package count : %d ", node_count );

	while ( i_count < node_count )
	{
		size_t len = BUFFER_SIZE_1M;

		if ( *wr_size >= i_size )
		{
			close( *sfd );
			init_bus_t * init = (init_bus_t *) init_b;

			if ( init->mod == USB_RECORD_SIG )
			{
				*start_stop	= -1;
				stop		= 0;
				*wr_size	= 0;
				ts_add_toal	= 0;
				
				break;
			}
			*wr_size	= 0;
			ts_add_toal	= 0;
			*sfd		= descriptor_generation( init_b, incom_count, off );
			if ( *sfd < 0 )
			{
				r_time_out_flag = 1;
				break;
			}
		}

		while ( len > 0 )
		{
			ret = write( *sfd, jobsqueue_date[node + i_count], len );
			if ( ret < 0 )
			{
				if ( errno == EINTR )
					continue;
				perror( "write()" );
				return(-1);
			}
			
			*wr_size	+= (ret / 1048664);
			len		-= ret;
			ts_add_toal++;
		}


		++i_count;
	}


	return(ret);
}


static int  creat_thread( void *init_b, pthread_attr_t *re_attr )
{
	int			result		= -1;
	size_t			stacksize	= 20480;
	static pthread_attr_t	attr;
	init_bus_t		* init = (init_bus_t *) init_b;

	result = pthread_attr_init( &attr );
	if ( result != 0 )
	{
		DEBUG( "pthread_attr_init :%s", strerror( errno ) );
	}

	result = pthread_attr_setstacksize( &attr, stacksize );

	result = pthread_create( &init->cache_copy, &attr, read_cache, init );
	if ( result != 0 )
	{
		DEBUG( "create thread fail \n" );
		return(result);
	}
	re_attr = &attr;
	return(result);
}


static void desory_thread( void *init_b, pthread_attr_t *attr )
{
	int result = -1;

	init_bus_t * init = (init_bus_t *) init_b;

	if ( pthread_join( init->cache_copy, NULL ) == 0 )
		DEBUG( "init_bus_base->cache_copy finish \n" );

	result = pthread_attr_destroy( attr );
	if ( result != 0 )
	{
		DEBUG( "pthread_attr_init :%s", strerror( errno ) );
	}
}


/*
 * 磁盘检查
 * 返回值
 * -1 磁盘空间不足 ,0 磁盘空间充足
 */
static int disk_check( void )
{
	int	free_size, ret = -1, i;
	char	path[100] = "";

	if ( !get_stata_path() )
		return(ret);

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

	if ( ret < 0 || path == NULL )
		return(ret);


	free_size = get_storage_dev_info( path, FREE_CAPACITY );
	if ( free_size < 0 )
		return(ret = -1);

	if ( free_size < MAX_FREE )
	{
		fprintf( stderr, "%s", "USB space is too small" );
		ret = -1;
	}

	return(ret);
}


static void record_ts_count( int first_incom_count, int next_incom_count )
{
	char orig[5] = "", repl[5] = "";
	gcvt_char( first_incom_count, 1, orig );
	gcvt_char( next_incom_count, 1, repl );

	config_set_config( SYS_ETC_CONF, orig, (uint8_t *) repl, "WriteRecord" );
}


/*
 * 产生文件
 * incom_count  记录开始录制时的参数
 * off   循环时需要的开关
 * 返回有效描述符
 */
static int descriptor_generation( void *init_b, int incom_count, int  *off )
{
	int		sfd			= -1, re_mod = 0, i_mod;
	char		path_name[PATHSIZE]	= "";
	int		next_incom_count	= 0;
	static int	first_incom_count	= 0;

	init_bus_t * init = (init_bus_t *) init_b;
	config_read( get_profile()->script_configfile );
	s_config * dconfig = config_t();

	next_incom_count	= dconfig->scfg_Param.stream_usb_used_count;
	i_mod			= init->mod;


	do
	{
		re_mod = mod_manage( i_mod, &next_incom_count );
		DEBUG( "re_mod = %d", re_mod );
		if ( re_mod == 4 )
		{
			stop	= 0;
			sfd	= -1;
			break;
		}else if ( re_mod == 5 )
		{
			/*
			 * 循环模式处理
			 * 不论何只记录第一次满足条件的值，其它任何时候不得改变
			 * 接着对满足first_incom_count后继的加值做重复置位操作
			 */
			if ( *off == 0 )
			{
				first_incom_count	= dconfig->scfg_Param.stream_usb_used_count;    /* 最后一次的记录 */
				next_incom_count	= incom_count;                                  /* 最初进来和值 */

				record_ts_count( first_incom_count, next_incom_count );
				DEBUG( "next_incom_count:%d max :%d", next_incom_count, first_incom_count );
				*off = 1;
			}

			++next_incom_count;

			if ( first_incom_count <= next_incom_count )
				next_incom_count = first_incom_count; /* 最初进来和值 */
		}


		if ( management_document( init->path, dconfig->configParam.usb_tsfilename, next_incom_count, path_name ) < 0 )
		{
			return(sfd);
			break;
		}

		sfd = open( path_name, O_WRONLY | O_CREAT | O_TRUNC );
		if ( sfd < 0 )
		{
			if ( errno != EINTR )
			{
				perror( "open()" );
			}
		}

		int f_incom_count = next_incom_count - 1;
		record_ts_count( f_incom_count, next_incom_count );
		tmp_dsplay = rindex( path_name, '/' );
	}
	while ( sfd < 0 );

	return(sfd);
}


/*
 * 处理写入设备
 */
static int  creat_job_thread( void *init_b )
{
	int		result		= -1, sfd = -1, count = 0;
	int		node		= 0, off = 0;
	int		start_stop	= 0;
	pthread_attr_t	attr;
	size_t		wr_size = 0;
	wait_stop = 0;
	config_read( get_profile()->script_configfile );
	s_config * dconfig = config_t();

	ts_lenth	= dconfig->configParam.usb_tsfilesize / 2; /* by jh 2016-8-17 */
	ts_add_toal	= 0.0;
	int incom_count = dconfig->scfg_Param.stream_usb_used_count;


	if ( creat_thread( init_b, &attr ) != 0 )
		return(result);

	sfd = descriptor_generation( init_b, incom_count, &off );
	if ( sfd < 0 )
		return(result);


	while ( 1 )
	{
		if ( stop <= 0 )
		{
			while ( wait_stop != 0 )
				usleep( 0 );
			break;
		}

		if ( (stop = recv_usb_notify() ) <= 0 )
			break;

		if ( start_stop < 0 )
		{
			while ( wait_stop != 1 )
				usleep( 0 );
			stop = 0;
			break;
		}

		nano_sleep( 2, 0 );

		count = fetch_token( token, BURST );
		if ( count < 0 )
		{
			fprintf( stderr, "fetch_token()\n" );
			return(-1);
			break;
		}

		if ( r_time_out_flag == 1 )
			break;

		if ( count == 0 )
			continue;

		if ( (node = node_seach() ) < 0 )
			continue;

		if ( (result = wr_usb( &sfd, count, node, &wr_size, init_b, incom_count, &off, &start_stop ) ) < 0 )
		{
			destory_date( count, node );
			break;
		}

		destory_date( count, node );
	}

/*	DEBUG( "-------------" ); */
	desory_thread( init_b, &attr );
/*	DEBUG( "-----2--------" ); */
	clean_date();

	close( sfd );

	_destroy( token );

	return(result);
}


/*
 * 写流时信息
 */
static void chech_message()
{
	static int	count		= 0;
	char		ch[20]		= "";
	char		ch_1[20]	= "";

	video_status_lock();
	s_config *dconfig = config_t();

	snprintf( ch, 16, "DVB-T %.3fM", dconfig->localstatus.cfig_ad9789_ftw_bpf );

	switch ( count )
	{
	case 0:
	{
		lcd_Write_String( 0, ch );
		lcd_Write_String( 1, " Record...      " );
		count++;
	}
	break;
	case 1:
	{
		int len = strlen( tmp_dsplay );
		snprintf( ch_1, len + 1, "%s", tmp_dsplay + 1 );
		lcd_Write_String( 0, ch );
		lcd_Write_String( 1, "                " );
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
uint8_t write_stream_info( void )
{
	signal_open( USB_WRITE );
	start_alarm();
	set_state_desplay( MONITOER_TRUE, USB_WRITE_SIGNAL_START, USB_WRITE_SIGNAL_FLAG );
	function_inter( chech_message );

	return(0);
}


/*
 * 消息设置
 */
static void  send_usb_message( pthread_t usb_action_t, int signum, void *dcfig, usb_handler usb_hand, int is_start )
{
	union sigval		usbsigval;
	static usb_operation_t	usb_cation;
	s_config		*dconfig = (s_config *) dcfig;
	usb_cation.op_mod	= dconfig->scfg_Param.stream_usb_record_mod;/* USB_SINGLE; */
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

uint8_t  send_usb_writ_message()
{
	extern pthread_t action_usbop;

	s_config *dconfig = config_get_config();

	send_usb_message( action_usbop, SIGUSR1, dconfig, writ_usb, START_R );

	nano_sleep( 0, 1000 );

	send_usb_stop_message( usb_sig, SIGUSR2, dconfig, START_R );

	return(1);
}


/*
 * 读中断
 */
static void inter_signal( uint8_t* map_addr, uint8_t *post )
{
	struct timeval	tpstart;
	char		tmp;
	gettimeofday( &tpstart, NULL );

	while ( ( (tmp = map_addr[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)]) & 0x1) != 1 )
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

	map_addr[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)] = tmp & 0xFE;

/*
 * if ( 1 == ((tmp = map_addr[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)])&0x1) )
 * {
 *      map_addr[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)] = tmp & 0xFE;
 * }
 */
}


/* #define __SET_TIME_P */
static void *read_cache( void *arg )
{
	init_bus_t	* init = (init_bus_t *) arg;
	uint8_t		post;

	if ( !init )
		return(NULL);

	token = tokens_init( CPS, BURST );
	if ( !token )
	{
		fprintf( stderr, "tokens_init()\n" );
		return(NULL);
	}
#if defined(__SET_TIME_P)
	float		timeuse;
	struct timeval	tpstart, tpend;
	int		t	= 0;
	int		count	= 0;
	gettimeofday( &tpstart, NULL );                 /* 开始计算程序运行时间 */
#endif
	while ( 1 )
	{
		inter_signal( init->mem, &post );

		if ( is_usb_online() != DEVACTT )       /*  */
			r_time_out_flag = 1;

		if ( r_time_out_flag == 1 )
			break;

		token_hander( post );

		if ( stop <= 0 )
		{
			wait_stop = 1;
			break;
		}

#if defined(__SET_TIME_P)
		/* 结束计算程序运行时间 */
		++count;
		gettimeofday( &tpend, NULL );
		timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);
		timeuse /= 1000000;
		printf( "\t%d Time : %f count=%d \r\n", ++t, timeuse, count );
#endif
	}

	if ( r_time_out_flag != 1 )
		stop = 1;
	else
		stop = 0;
	return(NULL);
}


void exit_retset_bus( void *init_b, uint8_t* map_base )
{
	/* init_bus_t* init = (init_bus_t *) init_b; */
	map_base[(BUS_OFFSET_ADDR + 0x07) / sizeof(uint8_t)]	= 0x00;
	map_base[(BUS_OFFSET_ADDR + 0x12) / sizeof(uint8_t)]	= 0x00;
	/* map_base[(BUS_OFFSET_ADDR + 6) / sizeof(uint8_t)]	= init->bus_t.usb_dir; */
}


static int  usb_write_handler( char *i_path, off_t size, int play_mod )
{
	int	ret = -1;
	int	fd, s_fd;

	system( RULER_STRING_ARG );

	if ( !i_path )
		return(ret);


	if ( (fd = open_mem_fd() ) < 0 )
		return(ret);

	init_t.mem = init_mem( MAP_SIZE, fd, MAP_BASE_ADDR );
	if ( !init_t.mem )
	{
		close_mem_fd( fd );
		return(ret);
	}

	if ( (s_fd = open_mem_fd() ) < 0 )
	{
		destory_mem( init_t.mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	sharemem_base = init_mem( MEMSIZE_2M + 0xFF, s_fd, MEMSTARTADD_2 );
	if ( !sharemem_base )
	{
		close_mem_fd( s_fd );
		destory_mem( init_t.mem, MAP_SIZE );
		close_mem_fd( fd );
		return(ret);
	}

	memset( sharemem_base, 0, MEMSIZE_2M + 0xFF );

	initialize_bus( &init_t, init_t.mem );
	init_t.mod = play_mod;
	memset( init_t.path, 0, PATHLENGTH );
	snprintf( init_t.path, strlen( i_path ) + 1, "%s", i_path );

	write_stream_info();
	comom_stream_info( write_stream_info );
	creat_job_thread( &init_t );
	exit_retset_bus( &init_t, init_t.mem );
	memset( sharemem_base, 0, MEMSIZE_2M + 0xFF );
	destory_mem( sharemem_base, MEMSIZE_2M + 0xFF );
	sharemem_base = NULL;
	close_mem_fd( s_fd );
	destory_mem( init_t.mem, MAP_SIZE );
	close_mem_fd( fd );
	r_time_out_flag			= 0;
	discontrl_t()->usb_wr_flag	= USBWRITESET;

	return(0);
}


static void usb_sop_notify()
{
	lcd_Write_String( 1, "                " );
	lcd_Write_String( 1, " stop...        " );
	nano_sleep( 1, 0 );
}


static int32_t writ_usb( void *usb_hand )
{
	int	ret		= -1, i;
	char	path[100]	= "";
	stop = 1;

	if ( is_usb_online() != DEVACTT )
	{
		discontrl_t()->usb_wr_flag = USBWRITESET;
		goto goback;
	}

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
	{
		DEBUG( "path error" );
		ret = -1;
		goto goback;
	}

	if ( path == NULL )
	{
		discontrl_t()->usb_wr_flag	= USBWRITESET;
		ret				= -1;
		goto goback;
	}


	if ( disk_check() < 0 )
	{
		lcd_Write_String( 0, " USB space is - " );
		lcd_Write_String( 1, " too small      " );
		nano_sleep( 1, 0 );
		discontrl_t()->usb_wr_flag	= USBWRITESET;
		ret				= -1;
		goto goback;;
	}

	usb_operation_t *usb_action = (usb_operation_t *) usb_hand;

	if ( !usb_action )
		goto goback;

	if ( usb_action->is_start == START_STOP )
	{
		discontrl_t()->usb_wr_flag	= USBWRITESET;
		ret				= -1;
		goto goback;
	}


	usb_write_handler( path, usb_action->ts_size, usb_action->op_mod );

goback:
	stop_alarm();
	signal_close();

	s_config *dconfig = config_get_config();
	send_usb_stop_message( usb_sig, SIGUSR2, dconfig, START_STOP );

	loop_cl_cah();

	usb_sop_notify();
	if ( discontrl_t()->record_auto_flag == USB_AUTO_HAND )
	{
		discontrl_t()->record_auto_flag = DEFAULTE;
		paren_menu();
	}else
		current_menu();
	return(ret);
}


