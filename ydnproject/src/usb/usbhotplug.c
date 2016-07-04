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


#include <usbhotplug.h>
#include <debug.h>


static void CLEAR_DEV( void );


static int CHECK_PARTS( SCSI_USB_DEV *dev );


static int ADD_DEV( SCSI_USB_DEV *dev );


static int INIT_DEV( SCSI_USB_DEV *dev, int index, char *type );


static int check_mount( SCSI_USB_DEV *dev );


static int do_mount( SCSI_USB_DEV *dev );


static int process_dev( SCSI_USB_DEV *dev );


static int find_device( void );


static SCSI_USB_DEV	*f_first_dev;
static SCSI_USB_DEV	*back_first_dev;
static bool		insert			= FALSE;
static bool		find_attached		= FALSE;
static bool		find_new		= FALSE;
static int		is_manual_umount	= 1;

static STATA_PATH *stata_path;


/* 它需要和分区检查共享一个变量，来作为一个小小的延时
 * 困操作系统的反应不是很快，所以不能做一次判断，为了不浪费
 * 资源又不能一会会就双来一次判断
 * 所以特别设置此标志 disc_flag,并且在卸载时清零
 */
int disc_flag = 0;


STATA_PATH *get_stata_path()
{
	return(stata_path);
}


#if 0 /* 暂时不用 */
char  *str_replace( const char *string, const char *substr, const char *replacement )
{
	char	*tok	= NULL;
	char	*newstr = NULL;
	char	*oldstr = NULL;

	if ( substr == NULL || replacement == NULL )
		return(strdup( string ) );

	newstr = strdup( string );

	while ( (tok = strstr( newstr, substr ) ) )
	{
		oldstr	= newstr;
		newstr	= malloc( strlen( oldstr ) - strlen( substr ) + strlen( replacement ) + 1 );
		if ( newstr == NULL )
		{
			free( oldstr );
			return(NULL);
		}

		memcpy( newstr, oldstr, tok - oldstr );
		memcpy( newstr + (tok - oldstr), replacement, strlen( replacement ) );
		memcpy( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen( substr ), strlen( oldstr ) - strlen( substr ) - (tok - oldstr) );
		memset( newstr + strlen( oldstr ) - strlen( substr ) + strlen( replacement ), 0, 1 );
		free( oldstr );
		usleep( 0 );
	}
	return(newstr);
}


#endif


/*
 * 功  能：添加新设备到链表中
 * 参  数：设备结构体指针dev
 * 返回值：KK_FS_OK
 */
static int ADD_DEV( SCSI_USB_DEV *dev )
{
	if ( f_first_dev )
	{
		do_umount( f_first_dev );
		dev->next_dev	= f_first_dev;
		f_first_dev	= dev;
	} else {
		f_first_dev = dev;

		dev->next_dev = NULL;
	}
	return(KK_FS_OK);
}


/*
 * 功  能：设备初始工作
 * 参  数：设备结构体指针dev ，index,type
 * 返回值：KK_FS_OK
 */
static int INIT_DEV( SCSI_USB_DEV *dev, int index, char *type )
{
	dev->index = index;

	if ( !strncmp( type, "CD-ROM", 6 ) ) /* cd-rom //这个暂时没用，留待以后扩展使用 */
	{
		DEBUG( "CD-ROM\n" );
		dev->type	= 1;
		dev->part_num	= 1;
		sprintf( dev->devfile_parts[0], FILE_DEV_PART_TEMPL "cd", index );
		strcpy( dev->mount_path[0], USB_CDROM_MP );
		DEBUG( "---%d---,%s,%s\n", dev->index, dev->devfile_parts[0], dev->mount_path[0] );
	} else { /* usb disk */
		dev->type	= 2;
		dev->part_num	= CHECK_PARTS( dev );
	}
	return(KK_FS_OK);
}


