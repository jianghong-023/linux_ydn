
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
 * 取得 usb 设备信息
 *
 *
 *************/
static int getSusbinfo(char * usbmountpoint)
{
	struct statfs stat_fs;
	float freebytes = 0;
	float totalbytes = 0;
	char t_buf[16];
	char f_buf[16];



	if (statfs(usbmountpoint, &stat_fs) == -1) {  //获取分区的状态
		DEBUG("statfs failed for path->[%s]\n", usbmountpoint);
		return(-1);
	}

	//详细的分区总容量
	totalbytes = (uint64_t)stat_fs.f_blocks * (uint64_t)stat_fs.f_frsize/1024/1024;
	//详细的剩余空间容量
	freebytes = (uint64_t)stat_fs.f_bfree * (uint64_t)stat_fs.f_frsize/1024/1024;

	sprintf(t_buf,"Total:%.2fMB",totalbytes);
	sprintf(f_buf,"Free :%.2fMB",freebytes);
	lcd_clear(discontrl_t()->lcdfd);   // 并清除内存
	lcd_Write_String(0,t_buf);
	lcd_Write_String(1,f_buf);

	nano_sleep(3,0);

	return 0;
}

uint64_t get_usb_free_size(char * usbmountpoint)
{
	struct statfs stat_fs;
	uint64_t freebytes = 0;

	if (statfs(usbmountpoint, &stat_fs) == -1) {  //获取分区的状态
		DEBUG("statfs failed for path->[%s]\n", usbmountpoint);
		return(-1);
	}

	
	//详细的剩余空间容量
	freebytes = (uint64_t)stat_fs.f_bfree * (uint64_t)stat_fs.f_frsize;

	return freebytes;
}


/**
 *
 *
 * 等待usb的链接
 *
 *
 ********************/
static int loop_usb_sat()
{
	float timeuse,sdtime,rstime;
	struct timeval tpstart;


	gettimeofday(&tpstart,NULL);

	timeuse = 1000000*(tpstart.tv_sec)+tpstart.tv_usec;

	while(1) {
		usleep(100);

		if(get_stata_path()->is_active != DEVROMV) {

			if(get_stata_path()->hostusbpath !=NULL) {
				return 0;
				break;
			}
		} else {

			lcd_clear(discontrl_t()->lcdfd);   // 并清除内存
			lcd_Write_String(0,"Please insert US");
			lcd_Write_String(1,"B device        ");
			//DEBUG("Please wait for you to find the USB path for the file\n");
			nano_sleep(1,0);
		}
		gettimeofday(&tpstart,NULL);

		sdtime = 1000000*(tpstart.tv_sec)+tpstart.tv_usec;
		rstime = sdtime - timeuse;

		if(rstime > TMIER_OUT) {
			lcd_clear(discontrl_t()->lcdfd);   // 并清除内存
			lcd_Write_String(0,"USB timeout...  ");
			lcd_Write_String(1,"                ");
			nano_sleep(1,0);

			return -1;
			break;
		}

	}
}

/**
 *
 * 检查usb链接情况，并返回信息
 *
 *
 *
 ***********/
uint8_t get_usb_usrinfo()
{
	static int falg = 0;
	char FileName[50]= {0};


	if(falg == 0) {
		falg = 1;
		if(loop_usb_sat() < 0) {
			DEBUG("find device fail");

			return -1;
		}

	} else {

		return -1;
	}

	sprintf(FileName,"%s",get_stata_path()->hostusbpath);
	getSusbinfo(FileName);
	falg = 0;

	return 0;
}


/**
 *
 * 卸载usb
 *
 *
 *******************/
uint8_t manu_do_mount_dev()
{
	// 卸载usb设备
	static int falg = 0;
	char FileName[50]= {0};

	if(falg == 0) {
		falg = 1;
		if(get_stata_path()->is_active == DEVROMV) {

			if(get_stata_path()->hostusbpath !=NULL) {

				lcd_clear(discontrl_t()->lcdfd);   // 并清除内存
				lcd_Write_String(0,"Not device remov");
				lcd_Write_String(1,"e...           ");
				nano_sleep(1,0);
				return -1;

			}

		} else {
			sprintf(FileName,"%s",get_stata_path()->hostusbpath);

			if (umount2(FileName,MNT_FORCE) == 0) {
				char tmpdir[50] = {0};

				sprintf(tmpdir, "rm -rf %s", FileName);
				system(tmpdir);
				DEBUG("umount %s success\n", FileName);
				usleep(500000);
				lcd_clear(discontrl_t()->lcdfd);   // 并清除内存
				lcd_Write_String(0,"Successful remov");
				lcd_Write_String(1,"al of USB       ");
				nano_sleep(2,0);

			} else {
				DEBUG("umount %s failed\n", FileName);

			}

			memset(FileName,0,50);
			sprintf(FileName,"%s",get_stata_path()->diskpath);

			if (umount2(FileName,MNT_FORCE) == 0) {

				DEBUG("umount %s success\n", FileName);

			}

		}
	}

	falg = 0;

	return 0;
}