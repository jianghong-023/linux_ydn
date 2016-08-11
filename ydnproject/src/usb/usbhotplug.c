/*
 * �ļ����ƣ� usbhotplug.c
 * �ļ���ʶ��
 * ժ    Ҫ�� USB�豸̽�⼰�Զ����أ�ж��
 * ��ǰ�汾�� xxxxx
 * ��    �ߣ� jianghong_023<jianghong_023@163.com>
 * ������ڣ� 2015-08-21
 *
 * ȡ���汾�� ��
 * ԭ  ���ߣ� jianghong_023<jianghong_023@163.com>
 * ������ڣ� 2015-08-21
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <menu_foun.h>
#include <usbhotplug.h>
#include <debug.h>
#include <common_conf.h>
#include <init-configuration.h>


#define UEVENT_BUFFER_SIZE 2048


static usb_no_and_stat_t usb_dev_info;


usb_no_and_stat_t *get_stata_path()
{
	return(&usb_dev_info);
}


static int install_hotplug_sock( void )
{
	const int	buffersize = 1024;
	int		ret;

	struct sockaddr_nl snl;
	bzero( &snl, sizeof(struct sockaddr_nl) );
	snl.nl_family	= AF_NETLINK;
	snl.nl_pid	= getpid();
	snl.nl_groups	= 1;

	int sock = socket( PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT );
	if ( sock == -1 )
	{
		perror( "socket" );
		return(-1);
	}
	setsockopt( sock, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize) );

	ret = bind( sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl) );
	if ( ret < 0 )
	{
		perror( "bind" );
		close( sock );
		return(-1);
	}

	return(sock);
}


/*
 * ʶ���豸״̬
 * ����ֵ :
 * 0 �豸�γ�,1 �豸����,������Ч
 */
static int identify_act( char *status )
{
	int ret = -1;

	if ( strncasecmp( status, "add", strlen( status ) ) == 0 )
		ret = DEVACTT;
	else if ( strncasecmp( status, "remove", strlen( status ) ) == 0 )
		ret = DEVROMV;


	return(ret);
}


/*
 * �Ƴ��豸
 */
int dev_umount( void  *usb_dev_info )      /* ok */
{
	usb_no_and_stat_t *usb_dev = (usb_no_and_stat_t *) usb_dev_info;


	int i = 0, ret = -1;

	int index = 0;

	if ( !usb_dev )
		return(ret);

	if ( usb_dev->flag_off == 0 )
	{
		usb_dev->flag_off = 1;
	}else
		return(ret);

	if ( !usb_dev->devfile_parts )
	{
		DEBUG( "Device does not exist " );
		return(ret);
	}

	if ( usb_dev->part_num == 0 )
		return(ret);


	if ( usb_dev->part_num > 1 )
	{
		i++; /* if disk ignore first part disc */
	}

	for (; i < usb_dev->part_num; i++ )
	{
		if ( !usb_dev->mount_path || !usb_dev->mount_path[i] )
		{
			DEBUG( "directory does not exist" );
			break;
		}

		if ( umount2( usb_dev->mount_path[i], MNT_FORCE ) == 0 ) /* ע�ⲻ��umount /dev/sda1 ����umount /tmp/disk/ ֮����û��ܷ��ʵ�·�� */
		{
			DEBUG( "umount %s success\n", usb_dev->mount_path[i] );
		} else {
			DEBUG( "umount %s failed\n", usb_dev->mount_path[i] );
		}
		usb_dev->is_active = DEVROMV;

		++index;
	}

	return(ret);
}


/*
 * ·������
 *
 */
