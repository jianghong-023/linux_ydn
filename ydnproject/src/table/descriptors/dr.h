/*****************************************************************************
 * dr.h
 * Copyright (C) 2001-2010 VideoLAN
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
 *****************************************************************************/

/*!
 * \file <dr.h>
 * \author Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
 * \brief Gather all dr_*.h into one.
 *
 * Gathers all dr_*.h into one. Use this header if you need a lot of them.
 */

#ifndef _DVBPSI_DR_H_
#define _DVBPSI_DR_H_

#include "dr_0b.h"

#include "dr_0e.h"

#include "dr_52.h"

#include "dr_59.h"


#else
#error "Multiple inclusions of dr.h"
#endif
