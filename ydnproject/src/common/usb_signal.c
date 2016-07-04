#define _GNU_SOURCE
#include <pthread.h>
#include <usb_signal.h>
#include <common.h>
#include <menu_foun.h>
#include <debug.h>
#include <init-configuration.h>

static usb_operation_t usb_signal;

void set_usb_signal( void *usb_action )
{
	s_config	*dconfig	= config_get_config();
	usb_operation_t *sig		= (usb_operation_t *) usb_action;
	pthread_mutex_lock( &dconfig->set_usb_glob_info_lock );
	usb_signal.is_start = sig->is_start;
	pthread_mutex_unlock( &dconfig->set_usb_glob_info_lock );
	DEBUG( "is_start : %d ", usb_signal.is_start );
}


static void usb_signal_hander( int signum, siginfo_t *siginfo, void *not_used )
{
	if ( SIGUSR2 == signum )
	{
		usb_operation_t *usb_action = siginfo->si_value.sival_ptr;
		set_usb_signal( usb_action );
	}
}


void usb_op_signal( void )
{
	struct sigaction sig;

	sig.sa_sigaction = usb_signal_hander;
	sigemptyset( &sig.sa_mask );
	sig.sa_flags = SA_SIGINFO;

	if ( sigaction( SIGUSR2, &sig, NULL ) < 0 )
	{
		fprintf( stderr, "%s: %d:%s\n", __FUNCTION__, __LINE__, strerror( errno ) );
		exit( 1 );
	}

	while ( 1 )
		nano_sleep( 0, 500000000 );
}


int get_usb_signal( void )
{
	return(usb_signal.is_start);
}


/*
 * ÏûÏ¢ÉèÖÃ
 */
void  send_usb_stop_message( pthread_t usb_action_t, int signum, void *dcfig, int is_start )
{
	union sigval		usbsigval;
	static usb_operation_t	usb_cation;
	s_config		*dconfig = (s_config *) dcfig;
	usb_cation.op_mod	= dconfig->scfg_Param.stream_usb_mod;/* USB_SINGLE; */
	usb_cation.ts_size	= dconfig->configParam.usb_tsfilesize;
	usb_cation.is_start	= is_start;
	usbsigval.sival_ptr	= &usb_cation;


	if ( pthread_sigqueue( usb_action_t, signum, usbsigval ) != 0 )
		perror( "send error\n" );
}


