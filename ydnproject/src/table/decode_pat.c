/*****************************************************************************
* decode_pat.c: PAT decoder example
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


/*
 * #if defined(HAVE_INTTYPES_H)
 * #include <inttypes.h>
 * #elif defined(HAVE_STDINT_H)
 * #include <stdint.h>
 * #endif
 */
#include <stdint.h>
/* The libdvbpsi distribution defines DVBPSI_DIST */

#include "dvbpsi.h"
#include "psi.h"
#include "pat.h"
#include "section_put.h"
#include <pares_psi.h>

struct parse_ts_id parse_ts_id;


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
* DumpPAT
*****************************************************************************/
static void DumpPAT( void* p_zero, dvbpsi_pat_t* p_pat )
{
	dvbpsi_pat_program_t* p_program = p_pat->p_first_program;

	parse_ts_id.i_number	= 0;
	parse_ts_id.i_pmt_pid	= 0;
	DEBUG( "\n" );
	DEBUG( "New PAT\n" );
	DEBUG( "  transport_stream_id : %d\n", p_pat->i_ts_id );
	DEBUG( "  version_number      : %d\n", p_pat->i_version );
	DEBUG( "    | program_number @ [NIT|PMT]_PID\n" );

	while ( p_program )
	{
		if ( p_program->i_number != 0x00 && p_program->i_pid != 0x00 )
		{
			parse_ts_id.i_parse_status	= 0x01;
			parse_ts_id.i_number		= p_program->i_number;
			parse_ts_id.i_pmt_pid		= p_program->i_pid;
		}

		DEBUG( "i_number:%x  i_pmt_pid:%x ", parse_ts_id.i_number, parse_ts_id.i_pmt_pid );

		DEBUG( "    | %14d @ 0x%x (%d)\n",
		       p_program->i_number, p_program->i_pid, p_program->i_pid );
		p_program = p_program->p_next;
	}


	DEBUG( "  active              : %d\n", p_pat->b_current_next );
	dvbpsi_pat_delete( p_pat );
}


/*
 * static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
 * {
 *    switch(level)
 *    {
 *        case DVBPSI_MSG_ERROR: fDEBUG(stderr, "Error: "); break;
 *        case DVBPSI_MSG_WARN:  fDEBUG(stderr, "Warning: "); break;
 *        case DVBPSI_MSG_DEBUG: fDEBUG(stderr, "Debug: "); break;
 *        default: / * do nothing * /
 *            return;
 *    }
 *    fDEBUG(stderr, "%s\n", msg);
 * }
 */


/*****************************************************************************
* main
*****************************************************************************/
/* int main(int i_argc, char* pa_argv[]) */
int pat_parse_enter( char *file )
{
	int		i_fd;
	uint8_t		data[188];
	dvbpsi_t	*p_dvbpsi;
	bool		b_ok;


	i_fd = open( file, O_RDONLY, S_IRUSR | S_IWUSR );
	if ( i_fd < 0 )
		return(1);

	p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		goto out;

	if ( !dvbpsi_pat_attach( p_dvbpsi, DumpPAT, NULL ) )
		goto out;

	b_ok = ReadPacket( i_fd, data );
	int count = 1024;
	while ( b_ok )
	{
		uint16_t i_pid = ( (uint16_t) (data[1] & 0x1f) << 8) + data[2];
		if ( i_pid == 0x0 )
			dvbpsi_packet_push( p_dvbpsi, data );
		--count;
		b_ok = ReadPacket( i_fd, data );
		if ( parse_ts_id.i_parse_status == 0x01 )
			break;
		else if ( count <= 0 )
			break;
	}

out:
	if ( p_dvbpsi )
	{
		dvbpsi_pat_detach( p_dvbpsi );
		dvbpsi_delete( p_dvbpsi );
	}
	close( i_fd );
	parse_ts_id.i_parse_status = 0x00;
	return(0);
}


