#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <main_call.h>
#include <dev-info.h>
#include <gpio_config.h>
#include <reset-oparetion.h>
#include <debug.h>
#include <menu_foun.h>

#define MEMDEV_IOC_MAGIC 'G'
#define F_CENTER \
	_IO( MEMDEV_IOC_MAGIC, 0x65 )
#define BPF \
	_IO( MEMDEV_IOC_MAGIC, 0x66 )
#define DEF \
	_IO( MEMDEV_IOC_MAGIC, 0x67 )
#define CGF_MHZ \
	_IO( MEMDEV_IOC_MAGIC, 0x68 )

#define FR_OUT \
	_IO( MEMDEV_IOC_MAGIC, 0x69 )


void input_mod_gpio( uint8_t i_mod );


static int adf4350_configuration( void );


static volatile  int progr_bar;

void set_progr_bar( int value )
{
	progr_bar = value;
}


int get_progr_bar( void )
{
	return(progr_bar);
}


/* Baseband Digital Upconverter */
static double ftw_clac( double mhz )
{
	double res, tmp;

	if ( mhz == 0 )
		mhz = 500.000;
	tmp = mhz / (2400 / 16) * ( (1 << 24) - 1);

	res = ( (int) (tmp + 0.5) );

	return(res);
}


/* Digital 16× Tunable Band-Pass Filter */
static double bpf_calc( double mhz )
{
	double res, tmp;

	if ( mhz == 0 )
		mhz = 500.000;

	tmp = mhz / (2400) * ( (1 << 16) - 1);

	res = ( (int) (tmp + 0.5) );
	return(res);
}


/*
 * adf4350 配置
 *
 *
 ************************************/
static int adf4350_configuration( void )
{
	int fd;
	fd = open( "/dev/adf4350", O_RDWR | O_SYNC );
	if ( fd < 0 )
	{
		printf( "4350 open error\n" );
	}


	return(fd);
}


/*
 *
 *
 */

static float powrt_calc( uint8_t channel_width, signed char level, float fre_q )
{
	float	rf_power_init;
	int	freq = fre_q;
	rf_power_init = 0x86;
	switch ( channel_width )
	{
	case 0x00: rf_power_init	= rf_power_init; break;                                 /* 8m */
	case 0x01: rf_power_init	= rf_power_init / pow( pow( 10, 0.05 ), 0.6 ); break;   /* 7m */
	case 0x02: rf_power_init	= rf_power_init / pow( pow( 10, 0.05 ), 1.6 ); break;   /* 6m */
	default: rf_power_init		= rf_power_init;
	}

//	DEBUG( "freq=%d ", freq );

	if ( freq >= 30 && freq <= 60 )
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 1.2 ) / pow( pow( 10, 0.05 ), 0.3 / (60 - 30) * (60 - freq) );
	else if ( freq > 60 && freq <= 140 )
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 1.2 ) / pow( pow( 10, 0.05 ), 0.8 / (140 - 60) * (freq - 60) );
	else if ( freq > 140 && freq <= 240 )
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 2 );
	else if ( freq > 240 && freq <= 410 )
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 0.3 ) / pow( pow( 10, 0.05 ), 1.7 / (410 - 240) * (410 - freq) );
	else if ( freq > 410 && freq <= 470 )
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 0.3 );
	else if ( freq > 470 && freq <= 500 )
	{
		rf_power_init = rf_power_init / pow( pow( 10, 0.05 ), 0.3 / (500 - 470) * (500 - freq) );
		//DEBUG( "rf_power_init = %f ", rf_power_init );
	}else if ( freq > 500 && freq <= 550 )
	{
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.0 / (550 - 500) * (freq - 500) );
	}else if ( freq > 550 && freq <= 620 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.0 );
	else if ( freq > 620 && freq <= 690 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.0 ) * pow( pow( 10, 0.05 ), 0.8 / (690 - 620) * (freq - 620) );
	else if ( freq > 690 && freq <= 770 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.8 );
	else if ( freq > 770 && freq <= 810 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 0.9 ) * pow( pow( 10, 0.05 ), 0.8 / (810 - 770) * (freq - 770) );
	else if ( freq > 810 && freq <= 870 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.7 );
	else if ( freq > 870 && freq <= 960 )
		rf_power_init = rf_power_init * pow( pow( 10, 0.05 ), 1.1 ) * pow( pow( 10, 0.05 ), 1.2 / (960 - 870) * (freq - 870) );
	else rf_power_init = rf_power_init;
	return(rf_power_init);
}


/*
 *
 *
 */
static void choose_f( float fre_q, uint8_t *FSC_H, uint8_t *FSC_L )
{
	int freq = fre_q;

	if ( freq >= 30 && freq <= 620 )
	{
		*FSC_H = 0x02; *FSC_L = 0xf0;
	}else if ( freq > 620 && freq <= 770 )
	{
		*FSC_H = 0x03; *FSC_L = 0x00;
	}else if ( freq > 770 && freq <= 810 )
	{
		*FSC_H = 0x03; *FSC_L = 0x7f;
	}else if ( freq > 810 && freq <= 870 )
	{
		*FSC_H = 0x03; *FSC_L = 0x7f;
	}else if ( freq > 870 && freq <= 960 )
	{
		*FSC_H = 0x03; *FSC_L = 0xf0;
	}else {
		*FSC_H = 0x02; *FSC_L = 0xf0;
	}
}


