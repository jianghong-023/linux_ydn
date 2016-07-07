/*
 * 文件名称： usbhotplug.h
 * 文件标识：
 * 摘    要： 头文件定义及结构体定义
 * 当前版本： xxxxx
 * 作    者： jianghong_023<jianghong_023@163.com>
 * 完成日期： 2015-08-21
 *
 * 取代版本： 无
 * 原  作者： jianghong_023<jianghong_023@163.com>
 * 完成日期： 2015-08-21
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
	char			devfile_parts[MAX_PART_NUM][MAX_NAME_LEN];      /*存储每个分区的设备文件*/
	char			mount_path[MAX_PART_NUM][MAX_NAME_LEN];         /*与上面对应的mount点*/
	int			part_num;                                       /* 4分区数 */
	struct s_scsi_usb_dev	*next_dev;                                      /* 4指向下一个设备 */
	char			hoststr[200];                                   /* 备份当前信息 */
} SCSI_USB_DEV;


typedef struct dev_stata_path STATA_PATH;
struct dev_stata_path {
	char		hostusbpath[100];                                       /* 设备挂在路径存储 */
	char		diskpath[50];                                           /* usb 系统路径 */
	int		is_active;                                              /* 是否移除设备 */
	pthread_mutex_t usblock;
	SCSI_USB_DEV	doumunt;
};

typedef struct dev_usb_path USB_PATH_T;
struct dev_usb_path {
	char		hostusbpath[100];                                       /* 设备挂在路径存储 */
	char		diskpath[50];                                           /* usb 系统路径 */
	int		is_active;                                              /* 是否移除设备 */
	pthread_mutex_t usblock;

};

int do_umount( SCSI_USB_DEV *dev );


STATA_PATH *get_stata_path();


void thread_for_usb( void );


#endif