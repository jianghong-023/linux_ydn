/*
 * �ļ����ƣ� usbhotplug.h
 * �ļ���ʶ��
 * ժ    Ҫ�� ͷ�ļ����弰�ṹ�嶨��
 * ��ǰ�汾�� xxxxx
 * ��    �ߣ� jianghong_023<jianghong_023@163.com>
 * ������ڣ� 2015-08-21
 *
 * ȡ���汾�� ��
 * ԭ  ���ߣ� jianghong_023<jianghong_023@163.com>
 * ������ڣ� 2015-08-21
 *
 */


#ifndef _ubsdev_
#define _ubsdev_
#include <stdbool.h>
#include <sys/mount.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <stddef.h>
#include <poll.h>
#include <signal.h>
#include <syslog.h>

#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <sys/wait.h>
#include <pthread.h>


#include "defineident.h"

typedef struct s_scsi_usb_dev {
	int			type;                                           /*1 cdrom 2 disk */
	int			index;                                          /*like host0 host1*/
	char			file_statu[MAX_NAME_LEN];                       /*like "/proc/scsi/usb-storage-%d/%d"*/
	char			devfile_parts[MAX_PART_NUM][MAX_NAME_LEN];      /*�洢ÿ���������豸�ļ�*/
	char			mount_path[MAX_PART_NUM][MAX_NAME_LEN];         /*�������Ӧ��mount��*/
	int			part_num;                                       /* 4������ */
	struct s_scsi_usb_dev	*next_dev;                                      /* 4ָ����һ���豸 */
	char			hoststr[200];                                   /* ���ݵ�ǰ��Ϣ */
} SCSI_USB_DEV;


typedef struct dev_stata_path STATA_PATH;
struct dev_stata_path {
	char		hostusbpath[100];                                       /* �豸����·���洢 */
	char		diskpath[50];                                           /* usb ϵͳ·�� */
	int		is_active;                                              /* �Ƿ��Ƴ��豸 */
	pthread_mutex_t usblock;
	SCSI_USB_DEV	doumunt;
};

typedef struct dev_usb_path USB_PATH_T;
struct dev_usb_path {
	char		hostusbpath[100];                                       /* �豸����·���洢 */
	char		diskpath[50];                                           /* usb ϵͳ·�� */
	int		is_active;                                              /* �Ƿ��Ƴ��豸 */
	pthread_mutex_t usblock;

};

int do_umount( SCSI_USB_DEV *dev );


STATA_PATH *get_stata_path();


void thread_for_usb( void );


#endif