static void dev_mount( usb_no_and_stat_t  *usb_dev )
{
	char	tmpdir[50]	= { 0 };
	DIR	* p_dir		= NULL;
	int	index		= 0, i = 0, ret = 0;

	usb_dev->flag_off = 0;

	if ( !usb_dev->devfile_parts )
	{
		DEBUG( "Device does not exist " );
		return;
	}

	if ( p_dir == opendir( USB_DISK_MP ) )
	{
		ret = mkdir( USB_DISK_MP, (0777) );
	}
	closedir( p_dir );
	if ( ret == -1 )
	{
		memset( usb_dev->mount_path[i], 0, 100 );
		return;
	}

	if ( usb_dev->part_num > 1 )
	{
		i++;
	}
	for (; i < usb_dev->part_num; i++ ) /* mount -t vfat -o umask=000,noatime,async,rw /dev/sda1 tmp/ */
	{
		sprintf( tmpdir, USB_DISK_MP "/disk%d", index );
		if ( p_dir == opendir( tmpdir ) )
			ret = mkdir( tmpdir, (0777) );
		closedir( p_dir );

		memset( usb_dev->mount_path[i], 0, 100 );

		if ( ret == -1 )
			return;

		sprintf( usb_dev->mount_path[i], USB_DISK_MP "/disk%d/part%d", index, i );
		mode_t mask = umask( 0777 );

		if ( p_dir == opendir( usb_dev->mount_path[i] ) )
		{
			ret = mkdir( usb_dev->mount_path[i], (0777 & ~mask) );
			if ( ret == -1 )
			{
				perror( "mkdir" );
				memset( usb_dev->mount_path[i], 0, 100 );
				continue;
			}
		}
		closedir( p_dir );
		++index;

		if ( ret == -1 )
			return;

		ret = mount( usb_dev->devfile_parts[i], usb_dev->mount_path[i], "vfat", 0, "codepage=437,iocharset=iso8859-1" );
		if ( ret == -1 )
		{
			perror( "mount()" );
			memset( usb_dev->mount_path[i], 0, 100 );
			return;
		}

		usb_dev->is_active = DEVACTT;

		auto_usb_test();
	}
}


/*
 * �豸�������Ƴ�
 *
 */
static void dev_info_hand( int ret_status, usb_no_and_stat_t  *usb_dev )
{
	switch ( ret_status )
	{
	case DEVACTT: {
		dev_mount( usb_dev );
	}
	break;

	case DEVROMV: {
		dev_umount( usb_dev );
	}
	break;
	default:
		DEBUG( "Equipment logo does not match" );
	}
}


/*
 * �豸����
 * devfile_parts  �����豸·��
 * n_part  0��֧�ַ�����1 ֧�ַ���
 * �����豸��
 */
static int seach_dev( char *dev_no, usb_no_and_stat_t  *usb_dev, int n_part )
{
	int parts_num = -1, length, i;
	length = strlen( dev_no );

	for ( i = 0; i < length && dev_no[i] != '\0'; i++ )
	{
		if ( dev_no[i] == 's' )
		{
			++i;
			if ( dev_no[i] == 'd' )
			{
				++i;
				if ( dev_no[i] >= 'a' && dev_no[i] <= 'z' )
				{
					++i;

					if ( dev_no[i] >= '1' && dev_no[i] < '9' )
					{
						parts_num = dev_no[i] - 48;
						if ( n_part == 1 )
						{
							usb_dev->part_num += 1;
							sprintf( usb_dev->devfile_parts[parts_num - 1], "/dev/%s", dev_no );
						}else{
							usb_dev->part_num = 1;
							sprintf( usb_dev->devfile_parts[parts_num - 1], "/dev/%s", dev_no );
						}
						break;
					}else
						break;
				}else
					break;
			}else
				break;
		}else
			break;
	}

	return(parts_num);
}


/*
 * ��  �ܣ������豸�����ҵ������豸����Ӧ�Ĵ���
 * ��  ������
 * ����ֵ����
 * ע    ����ѯ���ʱ��Ϊ2s
 */
void thread_for_usb()
{
	int hotplug_sock, part_num;
	hotplug_sock = install_hotplug_sock();
	usb_no_and_stat_t *info_usb = &usb_dev_info;

	while ( 1 )
	{
		char	*strtok_tmp_ptr = NULL;
		char	*status, *dev_no;
		char	dev_ord[8]			= "";
		char	buf[UEVENT_BUFFER_SIZE * 2]	= { 0 };


		recv( hotplug_sock, &buf, sizeof(buf), 0 );


		dev_no	= rindex( buf, '/' );
		status	= strtok_r( buf, "@", &strtok_tmp_ptr );

		if ( (!dev_no) || (!status) )
			continue;

		DEBUG( "dev_no = %s  status= %s", dev_no, status );
		snprintf( dev_ord, strlen( dev_no ), "%s", dev_no + 1 );

		int ret = identify_act( status );
		if ( ret == DEVACTT )
		{
			part_num = seach_dev( dev_ord, info_usb, 0 );
			if ( part_num > 0 )
				dev_info_hand( ret, info_usb );
		}else if ( ret == DEVROMV )
			dev_info_hand( ret, info_usb );
	}
}


