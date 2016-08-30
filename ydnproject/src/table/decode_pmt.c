/*****************************************************************************
* decode_pmt.c: PAT decoder example
*----------------------------------------------------------------------------
* Copyright (C) 2001-2011 VideoLAN
* $Id$
*
* Authors: Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
*----------------------------------------------------------------------------
*
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>


#include <stdint.h>


#include "dvbpsi.h"
#include "psi.h"
#include "descriptor.h"
#include "pmt.h"
#include "./descriptors/dr.h"
#include "section_put.h"
#include <pares_psi.h>


#define SYSTEM_CLOCK_DR		0x0B
#define MAX_BITRATE_DR		0x0E
#define STREAM_IDENTIFIER_DR	0x52
#define SUBTITLING_DR		0x59

extern struct parse_ts_id parse_ts_id;


/*****************************************************************************
* ReadPacket
*****************************************************************************/
static bool ReadPacket( int i_fd, uint8_t* p_dst )
{
	int	i	= 187;
	int	i_rc	= 1;

	p_dst[0] = 0;

	while ( (p_dst[0] != 0x47) && (i_rc > 0) )
	{
		i_rc = read( i_fd, p_dst, 1 );
	}

	while ( (i != 0) && (i_rc > 0) )
	{
		i_rc = read( i_fd, p_dst + 188 - i, i );
		if ( i_rc >= 0 )
			i -= i_rc;
	}

	return( (i == 0) ? true : false);
}


/*****************************************************************************
* GetTypeName
*****************************************************************************/
static char* GetTypeName( uint8_t type )
{
	switch ( type )
	{
	case 0x00:
		return("Reserved");
	case 0x01:
		return("ISO/IEC 11172 Video");
	case 0x02:
		return("ISO/IEC 13818-2 Video");
	case 0x03:
		return("ISO/IEC 11172 Audio");
	case 0x04:
		return("ISO/IEC 13818-3 Audio");
	case 0x05:
		return("ISO/IEC 13818-1 Private Section");
	case 0x06:
		return("ISO/IEC 13818-1 Private PES data packets");
	case 0x07:
		return("ISO/IEC 13522 MHEG");
	case 0x08:
		return("ISO/IEC 13818-1 Annex A DSM CC");
	case 0x09:
		return("H222.1");
	case 0x0A:
		return("ISO/IEC 13818-6 type A");
	case 0x0B:
		return("ISO/IEC 13818-6 type B");
	case 0x0C:
		return("ISO/IEC 13818-6 type C");
	case 0x0D:
		return("ISO/IEC 13818-6 type D");
	case 0x0E:
		return("ISO/IEC 13818-1 auxillary");
	default:
		if ( type < 0x80 )
			return("ISO/IEC 13818-1 reserved");
		else
			return("User Private");
	}
}


/*****************************************************************************
* DumpMaxBitrateDescriptor
*****************************************************************************/
static void DumpMaxBitrateDescriptor( dvbpsi_max_bitrate_dr_t* bitrate_descriptor )
{
	printf( "Bitrate: %d\n", bitrate_descriptor->i_max_bitrate );
}


/*****************************************************************************
* DumpSystemClockDescriptor
*****************************************************************************/
static void DumpSystemClockDescriptor( dvbpsi_system_clock_dr_t* p_clock_descriptor )
{
/*
 *  printf("External clock: %s, Accuracy: %E\n",
 *     p_clock_descriptor->b_external_clock_ref ? "Yes" : "No",
 *     p_clock_descriptor->i_clock_accuracy_integer *
 *     //pow(10.0, -(double)p_clock_descriptor->i_clock_accuracy_exponent));
 */
}


/*****************************************************************************
* DumpStreamIdentifierDescriptor
*****************************************************************************/
static void DumpStreamIdentifierDescriptor( dvbpsi_stream_identifier_dr_t* p_si_descriptor )
{
	printf( "Component tag: %d\n",
		p_si_descriptor->i_component_tag );
}


/*****************************************************************************
* DumpSubtitleDescriptor
*****************************************************************************/
static void DumpSubtitleDescriptor( dvbpsi_subtitling_dr_t* p_subtitle_descriptor )
{
	int a;

	printf( "%d subtitles,\n", p_subtitle_descriptor->i_subtitles_number );
	for ( a = 0; a < p_subtitle_descriptor->i_subtitles_number; ++a )
	{
		printf( "       | %d - lang: %c%c%c, type: %d, cpid: %d, apid: %d\n", a,
			p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[0],
			p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[1],
			p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[2],
			p_subtitle_descriptor->p_subtitle[a].i_subtitling_type,
			p_subtitle_descriptor->p_subtitle[a].i_composition_page_id,
			p_subtitle_descriptor->p_subtitle[a].i_ancillary_page_id );
	}
}


/*****************************************************************************
* DumpDescriptors
*****************************************************************************/
static void DumpDescriptors( const char* str, dvbpsi_descriptor_t* p_descriptor )
{
	int i;

	while ( p_descriptor )
	{
		DEBUG( "%s 0x%02x : ", str, p_descriptor->i_tag );
		switch ( p_descriptor->i_tag )
		{
		case SYSTEM_CLOCK_DR:
			DumpSystemClockDescriptor( dvbpsi_DecodeSystemClockDr( p_descriptor ) );
			break;
		case MAX_BITRATE_DR:
			DumpMaxBitrateDescriptor( dvbpsi_DecodeMaxBitrateDr( p_descriptor ) );
			break;
		case STREAM_IDENTIFIER_DR:
			DumpStreamIdentifierDescriptor( dvbpsi_DecodeStreamIdentifierDr( p_descriptor ) );
			break;
		case SUBTITLING_DR:
			DumpSubtitleDescriptor( dvbpsi_DecodeSubtitlingDr( p_descriptor ) );
			break;
		default:
			DEBUG( "\"" );
			for ( i = 0; i < p_descriptor->i_length; i++ )
				DEBUG( "%c", p_descriptor->p_data[i] );
			DEBUG( "\"\n" );
		}
		p_descriptor = p_descriptor->p_next;
	}
};


