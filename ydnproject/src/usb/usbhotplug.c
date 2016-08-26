/*
 * 文件名称： usbhotplug.c
 * 文件标识：
 * 摘    要： USB设备探测及自动挂载，卸载
 * 当前版本： xxxxx
 * 作    者： jianghong_023<jianghong_023@163.com>
 * 完成日期： 2015-08-21
 *
 * 取代版本： 无
 * 原  作者： jianghong_023<jianghong_023@163.com>
 * 完成日期： 2015-08-21
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
#include <unistd.h>
#include <fcntl.h>
#include <linux/fd.h>
#include <sys/mount.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <menu_foun.h>
#include <usbhotplug.h>
#include <debug.h>
#include <common_conf.h>
#include <init-configuration.h>


#define UEVENT_BUFFER_SIZE 2048


static usb_no_and_stat_t usb_dev_info;

static int usb_pl_status;


usb_no_and_stat_t *get_stata_path()
{
	return(&usb_dev_info);
}


static void set_usb_status( int value )
{
	usb_pl_status = value;
}


int get_usb_status( void )
{
	return(usb_pl_status);
}


static void delay_time()
{
	set_usb_status( 1 );
	usb_pl_message();
	nano_sleep( 3, 0 );
	current_menu();
	set_usb_status( 0 );
}


/*
 * 弹出usb
 */
int  eject_usb( void *usb_stat_t )
{
	int		usb_fd;
	int		result = -1;
	int		flage;
	sg_io_hdr_t	d_usb;
	unsigned char	allow_r_b[6]		= { ALLOW_MEDIUM_REMOVAL, 0, 0, 0, 0, 0 };;
	unsigned char	start_stop_d_b[6]	= { START_STOP, 0, 0, 0, 1, 0 };
	unsigned char	start_stop_d_2b[6]	= { START_STOP, 0, 0, 0, 2, 0 };
	unsigned char	t_list[2];
	unsigned char	buffer[32];

	usb_no_and_stat_t *device_path = (usb_no_and_stat_t *) usb_stat_t;

	if ( !device_path )
		return(result);


	usb_fd = open( device_path->devfile_parts[0], O_RDONLY | O_NONBLOCK );
	if ( usb_fd < 0 )
		return(result);
	memset( &d_usb, 0, sizeof(sg_io_hdr_t) );


	if ( (ioctl( usb_fd, SG_GET_VERSION_NUM, &flage ) < 0) || (flage < 30000) )
	{
		DEBUG( "not an sg device, or old sg driver\n" );
		goto error__;
	}

	d_usb.interface_id	= 'S';
	d_usb.cmd_len		= 6;
	d_usb.mx_sb_len		= sizeof(buffer);
	d_usb.dxfer_direction	= SG_DXFER_NONE;
	d_usb.dxfer_len		= 0;
	d_usb.dxferp		= t_list;
	d_usb.sbp		= buffer;
	d_usb.timeout		= 10000;
	d_usb.cmdp		= allow_r_b;

	result = ioctl( usb_fd, SG_IO, (void *) &d_usb );
	if ( result < 0 )
	{
		goto error__;
	}
	d_usb.cmdp	= start_stop_d_b;
	result		= ioctl( usb_fd, SG_IO, (void *) &d_usb );
	if ( result < 0 )
	{
		goto error__;
	}
	d_usb.cmdp	= start_stop_d_2b;
	result		= ioctl( usb_fd, SG_IO, (void *) &d_usb );
	if ( result < 0 )
	{
		goto error__;
	}

	ioctl( usb_fd, BLKRRPART );

	memset( device_path->devfile_parts[0], 0, MAX_NAME_LEN );
	memset( device_path->mount_path[0], 0, MAX_NAME_LEN );
	device_path->is_active = DEVROMV;
	close( usb_fd );

	DEBUG( "result :%d ", result );
	return(result);

error__:
	close( usb_fd );
	return(result);
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
 * 识别设备状态
 * 返回值 :
 * 0 设备拔出,1 设备插入,其它无效
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
 * 移出设备
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

		if ( umount2( usb_dev->mount_path[i], MNT_FORCE ) == 0 ) /* 注意不是umount /dev/sda1 而是umount /tmp/disk/ 之类的用户能访问的路径 */
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
 * 路径分配
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
			DEBUG( "usb_dev->devfile_parts[i]:%s usb_dev->mount_path[i]:%s", usb_dev->devfile_parts[i], usb_dev->mount_path[i] );
			perror( "mount()" );
			memset( usb_dev->mount_path[i], 0, 100 );
			return;
		}

		usb_dev->is_active = DEVACTT;

		delay_time();
		auto_usb_test();
	}
}


/*
 * 设备挂载与移出
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
 * 设备搜索
 * devfile_parts  返回设备路径
 * n_part  0不支持分区，1 支持分区
 * 返回设备号
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
						DEBUG( "dev_no :%s", dev_no );
						if ( n_part == 1 )
						{
							usb_dev->part_num += 1;
							sprintf( usb_dev->devfile_parts[parts_num - 1], "/dev/%s", dev_no );
						}else{
							usb_dev->part_num = 1;
							sprintf( usb_dev->devfile_parts[0], "/dev/%s", dev_no );
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
 * 功  能：查找设备，对找到后有设备做相应的处理
 * 参  数：无
 * 返回值：无
 * 注    ：查询间隔时间为2s
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