/*
 * 功  能：如果在/proc/scsi/scsi/中发现的设备是usb-storage，则进一步在/proc/partitions/中分析其所
 *                包含几个分区，并将其记录下来dev->devfile_parts，mount路径也一并设置dev->mount_path
 *                例如发现一个U盘，仅仅有一个分区，系统将其挂载在/dev/sda1/下，则可以将其mount路径设置成
 *                用户可以访问的/tmp/usbdisk/disk0/part0/下
 * 参  数：设备结构体指针dev
 * 返回值：part_num(分区数量)
 */
static int CHECK_PARTS( SCSI_USB_DEV *dev )
{
	int	len		= 0;
	int	part_num	= 0;
	char	buf[512]	= { 0 };                /* 1024 is enough for save information of FILE partitions */

	char	*delim		= "\n";
	char	*line		= NULL;
	char	*strtok_tmp_ptr = NULL;

	char	*seek_sd	= NULL;                 /* USED FOR DEVICE MOUNTED ON SD* */
	char	*seek_hd	= NULL;                 /* USED FOR DEVICE MOUNTED ON HD* */

	FILE *fd = NULL;

	fd = fopen( FILE_DISC_PARTS_CHECK, "r" );       /* 直接操作open打开系统文件 */
	if ( fd != NULL )
	{
		len = fread( buf, 1, sizeof(buf), fd );
/*		DEBUG("---%s",buf); */
		fclose( fd );
		if ( len > 0 )
		{
			line = strtok_r( buf, delim, &strtok_tmp_ptr );

			while ( line )
			{
				seek_sd = strstr( line, "sd" );
				seek_hd = strstr( line, "hd" );

				if ( seek_sd )
				{
					if ( seek_sd[2] >= 'a' && seek_sd[2] <= 'z' )
					{
						if ( seek_sd[3] >= '1' && seek_sd[3] < '9' )
						{
							DEBUG( "find device [%s]\n", seek_sd );
							sprintf( dev->devfile_parts[part_num], "/dev/%s", seek_sd );
							sprintf( dev->mount_path[part_num], USB_DISK_MP "/disk%d/part%d", dev->index, part_num );
							/*
							 *  usbstatus: ok
							 *  usbsdx: /dev/sdax
							 *  usbdir: /tmp/diskx/partx/
							 * 
							 * 记录设备信息
							 */
							if ( find_new == TRUE )
							{
								disc_flag	= 1;
								find_new	= FALSE;
								char tmp_buf[50];

								memset( dev->hoststr, 0, 200 );

								/* 存储路径 */
								pthread_mutex_lock( &stata_path->usblock );

								memset( stata_path->hostusbpath, 0, strlen( stata_path->hostusbpath ) );
								memset( stata_path->diskpath, 0, strlen( stata_path->diskpath ) );

								sprintf( stata_path->hostusbpath, "%s", dev->mount_path[part_num] );
								sprintf( stata_path->diskpath, "%s", dev->devfile_parts[part_num] );
								sprintf( tmp_buf, "umount -l %s", stata_path->hostusbpath );
								system( tmp_buf );
								stata_path->is_active = DEVACTT;

								pthread_mutex_unlock( &stata_path->usblock );

								DEBUG( "Find new devcie \r\n" );
							}

							part_num++;
							if ( part_num == MAX_PART_NUM )
							{
								break;/* too many parts ignore */
							}
						}
					}
				}
				if ( seek_hd )
				{
					if ( seek_hd[2] >= 'a' && seek_hd[2] <= 'z' )
					{
						if ( seek_hd[3] >= '1' && seek_hd[3] < '9' )
						{
/*                            DEBUG("find device [%s] \n",seek_sd); */
							sprintf( dev->devfile_parts[part_num], "/dev/%s", seek_hd );
							sprintf( dev->mount_path[part_num], USB_DISK_MP "/disk%d/part%d", dev->index, part_num );
							part_num++;
							if ( part_num == MAX_PART_NUM )
							{
								break;/* too many parts ignore */
							}
						}
					}
				}
				line = strtok_r( NULL, delim, &strtok_tmp_ptr );
				usleep( 0 );
			}
		} else {
			DEBUG( "len %d", len );
		}
	} else {
		perror( FILE_DISC_PARTS_CHECK );
	}
	return(part_num);
}


