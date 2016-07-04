#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gpio_config.h>
#include <defineident.h>
#include <debug.h>


/*
 * 根据当前是HDMI/CBVS/YPbPr设置模式
 *
 *
 */
void gpio_input_mod( uint8_t i_mod )
{
	int	fd;
	char	*base;
	fd = open( "/dev/mem", O_RDWR | O_SYNC );
	if ( -1 == fd )
	{
		printf( "error fd\n" );
		exit( -1 );
	}

	base = mmap( NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xFF200000 );
	if ( base == 0 )
	{
		printf( "base error!\n" );
		exit( -1 );
	}

	base[0x930] = i_mod;

	close( fd );

	munmap( base, 0x1000 );
}


/*
 * LCD 背光开关
 * onoff 0x1开，0x0关
 */
void lcd_bkl_enable( uint8_t onoff )
{
	int	fd;
	char	*base;
	fd = open( "/dev/mem", O_RDWR | O_SYNC );
	if ( -1 == fd )
	{
		printf( "error fd\n" );
		exit( -1 );
	}

	base = mmap( NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xFF200000 );
	if ( base == 0 )
	{
		printf( "base error!\n" );
		exit( -1 );
	}
	
	base[0x800] = onoff&0x01;

	close( fd );

	munmap( base, 0x1000 );
}


