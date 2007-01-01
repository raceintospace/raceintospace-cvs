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

#ifndef ui8
typedef unsigned char ui8;
#endif

#ifndef i8
typedef char i8;
#endif

#ifndef ui16
typedef unsigned short ui16;
#endif

#ifndef i16
typedef short i16;
#endif

#ifndef ui32
typedef unsigned long ui32;
#endif

#ifndef i32
typedef long	i32;
#endif

#include "pace.h"

//#include <process.h>
#include <stdio.h>
#include <string.h>
//#include <dir.h>
#include <ctype.h>
//#include <mem.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <alloc.h>
//#include <conio.h>
#include <fcntl.h>
//#include <dos.h>
//#include <io.h>
#include <time.h>
//#include <bios.h>
#include <math.h>
#include <sys/stat.h>

// AIL includes
//#include <ail.h>
//#include <gen.h>
//#include <soundfx.h>

// EMS Includes
#include "sv_lib.h"

// Gravis Ultrasound Includes  (removed from CD-ROM
//#include "gus\forte.h"
//#include "gus\extern.h"
//#include "gus\gf1proto.h"
//#include "gus\ultraerr.h"

// CD-ROM Includes
// #include "cdmaster.h"
// #include "cdrom.h"

// GENUS includes
//#include <gxLib.h>
//#include <grLib.h>

//  my includes

#pragma pack(1)

#include <data.h>     // main data structures
#include <proto.h>    // prototypes and general defines
#include <music.h>    // defines for music names

#define random brandom
#undef NULL
#define NULL 0
