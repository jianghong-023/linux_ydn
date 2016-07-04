#include <reset-oparetion.h>
#include <readbus.h>
#include <io_rw.h>
#include <freememscript.h>
#include <debug.h>
#include <common_conf.h>


struct _bus_initial {
	bus_init	bus_inttt;
	uint8_t		* map_base;
};


static int put_data( char *data, short offest, char *map_base, long );


struct _bus_initial * malloc_bus_addr( int *fd_mmap );


static struct _bus_initial *init_bus_map = NULL;


struct _bus_initial * malloc_bus_addr( int *fd_mmap )
{
	init_bus_map = (struct _bus_initial*) malloc( sizeof(struct _bus_initial) );
	memset( init_bus_map, 0, sizeof(struct _bus_initial) );

	*fd_mmap = open( "/dev/mem", O_RDWR | O_SYNC );
	if ( *fd_mmap == -1 )
	{
		exit( -1 );
	}

	init_bus_map->map_base = mmap( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd_mmap, MAP_BASE_ADDR );

	if ( init_bus_map->map_base == 0 )
	{
		printf( "bus mmap NULL pointer!\n" );
	}

	return(init_bus_map);
}


void free_bus_addr( struct _bus_initial *bus_map, int fd )
{
	munmap( bus_map->map_base, MAP_SIZE );
	free( bus_map );
	close( fd );
}


void audio_video_pid( uint8_t *bus_addr, s_config *dconfig )
{
	uint16_t v_pid = (uint16_t) atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	writeb( GET_HEX( v_pid, 2 ), &bus_addr[(BUS_INIT_BASE + 0x15) / sizeof(uint8_t)] );     /* pid1[12:8] */
	writeb( GET_HEX( v_pid, 1 ), &bus_addr[(BUS_INIT_BASE + 0x16) / sizeof(uint8_t)] );     /* pid1[7:0] video pid */

	uint16_t a_pid = (uint16_t) atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid );
	writeb( GET_HEX( a_pid, 2 ), &bus_addr[(BUS_INIT_BASE + 0x17) / sizeof(uint8_t)] );     /* pid2[12:8] */
	writeb( GET_HEX( a_pid, 1 ), &bus_addr[(BUS_INIT_BASE + 0x18) / sizeof(uint8_t)] );     /* pid2[7:0] audio pid */
}


void filter_ts_pmt( uint8_t* map_base, char pmt_0, char pmt_1 )
{
	writeb( pmt_0, &map_base[(BUS_INIT_BASE + 0x93) / sizeof(uint8_t)] );
	writeb( pmt_1, &map_base[(BUS_INIT_BASE + 0x94) / sizeof(uint8_t)] );
}


/* 0x00 enable 0x01 deenable */
void eit_insert_table( uint8_t* map_base, char eitflags )
{
	writeb( eitflags, &map_base[(BUS_INIT_BASE + 0x1B) / sizeof(uint8_t)] );
}


/* 0x00 enable 0x01 deenable */
void nit_insert_table( uint8_t* map_base, char nitflags )
{
	writeb( nitflags, &map_base[(BUS_INIT_BASE + 0x1A) / sizeof(uint8_t)] );
}


/*  */
void reset_dvb_t( char res, uint8_t* map_base )
{
	writeb( res, &map_base[(BUS_INIT_BASE + 0x00) / sizeof(uint8_t)] );
}


void reset_usb_op( char value, uint8_t* map_base )
{
	writeb( value, &map_base[(BUS_INIT_BASE + 0x19) / sizeof(uint8_t)] );
}


void modulator_set( uint8_t* map_base, s_config *dconfig )
{
	writeb( dconfig->scfg_Param.encode_rate, &map_base[(BUS_INIT_BASE + 0x01) / sizeof(uint8_t)] );
	writeb( dconfig->scfg_Param.modulelate_mode, &map_base[(BUS_INIT_BASE + 0x02) / sizeof(uint8_t)] );
	writeb( dconfig->scfg_Param.fft_mode, &map_base[(BUS_INIT_BASE + 0x03) / sizeof(uint8_t)] );
	writeb( dconfig->scfg_Param.deltal, &map_base[(BUS_INIT_BASE + 0x04) / sizeof(uint8_t)] );
	writeb( dconfig->scfg_Param.channel_width, &map_base[(BUS_INIT_BASE + 0x05) / sizeof(uint8_t)] );
#if 1
	DEBUG( "encode_rate =%d \n", dconfig->scfg_Param.encode_rate );
	DEBUG( "modulelate_mode =%d \n", dconfig->scfg_Param.modulelate_mode );
	DEBUG( "fft_mode =%d \n", dconfig->scfg_Param.fft_mode );
	DEBUG( "deltal =%d \n", dconfig->scfg_Param.deltal );
	DEBUG( "channel_width =%d \n", dconfig->scfg_Param.channel_width );
#endif
}


