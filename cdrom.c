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
// Code for CD-ROM Access
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "cdmaster.h"

#define OKAY 0

// CD-ROM commands
#define CDPLAY 1
#define CDSTOP 2
#define CDSTATUS 3
#define CDRESUME 4
#define CDPAUSE 5
#define CDRESET 6

// CD-ROM errors
#define NOROM   55
#define NODRIVE 66
#define NOCD    77

extern struct cdtable *cdt;

int CDAccess(int drive,int track,char mode) {
   switch(mode) {
      case CDPLAY: return playcdtrack(drive,track,0,-2);
      case CDSTOP: return cdstop(drive);
      case CDSTATUS: return cdstatus(drive); // status&CDISPLAYING  status&CDISPAUSED
      case CDRESUME: return cdresume(drive);
      case CDPAUSE: return cdpause(drive);
      case CDRESET: return cdreset(drive);
      default: return 0;
   }
}


// Determine where the CD-ROM drive is
int AquireDrive(void)
{
   int drive;

	if (!ismscdex()) return NOROM;
	if (!(getnumcdroms())) return NODRIVE;
	if (!(drive= getfirstcdrom())) return NODRIVE;

	cdt=createaudiotoc(drive);
   if (!cdt) return NOCD;
   cdt=buildaudiotoc(drive);
   return drive;
}


