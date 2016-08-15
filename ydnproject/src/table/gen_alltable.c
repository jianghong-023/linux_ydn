/*****************************************************************************
* gen_pat.c: PAT generator
*----------------------------------------------------------------------------
* Copyright (C) 2001-2011 VideoLAN
* $Id: gen_pat.c,v 1.3 2002/10/07 14:15:14 sam Exp $
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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include  "dvbpsi.h"
#include  "psi.h"
#include  "pat.h"
#include  "pmt.h"
#include  "nit.h"
#include  "sdt.h"
#include  "eit.h"
#include  "section_put.h"
#include "gen_alltable.h"


usr_cfg_dvb_t	*usr_dvb_t;
usr_def_des_t	usr_def_t;


static int generate_pat_table_t( usr_cfg_pat_t *pat_cfg );


static int generate_pmt_table_t( usr_cfg_pmt_t *pmt_cfg, uint16_t pmt_pid );


static int generate_sdt_table_t( usr_cfg_sdt_t *sdt_cfg );


static int generate_nit_table_t( usr_cfg_nit_t *nit_cfg, usr_cfg_dvb_t* dvb_des_cfg, usr_def_des_t* usr_des_cfg );


static int generate_eit_table_t();


/*****************************************************************************
*
*****************************************************************************/
static int  generate_pat_table_t( usr_cfg_pat_t *pat_cfg )
{
	uint8_t			packet[188];
	dvbpsi_pat_t		pat;
	dvbpsi_psi_section_t	* p_section1;

/*	int i; */

	dvbpsi_t *p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		return(1);

	/* PAT generation */
	dvbpsi_pat_init( &pat, pat_cfg->ts_id, pat_cfg->pat_version, pat_cfg->pat_current_next );
	dvbpsi_pat_program_add( &pat, pat_cfg->program_number, pat_cfg->pat_pid );
	p_section1 = dvbpsi_pat_sections_generate( p_dvbpsi, &pat, pat_cfg->max_num_program );

	/* TS packets generation */
	packet[0]	= 0x47;
	packet[1]	= packet[2] = 0x00;
	packet[3]	= 0x00;

	writePSI( packet, p_section1, PAT_TABLE );

	dvbpsi_DeletePSISections( p_section1 );

	dvbpsi_pat_empty( &pat );

	dvbpsi_delete( p_dvbpsi );
	return(0);
}


static int generate_pmt_table_t( usr_cfg_pmt_t *pmt_cfg, uint16_t pmt_pid )
{
	uint8_t packet[188];

	dvbpsi_pmt_t pmt;

	dvbpsi_psi_section_t* p_section1;


	dvbpsi_t *p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		return(1);

	/* PMT generation */
	dvbpsi_pmt_init( &pmt, pmt_cfg->program_number, pmt_cfg->pmt_version, pmt_cfg->pmt_current_next,
			 pmt_cfg->pcr_pid );

	dvbpsi_pmt_es_add( &pmt, pmt_cfg->video_type, pmt_cfg->video_pid );
	dvbpsi_pmt_es_add( &pmt, pmt_cfg->audio_type, pmt_cfg->audio_pid[0] );


	p_section1 = dvbpsi_pmt_sections_generate( p_dvbpsi, &pmt );

#if 0
	DEBUG( "program_number =%x ", pmt_cfg->program_number );
	DEBUG( "pmt_version =%x ", pmt_cfg->pmt_version );
	DEBUG( "pcr_pid =%x ", pmt_cfg->pcr_pid );
	DEBUG( "video_pid =%x ", pmt_cfg->video_pid );
	DEBUG( "audio_pid =%x ", pmt_cfg->audio_pid[0] );
	DEBUG( "pmt_pid[1]=%x", pmt_pid );

#endif

	packet[0]	= 0x47;
	packet[1]	= GET_HEX( pmt_pid, 2 );
	packet[2]	= GET_HEX( pmt_pid, 1 );
	packet[3]	= 0x00;

	writePSI( packet, p_section1, PMT_TABLE );


	dvbpsi_DeletePSISections( p_section1 );


	dvbpsi_pmt_empty( &pmt );
	dvbpsi_delete( p_dvbpsi );
	return(0);
}