void fpga_w_addr( bus_init *bus_inttt, uint8_t* map_base )
{
	writeb( bus_inttt->usb_rdaddr_b3124, &map_base[(BUS_OFFSET_ADDR + 0x0e) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b2316, &map_base[(BUS_OFFSET_ADDR + 0x0F) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b158, &map_base[(BUS_OFFSET_ADDR + 0x10) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b70, &map_base[(BUS_OFFSET_ADDR + 0x11) / sizeof(uint8_t)] );
}

void fpga_r_addr( bus_init *bus_inttt, uint8_t* map_base )
{
	writeb( bus_inttt->usb_rdaddr_b3124, &map_base[(BUS_OFFSET_ADDR + 0x0a) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b2316, &map_base[(BUS_OFFSET_ADDR + 0x0b) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b158, &map_base[(BUS_OFFSET_ADDR + 0x0c) / sizeof(uint8_t)] );
	writeb( bus_inttt->usb_rdaddr_b70, &map_base[(BUS_OFFSET_ADDR + 0x0d) / sizeof(uint8_t)] );
}


void usb_start_rwop( char value, uint8_t* map_base )
{
	writeb( value, &map_base[(BUS_OFFSET_ADDR + 0x07) / sizeof(uint8_t)] );
}


void usbr_interrupt_enable( char value, uint8_t* map_base )
{
	writeb( value, &map_base[(BUS_OFFSET_ADDR + 0x13) / sizeof(uint8_t)] );
}


void usb_dir( char value, uint8_t* map_base )
{
	writeb( value, &map_base[(BUS_OFFSET_ADDR + 6) / sizeof(uint8_t)] );/* 改0时为读 */
}


void init_bus( void )
{
	int		fd_mmap;
	s_config	*dconfig = config_get_config();

	struct _bus_initial *bus_addr = NULL;
	bus_addr = malloc_bus_addr( &fd_mmap );


	/*
	 * 产生配置表
	 */
	gener_table();

	/* audio_video_pid( bus_addr->map_base, dconfig ); */

	uint16_t	pmtpid = (uint16_t) atoi( (char *) dconfig->scfg_Param.encoder_pmt_pid );
	uint8_t		pmt_pid_a[2];
	pmt_pid_a[0]	= GET_HEX( pmtpid, 2 );
	pmt_pid_a[1]	= GET_HEX( pmtpid, 1 );

#if 1
	DEBUG( "encoder_pmt_pid =%x ", pmtpid );
	DEBUG( "pmtpid[0]= %02x  pmtpid[1]=%02x ", pmt_pid_a[0], pmt_pid_a[1] );

	DEBUG( "reset =%d \n", dconfig->configParam.reset );
	DEBUG( "encoder_pmt_pid =%d \n", atoi( (const char *) dconfig->scfg_Param.encoder_pmt_pid ) );
	DEBUG( "encoder_pcr_pid =%d \n", atoi( (const char *) dconfig->scfg_Param.encoder_pcr_pid ) );
	int	_pid		= atoi( (const char *) dconfig->scfg_Param.encoder_video_pid );
	char	__buf[14]	= { 0 };
	sprintf( __buf, "WD%02x%02x", GET_HEX( _pid, 2 ), GET_HEX( _pid, 1 ) );
	DEBUG( "encoder_video_pid =%04x   l=%02x  h=%02x  %s\n", _pid, GET_HEX( _pid, 1 ), GET_HEX( _pid, 2 ), __buf );
	DEBUG( "encoder_audio_pid =%d \n", atoi( (const char *) dconfig->scfg_Param.encoder_audio_pid ) );

#endif
	usleep( 1000 );

	filter_ts_pmt( bus_addr->map_base, pmt_pid_a[0], pmt_pid_a[1] );
	eit_insert_table( bus_addr->map_base, 0x01 );
	nit_insert_table( bus_addr->map_base, 0x01 );
	/* 关闭usb操作 */
	writeb( 0x00, &bus_addr->map_base[(BUS_INIT_BASE + 0x07) / sizeof(uint8_t)] );
	modulator_set( bus_addr->map_base, dconfig );

	writeb( 0x01, &bus_addr->map_base[(BUS_INIT_BASE + 0x06) / sizeof(uint8_t)] );  /* usb dir */
	reset_dvb_t( dconfig->configParam.reset & 0x01, bus_addr->map_base );           /* dvb_t 复位 */
	usleep( 5000 );
	reset_dvb_t( dconfig->configParam.reset & 0x00, bus_addr->map_base );           /* dvb_t 复位 */
	usleep( 5000 );
	free_bus_addr( bus_addr, fd_mmap );

	
}


void put_tale( char *data, int type )
{
	if ( !data )
		return;

	char	* map_base = NULL;
	int	fd_mmap;/* ---------------------mmap------------------------- */
	fd_mmap = open( "/dev/mem", O_RDWR | O_SYNC );  if ( fd_mmap == -1 )
	{
		return;
	}
	/* ---------------------------bus state address map------------------------------- */
	map_base = mmap( NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, MAP_BASE_ADDR );
	if ( map_base == NULL )
	{
		printf( "bus mmap NULL pointer!\n" ); return;
	}

	switch ( type )
	{
	case NIT_TABLE:
	{
		writeb( NIT_START, &map_base[(BUS_INIT_BASE + NIT_STOP_START_BIT) / sizeof(uint8_t)] );
		put_data( data, NIT_OFFSET, map_base, 0 );
		writeb( NIT_STOP, &map_base[(BUS_INIT_BASE + NIT_STOP_START_BIT) / sizeof(uint8_t)] );
	}
	break;
	case SDT_TABLE:
	{
		writeb( SDT_START, &map_base[(BUS_INIT_BASE + SDT_STOP_START_BIT) / sizeof(uint8_t)] );
		put_data( data, SDT_OFFSET, map_base, 0 );
		writeb( SDT_STOP, &map_base[(BUS_INIT_BASE + SDT_STOP_START_BIT) / sizeof(uint8_t)] );
	}
	break;
	case PAT_TABLE:
	{
		writeb( PAT_START, &map_base[(BUS_INIT_BASE + PAT_STOP_START_BIT) / sizeof(uint8_t)] );
		put_data( data, PAT_OFFSET, map_base, 0 );
		writeb( PAT_STOP, &map_base[(BUS_INIT_BASE + PAT_STOP_START_BIT) / sizeof(uint8_t)] );
	}
	break;
	case PMT_TABLE:
	{
		writeb( PMT_START, &map_base[(BUS_INIT_BASE + PMT_STOP_START_BIT) / sizeof(uint8_t)] );
		put_data( data, PMT_OFFSET, map_base, 0 );
		writeb( PMT_STOP, &map_base[(BUS_INIT_BASE + PMT_STOP_START_BIT) / sizeof(uint8_t)] );
	}
	break;

	case EIT_TABLE:
	{
		static int count = 0;
		writeb( PMT_START, &map_base[(BUS_INIT_BASE + EIT_STOP_START_BIT) / sizeof(uint8_t)] );
		put_data( data, EIT_OFFSET, map_base, count );
		writeb( PMT_STOP, &map_base[(BUS_INIT_BASE + EIT_STOP_START_BIT) / sizeof(uint8_t)] );
		count += 188;
		if ( count > 188 )
			count = 0;
	}
	break;
	}
	DEBUG( "type %x:", type );
#if 1
	int	i;
	int	y = 0;

	for ( i = 0; i < 188; i++ )
	{
		printf( "%02x ", data[i] );

		++y;

		if ( y >= 20 )
		{
			y = 0;

			DEBUG( "\n" );
		}
	}
	DEBUG( "\n" );
#endif

	munmap( map_base, MAP_SIZE );
	close( fd_mmap );
}


static int put_data( char *data, short offest, char *map_base, long adrr )
{
	int i = -1, ret = -1;

	if ( !data )
		return(0);

	for ( i = 0; i <= 187; i++ )
	{
		writeb( data[i], &map_base[(BUS_INIT_BASE + offest + adrr + i) / sizeof(uint8_t)] );
	}

	return(ret = i);
}


