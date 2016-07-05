#include <freememscript.h>
#include <debug.h>
#include <defineident.h>

void flush_conf( void )
{
	FILE *file;

	if ( access( RULER_STRING, F_OK ) != 0 )
	{
		file = fopen( RULER_STRING, "w+" );
		if ( NULL == file )
		{
			printf( "File created failed ...\n" );
			return;
		}

		fprintf( file, "#!/bin/sh\n\n" );
		fprintf( file, "cl=\"$1\"\n" );
		fprintf( file, "#		echo \"cl=\"$cl\n\n");


		/*
		 * fprintf(file,"used=`free -m | awk 'NR==2' | awk '{printf \"\%%d\\n\",$3}'`\n");
		 * fprintf(file,"free -m | awk 'NR==2' | awk '{printf \"used mem \%%dKB\\n\",$3}'\n");
		 * fprintf(file,"free=`free -m | awk 'NR==2' | awk '{printf \"\%%d\\n\",$4}'`\n");
		 * fprintf(file,"free -m | awk 'NR==2' | awk '{printf \"free mem \%%dKB\\n\",$4}'\n\n");
		 */
		fprintf( file, "used=`free -m | awk 'NR==2' | awk '{printf \"\"}'`\n" );
		fprintf( file, "free -m | awk 'NR==2' | awk '{printf \"\"}'\n" );
		fprintf( file, "free=`free -m | awk 'NR==2' | awk '{printf \"\"}'`\n" );
		fprintf( file, "free -m | awk 'NR==2' | awk '{printf \"\"}'\n\n" );
		fprintf( file, "		echo '------------------' >> /var/log/mem.log\n");
		fprintf( file, "date >> /var/log/mem.log\n" );
		fprintf( file, "echo 'Memory usage | [Use: ${used}MB] [Free: ${free}MB]' >> /var/log/mem.log\n\n" );
		fprintf( file, "if [ \"$free\"2 -le 800002 ] ; then\n" );
		fprintf( file, "			sync && echo 1 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			sync && echo 2 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			sync && echo 3 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "#			sync && echo 0 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			echo 'OK' >> /var/log/mem.log\n");
		fprintf( file, "else\n" );
		fprintf( file, "			echo 'Not required' >> /var/log/mem.log\n");
		fprintf( file, "fi\n\n" );
		fprintf( file, "#			echo 'argv c1='$cl\n");
		fprintf( file, "if [ \"$cl\"2 -eq 22 ] ; then\n" );
		fprintf( file, "			sync && echo 1 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			sync && echo 2 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			sync && echo 3 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "#			sync && echo 0 > /proc/sys/vm/drop_caches\n");
		fprintf( file, "			echo 'clear cache  ok' >> /var/log/mem.log\n");
		fprintf( file, "else\n" );
		fprintf( file, "			echo 'not clear operation' >> /var/log/mem.log\n");
		fprintf( file, "fi\n\n" );
		usleep( 5000 );
		chmod( RULER_STRING, 0715 );
		fclose( file );
	} /*
	   * else {
	   * printf("file flush exists...\n");
	   */

	/* } */
}


void system_conf( void )
{
	FILE* file;
	if ( access( SYS_CONF, F_OK ) != 0 )
	{
		file = fopen( SYS_CONF, "w+" );
		if ( NULL == file )
		{
			printf( "File created failed ...\n" );
			return;
		}
		fprintf( file, "#!/bin/sh\n\n" );
		fprintf( file, "#system config script\n\n" );
		fprintf( file, "#jianghong_023@163.com\n\n" );
		fprintf( file, "#2015-09-16\n\n" );
		fprintf( file, "if [ $# -lt 3 ];then\n" );
		fprintf( file, "		echo \"no arguments\"\n\n");
		fprintf( file, "else\n\n" );
		fprintf( file, "#		for args in $@;do\n");
		fprintf( file, "#			echo $args\n");
		fprintf( file, "#		done\n\n");
		fprintf( file, "		orgvalue=\"$1\"\n");
		fprintf( file, "		newvalue=\"$2\"\n");
		fprintf( file, "		line=\"$3\"\n");
		fprintf( file, "#		echo $orgvalue\n\n");
		fprintf( file, "#		echo $newvalue\n\n");
		fprintf( file, "#		echo $line\n\n");
		fprintf( file, "	sed -i \"$line s/$orgvalue/$newvalue/g\" /etc/dvb.conf\n");
		fprintf( file, "fi\n" );
		usleep( 5000 );
		chmod( SYS_CONF, 0715 );
		fclose( file );
	} /*
	   * else {
	   * printf("file sysconf exists...\n");
	   */

	/* } */
}


void system_reset_conf( void )
{
	FILE* file;

	file = fopen( RES_ETC_CONF, "w+" );
	if ( NULL == file )
	{
		printf( "File created failed ...\n" );
		return;
	}
	fprintf( file, "#!/bin/sh\n\n" );
	fprintf( file, "cd /etc \n\n" );
	fprintf( file, "rm -rf dvb.conf \n" );
	fprintf( file, "cd - \n" );
	fprintf( file, "/sbin/reboot\n" );
	usleep( 5000 );
	chmod( RES_ETC_CONF, 0715 );
	fclose( file );
}


void sys_ect_conf( void )
{
	FILE* file;
	chmod( "/etc", 0777 );
	if ( access( SYS_ETC_CONF, F_OK ) != 0 )
	{
		file = fopen( SYS_ETC_CONF, "w+" );
		if ( NULL == file )
		{
			printf( "File created failed ...\n" );
			return;
		}

		fprintf( file, "#!/bin/sh\n\n" );
		fprintf( file, "# dvd project configuration file 2015/9/15\n\n" );
		fprintf( file, "# CreateFileTime: 2015-09-15-105725\n" );
		fprintf( file, "# LsatTime: 2015-09-15-105725\n\n" );
		fprintf( file, "# The following configuration parameters\n\n" );
		fprintf( file, "# ExternalInterface eth0\n\n" );

		fprintf( file, "#			echo $args\n");
		fprintf( file, "#		done\n\n");
		fprintf( file, "# Parameter: GatewayAddress\n" );
		fprintf( file, "# Default: Find it from GatewayInterface\n" );
		fprintf( file, "# Optional\n" );
		fprintf( file, "#		\n");
		fprintf( file, "# Set this to the internal IP address of the gateway.  Not normally required.\n\n" );
		fprintf( file, "# GatewayAddress 192.168.9.1\n\n" );
		fprintf( file, "#-------------------------\n" );
		fprintf( file, "# 1.cy22393: config\n" );
		fprintf( file, "#-------------------------\n\n" );
		fprintf( file, "# Adv7842Interface default\n\n" );
		fprintf( file, "videomod HDMI_M\n\n" );
		fprintf( file, "# 2.gpio config\n\n" );
		fprintf( file, "# 3.adv7842 config\n\n" );
		fprintf( file, "Brightness 128\n\n" );
		fprintf( file, "Contrast 128\n\n" );
		fprintf( file, "Saturation 128\n\n" );
		fprintf( file, "Videohue 127\n\n" );

		fprintf( file, "CpBrightness 128\n\n" );
		fprintf( file, "CpContrast 128\n\n" );
		fprintf( file, "CpSaturation 128\n\n" );
		fprintf( file, "CpVideohue 127\n\n" );

		fprintf( file, "VideoNorm AUTO\n\n" );
		fprintf( file, "# 4.h.246 config\n\n" );
		fprintf( file, "ProfileConfig  HIGH_CFG\n\n" );
		fprintf( file, "LevelConfig  Level4\n\n" );
		fprintf( file, "LcdTime  t10s\n\n" );
		fprintf( file, "RateMode CBR\n\n" );
		fprintf( file, "VideoBitrate 8000 \n\n" );
		fprintf( file, "AudioBitrate 64_kbps \n\n" );
		fprintf( file, "# 5.adf4350 config\n\n" );
		fprintf( file, "# 6.ad9789 config\n\n" );
		fprintf( file, "AD9789CFG  500.00\n\n" );
		fprintf( file, "EncodeRate ENCODE_RATE_8_7\n\n" );
		fprintf( file, "ModulelateMode MODULATE_M_64QAM\n\n" );
		fprintf( file, "FftMode FFT_2K\n\n" );
		fprintf( file, "RFLevel -16.0\r\n" );
		fprintf( file, "Deltal F_DELTAL_32_1\n\n" );
		fprintf( file, "ChannelWidth 8MHz\n\n" );
		fprintf( file, "AudioFormat MPEG-2\n\n" );

		fprintf( file, "# PID configuration\n\n" );
		fprintf( file, "PMTPID 100\n\n" );
		fprintf( file, "PRCPID 103\n\n" );
		fprintf( file, "VideoPID 101\n\n" );
		fprintf( file, "AudioPID 102\n\n" );
		fprintf( file, "ProgrNmu 101\n\n" ); /* NETWORKID */
		fprintf( file, "NETWORKID 1\n\n" );

		fprintf( file, "StreamTSID 1\n\n" );
		fprintf( file, "StreamONID 1\n\n" );

		fprintf( file, "NetWorkName network-01\n\n" );
		fprintf( file, "NETWORKNUMB 31\n\n" );
		fprintf( file, "EITISERTENABLE Enable\n\n" );
		fprintf( file, "RFENABLE Denable\n\n" );
		fprintf( file, "LockPwdStu Denable\n\n" );
		fprintf( file, "ProgramOutput Denable\n\n" );
		fprintf( file, "NITLCNMOD European_Cfg\n\n" );
		fprintf( file, "ProgramName TV-101\n\n" ); /*  */
		fprintf( file, "ServiceName TV-Provider\n\n" );
		fprintf( file, "# USB configuration\n\n" );
		fprintf( file, "PlayModle SINGALMENT\n\n" );
		fprintf( file, "WriteRecord 0\n\n" );
		fprintf( file, "RecordModle RECORDSIG\n\n" );
		fprintf( file, "tsfilename default\n\n" );
		fprintf( file, "tsfilesize 36\n\n" );

		usleep( 5000 );
		chmod( SYS_ETC_CONF, 0777 );
		fclose( file );
	}


	/*
	 * else {
	 * printf("file dvbconf exists...\n");
	 */

	/* } */
	chmod( "/etc", 0715 );
}


void mem_look_clear( void )
{
	struct sysinfo info;
	sysinfo( &info );
	/* DEBUG(" Not used memory is %ldK \n",info.freeram/1024); */
	if ( info.freeram / 1024 <= LIMITMEM )
		system( RULER_STRING );
}


void loop_cl_cah()
{
	int i = 0;
	for (; i < 1; i++ )
	{
		usleep( 6000 );
		mem_look_clear();
	}
}


