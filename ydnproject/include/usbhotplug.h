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

typedef struct usb_no_stat_t {
	int	is_active;
	int	flag_off;
	int32_t part_num;
	char	devfile_parts[MAX_PART_NUM][MAX_NAME_LEN];
	char	mount_path[MAX_PART_NUM][MAX_NAME_LEN];
}usb_no_and_stat_t;

int dev_umount( void *dev );


int eject_usb( void * );


usb_no_and_stat_t *get_stata_path();


int get_usb_status( void );


void thread_for_usb( void );


#endif