/*
 * ad9789 配置
 *
 *
 ************************************/
int ad9789_configuration()
{
	int		fd, res;
	unsigned int	tmp;
	uint8_t		rf_power;
	float		freq = 0.0;
	cfgt_t		cfgt_t;
	float		rf_power_tmp;
	signed char	level_delta;

	signed char	level_init = -16;
	float		level;

	fd = open( "/dev/ad9789", O_RDWR | O_SYNC );
	if ( fd < 0 )
	{
		printf( "9789 open error\n" );
	}

	if ( config_read( get_profile()->script_configfile ) < 0 )
		return(-1);
	s_config *dconfig = config_get_config();
	/* 获取命令及MHz */
	freq		= dconfig->localstatus.cfig_ad9789_ftw_bpf;
	level		= dconfig->scfg_Param.modulator_rf_level;
	tmp		= ftw_clac( freq );
	rf_power	= powrt_calc( dconfig->scfg_Param.channel_width, level, freq );

	choose_f( freq, &cfgt_t.FSC_H, &cfgt_t.FSC_L );

	level_delta	= level_init - level;
	rf_power_tmp	= pow( 1 / pow( 10, 0.05 ), level_delta );
	rf_power_tmp	= rf_power * rf_power_tmp + 0.5;
	rf_power	= (int) rf_power_tmp;

	cfgt_t.flags	= tmp & 0xffffff;
	cfgt_t.rf_power = rf_power & 0xff;
	cfgt_t.fr_out	= dconfig->scfg_Param.modulator_rf_on;
#if 0
	DEBUG( "fr out :%x   cfgt_t.fr_out:%x", dconfig->scfg_Param.modulator_rf_on, cfgt_t.fr_out );
	DEBUG( "flags %ld ", cfgt_t.flags );
	DEBUG( "rf_power %d ", cfgt_t.rf_power );
#endif
	usleep( 4000 );
	res = ioctl( fd, F_CENTER, &cfgt_t );
	if ( res == -1 )
		DEBUG( "ioctl 9789 error" );

	tmp = bpf_calc( freq );


	if ( dconfig->scfg_Param.channel_width == 0x00 )/* 8M */
	{
		cfgt_t.flags	= 0x00;
		res		= ioctl( fd, CGF_MHZ, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );


		res = ioctl( fd, FR_OUT, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );


		cfgt_t.flags	= tmp;
		res		= ioctl( fd, BPF, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );
	}else if ( dconfig->scfg_Param.channel_width == 0x01 )/* 7M */
	{
		cfgt_t.flags	= 0x01;
		res		= ioctl( fd, CGF_MHZ, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );

		res = ioctl( fd, FR_OUT, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );


		cfgt_t.flags	= tmp;
		res		= ioctl( fd, BPF, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );
	}else if ( dconfig->scfg_Param.channel_width == 0x02 )/* 6M */
	{
		cfgt_t.flags	= 0x02;
		res		= ioctl( fd, CGF_MHZ, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );

		res = ioctl( fd, FR_OUT, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );


		cfgt_t.flags	= tmp;
		res		= ioctl( fd, BPF, &cfgt_t );
		if ( res == -1 )
			DEBUG( "ioctl 9789 error" );
	}


	return(fd);
}


int peripheral_dev_config( struct dvb_peripheral *devconfig )
{
	if(get_unlock_menu_exit())
		return 0;
	
	set_lock_menu_exit( 1 );
	interrupt_signals_mask(1);
	
	if ( devconfig )
	{
		
		
		struct dvb_peripheral *config = devconfig;

		if ( config )
		{
			int devlcfd = 0, devfrefd = 0, devmofd = 0, fd;
#if 1
			if ( (config->clockset) && (config->open_clockdev) )
			{
				config->clockset( devlcfd = config->open_clockdev(), config->i2c_0addr );
			} else
				DEBUG( "peripheral_dev_config fail...%d", devlcfd );

			close( devlcfd );

			nano_sleep( 1, 0 );

			if ( (config->pixefreq) && (config->open_freqdev) )
			{
				config->pixefreq( devfrefd = config->open_freqdev() );
			} else
				DEBUG( "peripheral_dev_config fail...%d", devfrefd );

			close( devfrefd );

			nano_sleep( 1, 0 );

			if ( (config->modopcodeset) && (config->open_uartdev) )
			{
				config->modopcodeset( devmofd = config->open_uartdev() );
			} else
				DEBUG( "peripheral_dev_config fail...%d", devmofd );

			close( devmofd );
//			DEBUG( "peripheral_dev_config end..." );
#endif

			/* adf4350 and ad9789 */
			if ( (fd = adf4350_configuration() ) < 0 )
			{
				DEBUG( "4350 config fail ..." );
			} else
				close( fd );
			usleep( 4000 );
			if ( (fd = ad9789_configuration() ) < 0 )
			{
				DEBUG( "9789 config fail ..." );
			} else
				close( fd );
			usleep( 4000 );
			/* init */
			init_bus();
			//progr_bar = 0x40;
			set_progr_bar(0x40);
		}

		
	}
	interrupt_signals_mask(0);
	set_lock_menu_exit( 0 );

	return(0);
}