/*****************************************************************************
* DumpPMT
*****************************************************************************/
static void DumpPMT( void* p_zero, dvbpsi_pmt_t* p_pmt )
{
	dvbpsi_pmt_es_t* p_es = p_pmt->p_first_es;
	printf( "\n" );
	printf( "New active PMT\n" );
	printf( "  program_number : %d\n",
		p_pmt->i_program_number );
	printf( "  version_number : %d\n",
		p_pmt->i_version );
	printf( "  PCR_PID        : 0x%x (%d)\n",
		p_pmt->i_pcr_pid, p_pmt->i_pcr_pid );

	parse_ts_id.i_pcr_pid = p_pmt->i_pcr_pid;
	DumpDescriptors( "    ]", p_pmt->p_first_descriptor );
	printf( "    | type @ elementary_PID\n" );
	int i = 0;
	while ( p_es )
	{
		DEBUG( "    | 0x%02x (%s) @ 0x%x (%d)  byte[%d]\n",
		       p_es->i_type, GetTypeName( p_es->i_type ),
		       p_es->i_pid, p_es->i_pid, ++i );
		if ( p_es->i_type == 0x1b || p_es->i_type == 0x01 )
		{
			parse_ts_id.i_video_pid = p_es->i_pid;
		}
		if ( p_es->i_type == 0x03 )
		{
			parse_ts_id.i_audieo_pid = p_es->i_pid;
			DEBUG( "   audio pid :%x \n", parse_ts_id.i_audieo_pid );
			parse_ts_id.i_parse_status = 0x02;
		}
		DumpDescriptors( "    |  ]", p_es->p_first_descriptor );

		p_es = p_es->p_next;
		DEBUG( "%d", i );
	}


	dvbpsi_pmt_delete( p_pmt );
}


/*
 * static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
 * {
 *    switch(level)
 *    {
 *        case DVBPSI_MSG_ERROR: fprintf(stderr, "Error: "); break;
 *        case DVBPSI_MSG_WARN:  fprintf(stderr, "Warning: "); break;
 *        case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: "); break;
 *        default: / * do nothing * /
 *            return;
 *    }
 *    fprintf(stderr, "%s\n", msg);
 * }
 */


/*****************************************************************************
* main
*****************************************************************************/
int pmt_parse_enter( char *file, uint16_t i_program_number, uint16_t i_pmt_pid )
{
	int		i_fd;
	uint8_t		data[188];
	dvbpsi_t	*p_dvbpsi;
	bool		b_ok;


	DEBUG( "i_program_number =%04x  i_pmt_pid=%04x", i_program_number, i_pmt_pid );
	i_fd = open( file, O_RDONLY, S_IRUSR | S_IWUSR );
	if ( i_fd < 0 )
		return(1);


	p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		goto out;

	if ( !dvbpsi_pmt_attach( p_dvbpsi, i_program_number, DumpPMT, NULL ) )
		goto out;

	b_ok = ReadPacket( i_fd, data );
	int count = 1024;
	while ( b_ok )
	{
		uint16_t i_pid = ( (uint16_t) (data[1] & 0x1f) << 8) + data[2];
		if ( i_pid == i_pmt_pid )
		{
			dvbpsi_packet_push( p_dvbpsi, data );
			/* DEBUG("---------------------"); */
#if 0
			uint16_t	secion_length		= (data[1] & 0x0F) << 8 | data[2];
			uint16_t	program_info_length	= (data[10] & 0x0F) << 8 | data[11];
			int		pos			= 12;

			if ( program_info_length != 0 )
				pos += program_info_length;

			DEBUG( "secion_length:%x  program_info_length:%x  pos = %d", secion_length, program_info_length, pos );
			for (; pos <= (secion_length + 2) - 4; )
			{
				uint8_t stream_type = data[pos];

				uint32_t elmentary_pid = ( (data[pos + 1] << 8) | (data[pos + 2] & 0x1FFF) );


				uint32_t es_info_lenght = (data[pos + 3] & 0x0F) << 8 | data[pos + 4];

				if ( es_info_lenght != 0 )
				{
					uint32_t description = data[pos + 5];

					int len;

					for ( len = 2; len <= es_info_lenght; len++ )
					{
						description = description << 8 | data[pos + 4 + len];
					}

					pos += 5;
				}

				DEBUG( "stream_type:%x elmentary_pid:%x es_info_lenght:%x ", stream_type, elmentary_pid, es_info_lenght );
			}
#endif

#if 0
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
		}
		--count;

		b_ok = ReadPacket( i_fd, data );
		if ( parse_ts_id.i_parse_status == 0x02 )
			break;
		else if ( count <= 0 )
			break;
	}

out:
	if ( p_dvbpsi )
	{
		dvbpsi_pmt_detach( p_dvbpsi );
		dvbpsi_delete( p_dvbpsi );
	}
	close( i_fd );
	parse_ts_id.i_parse_status = 0;
	return(0);
}