/*
 * 功  能：线程扫描时会不停的调用该接口去清除之前获取的设备信息
 * 参  数：无
 * 返回值：无
 */
static void CLEAR_DEV( void )
{
	SCSI_USB_DEV	*cur_dev = f_first_dev;
	SCSI_USB_DEV	*tmp_dev;
	while ( cur_dev )
	{
		/* 清除时的备份信息 */
		memcpy( back_first_dev, cur_dev, sizeof(SCSI_USB_DEV) );
		tmp_dev = cur_dev;
		cur_dev = cur_dev->next_dev;
		free( tmp_dev );
		usleep( 0 );
	}
	f_first_dev = NULL;
}


/*
 * 功  能：查找设备数和类型；
 * 参  数：无
 * 返回值：无
 */
static int find_device()
{
	FILE	*fd		= NULL;
	int	len		= 0;
	int	dev_num		= 0;
	char	buf[512]	= { 0 };
	char	*seek		= NULL;
	/* char *tmp = NULL; */
	SCSI_USB_DEV *new_dev = NULL;

	/* clear exist device */
	CLEAR_DEV();

	/* add new device */
	fd = fopen( FILE_DEV_CHECK, "r" );

	if ( fd > 0 )
	{
		/*
		 * 
		 * 如果没有任何设备接入，那么/proc/scsi/scsi中仅仅包含下列字符 "Attached devices:"
		 * 文件长度为18。所以下面以20个字符来做判断。
		 * 
		 */
		len = fread( buf, 1, sizeof(buf), fd );
		fclose( fd );
		if ( len < 20 && find_attached == TRUE )
		{
			DEBUG( "Device removed......%d \n", find_attached );


			if ( find_attached == TRUE ) /*  // 防止无止境卸载设备 */
			{
				find_attached = FALSE;
				pthread_mutex_lock( &stata_path->usblock );
				stata_path->is_active = DEVROMV;
				pthread_mutex_unlock( &stata_path->usblock );
				do_umount( back_first_dev );
			}

			return(KK_FS_OK);
		}
		if ( len > 0 )
		{
			/*
			 * 
			 * 如果文件打开成功，而且长度大于20，表示里面是有设备记录的，如果find_attached标志之前是FALSE
			 * 表示有新的动作:设备插入系统
			 * 
			 */
			if ( len > 20 && find_attached == FALSE )
			{
				find_attached	= TRUE;
				find_new	= TRUE;
				DEBUG( "Device Inserted......\n" );
			}

			if ( disc_flag == 0 ) /* 它需要和分区检查共享一个变量，来作为一个小小的延时 */
			{/*
				 * 困操作系统的反应不是很快，所以不能做一次判断，为了不浪费
				 * 资源又不能一会会就双来一次判断
				 * 所以特别设置此标志
				 */

				seek = buf;
				while ( seek && ( (strlen( seek ) ) > 0) )
				{
					seek = strstr( seek, "Host: scsi" );
					if ( seek && (seek[0] == 'H') )
					{
						seek	+= strlen( "Host: scsi" );
						seek	= strstr( seek, "Type:" );
						if ( seek && (seek[0] == 'T') )
						{
							seek += strlen( "Type:" );
							while ( *seek == ' ' )
							{
								seek++;
							}
							new_dev = malloc( sizeof(SCSI_USB_DEV) );
							if ( new_dev == NULL )
							{
								perror( "SCSI_USB_DEV malloc failed...\r\n" );
								return(-1);
							}

							INIT_DEV( new_dev, dev_num, seek );
							ADD_DEV( new_dev );
							dev_num++;
/*							DEBUG("dev_num = %d\n", dev_num); // 打印插入设备数量 */
						}
					}
				}
			}
		}
	} else {
		DEBUG( FILE_DEV_CHECK );
	}
	return(dev_num);
}


/*
 * 功  能：检测设备是不是挂载在指定的目标上；
 * 参  数：设备结构体指针dev
 * 返回值：成功		失败
 *		  KK_FS_OK  KK_FS_FAIL
 */
