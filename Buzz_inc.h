/*
    Copyright (C) 2005 Michael K. McCarty & Fritz Bronner

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "race.h"		// autoconf header
#include "int_types.h"
#include "pace.h"

#ifndef ui8
typedef uint8_t ui8;
#endif

#ifndef i8
typedef int8_t i8;
#endif

#ifndef ui16
typedef uint16_t ui16;
#endif

#ifndef i16
typedef int16_t i16;
#endif

#ifndef ui32
typedef uint32_t ui32;
#endif

#ifndef i32
typedef int32_t i32;
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>

// EMS Includes
#include "sv_lib.h"

#pragma pack(1)

int never_fail;

#include "data.h"     // main data structures
#include "proto.h"    // prototypes and general defines
#include "music.h"    // defines for music names

#include "endianness.h"

#define random brandom
#undef NULL
#define NULL 0
