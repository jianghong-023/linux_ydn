#ifndef __SECTION_PUT__H

#define __SECTION_PUT__H

#include "../../include/defineident.h"
#include "../../include/debug.h"

#include "gen_alltable.h"


#include "dvbpsi.h"

void writePSI( uint8_t* p_packet, dvbpsi_psi_section_t* p_section, int type );


void message( dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg );


#endif