static int check_mount( SCSI_USB_DEV *dev )
{
	FILE	*fd		= NULL;
	int	len		= 0;
	int	i		= 0;
	char	buf[512]	= { 0 };
	char	*seek		= NULL;

	fd = fopen( FILE_MOUNT_CHECK, "r" );
	if ( fd > 0 )
	{
		len = fread( buf, 1, sizeof(buf), fd );
		fclose( fd );
		if ( len > 0 )
		{
			buf[len] = '\0';
			if ( dev->type == 2 && dev->part_num > 1 )
			{
				i++;                            /*if disk ignore first part disc*/
			}
			for (; i < dev->part_num; i++ )
			{
				seek = strstr( buf, dev->devfile_parts[i] );
				if ( seek != NULL )
				{
					return(KK_FS_OK);       /*have one part mounted return 1*/
				}
			}
		}
	} else {
		perror( FILE_MOUNT_CHECK );
	}
	return(KK_FS_FAIL);
}


/*
 * 功  能：对找到的设备（CDROM,USB）进行挂载；
 * 参  数：设备结构体指针dev
 * 返回值：成功		失败
 *		  KK_FS_OK  KK_FS_FAIL
 * 注：目前我们开板不支持CDROM设备，所以
 * 在测试时可以不用测试此段关于CDROM代码
 */
static int do_mount( SCSI_USB_DEV *dev )
{
	int		i		= 0;
	int		is_vcd		= 0;
	int		mount_ok	= 0;
	char		fstype[10]	= { 0 };
	char		tmpdir[50]	= { 0 };
	char		check_cmd[50]	= { 0 };
	char		mount_data[30]	= { 0 };
	char		mount_cmd[64]	= { 0 };
	unsigned long	mountflags	= 0;

	if ( dev->type == 1 && is_manual_umount == 0 )
	{
		strcpy( fstype, "iso9660" );
		mountflags = 0xc0ed0000 | 1;
		strcpy( mount_data, "codepage=936,iocharset=gb2312" );
		if ( mount( dev->devfile_parts[0], dev->mount_path[0], fstype, mountflags, mount_data ) == 0 )
		{
			mount_ok = 1;
			DEBUG( "mount -t %s %s %s success\n", fstype, dev->devfile_parts[0], dev->mount_path[0] );
			/* check is vcd */
			sprintf( check_cmd, "ls %s/vcd/*.vcd", dev->mount_path[0] );
			is_vcd = (system( check_cmd ) == 0);
			if ( is_vcd )
			{
				if ( umount( dev->mount_path[0] ) == 0 )
				{
					DEBUG( "umount %s success(vcd iso9660)\n", dev->devfile_parts[0] );
				} else {
					is_vcd = 0;
					DEBUG( "umount %s failed (vcd iso9660)\n", dev->devfile_parts[0] );
				}
			}
		} else {
			mount_ok = 0;
		}

		if ( mount_ok == 0 || is_vcd )
		{
			DEBUG( "mount -t %s %s %s failed, try cdfs\n", fstype, dev->devfile_parts[0], dev->mount_path[0] );
			strcpy( fstype, "cdfs" );
			if ( mount( dev->devfile_parts[0], dev->mount_path[0], fstype, mountflags, mount_data ) == 0 )
			{
				DEBUG( "mount -t %s %s %s success\n", fstype, dev->devfile_parts[0], dev->mount_path[0] );
				return(KK_FS_OK);
			} else {
				DEBUG( "mount -t %s %s %s failed, try cdfs\n", fstype, dev->devfile_parts[0], dev->mount_path[0] );
				return(KK_FS_FAIL);
			}
		}
	} else if ( dev->type == 2 )
	{
		sprintf( tmpdir, USB_DISK_MP "/disk%d", dev->index );
		mkdir( tmpdir, 0777 );
		strcpy( fstype, "vfat" );
		mountflags = 0xc0ed0000;
		strcpy( mount_data, "codepage=437,iocharset=iso8859-1" );

		if ( dev->part_num > 1 )
		{
			i++;                    /*if disk ignore first part disc*/
		}
		for (; i < dev->part_num; i++ ) /* mount -t vfat -o umask=000,noatime,async,rw /dev/sda1 tmp/ */
		{
			mkdir( dev->mount_path[i], 0777 );

			sprintf( mount_cmd, "mount -t %s -o umask=000,noatime,async,%s,rw %s %s", fstype, mount_data, dev->devfile_parts[i], dev->mount_path[i] );
			system( mount_cmd );
		}
	} else {
		return(KK_FS_FAIL);
	}
	return(KK_FS_OK);
}


