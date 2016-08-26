#include <sys/mman.h>
#include <common_interface.h>
#include <usbhotplug.h>
#include <usb_signal.h>
#include <key-ctl.h>
#include <common_conf.h>
#include <menu_foun.h>


/*
 *
 * usb 实时检测是否在线
 * */
int is_usb_online( void )
{
	int retoline = -1;

	if ( get_stata_path()->is_active != DEVROMV )
		retoline = DEVACTT;
	else
		retoline = DEVROMV;

	return(retoline);
}


/*
 * 返回值; 1读状态,0 停止
 */
static int32_t is_usb_status( int32_t mod )
{
	int32_t retmod = -1;

	if ( mod == START_R )
		retmod = (mod & 0x1);
	else if ( mod == STARTSTOP )
		retmod = (mod & 0x0);

	return(retmod);
}


/*
 * 等待USB消息通知
 */
uint32_t recv_usb_notify( void )
{
	int rcv_mod, retstat;

	rcv_mod = get_usb_signal();
	retstat = is_usb_status( rcv_mod );

	return(retstat);
}


/*
 * usb 检测是否插入
 */
int usb_probe( void )
{
	float		timeuse = 0.0, sdtime, rstime;
	int		mask	= 0, ret = -1; /* 防止超时检测时不断刷新 */
	struct timeval	tpstart;
	

	while ( 1 )
	{
		 discontrl_t()->delay_statusl = DELAY_ON;

		while(get_usb_status() != 0)
			nano_sleep( 0, 1000 );
		 
		if ( get_stata_path()->is_active != DEVROMV )
		{
			if ( get_stata_path()->mount_path[0] != NULL )
			{
				lcd_clear( discontrl_t()->lcdfd ); /* 并清除内存 */
				lcd_Write_String( 0, "Connect USB...  " );
				lcd_Write_String( 1, "                " );
				nano_sleep( 0, 500000000 );
				ret = 0;
				discontrl_t()->delay_statusl = DELAY_OFF;
				break;
			}
		} else if ( mask == 0 )
		{
			mask = 1;
			lcd_clear( discontrl_t()->lcdfd ); /* 并清除内存 */
			
			lcd_Write_String( 0, "Please insert US" );
			lcd_Write_String( 1, "B               " );
			
			nano_sleep( 0, 500000000 );
			gettimeofday( &tpstart, NULL );
			timeuse = 1000000 * (tpstart.tv_sec) + tpstart.tv_usec;
		}
		gettimeofday( &tpstart, NULL );

		sdtime	= 1000000 * (tpstart.tv_sec) + tpstart.tv_usec;
		rstime	= sdtime - timeuse;
		nano_sleep( 0, 1000 );
		if ( rstime > TMIER_OUT )
		{
			lcd_clear( discontrl_t()->lcdfd ); /* 并清除内存 */
			
			lcd_Write_String( 0, "USB timeout...  " );
			lcd_Write_String( 1, "                " );

			nano_sleep( 0, 500000000 );
			discontrl_t()->delay_statusl = DELAY_OFF;
			return(-1);
		}
		ret = 0;
	}

	return(ret);
}


/*
 * 打开ts流文件
 */
int open_stream_file( char *streamfilename )
{
	int sfd;
	sfd = open( streamfilename, O_RDONLY, S_IRUSR | S_IWUSR );
	if ( sfd < 0 )
	{
		perror( "open_stream_file()\n" );
	}

	return(sfd);
}


/*
 * 关闭ts流文件usb_action_t
 */
void close_stream_file( int sfd )
{
	close( sfd );
}


/*
 * 建立内存映射区
 */
uint8_t* init_mem( size_t mapsize, size_t fd, off_t map_addr )
{
	uint8_t* map_base;
	map_base = mmap( NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_addr ); /* 0x10000, 0xFF200000 */
	if ( map_base == MAP_FAILED )
	{
		perror( "init_mem()\n" );
	}
	return(map_base);
}


/*
 * 销毁内存映射区
 */
void destory_mem( void *addr, size_t mapsize )
{
	munmap( addr, mapsize );
}


/*
 * 打开内存区
 */
int open_mem_fd( void )
{
	int fd;

	fd = open( "/dev/mem", O_RDWR | O_SYNC );
	if ( fd < 0 )
	{
		perror( "open_mem()\n" );
	}

	return(fd);
}


/*
 * 关闭内存区
 */
void close_mem_fd( int fd )
{
	close( fd );
}


