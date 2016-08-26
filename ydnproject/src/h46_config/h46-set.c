#include <h46-comctrl.h>
#include <h46-set.h>
#include <debug.h>
#include <math_instance.h>
#include <init-configuration.h>

static void audio_birate( int audio_bitrate, int fd );


enum h46_opt {
	v72060p = 30,
	v57650i,
	v48050i,
	v108060i,
	v108050i,
	v108050p,
	v108030p,
	v57650p
};


static int vn_calc( double v_bitrate, double framerate )
{
	int res;

	if ( v_bitrate == 0 || framerate == 0 )
		return(res = -1);

	int	v0	= 19 * 8 * (framerate);
	int	v1	= v_bitrate * 1000 + v0;
	int	v2	= 183 * 8 * (framerate);
	int	v3	= (v1 + v2) * (188 / 184);
	res = ( (int) (v3 + 0.5) );

	DEBUG( "v0=%d \n", v0 );
	DEBUG( "v1=%d \n", v1 );
	DEBUG( "v2=%d \n", v2 );
	DEBUG( "v3=%d \n", v3 );
	return(res);
}


static int sn_calc( double sysofms )
{
	double	tmp;
	int	res;

	if ( sysofms == 0 )
		return(res = -1);

	tmp = (188 * 8) / (sysofms / 1000);

	res = ( (int) (tmp + 0.5) );
	DEBUG( "S =%d \n", res );
	return(res);
}


/*
 * static int an_calc(double a_bitrate,double fre)
 * {
 * double tmp;
 * int res;
 * DEBUG("a_bitrate=%f \n",a_bitrate);
 * if(a_bitrate == 0 || fre == 0 )
 * return res = -1;
 *
 * tmp = a_bitrate * 1000 * 1152 /fre / 8;
 * int A0 = ((int)(tmp + 0.5));
 * int A1 =A0 + 14;
 * tmp = A1 / 184;
 * int A2 = ((int)(tmp + 0.5));
 *
 * tmp  = 188 * 8 * A2 /1152 / fre;
 *
 * int A3 = ((int)(tmp + 0.5));
 * res = A3;
 * DEBUG("A0=%d \n",A0);
 * DEBUG("A1=%d \n",A1);
 * DEBUG("A2=%d \n",A2);
 * DEBUG("A3=%d \n",A3);
 *
 *
 *
 * return res;
 * }
 */

static int  MUX_RATE( int v3, int a3, int s0, int s1, int s2, int s3 )
{
	double	tmp;
	int	res;

	tmp = (v3 + a3 * 1000 + s0 + s1 + s2 + s3) * 1.05 / 1000;

	res = ( (int) (tmp + 0.5) );

	DEBUG( "mux_rate = %d   %x \n", res, res );
	return(res);
}