/*
 * 功  能：对已经挂载的设备（CDROM,USB）进行卸载；
 * 参  数：设备结构体指针dev
 * 返回值：成功		失败
 *		  KK_FS_OK  KK_FS_FAIL
 */
int do_umount( SCSI_USB_DEV *dev )      /* ok */
{
	int	i		= 0;
	char	tmpdir[50]	= { 0 };

	if ( dev->type == 1 )           /* cdrom */
	{
		if ( umount2( dev->mount_path[0], MNT_FORCE ) == 0 )
		{
			DEBUG( "umount %s success\n", dev->devfile_parts[0] );
			return(KK_FS_OK);
		} else {
			DEBUG( "umount %s failed\n", dev->devfile_parts[0] );
			return(KK_FS_FAIL);
		}
	} else if ( dev->type == 2 )                                            /* usb-storage */

	{
		disc_flag = 0;
		if ( dev->part_num > 1 )
		{
			i++;                                                    /* if disk ignore first part disc */
		}

		for (; i < dev->part_num; i++ )
		{
			if ( umount2( dev->mount_path[i], MNT_FORCE ) == 0 )    /* 注意不是umount /dev/sda1 而是umount /tmp/disk/ 之类的用户能访问的路径 */
			{
				DEBUG( "umount %s success\n", dev->mount_path[i] );
				system( dev->mount_path[i] );
			} else {
				DEBUG( "umount %s failed\n", dev->mount_path[i] );
			}
		}
		sprintf( tmpdir, "rm -rf "USB_DISK_MP "/disk%d", dev->index );
		system( tmpdir );
	}

	return(KK_FS_OK);
}


/*
 * 功  能：检查设备是否挂载上,如果设备没有挂载
 *                则自行挂载设备;
 * 参  数：设备结构体指针dev
 * 返回值：成功		失败
 *		  KK_FS_OK    无
 */
static int process_dev( SCSI_USB_DEV* dev )
{
	if ( find_attached == TRUE )                    /* 检测设备是否插上 */
	{
		if ( check_mount( dev ) == KK_FS_FAIL ) /* 检测设备是否mount上 */
		{
			do_mount( dev );
		} else {
			if ( insert == FALSE )
			{
				insert = TRUE;
			}
		}
	} else { /* 如果是已经removed device 就直接将其umount掉 */
		if ( check_mount( dev ) == KK_FS_OK )
		{
			do_umount( dev );
		}
		insert = FALSE;
	}
	return(KK_FS_OK);
}


/*
 * 功  能：查找设备，对找到后有设备做相应的处理
 * 参  数：无
 * 返回值：无
 * 注    ：查询间隔时间为2s
 */
void thread_for_usb()
{
	DEBUG( "USB enter...\r\n" );
	SCSI_USB_DEV *cur_dev = NULL;
	mkdir( USB_DISK_MP, 0777 );
	mkdir( USB_CDROM_MP, 0777 );

	back_first_dev = malloc( sizeof(SCSI_USB_DEV) );

	stata_path = malloc( sizeof(STATA_PATH) );

	if ( (back_first_dev == NULL) && (stata_path == NULL) )
	{
		perror( "thread_for_usb malloc failed ...\r\n" );
		exit( 1 );
	}

	memset( back_first_dev, 0, sizeof(SCSI_USB_DEV) );
	memset( stata_path, 0, sizeof(STATA_PATH) );


	while ( 1 )
	{
		find_device();                  /* 查找设备 初始化设备列表 */
		cur_dev = f_first_dev;
		while ( cur_dev )
		{
			process_dev( cur_dev ); /* 对每个设备进行处理 */
			cur_dev = cur_dev->next_dev;
		}
		usleep( 500000 );
	}
}


