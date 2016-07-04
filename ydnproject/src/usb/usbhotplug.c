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


/* ����Ҫ�ͷ�����鹲��һ������������Ϊһ��СС����ʱ
 * ������ϵͳ�ķ�Ӧ���Ǻܿ죬���Բ�����һ���жϣ�Ϊ�˲��˷�
 * ��Դ�ֲ���һ����˫��һ���ж�
 * �����ر����ô˱�־ disc_flag,������ж��ʱ����
 */
int disc_flag = 0;


STATA_PATH *get_stata_path()
{
	return(stata_path);
}


#if 0 /* ��ʱ���� */
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
 * ��  �ܣ�������豸��������
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ��KK_FS_OK
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
 * ��  �ܣ��豸��ʼ����
 * ��  �����豸�ṹ��ָ��dev ��index,type
 * ����ֵ��KK_FS_OK
 */
static int INIT_DEV( SCSI_USB_DEV *dev, int index, char *type )
{
	dev->index = index;

	if ( !strncmp( type, "CD-ROM", 6 ) ) /* cd-rom //�����ʱû�ã������Ժ���չʹ�� */
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
 * ��  �ܣ������/proc/scsi/scsi/�з��ֵ��豸��usb-storage�����һ����/proc/partitions/�з�������
 *                ���������������������¼����dev->devfile_parts��mount·��Ҳһ������dev->mount_path
 *                ���緢��һ��U�̣�������һ��������ϵͳ���������/dev/sda1/�£�����Խ���mount·�����ó�
 *                �û����Է��ʵ�/tmp/usbdisk/disk0/part0/��
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ��part_num(��������)
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

	fd = fopen( FILE_DISC_PARTS_CHECK, "r" );       /* ֱ�Ӳ���open��ϵͳ�ļ� */
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
							 * ��¼�豸��Ϣ
							 */
							if ( find_new == TRUE )
							{
								disc_flag	= 1;
								find_new	= FALSE;
								char tmp_buf[50];

								memset( dev->hoststr, 0, 200 );

								/* �洢·�� */
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
 * ��  �ܣ��߳�ɨ��ʱ�᲻ͣ�ĵ��øýӿ�ȥ���֮ǰ��ȡ���豸��Ϣ
 * ��  ������
 * ����ֵ����
 */
static void CLEAR_DEV( void )
{
	SCSI_USB_DEV	*cur_dev = f_first_dev;
	SCSI_USB_DEV	*tmp_dev;
	while ( cur_dev )
	{
		/* ���ʱ�ı�����Ϣ */
		memcpy( back_first_dev, cur_dev, sizeof(SCSI_USB_DEV) );
		tmp_dev = cur_dev;
		cur_dev = cur_dev->next_dev;
		free( tmp_dev );
		usleep( 0 );
	}
	f_first_dev = NULL;
}


/*
 * ��  �ܣ������豸�������ͣ�
 * ��  ������
 * ����ֵ����
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
		 * ���û���κ��豸���룬��ô/proc/scsi/scsi�н������������ַ� "Attached devices:"
		 * �ļ�����Ϊ18������������20���ַ������жϡ�
		 * 
		 */
		len = fread( buf, 1, sizeof(buf), fd );
		fclose( fd );
		if ( len < 20 && find_attached == TRUE )
		{
			DEBUG( "Device removed......%d \n", find_attached );


			if ( find_attached == TRUE ) /*  // ��ֹ��ֹ��ж���豸 */
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
			 * ����ļ��򿪳ɹ������ҳ��ȴ���20����ʾ���������豸��¼�ģ����find_attached��־֮ǰ��FALSE
			 * ��ʾ���µĶ���:�豸����ϵͳ
			 * 
			 */
			if ( len > 20 && find_attached == FALSE )
			{
				find_attached	= TRUE;
				find_new	= TRUE;
				DEBUG( "Device Inserted......\n" );
			}

			if ( disc_flag == 0 ) /* ����Ҫ�ͷ�����鹲��һ������������Ϊһ��СС����ʱ */
			{/*
				 * ������ϵͳ�ķ�Ӧ���Ǻܿ죬���Բ�����һ���жϣ�Ϊ�˲��˷�
				 * ��Դ�ֲ���һ����˫��һ���ж�
				 * �����ر����ô˱�־
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
/*							DEBUG("dev_num = %d\n", dev_num); // ��ӡ�����豸���� */
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
 * ��  �ܣ�����豸�ǲ��ǹ�����ָ����Ŀ���ϣ�
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ���ɹ�		ʧ��
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
 * ��  �ܣ����ҵ����豸��CDROM,USB�����й��أ�
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ���ɹ�		ʧ��
 *		  KK_FS_OK  KK_FS_FAIL
 * ע��Ŀǰ���ǿ��岻֧��CDROM�豸������
 * �ڲ���ʱ���Բ��ò��Դ˶ι���CDROM����
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
 * ��  �ܣ����Ѿ����ص��豸��CDROM,USB������ж�أ�
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ���ɹ�		ʧ��
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
			if ( umount2( dev->mount_path[i], MNT_FORCE ) == 0 )    /* ע�ⲻ��umount /dev/sda1 ����umount /tmp/disk/ ֮����û��ܷ��ʵ�·�� */
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
 * ��  �ܣ�����豸�Ƿ������,����豸û�й���
 *                �����й����豸;
 * ��  �����豸�ṹ��ָ��dev
 * ����ֵ���ɹ�		ʧ��
 *		  KK_FS_OK    ��
 */
static int process_dev( SCSI_USB_DEV* dev )
{
	if ( find_attached == TRUE )                    /* ����豸�Ƿ���� */
	{
		if ( check_mount( dev ) == KK_FS_FAIL ) /* ����豸�Ƿ�mount�� */
		{
			do_mount( dev );
		} else {
			if ( insert == FALSE )
			{
				insert = TRUE;
			}
		}
	} else { /* ������Ѿ�removed device ��ֱ�ӽ���umount�� */
		if ( check_mount( dev ) == KK_FS_OK )
		{
			do_umount( dev );
		}
		insert = FALSE;
	}
	return(KK_FS_OK);
}


/*
 * ��  �ܣ������豸�����ҵ������豸����Ӧ�Ĵ���
 * ��  ������
 * ����ֵ����
 * ע    ����ѯ���ʱ��Ϊ2s
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
		find_device();                  /* �����豸 ��ʼ���豸�б� */
		cur_dev = f_first_dev;
		while ( cur_dev )
		{
			process_dev( cur_dev ); /* ��ÿ���豸���д��� */
			cur_dev = cur_dev->next_dev;
		}
		usleep( 500000 );
	}
}