static void __v72060p__( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "ENC1";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA90000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


static void __v72050p__( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "ENC1";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA90000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


static void audio_birate( int audio_bitrate, int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };


	/*
	 * 文件配置
	 * audio A_BITRATE
	 */
	buf = "WA01804";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	sprintf( __buf, "WD%02x%02x", GET_HEX( audio_bitrate, 2 ), GET_HEX( audio_bitrate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "audio_bitrate =%04x   l=%02x  h=%02x  %s\n", audio_bitrate, GET_HEX( audio_bitrate, 1 ), GET_HEX( audio_bitrate, 2 ), __buf );
	usleep( 2000 );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


static void audio_init( int audio_bitrate, int fd )
{
	/*
	 * char *buf=NULL;
	 * struct timeval tpstart;
	 */


	/*
	 * A_SAMPLE
	 * 0x1802, bit [1:0]
	 * Specify the audio sampling frequency.
	 * 0: 48 kHz
	 * 1: 44.1 kHz
	 * 2: 32 kHz
	 * 3: Reserved
	 * AV_RESYNC_THRESHOLD
	 * 0x1802, bit [13:2]
	 * buf = "0x01802";
	 * uart_send(fd,buf);
	 * usleep(2000);
	 * gettimeofday(&tpstart,NULL);
	 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)) {
	 * ;
	 * }
	 * buf = "WD0000";   // A_SAMPLE = 0,bit [1:0] = 0,48khz ; AV_RESYNC_THRESHOLD = 0,bit [13:2];Default value: 10 ms ("0x1C2")
	 * uart_send(fd,buf);
	 * usleep(2000);
	 * gettimeofday(&tpstart,NULL);
	 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)) {
	 * ;
	 * }
	 */

	audio_birate( audio_bitrate, fd );
}


static void other_pi( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA90000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


void h64_common_set( int fd )
{
	DEBUG( "h64 init set.." );
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "CHK";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "RST";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "DP";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


void v72060p_mod( int fd )
{
	char		*buf		= NULL;
	char		__buf[14]	= { 0 };
	struct timeval	tpstart;
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	DEBUG( "v72060p_mod.." );
	h64_common_set( fd );
	__v72060p__( fd );

	/* v72060p */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 */


/*
 * V_BITRATE  = 6000
 * FrameRate  = 30000/1001  60p
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	/* buf = "WD0064";	//V_PID=0x0064 */
	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	/* buf = "WD0065";	//A_PID=0x0065 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR scfg_Param.encoder_video_ratemode_fre
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );

	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );
	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps/
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int v_bitrate = dconfig->scfg_Param.encoder_video_bitrate;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_bitrate, 2 ), GET_HEX( v_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1502"; /* V_FORMAT:1280*720p(60Hz) */
/*	buf = "WD1503";	//V_FORMAT:1280*720p(50Hz) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0001";
	 * uart_send(fd,buf);
	 * usleep(2000);
	 */
	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_GOP_STRUCT=IBBP V_STRM_FORMAT=0 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	audio_init( audio_bitrate, fd );

	buf = "ENC1";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


void v72050p_mod( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	h64_common_set( fd );
	__v72050p__( fd );

	/* v72050p */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 */


/*
 * V_BITRATE  = 6000
 * FrameRate  = 50  50p
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0065";	//A_PID=0x0065
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0030"; /* V_RATE_MODE */
/*	buf = "WD0031";	//V_RATE_MODE */
	uart_send( fd, buf );
	usleep( 2000 );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1502";	//V_FORMAT:1280*720p(60Hz) */
	buf = "WD1503"; /* V_FORMAT:1280*720p(50Hz) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
	 * uart_send(fd,buf);
	 * usleep(2000);
	 */
	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_GOP_STRUCT=IBBP V_STRM_FORMAT=0 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );
	DEBUG( "ES_INFO_00 =%s ", __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
#if 1
	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	//buf = "WD0000"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
#endif

/*
 * buf = "WA80116";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD0132";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	audio_init( audio_bitrate, fd );

	buf = "ENC1";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	ret_opt( fd );
}


void v57650i_mod( int fd )
{
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	h64_common_set( fd );
	other_pi( fd );
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };
	/* v57650i */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 */


/*
 * V_BITRATE  = 6000
 * FrameRate  = 25  50i
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */

	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0065";	//A_PID=0x0065
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	/* buf = "WD0030";	//V_RATE_MODE=CBR */


/*
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );
	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1005"; /* V_FORMAT:720*576(50i) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_SWAP */
/*	buf = "WD0001";  //V_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_GOP_STRUCT=IBBP V_STRM_FORMAT=0 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0026"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD007D"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0056"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0007"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA80116";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD0132";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	audio_init( audio_bitrate, fd );

	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	ret_opt( fd );
}


void v48050i_mod( int fd )
{
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	DEBUG( "v48050i_mod.." );
	h64_common_set( fd );
	other_pi( fd );
	/* v48050i */
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };

	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD2710";	//SYSTEM_RATE=10Mbps
 * buf = "WD7530";	//SYSTEM_RATE=30Mbps
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 25  50i
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0030"; /* V_RATE_MODE */
	uart_send( fd, buf );
	usleep( 2000 );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_AVE_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1004"; /* V_FORMAT:720*480(50i) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *      buf = "WA0140E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0010";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_MAX_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* VIN_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA01540";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0002";  //VIN_SWAP
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0026"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD007D"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0056"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0007"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA01542";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01544";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01546";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01548";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154A";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154C";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 *  buf = "WA01550";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01552";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01554";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015A0";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015A2";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015A4";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015A6";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015AA";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015AC";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 *  buf = "WA015B2";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA015B4";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *  buf = "WA800EC";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD000D";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA80114";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0302";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA80118";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD444D";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011A";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD56FF";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011C";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3F28";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	audio_init( audio_bitrate, fd );
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


/* 20160128 jh ,蓝屏 */
void v48060i_mod( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	h64_common_set( fd );
	char __buf[14] = { 0 };
	other_pi( fd );
	/* v48050i */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 30  60i
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE=188 bytes */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15:0]:INITIAL_STC_H:base[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15:0]:INITIAL_STC_L:base[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:INITIAL_STC_LL:base[0] BIT[8:0]:INITIAL_STC_ext:ext[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */

	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0065";	//A_PID=0x0065
 * uart_send(fd,buf);
 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1004"; /* V_FORMAT:720*480(60i) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* VIN_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * h.264 Level
 *  Level 4   40 =>
 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_GOP_STRUCT|V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

/* ******************following for filter************************* // */
	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0026";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD007D";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0056";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0007";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/* ******************************************************************* // */

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA80116";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD0132";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	audio_init( audio_bitrate, fd );

	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	ret_opt( fd );
}


void v108060i_mod( int fd )
{
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	DEBUG( "v108060i_mod.." );
	h64_common_set( fd );
	other_pi( fd );
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };
	/* v108060i */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD0001";	//bit[2]:PCR_FIRST      bit[1:0]:AUTO_NULL_PACKET */
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 30  60i
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	/* buf = "WD0064";	//V_PID=0x0064 */

	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	/* uart_send(fd,buf); */
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0065";	//A_PID=0x0065
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1000"; /* V_FORMAT:1920*1080(59.94/60i) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_SWAP */
/*	buf = "WD0001";  //V_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0002"; /* V_GOP_STRUCT V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /* V_V420_FILTER_00 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA80116";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD0132";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA8011E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD3F28";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */
	audio_init( audio_bitrate, fd );
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	ret_opt( fd );
}


void v108050i_mod( int fd )
{
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	DEBUG( "v108050i_mod.." );
	h64_common_set( fd );
	other_pi( fd );

	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };
	/* v108060i */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001";	//bit[2]:PCR_FIRST      bit[1:0]:AUTO_NULL_PACKET 
//	buf = "WD0000"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 25  50i
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */

	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0065";	//A_PID=0x0065
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG("a_pid=%d ",a_pid);
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG("pmt_pid=%d ",pmt_pid);
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG("pcr_pid=%d ",pcr_pid);
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1001"; /* V_FORMAT:1920*1080(50i) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_SWAP */
/*	buf = "WD0001";  //V_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );

	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0002"; /* V_GOP_STRUCT=IBBP V_STRM_FORMAT=2 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /* V_V420_FILTER_00 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA80116";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD0132";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA8011E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 * buf = "WD3F28";  //
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 * while((uart_rcv_ok(fd)!=1) && (time_out(tpstart)<=50)){;}
 */
	audio_init( audio_bitrate, fd );
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	ret_opt( fd );
}


void v108050p_mod( int fd )
{
	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config *dconfig = config_get_config();

	h64_common_set( fd );
	other_pi( fd );
	char		*buf = NULL;
	struct timeval	tpstart;
	char		__buf[14] = { 0 };
	/* v108050p */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD0001";	//bit[2]:PCR_FIRST      bit[1:0]:AUTO_NULL_PACKET */
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 50  50p
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0065";	//A_PID=0x0065
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD151F"; /* V_FORMAT:1920*1080(50p) */
/*	buf = "WD151E";  //V_FORMAT:1920*1080(60p) */

	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_SWAP */
/*	buf = "WD0001";  //V_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0002"; /* V_GOP_STRUCT V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /* V_V420_FILTER_00 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01478";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0132"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	audio_init( audio_bitrate, fd );
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


void v108060p_mod( int fd )
{
	char		*buf = NULL;
	struct timeval	tpstart;

	/* 文件配置 */
	config_read( get_profile()->script_configfile );
	s_config	*dconfig	= config_get_config();
	char		__buf[14]	= { 0 };
	h64_common_set( fd );
	other_pi( fd );
	/* v108060p */
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD0001";	//bit[2]:PCR_FIRST      bit[1:0]:AUTO_NULL_PACKET */
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3E80";	//SYSTEM_RATE=16000Kbps
 * buf = "WD7530";	//SYSTEM_RATE=30000Kbps
 * buf = "WD1F40";	//SYSTEM_RATE=8000Kbps
 * buf = "WD7D00";	//SYSTEM_RATE=32000Kbps ;Failure!!!
 * uart_send(fd,buf);
 * usleep(2000);
 * V_BITRATE  = 6000
 * FrameRate  = 30  60p
 * Fre  48000
 */
	memset( __buf, 0, 14 );

	int	video_bitrate	= dconfig->scfg_Param.encoder_video_bitrate;
	int	audio_bitrate	= dconfig->scfg_Param.encoder_audio_bitrate;
	int	sys_rate	= MUX_RATE( vn_calc( video_bitrate, 30 ), audio_bitrate,
					    sn_calc( CYCOFPAT_MS ), sn_calc( CYCOFPMT_MS ), sn_calc( CYCOFSIT_MS ), sn_calc( CYCOFPCR_MS ) );
	sprintf( __buf, "WD%02x%02x", GET_HEX( sys_rate, 2 ), GET_HEX( sys_rate, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "MUX_RATE =%04x   l=%02x  h=%02x  %s\n", sys_rate, GET_HEX( sys_rate, 1 ), GET_HEX( sys_rate, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[32:17] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0100E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_BASE[16:1] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01010";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* BIT[15]:PCR_BASE[0]  BIT[8:0]:PCR_EXT[8:0] */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01016";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//V_PID=0x0064
	 * uart_send(fd,buf);
	 */

	memset( __buf, 0, 14 );
	int v_pid = atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_pid, 2 ), GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", v_pid, GET_HEX( v_pid, 1 ), GET_HEX( v_pid, 2 ), __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01018";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0065";	//A_PID=0x0065
 * uart_send(fd,buf);
 */
	memset( __buf, 0, 14 );
	int a_pid = atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	DEBUG("a_pid =%d ",a_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( a_pid, 2 ), GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0066";	//PMT_PID=0x0066
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pmt_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid );
	DEBUG("pmt_pid =%d ",pmt_pid);
	sprintf( __buf, "WD%02x%02x", GET_HEX( pmt_pid, 2 ), GET_HEX( pmt_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0101E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD0064";	//PCR_PID=0x0064
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	int pcr_pid = atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid );
	sprintf( __buf, "WD%02x%02x", GET_HEX( pcr_pid, 2 ), GET_HEX( pcr_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PAT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PMT_CYC=90ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* SIT_CYC=900ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* PCR_CYC=30ms */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01146";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0030";	//V_RATE_MODE=CBR
 * buf = "WD0031";	//V_RATE_MODE=VBR
 * uart_send(fd,buf);
 * usleep(2000);
 */
	memset( __buf, 0, 14 );
	int rate_mode = dconfig->scfg_Param.encoder_video_ratemode_fre;
	sprintf( __buf, "WD%02x%02x", GET_HEX( rate_mode, 2 ), GET_HEX( rate_mode, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "rate mode =%s ", __buf );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1F40";	//V_BITRATE=8000kbps
 * buf = "WD2EE0";	//V_BITRATE=12000kbps
 * buf = "WD1770";	//V_BITRATE=6000kbps
 * buf = "WD7530";	//V_BITRATE=30000kbps
 * buf = "WD5DC0";	//V_BITRATE=24000kbps
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );

	sprintf( __buf, "WD%02x%02x", GET_HEX( video_bitrate, 2 ), GET_HEX( video_bitrate, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "video_bitrate =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1F40";	//VBR:V_AVE_BITRATE=8000kbps */
	buf = "WD0FA0"; /* VBR:V_AVE_BITRATE=4000kbps */
/*	buf = "WD0000";	//CBR:V_AVE_BITRATE=0000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD151E"; /* V_FORMAT:1920*1080(60p) */

	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_SWAP */
/*	buf = "WD0001";  //V_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01420";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD1770";  //V_MIN_BITRATE=6000kbps */
	buf = "WD0BB8"; /* V_MIN_BITRATE=3000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 *  h.264 profile config
	 * 0 auot deceted
	 * 1 high profile
	 * 2 main profile
	 * 3 base profile
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0001";  //V_GOP_STRUCT=IBBP V_STRM_FORMAT=0
 * uart_send(fd,buf);
 * usleep(2000);
 */

	memset( __buf, 0, 14 );
	int v_profile = dconfig->scfg_Param.encoder_video_profile;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_profile, 2 ), GET_HEX( v_profile, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_profile =%s ", __buf );

	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * h.264 Level
	 *  Level 4   40 =>
	 */
	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	memset( __buf, 0, 14 );
	int v_level = dconfig->scfg_Param.encoder_video_level;
	sprintf( __buf, "WD%02x%02x", GET_HEX( v_level, 2 ), GET_HEX( v_level, 1 ) );

	uart_send( fd, __buf );
	usleep( 2000 );
	DEBUG( "v_level =%s ", __buf );

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD2EE0";  //V_MAX_BITRATE=12000kbps */
	buf = "WD1770"; /* V_MAX_BITRATE=6000kbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0002"; /* V_GOP_STRUCT V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /* V_V420_FILTER_00 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01478";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0147E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD000D"; /* ES_INFO_LENGTH */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80110";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD1BE0";  // ES_INFO_00={ 00011011(8bits), 111(3bits), V_PID[12:8](5bits) }
 * uart_send(fd,buf);
 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD1B%02x", (0x1F & GET_HEX( v_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80112";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD64F0";  // ES_INFO_02= { V_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( v_pid, 1 ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0302"; /* ES_INFO_04 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0142"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD03E0";  // ES_INFO_08= { 00000011(8bits), 111(3bits), A_PID[12:8](5bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD03%02x", (0x1F & GET_HEX( a_pid, 2 ) ) );
	uart_send( fd, __buf );

	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	/*
	 * buf = "WD65F0";  // ES_INFO_0A=  { A_PID[7:0](8bits), 1111(4bits), 0000(4bits) }
	 * uart_send(fd,buf);
	 */
	memset( __buf, 0, 14 );
	sprintf( __buf, "WD%02xF0", GET_HEX( a_pid, 1 ) );
	uart_send( fd, __buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* ES_INFO_0C */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	DEBUG( "v108060p_mod" );
	audio_init( audio_bitrate, fd );
	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


void v108030p_mod( int fd )
{
	/*
	 * 文件配置
	 * config_read(get_profile()->script_configfile);
	 * s_config *dconfig = config_get_config();
	 */

	DEBUG( "v108030p_mod.." );
	h64_common_set( fd );
	other_pi( fd );
	/* v108030p */
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2710"; /* SYSTEM_RATE=10Mbps */
/*	buf = "WD7530";	//SYSTEM_RATE=30Mbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* TS_PACKET_SIZE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA01146";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";	//TS_PACKET_SIZE
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0030"; /* V_RATE_MODE */
	uart_send( fd, buf );
	usleep( 2000 );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD17FF"; /* V_FORMAT:1920*1080(30p) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* V_VINPELCLK=1 ->74.25 or 148.5MHz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_MAX_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD00FF";/* V_STRM_FORMAT=255 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01470";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /* V_V420_FILTER_00 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01472";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01474";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0070"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01476";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0010"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01540";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1052";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01542";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01544";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WDFFFF"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01546";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0897"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01548";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0465"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0154A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0172"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0154C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD001A"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0154E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WDFFFF"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01550";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0780"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01552";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0438"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );

	buf = "WA01554";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015A0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
/*	buf = "WD0000";  // */
	buf = "WD4028"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015A2";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0078"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015A4";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0044"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015A6";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015A8";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015AA";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD003C"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015AC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD00FF"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015AE";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015B0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015B2";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD803C"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA015B4";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD001E"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA800EC";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD001E"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80114";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1405"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80116";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0132"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA80118";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD444D"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD56FF"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011C";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1B46"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA8011E";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD3F28"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


void v57650p_mod( int fd )
{
	/*
	 * 文件配置
	 * config_read(get_profile()->script_configfile);
	 * s_config *dconfig = config_get_config();
	 */

	DEBUG( "v57650p_mod.." );
	h64_common_set( fd );
	/* v57650p */
	char		*buf = NULL;
	struct timeval	tpstart;
	buf = "WA01000";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0100"; /* ENC_STC_MODE:Based on STCCLK27OPT */


/*
 * buf = "WD0200";	//ENC_STC_MODE:Based on VICLK
 * buf = "WD0000";	//ENC_STC_MODE:Based on MCLK27
 */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01002";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0020";	//STMODE STCLK
 * buf = "WD0006";	//STMODE STCLK=12.8MHz
 */
	buf = "WD0002"; /* STMODE STCLK=27Mhz */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01004";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0001"; /* bit[2]:PCR_FIRST     bit[1:0]:AUTO_NULL_PACKET */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01006";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD2710"; /* SYSTEM_RATE=10Mbps */
/*	buf = "WD7530";	//SYSTEM_RATE=30Mbps */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01008";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /*  */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01404";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0030"; /* V_RATE_MODE */
	uart_send( fd, buf );
	usleep( 2000 );

	buf = "WA01406";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01408";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* V_AVE_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA0140A";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD17FF"; /* V_FORMAT:720*576(50p) */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01422";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD1F40"; /* V_MAX_BITRATE */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01430";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD00FF"; /* V_STRM_FORMAT */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "WA01418";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	buf = "WD0000"; /* VIN_SWAP */
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WA01470";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0026";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 *  buf = "WA01472";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD007D";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01474";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0056";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01476";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0007";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01542";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01544";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01546";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01548";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154A";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154C";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA0154E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 *  buf = "WA01550";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01552";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA01554";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * //	buf = "WA015A0";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD8028";  //
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015A2";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0006";  //
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015A4";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0004";  //
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015A6";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0000";  //V_STRM_FORMAT
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015AA";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0000";  //V_STRM_FORMAT
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015AC";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0000";  //V_STRM_FORMAT
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //  buf = "WA015B2";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0000";  //V_STRM_FORMAT
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //
 * //	buf = "WA015B4";
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * //	buf = "WD0000";  //V_STRM_FORMAT
 * //	uart_send(fd,buf);
 * //	usleep(2000);
 * //	gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *  buf = "WA800EC";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD000D";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA80114";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0302";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA80118";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD444D";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011A";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD56FF";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011C";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD0000";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 *
 * buf = "WA8011E";
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}


/*
 * buf = "WD3F28";  //V_STRM_FORMAT
 * uart_send(fd,buf);
 * usleep(2000);
 * gettimeofday(&tpstart,NULL);
 */
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}

	buf = "ENC0";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
	ret_opt( fd );
}


void ret_opt( int fd )
{
	DEBUG( "ret_opt.." );
	char		*buf = NULL;
	struct timeval	tpstart;


	buf = "EST";
	uart_send( fd, buf );
	usleep( 2000 );
	gettimeofday( &tpstart, NULL );
	while ( (uart_rcv_ok( fd ) != 1) && (time_out( tpstart ) <= 50) )
	{
		;
	}
}


