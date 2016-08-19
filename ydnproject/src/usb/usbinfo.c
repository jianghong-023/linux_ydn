
#include <sys/mount.h>
#include <usbinfo.h>
#include <common_conf.h>
#include <stdint.h>
#include <usbhotplug.h>
#include <key-ctl.h>
#include <debug.h>
#include <menu_foun.h>


/*
 *
 *
 * ȡ�� usb �豸��Ϣ
 *
 *
 *************/
static int getSusbinfo( char * usbmountpoint )
{
	struct statfs	stat_fs;
	float		freebytes	= 0;
	float		totalbytes	= 0;
	char		t_buf[16];
	char		f_buf[16];


	if ( statfs( usbmountpoint, &stat_fs ) == -1 ) /* ��ȡ������״̬ */
	{
		DEBUG( "statfs failed for path->[%s]\n", usbmountpoint );
		return(-1);
	}

	/* ��ϸ�ķ��������� */
	totalbytes = (uint64_t) stat_fs.f_blocks * (uint64_t) stat_fs.f_frsize / 1024 / 1024;
	/* ��ϸ��ʣ��ռ����� */
	freebytes = (uint64_t) stat_fs.f_bfree * (uint64_t) stat_fs.f_frsize / 1024 / 1024;

	sprintf( t_buf, "Total:%.2fMB", totalbytes );
	sprintf( f_buf, "Free :%.2fMB", freebytes );
	lcd_clear( discontrl_t()->lcdfd ); /* ������ڴ� */
	lcd_Write_String( 0, t_buf );
	lcd_Write_String( 1, f_buf );

	nano_sleep( 3, 0 );

	return(0);
}


uint64_t get_usb_free_size( char * usbmountpoint )
{
	struct statfs	stat_fs;
	uint64_t	freebytes = 0;

	if ( statfs( usbmountpoint, &stat_fs ) == -1 ) /* ��ȡ������״̬ */
	{
		DEBUG( "statfs failed for path->[%s]\n", usbmountpoint );
		return(-1);
	}


	/* ��ϸ��ʣ��ռ����� */
	freebytes = (uint64_t) stat_fs.f_bfree * (uint64_t) stat_fs.f_frsize;

	return(freebytes);
}


/**
 *
 *
 * �ȴ�usb������
 *
 *
 ********************/
static int loop_usb_sat()
{
	float		timeuse, sdtime, rstime;
	struct timeval	tpstart;
	int		i;

	gettimeofday( &tpstart, NULL );

	timeuse = 1000000 * (tpstart.tv_sec) + tpstart.tv_usec;

	while ( 1 )
	{
		usleep( 100 );

		if ( get_stata_path()->is_active != DEVROMV )
		{
			for ( i = 0; i < get_stata_path()->part_num; i++ )
			{
				if ( get_stata_path()->mount_path[i] != NULL )
				{
					return(0);
					break;
				}
			}
		} else {
			lcd_clear( discontrl_t()->lcdfd ); /* ������ڴ� */
			lcd_Write_String( 0, "Please insert US" );
			lcd_Write_String( 1, "B device        " );
			/* DEBUG("Please wait for you to find the USB path for the file\n"); */
			nano_sleep( 1, 0 );
		}
		gettimeofday( &tpstart, NULL );

		sdtime	= 1000000 * (tpstart.tv_sec) + tpstart.tv_usec;
		rstime	= sdtime - timeuse;

		if ( rstime > TMIER_OUT )
		{
			lcd_clear( discontrl_t()->lcdfd ); /* ������ڴ� */
			lcd_Write_String( 0, "USB timeout...  " );
			lcd_Write_String( 1, "                " );
			nano_sleep( 1, 0 );

			return(-1);
			break;
		}
	}
}


/**
 *
 * ���usb�����������������Ϣ
 *
 *
 *
 ***********/
uint8_t get_usb_usrinfo()
{
	static int	falg		= 0, i;
	char		FileName[50]	= { 0 };


	if ( falg == 0 )
	{
		falg = 1;
		if ( loop_usb_sat() < 0 )
		{
			DEBUG( "find device fail" );

			return(-1);
		}
	} else {
		return(-1);
	}

	for ( i = 0; i < get_stata_path()->part_num; i++ )
	{
		if ( get_stata_path()->mount_path[i] == NULL )
			return(0);
		sprintf( FileName, "%s", get_stata_path()->mount_path[i] );
		getSusbinfo( FileName );
	}
	falg = 0;

	return(0);
}


/**
 *
 * ж��usb
 *
 *
 *******************/
uint8_t manu_do_mount_dev()
{
	/* ж��usb�豸 */
	static int	falg		= 0, i;
	char		FileName[50]	= { 0 };

	if ( falg == 0 )
	{
		falg = 1;
		if ( get_stata_path()->is_active == DEVROMV )
		{
			for ( i = 0; i < get_stata_path()->part_num; i++ )
			{
				if ( get_stata_path()->mount_path[i] == NULL )
					return(0);
				if ( get_stata_path()->mount_path[i] != NULL )
				{
					lcd_clear( discontrl_t()->lcdfd ); /* ������ڴ� */
					lcd_Write_String( 0, "Not device remov" );
					lcd_Write_String( 1, "e...           " );
					nano_sleep( 1, 0 );
					return(-1);
				}
			}
		} else {
			for ( i = 0; i < get_stata_path()->part_num; i++ )
			{
				if ( get_stata_path()->mount_path[i] == NULL )
					return(0);
				sprintf( FileName, "%s", get_stata_path()->mount_path[i] );

				if ( umount2( FileName, MNT_FORCE ) != 0 )
					DEBUG( "umount %s failed\n", FileName );


				int ret = eject_usb( get_stata_path() );
				if ( ret != -1 )
				{
					char tmpdir[50] = { 0 };

					sprintf( tmpdir, "rm -rf %s", FileName );
					system( tmpdir );
					DEBUG( "umount %s success\n", FileName );

					lcd_clear( discontrl_t()->lcdfd ); /* ������ڴ� */
					lcd_Write_String( 0, "Successful remov" );
					lcd_Write_String( 1, "al of USB       " );
					nano_sleep( 2, 0 );
				}
			}
		}
	}

	falg = 0;

	return(0);
}