#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <devcapacitylook.h>
#include <sys/vfs.h>
#include <defineident.h>
#include <debug.h>




int get_storage_dev_info(char * mount_point,int option)
{

	struct statfs stat_fs;
	uint64_t used_bytes	= 0;
	uint64_t free_bytes	= 0;
	uint64_t tota_bytes  = 0;
	int end_space	= 0;

	if(!mount_point)
		return -2;
	
	if (ERR == statfs(mount_point, &stat_fs)) {
		perror("get_storage_dev_info()\n");
		return -1;
	}

	tota_bytes  = (uint64_t)stat_fs.f_blocks * (uint64_t)stat_fs.f_frsize;
	free_bytes  = (uint64_t)stat_fs.f_bfree * (uint64_t)stat_fs.f_frsize;
	used_bytes  = (uint64_t)(tota_bytes - free_bytes);

	switch(option) {
	case TOTA_CAPACITY:
		end_space = tota_bytes/1024/1024;
		break;

	case USED_CAPACITY:
		end_space = used_bytes/1024/1024;
		break;

	case FREE_CAPACITY:
		end_space = free_bytes/1024/1024;
		
		break;

	default:
		return ERR;
	}

	return end_space;

}
