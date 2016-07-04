
#include <stdio.h>
#include "section_put.h"
#include "descriptor.h"
#include "dvbpsi.h"
#include "psi.h"

void message( dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg )
{
	switch ( level )
	{
	case DVBPSI_MSG_ERROR: DEBUG( "Error: " ); break;
	case DVBPSI_MSG_WARN:  DEBUG( "Warning: " ); break;
	//case DVBPSI_MSG_DEBUG: DEBUG( "Debug: " ); break;
	default: /* do nothing */
		return;
	}
}


/*****************************************************************************
* writePSI
*****************************************************************************/
void writePSI( uint8_t* p_packet, dvbpsi_psi_section_t* p_section, int type )
{
	p_packet[0] = 0x47;
#if 1
	while ( p_section )
	{
		uint8_t * p_pos_in_ts;
		uint8_t * p_byte	= p_section->p_data;
		uint8_t * p_end		= p_section->p_payload_end
					  + (p_section->b_syntax_indicator ? 4 : 0);

		p_packet[1]	|= 0x40;
		p_packet[3]	= (p_packet[3] & 0x0f) | 0x10;

		p_packet[4]	= 0x00; /* pointer_field */
		p_pos_in_ts	= p_packet + 5;

		while ( (p_pos_in_ts < p_packet + 188) && (p_byte < p_end) )
			*(p_pos_in_ts++) = *(p_byte++);
		while ( p_pos_in_ts < p_packet + 188 )
			*(p_pos_in_ts++) = 0xff;

#if 1
		while ( p_byte < p_end )
		{
			p_packet[1]	&= 0xbf;
			p_packet[3]	= (p_packet[3] & 0x0f) | 0x10;

			p_pos_in_ts = p_packet + 4;

			while ( (p_pos_in_ts < p_packet + 188) && (p_byte < p_end) )
				*(p_pos_in_ts++) = *(p_byte++);
			while ( p_pos_in_ts < p_packet + 188 )
				*(p_pos_in_ts++) = 0xff;
		}
#endif
		p_section = p_section;


		while ( p_section )
		{
			put_tale( (char *) p_packet, type );
#if 0
			int	i;
			int	y = 0;

			for ( i = 0; i < 188; i++ )
			{
				DEBUG( "%02x ", p_packet[i] );

				++y;

				if ( y >= 20 )
				{
					y = 0;

					DEBUG( "\n" );
				}
			}
			printf( "\n" );
#endif
			p_section = p_section->p_next;
		}

/*	    DEBUG("\n"); */
	}
#endif
}