static int generate_sdt_table_t( usr_cfg_sdt_t *sdt_cfg )
{
	uint8_t			packet[188];
	dvbpsi_sdt_t		sdt;
	dvbpsi_psi_section_t	* p_section1;

	dvbpsi_sdt_service_t *p_getservice;
	/* dvbpsi_descriptor_t   *p_descriptor; */

	dvbpsi_t *p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		return(1);

	/* sdT generation */

	dvbpsi_sdt_init( &sdt, sdt_cfg->sdt_table_id, sdt_cfg->sdt_extension, sdt_cfg->sdt_version,
			 sdt_cfg->sdt_current_next, sdt_cfg->network_id );
	p_getservice = dvbpsi_sdt_service_add( &sdt, sdt_cfg->program_number, sdt_cfg->sdt_eit_schedule, sdt_cfg->sdt_eit_present,
					       sdt_cfg->sdt_running_status, sdt_cfg->sdt_free_ca );
	dvbpsi_sdt_service_descriptor_add( p_getservice, sdt_cfg->sdt_descriptor_tag, sdt_cfg->sdt_descriptor_length,
					   sdt_cfg->service_data );
	p_section1 = dvbpsi_sdt_sections_generate( p_dvbpsi, &sdt );


	/* TS packets generation */
	packet[0]	= 0x47;
	packet[1]	= 0x00;
	packet[2]	= 0x11;

	packet[3] = 0x00;

	writePSI( packet, p_section1, SDT_TABLE );

	dvbpsi_DeletePSISections( p_section1 );

	dvbpsi_sdt_empty( &sdt );

	dvbpsi_delete( p_dvbpsi );
	return(0);
}


static int generate_nit_table_t( usr_cfg_nit_t *nit_cfg, usr_cfg_dvb_t* dvb_des_cfg, usr_def_des_t* usr_des_cfg )
{
	uint8_t			packet[188];
	dvbpsi_nit_t		nit;
	dvbpsi_psi_section_t	* p_section1;
	dvbpsi_nit_ts_t		* p_ts;


	dvbpsi_t *p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		return(1);

	/* NIT generation */

	dvbpsi_nit_init( &nit, nit_cfg->nit_table_id, nit_cfg->nit_extension,
			 nit_cfg->network_id, nit_cfg->nit_version, nit_cfg->nit_current_next );        /* ok */
	/*  */
	dvbpsi_nit_descriptor_add( &nit, nit_cfg->network_name_tag, nit_cfg->network_name_length,
				   nit_cfg->network_name );                                             /* ok */
	p_ts = dvbpsi_nit_ts_add( &nit, nit_cfg->ts_id, nit_cfg->network_id );                          /* ok */

	dvbpsi_nit_ts_descriptor_add( p_ts, dvb_des_cfg->dvb_t_descriptor_tag, dvb_des_cfg->dvb_t_descriptor_length,
				      dvb_des_cfg->dvb_t_descriptor );

	/* nit */

	dvbpsi_nit_ts_descriptor_add( p_ts, usr_des_cfg->user_defined_descriptor_tag, usr_des_cfg->user_defined_descriptor_length,
				      usr_des_cfg->user_defined_descriptor );

	p_section1 = dvbpsi_nit_sections_generate( p_dvbpsi, &nit, nit_cfg->nit_table_id );

#if 0

	DEBUG( "nit_table_id %d", nit_cfg->nit_table_id );
	DEBUG( "nit_cfg->nit_extension = %d ", nit_cfg->nit_extension );
	DEBUG( "network_id = %x ", nit_cfg->network_id );
	DEBUG( "nit_cfg->nit_version = %x ", nit_cfg->nit_version );
	DEBUG( "nit_cfg->nit_current_next = %x ", nit_cfg->nit_current_next );
	DEBUG( "nit_cfg->network_name_tag = %x ", nit_cfg->network_name_tag );
	DEBUG( "nit_cfg->network_name_length = %d ", nit_cfg->network_name_length );
	DEBUG( "nit_cfg->network_name = %s ", nit_cfg->network_name );
	DEBUG( "nit_cfg->network_name_tag = %x ", nit_cfg->network_name_tag );
	DEBUG( "nit_cfg->network_name_length=%d", nit_cfg->network_name_length );
	DEBUG( "nit_cfg->network_name=%s", nit_cfg->network_name );
	DEBUG( "nit_cfg->ts_id = %x ", nit_cfg->ts_id );
	DEBUG( "nit_cfg->network_id = %x ", nit_cfg->network_id );


	DEBUG( "dvb_t_descriptor_tag %x", dvb_des_cfg->dvb_t_descriptor_tag );
	DEBUG( "dvb_t_descriptor_length %d", dvb_des_cfg->dvb_t_descriptor_length );
	DEBUG( "dvb_t_descriptor %x", dvb_des_cfg->dvb_t_descriptor[0] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[1] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[2] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[3] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[4] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[5] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[6] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[7] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[8] );
	DEBUG( "%x", dvb_des_cfg->dvb_t_descriptor[9] );
	DEBUG( "dvb_t_descriptor[10] %x", dvb_des_cfg->dvb_t_descriptor[10] );


	DEBUG( "usr_des_cfg->user_defined_descriptor_tag= %x ", usr_des_cfg->user_defined_descriptor_tag );
	DEBUG( "usr_des_cfg->user_defined_descriptor_length= %x ", usr_des_cfg->user_defined_descriptor_length );

#endif
	/* TS packets generation */
	packet[0]	= 0x47;
	packet[1]	= 0x00;
	packet[2]	= 0x10;
	packet[3]	= 0x00;

	writePSI( packet, p_section1, NIT_TABLE );

	dvbpsi_DeletePSISections( p_section1 );

	dvbpsi_nit_empty( &nit );

	dvbpsi_delete( p_dvbpsi );
	return(0);
}


