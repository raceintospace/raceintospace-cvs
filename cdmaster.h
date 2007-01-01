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
#include "cdrom.h"

struct cdtable
{
	struct discinfo di;
	struct trackinfo *ti;
};

/* prototypes */
/* cdmaster.c */ struct cdtable *createaudiotoc(int drive);
/* cdmaster.c */ struct cdtable *buildaudiotoc(int drive);
/* cdmaster.c */ destroyaudiotoc(int drive);
/* cdmaster.c */ struct cdtable *getcdtable(int drive);
/* cdmaster.c */ struct discinfo *getdiscinfotable(int drive);
/* cdmaster.c */ struct trackinfo *gettrackinfotable(int drive);
/* cdmaster.c */ long gettrackframes(int drive, int track);
/* cdmaster.c */ playcdtrack(int drive, int track, int offset, int length);
/* cdmaster.c */ seektotrack(int drive, int track);