void set_pat_table( uint16_t program_number, uint16_t pmt_pid, uint8_t pat_version, uint16_t ts_id )
{
	usr_cfg_pat_t *usr_pat_t;
	usr_pat_t = calloc( 1, sizeof(usr_cfg_pat_t) );
	if ( !usr_pat_t )
		return;
	DEBUG("PMT PID:%d",pmt_pid);
	usr_pat_t->max_num_program	= MAX_NUM_PROGRAM;
	usr_pat_t->pat_current_next	= PAT_CURRENT_NEXT;
	usr_pat_t->pat_pid		= pmt_pid;
	usr_pat_t->pat_version		= pat_version;
	usr_pat_t->program_number	= program_number;
	usr_pat_t->ts_id		= ts_id;

	generate_pat_table_t( usr_pat_t );

	free( usr_pat_t );
}


void set_dvb_des( uint32_t centre_frequency, uint8_t bandwidth,
		  uint8_t constellation, uint8_t code_rat_HP_stream,
		  uint8_t code_rat_LP_stream, uint8_t guard_interval,
		  uint8_t transmission_mode
		  )
{
	uint8_t priority		= 0x0;          /* 1bit */
	uint8_t Time_Slicing_indicator	= 0x0;          /* 1bit */
	uint8_t MPE_FEC_indicator	= 0x0;          /* 1bit */
	uint8_t reserved_future_use1	= 0x0;          /* 2bit */
	uint8_t hierarchy_information	= 0x0;          /* NO 3bit */


	uint8_t		other_frequency_flag	= 0x0;  /* NO 1bit */
	uint32_t	reserved_future_use2	= 0x00000000;
	usr_dvb_t				= (usr_cfg_dvb_t *) calloc( 1, sizeof(usr_cfg_dvb_t) );
	usr_dvb_t->dvb_t_descriptor_tag		= DVB_DESCRIPTOR_TAG;
	usr_dvb_t->dvb_t_descriptor_length	= DVB_SCRIPTOR_LENGHT;
	usr_dvb_t->dvb_t_descriptor[0]		= centre_frequency >> 24;
	usr_dvb_t->dvb_t_descriptor[1]		= centre_frequency >> 16;
	usr_dvb_t->dvb_t_descriptor[2]		= centre_frequency >> 8;
	usr_dvb_t->dvb_t_descriptor[3]		= centre_frequency;
	usr_dvb_t->dvb_t_descriptor[4]		= ( (bandwidth << 5) & 0xe0) | ( (priority << 4) & 0x10) | ( (Time_Slicing_indicator << 3) & 0x08) | ( (MPE_FEC_indicator << 2) & 0x04) | reserved_future_use1;
	usr_dvb_t->dvb_t_descriptor[5]		= ( (constellation << 6) & 0xc0) | ( (hierarchy_information << 3) & 0x38) | code_rat_HP_stream;
	usr_dvb_t->dvb_t_descriptor[6]		= ( (code_rat_LP_stream << 5) & 0xe0) | ( (guard_interval << 3) & 0x18) | ( (transmission_mode << 1) & 0x06) | other_frequency_flag;
	usr_dvb_t->dvb_t_descriptor[7]		= reserved_future_use2 >> 24;
	usr_dvb_t->dvb_t_descriptor[8]		= reserved_future_use2 >> 16;
	usr_dvb_t->dvb_t_descriptor[9]		= reserved_future_use2 >> 8;
	usr_dvb_t->dvb_t_descriptor[10]		= reserved_future_use2;
#if 0
	DEBUG( "usr_dvb_t->dvb_t_descriptor[0] %x", usr_dvb_t->dvb_t_descriptor[0] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[1] %x", usr_dvb_t->dvb_t_descriptor[1] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[2] %x", usr_dvb_t->dvb_t_descriptor[2] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[3] %x", usr_dvb_t->dvb_t_descriptor[3] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[4] %x", usr_dvb_t->dvb_t_descriptor[4] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[5] %x", usr_dvb_t->dvb_t_descriptor[5] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[6] %x", usr_dvb_t->dvb_t_descriptor[6] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[7] %x", usr_dvb_t->dvb_t_descriptor[7] );
	DEBUG( "usr_dvb_t->dvb_t_descriptor[8] %x", usr_dvb_t->dvb_t_descriptor[8] );

#endif
}


void set_usr_des( uint8_t * usedescriptoe
		  , uint8_t deslength ,uint8_t program_number,uint16_t lcn_mode)
{
	uint16_t lcn = 0x0001;
	uint8_t channel_list_id = 0x00;  
	uint8_t channel_list_name_length = 0x00;
	uint32_t country_code = 0x00000000;
	uint8_t NorDigV2_descriptor_length = 0x04;
#if 0
	usr_def_t.user_defined_descriptor_tag = USER_DEFINE_DESC_TAG;
	memset( usr_def_t.user_defined_descriptor, 0, 8 );
	usr_def_t.user_defined_descriptor[0]		= usedescriptoe[0];
	usr_def_t.user_defined_descriptor[1]		= usedescriptoe[1];
	usr_def_t.user_defined_descriptor[2]		= usedescriptoe[2];
	usr_def_t.user_defined_descriptor[3]		= usedescriptoe[3];
	usr_def_t.user_defined_descriptor_length	= deslength;
#endif 	

	if(lcn_mode > 0x02)
		lcn_mode = 0x02;

 if(lcn_mode==0x00)	// European
  {
  	usr_def_t.user_defined_descriptor_tag = USER_DEFINE_DESC_TAG;
  	usr_def_t.user_defined_descriptor_length = 4;
    usr_def_t.user_defined_descriptor[0] = (program_number>>8)&0xff;
  	usr_def_t.user_defined_descriptor[1] = program_number&0xff;
  	usr_def_t.user_defined_descriptor[2] = 0xfc|(lcn>>8);
  	usr_def_t.user_defined_descriptor[3] = 0xff&lcn;

	DEBUG("European 0x00");
  }
  else if(lcn_mode==0x01) //NorDig V1
  {
  	usr_def_t.user_defined_descriptor_tag = USER_DEFINE_DESC_TAG;
  	usr_def_t.user_defined_descriptor_length = 4;
    usr_def_t.user_defined_descriptor[0] = (program_number>>8)&0xff;
  	usr_def_t.user_defined_descriptor[1] = program_number&0xff;
  	usr_def_t.user_defined_descriptor[2] = 0xc0|(lcn>>8);
  	usr_def_t.user_defined_descriptor[3] = 0xff&lcn;
	DEBUG("NorDig V1 0x01");
  }	
  else if(lcn_mode==0x02) //NorDig V2
  {
  	usr_def_t.user_defined_descriptor_tag = 0x87;
  	usr_def_t.user_defined_descriptor_length = 10;
    usr_def_t.user_defined_descriptor[0] = channel_list_id;
  	usr_def_t.user_defined_descriptor[1] = channel_list_name_length;
  	usr_def_t.user_defined_descriptor[2] = (country_code>>16)&0xff;
  	usr_def_t.user_defined_descriptor[3] = (country_code>>8)&0xff;
  	usr_def_t.user_defined_descriptor[4] = country_code&0xff;
  	usr_def_t.user_defined_descriptor[5] = NorDigV2_descriptor_length;
  	usr_def_t.user_defined_descriptor[6] = (program_number>>8)&0xff;
  	usr_def_t.user_defined_descriptor[7] = program_number&0xff;
  	usr_def_t.user_defined_descriptor[8] = 0xfc|(lcn>>8);
  	usr_def_t.user_defined_descriptor[9] = 0xff&lcn;
	DEBUG("NorDig V1 0x02");
  }	

	
}


void set_nit_table( uint16_t network_id, uint8_t *network_name,
		    uint8_t network_name_length, uint8_t nit_table_id,
		    uint8_t nit_version, uint16_t ts_id )
{
	usr_cfg_nit_t *usr_nit_t;
	usr_nit_t = calloc( 1, sizeof(usr_cfg_nit_t) );
	if ( !usr_nit_t )
		return;

	usr_nit_t->network_id		= network_id;
	usr_nit_t->network_name		= (uint8_t *) strdup( (char *) network_name );
	usr_nit_t->network_name_length	= network_name_length;
	usr_nit_t->nit_table_id		= nit_table_id;
	usr_nit_t->nit_version		= nit_version;
	usr_nit_t->ts_id		= ts_id;
	usr_nit_t->network_name_tag	= NETWORK_NAME_TAG;
	usr_nit_t->nit_current_next	= NIT_CURRENT_NEXT;
	usr_nit_t->nit_extension	= NIT_EXTENSION;

	generate_nit_table_t( usr_nit_t, usr_dvb_t, &usr_def_t );

	free( usr_nit_t );
}


void set_sdt_table( uint16_t network_id, uint16_t program_number,
		    uint8_t sdt_descriptor_length, uint8_t sdt_table_id,
		    uint8_t sdt_version, uint8_t   *service_data )
{
	usr_cfg_sdt_t *usr_sdt_t;
	usr_sdt_t = calloc( 1, sizeof(usr_cfg_sdt_t) );
	if ( !usr_sdt_t )
		return;

	usr_sdt_t->network_id			= network_id;
	usr_sdt_t->program_number		= program_number;
	usr_sdt_t->sdt_descriptor_length	= sdt_descriptor_length;
	usr_sdt_t->sdt_table_id			= sdt_table_id;
	usr_sdt_t->sdt_version			= sdt_version;
	usr_sdt_t->service_data			= (uint8_t *) strdup( (char *) service_data );

	usr_sdt_t->sdt_current_next	= SDT_CURRENT_NEXT;
	usr_sdt_t->sdt_descriptor_tag	= SDT_DESCRIPTOR_TAG;
	usr_sdt_t->sdt_eit_present	= SDT_EIT_PRESENT;
	usr_sdt_t->sdt_eit_schedule	= SDT_EIT_SCHEDULE;
	usr_sdt_t->sdt_extension	= SDT_EXTENSION;
	usr_sdt_t->sdt_free_ca		= SDT_FREE_CA;
	usr_sdt_t->sdt_running_status	= SDT_RUNNING_STATUS;

	generate_sdt_table_t( usr_sdt_t );
	free( usr_sdt_t->service_data );
	free( usr_sdt_t );
}


void set_pmt_table( uint16_t audio_pid, uint16_t video_pid,
		    uint16_t pcr_pid, uint16_t program_number, uint8_t pmt_version, uint16_t pmt_pid )
{
	usr_cfg_pmt_t *usr_pmt_t;
	usr_pmt_t = calloc( 1, sizeof(usr_cfg_pmt_t) );
	if ( !usr_pmt_t )
		return;
DEBUG("PMT PID:%d",pmt_pid);
	usr_pmt_t->audio_pid[0]		= audio_pid;
	usr_pmt_t->audio_pid[1]		= audio_pid;
	usr_pmt_t->pcr_pid		= pcr_pid;
	usr_pmt_t->pmt_version		= pmt_version;
	usr_pmt_t->program_number	= program_number;
	usr_pmt_t->video_pid		= video_pid;

	usr_pmt_t->video_type		= VIDEO_TYPE;
	usr_pmt_t->pmt_current_next	= PMT_CURRENT_NEXT;
	usr_pmt_t->audio_type		= AUDIO_TYPE;/* = 0x1b */

	generate_pmt_table_t( usr_pmt_t, pmt_pid );

	free( usr_pmt_t );
}


void set_eit_table( usr_cfg_eit_t *cfg_eit_t )
{
	generate_eit_table_t( cfg_eit_t );
}


static int generate_eit_table_t( usr_cfg_eit_t *cfg_eit_t )
{
	uint8_t packet_eit_1[188];
	uint8_t packet_eit_2[188];

	dvbpsi_eit_t		eit;
	dvbpsi_psi_section_t	* p_section1_eit;
	dvbpsi_eit_event_t	* p_event_1;
	dvbpsi_psi_section_t	* p_section2_eit;
	dvbpsi_eit_event_t	* p_event_2;
	uint16_t		L_1;

	dvbpsi_t *p_dvbpsi = dvbpsi_new( &message, DVBPSI_MSG_DEBUG );
	if ( p_dvbpsi == NULL )
		return(1);

	uint8_t eit_last_section_number = 1;
	uint8_t eit_last_table_id	= 0x4e;
	bool	eit_private_indicator	= 0;

	uint8_t		eit_section_number_1		= 0;
	uint8_t		eit_table_id_1			= 0x4e;
	bool		eit_current_next_1		= 1;
	uint8_t		eit_running_status_1		= 4;
	bool		eit_free_ca_1			= 0;
	uint16_t	eit_event_descriptor_length_1	= 15;
	uint8_t		eit_descriptor_tag_1		= 0x4d;
	uint8_t		eit_descriptor_length_1		= 13;
	uint8_t		eit_version_1			= 23;
	uint16_t	ts_id				= TS_ID;
	uint16_t	eit_event_id_1			= 0x0000;
	
	bool	eit_current_next_2	= 1;
	uint8_t eit_table_id_2		= 0x4e;
	uint8_t eit_version_2		= 24;
	
	uint8_t		eit_section_number_2		= 1;
	uint16_t	network_id			= 0x0001;
	uint8_t		eit_running_status_2		= 1;
	bool		eit_free_ca_2			= 0;
	uint16_t	eit_event_descriptor_length_2	= 15;
	uint8_t		eit_descriptor_tag_2		= 0x4d;
	uint8_t		eit_descriptor_length_2		= 13;
	uint16_t	eit_event_id_2			= 0x0001;

#if 0
	DEBUG( "nit_private_data =%s", cfg_eit_t->nit_private_data );
	DEBUG( "eit_now_startyear =%s", cfg_eit_t->eit_now_startyear );
	DEBUG( "eit_now_starttime =%s", cfg_eit_t->eit_now_starttime );
	DEBUG( "eit_now_duration  =%s", cfg_eit_t->eit_now_duration );
	DEBUG( "encoder_eit_now_eventname  =%s", cfg_eit_t->eit_now_eventname );

	DEBUG( "eit_next_startyear  =%s", cfg_eit_t->eit_next_startyear );
	DEBUG( "eit_next_starttime  =%s", cfg_eit_t->eit_next_starttime );
	DEBUG( "eit_next_duration  =%s", cfg_eit_t->eit_next_duration );
	DEBUG( "eit_next_eventname  =%s", cfg_eit_t->eit_next_eventname );


	/* ---------------------------- */


#endif

	uint16_t	Day_1	= (cfg_eit_t->eit_now_startyear[6] - 0x30) * 10 + cfg_eit_t->eit_now_startyear[7] - 0x30;
	uint16_t	Month_1 = (cfg_eit_t->eit_now_startyear[4] - 0x30) * 10 + cfg_eit_t->eit_now_startyear[5] - 0x30;
	uint16_t	Year_1	= (cfg_eit_t->eit_now_startyear[0] - 0x30) * 1000 + (cfg_eit_t->eit_now_startyear[1] - 0x30) * 100 + (cfg_eit_t->eit_now_startyear[2] - 0x30) * 10 + cfg_eit_t->eit_now_startyear[3] - 0x30 - 1900;

	/* 第一次时间转换 */
	uint32_t Time_1 = ( ( (cfg_eit_t->eit_now_starttime[0] - 0x30) << 20) & 0x00f00000) | ( ( (cfg_eit_t->eit_now_starttime[1] - 0x30) << 16) & 0x000f0000) | ( ( (cfg_eit_t->eit_now_starttime[2] - 0x30) << 12) & 0x0000f000)
			  | ( ( (cfg_eit_t->eit_now_starttime[3] - 0x30) << 8) & 0x00000f00) | ( ( (cfg_eit_t->eit_now_starttime[4] - 0x30) << 4) & 0x000000f0) | ( (cfg_eit_t->eit_now_starttime[5] - 0x30) & 0x0000000f);
	if ( Month_1 == 1 || Month_1 == 2 )
		L_1 = 1;
	else L_1 = 0;
	uint64_t	MJD_1			= 14956 + Day_1 + (int) ( (Year_1 - L_1) * 365.25) + (int) ( (Month_1 + 1 + L_1 * 12) * 30.6001);
	uint64_t	eit_start_time_1	= ( (MJD_1 << 24) & 0x000000ffff000000) | (Time_1 & 0x0000000000ffffff);
	uint32_t	eit_duration_1		= ( ( (cfg_eit_t->eit_now_duration[0] - 0x30) << 20) & 0x00f00000) | ( ( (cfg_eit_t->eit_now_duration[1] - 0x30) << 16) & 0x000f0000)
						  | ( ( (cfg_eit_t->eit_now_duration[2] - 0x30) << 12) & 0x0000f000) | ( ( (cfg_eit_t->eit_now_duration[3] - 0x30) << 8) & 0x00000f00)
						  | ( ( (cfg_eit_t->eit_now_duration[4] - 0x30) << 4) & 0x000000f0) | ( (cfg_eit_t->eit_now_duration[5] - 0x30) & 0x0000000f);


	/* 第二次时间转换 */
	uint16_t	Day_2	= (cfg_eit_t->eit_next_startyear[6] - 0x30) * 10 + cfg_eit_t->eit_next_startyear[7] - 0x30;
	uint16_t	Month_2 = (cfg_eit_t->eit_next_startyear[4] - 0x30) * 10 + cfg_eit_t->eit_next_startyear[5] - 0x30;
	uint16_t	Year_2	= (cfg_eit_t->eit_next_startyear[0] - 0x30) * 1000 + (cfg_eit_t->eit_next_startyear[1] - 0x30) * 100 + (cfg_eit_t->eit_next_startyear[2] - 0x30) * 10 + cfg_eit_t->eit_next_startyear[3] - 0x30 - 1900;
	uint16_t	L_2;  uint32_t Time_2 = ( ( (cfg_eit_t->eit_next_starttime[0] - 0x30) << 20) & 0x00f00000) | ( ( (cfg_eit_t->eit_next_starttime[1] - 0x30) << 16) & 0x000f0000)
						| ( ( (cfg_eit_t->eit_next_starttime[2] - 0x30) << 12) & 0x0000f000) | ( ( (cfg_eit_t->eit_next_starttime[3] - 0x30) << 8) & 0x00000f00)
						| ( ( (cfg_eit_t->eit_next_starttime[4] - 0x30) << 4) & 0x000000f0) | ( (cfg_eit_t->eit_next_starttime[5] - 0x30) & 0x0000000f);
	if ( Month_2 == 1 || Month_2 == 2 )
		L_2 = 1;
	else L_2 = 0;
	uint64_t	MJD_2			= 14956 + Day_2 + (int) ( (Year_2 - L_2) * 365.25) + (int) ( (Month_2 + 1 + L_2 * 12) * 30.6001);
	uint64_t	eit_start_time_2	= ( (MJD_2 << 24) & 0x000000ffff000000) | (Time_2 & 0x0000000000ffffff);
	uint32_t	eit_duration_2		= ( ( (cfg_eit_t->eit_next_duration[0] - 0x30) << 20) & 0x00f00000) | ( ( (cfg_eit_t->eit_next_duration[1] - 0x30) << 16) & 0x000f0000)
						  | ( ( (cfg_eit_t->eit_next_duration[2] - 0x30) << 12) & 0x0000f000) | ( ( (cfg_eit_t->eit_next_duration[3] - 0x30) << 8) & 0x00000f00)
						  | ( ( (cfg_eit_t->eit_next_duration[4] - 0x30) << 4) & 0x000000f0) | ( (cfg_eit_t->eit_next_duration[5] - 0x30) & 0x0000000f);


	dvbpsi_eit_init( &eit, eit_table_id_1, cfg_eit_t->program_number, eit_version_1, eit_current_next_1, ts_id, network_id, eit_last_section_number, eit_last_table_id );
	p_event_1 = dvbpsi_eit_event_add( &eit, eit_event_id_1, eit_start_time_1, eit_duration_1, eit_running_status_1, eit_free_ca_1, eit_event_descriptor_length_1 );
	dvbpsi_eit_event_descriptor_add( p_event_1, eit_descriptor_tag_1, eit_descriptor_length_1, cfg_eit_t->eit_now_eventname );
	p_section1_eit = dvbpsi_eit_sections_generate( p_dvbpsi, &eit, eit_table_id_1, eit_section_number_1, eit_last_section_number, eit_private_indicator );
	dvbpsi_eit_init( &eit, eit_table_id_2, cfg_eit_t->program_number, eit_version_2, eit_current_next_2, ts_id, network_id, eit_last_section_number, eit_last_table_id );
	p_event_2 = dvbpsi_eit_event_add( &eit, eit_event_id_2, eit_start_time_2, eit_duration_2, eit_running_status_2, eit_free_ca_2, eit_event_descriptor_length_2 );
	dvbpsi_eit_event_descriptor_add( p_event_2, eit_descriptor_tag_2, eit_descriptor_length_2, cfg_eit_t->eit_next_eventname );
	p_section2_eit = dvbpsi_eit_sections_generate( p_dvbpsi, &eit, eit_table_id_2, eit_section_number_2, eit_last_section_number, eit_private_indicator );

	packet_eit_1[0] = 0x47;
	packet_eit_1[1] = 0x00;
	packet_eit_1[2] = 0x12;
	packet_eit_1[3] = 0x00;
	packet_eit_2[0] = 0x47;
	packet_eit_2[1] = 0x00;
	packet_eit_2[2] = 0x12;
	packet_eit_2[3] = 0x00;


	writePSI( packet_eit_1, p_section1_eit, EIT_TABLE );
	writePSI( packet_eit_2, p_section2_eit, EIT_TABLE );
	dvbpsi_DeletePSISections( p_section1_eit );
	dvbpsi_DeletePSISections( p_section2_eit );
	dvbpsi_eit_empty( &eit );

	return(0);
}


