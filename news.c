
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
// Interplay's BUZZ ALDRIN's RACE into SPACE
//
// Formerly -=> LiftOff : Race to the Moon :: IBM version MCGA
// Copyright 1991 by Strategic Visions, Inc.
// Designed by Fritz Bronner
// Programmed by Michael K McCarty
//
// NewsCaster Main Files

#include "gamedata.h"
#include "Buzz_inc.h"
#include "externs.h"
#include "macros.h"

double load_news_anim_start;

static int totnews_offset;

#define PHYS_PAGE_OFFSET  0x4000
#define BUFFR_FRAMES 1
#define FIRST_FRAME 0

int bufsize;
int evflag, LOAD_US = 0, LOAD_SV = 0;
int Frame;
ui8 X_Offset, Y_Offset, Depth, Length, MaxFrame, AnimIndex;
ui16 handle0, handle1, handle2, handle3, handle4, handle5;
extern char Option;

SimpleHdr table[99];

struct rNews
{
	i16 offset;
	char chrs;
};

void
NFrame(int x1, int y1, int x2, int y2)
{
	grSetColor(0);
	Box(x1, y1, x2, y2 + 1);
	OutBox(x1, y1, x2, y2);
	grSetColor(3);
	Box(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	Box(x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	InBox(x1 + 3, y1 + 3, x2 - 3, y2 - 3);
}

void
GoNews(char plr)
{
	int i, j;
	struct rNews *list;

	memset(Name, 0x00, sizeof Name);
	memset(buffer, 0x00, 20480);   // clear buffer
	grSetColor(1);

	j = ResolveEvent(plr);
	if (j > 0)
		Data->Events[Data->Count] = j + 99;
	OpenNews(plr, buffer + 6000, (int) Data->Events[Data->Count]);

	Data->P[plr].eCount++;

	DispNews(plr, buffer + 6000, buffer);

	j = 0;
	memset(buffer + 6000, 0x00, 8000);	//clear memory
	list = (struct rNews *) buffer + 6000;
	for (i = 0; i < (int) strlen(buffer); i++)
	{
		if (buffer[i] == 'x')
		{
			list[i].chrs = j;
			list[i].offset = i;
		}
		else
			j++;
	}
}

// Open News Constructs a complete event array.
void
OpenNews(char plr, char *buf, int bud)
{
	int j, size;
	FILE *fp, *gork;
	char old[120];
	int i, len[5];

	size = (plr == 0) ? 232 : 177;
	i = (long) 500 *bud + (long) plr *250;

	//if (plr==1 && bud==22) i=11250L;
	// Event Card Info
	if (plr == 0)
		strcpy(&buf[0], "IN THE NEWS TODAY...x");
	else
		strcpy(&buf[0], "DEVELOPMENTS IN THE WORLD...x");

	fp = sOpen("EVENT.DAT", "rb", 0);
	fseek(fp, i, SEEK_SET);
	bufsize = strlen(buf);
	fread(&buf[bufsize], 249, 1, fp);
	fclose(fp);
	bufsize = strlen(buf);
	buf[bufsize] = 'x';
	//Astronaut info

	fp = sOpen("NEWS.DAT", "rb", 0);
	fread(&len[0], sizeof(len), 1, fp);

	for (i = 0; i < 5; i++)
	{
		Swap32bit(len[i]);
	}

	i = 0;

	for (j = 0; j < Data->P[plr].AstroCount; j++)
		if (Data->P[plr].Pool[j].Special > 0)
			i++;

	bufsize = strlen(buf);
	if (i > 0)
	{
		if (plr == 0)
			strcpy(&buf[bufsize], "xASTRONAUTS IN THE NEWS...x");
		else
			strcpy(&buf[bufsize], "xIN COSMONAUT NEWS...x");
	}

	for (j = 0; j < Data->P[plr].AstroCount; j++)
	{
		if (Data->P[plr].Pool[j].Special > 0)
		{						   // 12 ideas
			bufsize = strlen(buf);
			strcpy(&buf[bufsize], Data->P[plr].Pool[j].Name);
			i = len[0] + len[1] + (sizeof len) + 50 * (Data->P[plr].Pool[j].Special - 1);
			fseek(fp, i, SEEK_SET);
			bufsize = strlen(buf);
			fread(&buf[bufsize], 50, 1, fp);
		}
		if (Data->P[plr].Pool[j].Special == 1
			|| (Data->P[plr].Pool[j].Special > 0 && Data->P[plr].Pool[j].RetReas == 8))
		{						   //13 other things
			i = len[0] + len[1] + len[2] + (sizeof len) + 50 * (Data->P[plr].Pool[j].RetReas - 1);
			if (plr == 1)
				i += len[3];
			fseek(fp, i, SEEK_SET);
			bufsize = strlen(buf);
			fread(&buf[bufsize], 50, 1, fp);
		}
		Data->P[plr].Pool[j].Special = 0;
	}

	if (Data->Year >= 58)
	{
		if (Data->Season == 1 && plr == 0)
			strcpy(&buf[strlen(buf)], "xCHECK INTELLIGENCE, THE CIA REPORTS NEW INFORMATION.x\0");
		else if (Data->Season == 1 && plr == 1)
			strcpy(&buf[strlen(buf)], "xCHECK INTELLIGENCE, THE KGB REPORTS NEW INFORMATION.x\0");
	}

	//-----------------------------------------------------
	//Specs: check tracking station for directors message | 
	//-----------------------------------------------------
#if 1
	if (Option != -1)
	{
		if ((gork = sOpen((Option == 0) ? "SENDR.MSG" : "SENDH.MSG", "rb", 0)) != NULL)
		{
			fread(&old, sizeof(old), 1, gork);
			if (old[0] != 0x00)
			{
				if (Option == 0)
				{
					strcpy(&buf[strlen(buf)], "xCHECK THE TRACKING STATION, THE SOVIETx");
					strcpy(&buf[strlen(buf)], "DIRECTOR HAS SENT A NEW MESSAGE.x\0");
				}
				else if (Option == 1)
				{
					strcpy(&buf[strlen(buf)], "xCHECK THE TRACKING STATION, THE UNITEDx");
					strcpy(&buf[strlen(buf)], "STATES DIRECTOR HAS SENT A NEW MESSAGE.x\0");
				}
			}
			fclose(gork);
		}
	};
#endif

	bufsize = strlen(buf);
	if (Data->P[plr].Plans && 0xff)
		strcpy(&buf[bufsize], "xPLANETARY MISSION UPDATES...x");

	// Past Mission Info
	if (Data->P[plr].Plans & 0x0f)
	{							   // Failures
		if (Data->P[plr].Plans & 0x01)
			strcpy(&buf[strlen(buf)], "MARS FLYBY FAILS!x");
		if (Data->P[plr].Plans & 0x02)
			strcpy(&buf[strlen(buf)], "JUPITER FLYBY FAILS!x");
		if (Data->P[plr].Plans & 0x04)
			strcpy(&buf[strlen(buf)], "SATURN FLYBY FAILS!x");
	}
	if (Data->P[plr].Plans & 0xf0)
	{
		if (Data->P[plr].Plans & 0x10)
			strcpy(&buf[strlen(buf)], "MARS FLYBY SUCCESS!x");
		if (Data->P[plr].Plans & 0x20)
			strcpy(&buf[strlen(buf)], "JUPITER FLYBY SUCCESS!x");
		if (Data->P[plr].Plans & 0x40)
			strcpy(&buf[strlen(buf)], "SATURN FLYBY SUCCESS!x");
	}
	Data->P[plr].Plans = 0;

	// History info
	fseek(fp, sizeof(len), SEEK_SET);
	bufsize = strlen(buf);
	if (plr == 0)
		strcpy(&buf[bufsize], "xALSO IN THE NEWS...x");
	else
		strcpy(&buf[bufsize], "xOTHER EVENTS IN THE NEWS...x");
	bufsize = strlen(buf);

	if (plr == 1)
		fseek(fp, len[0], SEEK_CUR);	// go to start of Soviet news
	if (plr == 0)
		i = ((Data->Year - 57) * 6 + Data->Season * 3 + random(3)) * size;
	else
		i = ((Data->Year - 57) * 4 + Data->Season * 2 + random(2)) * size;
	fseek(fp, i, SEEK_CUR);

	fread(&buf[bufsize], size, 1, fp);
	fclose(fp);
	strcat(buf, "x");
	bufsize = strlen(buf);
	if (plr == 0)
		strcpy(&buf[bufsize], "xAND THAT'S THE NEWS, I'M CARTER WALCRITE.x");
	else
		strcpy(&buf[bufsize], "xTHIS CONCLUDES OUR NEWS, I'M SVETLANA IZVESTIA.x");
}

void
DispNews(char plr, char *src, char *dest)
{
	int i = 0, j = 0, k = 0;

	for (i = 0; i < (int) strlen(src); i++)
	{
		dest[j] = src[i];
		switch (dest[j])
		{
			case 'a':
			case 'd':
			case 'f':
			case 'j':
			case 'i':
				sprintf(&Name[0], "%d", evflag);
				strncpy(&dest[j], &Name[0], strlen(Name));
				j += strlen(Name) - 1;
				break;
			case 'b':
				switch (evflag)
				{
					case 0:
						strcpy(&Name[0], "FIRST");
						break;
					case 1:
						strcpy(&Name[0], "SECOND");
						break;
					case 2:
						strcpy(&Name[0], "THIRD");
						break;
				};
				strncpy(&dest[j], &Name[0], strlen(Name));
				j += strlen(Name) - 1;
				break;
			case 'c':
				strncpy(&dest[j], &Name[0], strlen(Name));
				j += strlen(Name) - 1;
				break;
			case 'e':
				strncpy(&dest[j], &Name[0], strlen(Name));
				j += strlen(Name) - 1;
				break;
			case 'h':
				switch (evflag)
				{
					case 0:
						strcpy(&Name[0], "PRIMARY");
						break;
					case 1:
						strcpy(&Name[0], "SECONDARY");
						break;
					case 2:
						strcpy(&Name[0], "THIRD");
						break;
					default:
						break;
				}
				break;
			case 'g':
				if (plr == 0)
				{
					if (Data->Year <= 59)
						strcpy(&Name[0], "EISENHOWER");
					if (Data->Year >= 60 && Data->Year <= 63)
						strcpy(&Name[0], "KENNEDY");
					if (Data->Year >= 64 && Data->Year <= 67)
						strcpy(&Name[0], "JOHNSON");
					if (Data->Year >= 68 && Data->Year <= 73)
						strcpy(&Name[0], "NIXON");
					if (Data->Year >= 74 && Data->Year <= 75)
						strcpy(&Name[0], "FORD");
					if (Data->Year >= 76 && Data->Year <= 79)
						strcpy(&Name[0], "CARTER");
				};
				if (plr == 1)
				{
					if (Data->Year < 64)
						strcpy(&Name[0], "KHRUSHCHEV");
					if (Data->Year == 64 && Data->Season == 0)
						strcpy(&Name[0], "KHRUSHCHEV");
					if (Data->Year == 64 && Data->Season == 1)
						strcpy(&Name[0], "BREZHNEV");
					if (Data->Year > 65)
						strcpy(&Name[0], "BREZHNEV");
				};
				strncpy(&dest[j], &Name[0], strlen(Name));
				j += strlen(Name) - 1;
				break;
			case 'm':
				strncpy(&dest[j], Data->P[plr].Name, strlen(Data->P[plr].Name));
				j += strlen(Data->P[plr].Name) - 1;
				break;
			case 'x':
				k = 0;
				break;
			case ' ':
				if (k > 34)
				{
					dest[j] = 'x';
					k = 0;
					if (src[i + 1] == ' ')
						i++;
				};
			default:
				break;
		};
		j++;
		k++;
	};
}

FILE *
PreLoadAnim(char plr, char mode)
{
	FILE *fp = NULL;

	gxClearDisplay(0, 0);
	ShBox(49, 54, 262, 122);
	InBox(53, 57, 258, 105);
	InBox(53, 108, 258, 119);
	RectFill(54, 109, 257, 118, 11);
	InBox(56, 60, 255, 72);
	RectFill(57, 61, 254, 71, 7);
	grSetColor(11);
	PrintAt(73, 68, "UPDATING NEWSCASTER ANIMATION");
	grSetColor(11);
	PrintAt(64, 81, "INSTALLING YOUR NEW");
	if (mode == 1)
		PrintAt(64, 90, "BLACK AND WHITE TV SET.");
	else
		PrintAt(64, 90, "COLOR TV SET.");
	grSetColor(9);
	if (plr == 0)
	{
		PrintAt(64, 99, "TYPE: ");
		grSetColor(1);
		PrintAt(0, 0, "U.S. NEWSCASTER");
	}
	else
	{
		PrintAt(64, 99, "TYPE: ");
		grSetColor(1);
		PrintAt(0, 0, "SOVIET NEWSCASTER");
	}
	FadeIn(2, pal, 10, 0, 0);
	if (plr == 1)
	{
		fp = LoadNewsAnim((mode == 0) ? 10 : 11, BUFFR_FRAMES, NULL);	//Angle
		LoadNewsAnim((mode == 0) ? 6 : 7, BUFFR_FRAMES, fp);	//Closing
		LoadNewsAnim((mode == 0) ? 4 : 5, BUFFR_FRAMES, fp);	//Opening
	}
	else
	{
		fp = LoadNewsAnim((mode == 0) ? 9 : 8, BUFFR_FRAMES, NULL);	//Angle
		LoadNewsAnim((mode == 0) ? 3 : 2, BUFFR_FRAMES, fp);	//Closing
		LoadNewsAnim((mode == 0) ? 1 : 0, BUFFR_FRAMES, fp);	//Opening
	};
	FadeOut(2, pal, 10, 0, 0);

	return fp;
}

void
DrawNews(char plr)
{

	gxClearDisplay(0, 0);
	memset(screen, 0xff, 320 * 113);
	pal[767] = pal[766] = pal[765] = 0x00;
	OutBox(0, 0, 319, 113);
	grSetColor(3);
	Box(1, 1, 318, 112);
	Box(2, 2, 317, 111);
	InBox(3, 3, 316, 110);
	ShBox(240, 3, 316, 22);
	RectFill(315, 20, 317, 21, 3);
	RectFill(241, 2, 242, 4, 3);
	IOBox(243, 3, 316, 19);
	grSetColor(1);
	PrintAt(258, 13, "CONTINUE");
	ShBox(0, 115, 319, 199);
	InBox(4, 118, 297, 196);
	RectFill(5, 119, 296, 195, 7 + 3 * plr);
	InBox(301, 118, 315, 196);
	RectFill(302, 119, 314, 195, 0);
	ShBox(303, 120, 313, 156);
	ShBox(303, 158, 313, 194);
	UPArrow(305, 126);
	DNArrow(305, 163);

}

void
DrawNText(char plr, char got)
{
	int xx = 12, yy = 128, i;
	char *buf;

	buf = buffer;
	grSetColor(1);
	for (i = 0; i < got; i++)
	{
		while (*buf != 'x')
			buf++;
		buf++;
		if (strncmp(&buf[0], "ASTRONAUTS IN THE NEWS", 22) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "ALSO IN THE NEWS", 16) == 0)
			grSetColor(12);
		if (strncmp(&buf[0], "IN COSMONAUT NEWS", 17) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "OTHER EVENTS IN THE NEWS", 24) == 0)
			grSetColor(12);
		if (strncmp(&buf[0], "PLANETARY", 9) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "CHECK INTEL", 11) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "CHECK THE TRACKING STATION", 26) == 0)
			grSetColor((plr == 0) ? 9 : 7);
	}

	for (i = 0; i < 8; i++)
	{
		RectFill(5, yy - 7, 296, yy + 1, 7 + 3 * plr);
		grMoveTo(xx, yy);
		if (strncmp(&buf[0], "ASTRONAUTS IN THE NEWS", 22) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "ALSO IN THE NEWS", 16) == 0)
			grSetColor(12);
		if (strncmp(&buf[0], "IN COSMONAUT NEWS", 17) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "OTHER EVENTS IN THE NEWS", 24) == 0)
			grSetColor(12);
		if (strncmp(&buf[0], "PLANETARY", 9) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "AND THAT'S THE NEWS", 19) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "THIS CONCLUDES OUR NEWS", 23) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "CHECK INTEL", 11) == 0)
			grSetColor(11);
		if (strncmp(&buf[0], "CHECK THE TRACKING STATION", 26) == 0)
			grSetColor((plr == 0) ? 9 : 7);

		while (buf[0] != 'x' && buf[0] != NULL)
		{
			DispChr(buf[0]);
			buf++;
		};
		yy += 9;
		buf++;
		if (*buf == NULL)
			i = 9;
	}

}

void
News(char plr)
{
	double last_secs;
	int bline = 0, ctop = 0, i;
	char cYr[5];
	ONEWS oNews;
	char loc = 0;
	ui8 Status = 0, BW = 0;
	FILE *fp = NULL;
	FILE *fout = NULL;

	//: LOAD_US & LOAD_SV  0 None 1 B/W 2 Color
	if (Data->Year <= 63)
	{
		BW = 1;
		if (plr == 0 && LOAD_US == 2)
		{
			LOAD_US = 0;
		};
		if (plr == 1 && LOAD_SV == 2)
		{
			LOAD_SV = 0;
		};
	}
	else
	{
		BW = 0;
		if (plr == 0 && LOAD_US == 1)
		{
			LOAD_US = 0;
		};
		if (plr == 1 && LOAD_SV == 1)
		{
			LOAD_SV = 0;
		};
	};

	///Specs: preload anims
	if (plr == 0 && LOAD_US == 0)
	{
		fp = PreLoadAnim(plr, BW);
		if (BW == 1)
			LOAD_US = 1;
		else
			LOAD_US = 2;
	}
	else if (plr == 1 && LOAD_SV == 0)
	{
		fp = PreLoadAnim(plr, BW);
		if (BW == 1)
			LOAD_SV = 1;
		else
			LOAD_SV = 2;
	}
//  DrawNews(plr);
	GoNews(plr);

#if 1
	if ((plr == 0 && LOAD_US == 0) || (plr == 1 && LOAD_SV == 0))
	{

		sprintf(cYr, "%d", 1900 + Data->Year);
		if (Data->Season == 1)
			DispBig(42 + (BW * 200), 40 - (plr * 4), "FALL", 0, -1);
		else
			DispBig(37 + (BW * 200), 40 - (plr * 4), "SPRING", 0, -1);
		DispBig(48 + (BW * 200), 63 - (4 * plr), &cYr[0], 0, -1);

	};
#endif
	for (i = 0; i < (int) strlen(buffer); i++)
		if (buffer[i] == 'x')
			bline++;
	bline -= 8;

	// File Structure is 84 longs 42 per side 

	fout = sOpen("EVENT.TMP", "w+b", 1);
	fseek(fout, 0, SEEK_END);
	oNews.offset = ftell(fout);
	oNews.size = (strlen(buffer));
	fwrite(buffer, strlen(buffer), 1, fout);
	fseek(fout, (plr * 42 + Data->P[plr].eCount - 1) * (sizeof(struct oldNews)), SEEK_SET);
	fwrite(&oNews, sizeof(struct oldNews), 1, fout);
	fclose(fout);
	PreLoadMusic(M_NEW1970);

	PlayMusic(0);

	if (plr == 1)
		fp = LoadNewsAnim((BW == 0) ? 10 : 11, 69, fp);	// Tom's News kludge
	else if (plr == 0)
		fp = LoadNewsAnim((BW == 0) ? 9 : 8, 69, fp);	// search TCS001 to find code
	loc = 1;
	Status = 0;

	strcpy(IDT, "i002\0");
	WaitForMouseUp();
	while (1)
	{
		key = 0;
		GetMouse_fast();
		last_secs = get_time();
		if (!(loc == 0 && Status == 1))
			NUpdateVoice();
		i = AnimSoundCheck();
		if (Status == 1 || (loc == 3 && i == 1))
			switch (loc)
			{
				case 0:		   //: Angle In
					if (plr == 1)
						LoadNewsAnim((BW == 0) ? 10 : 11, FIRST_FRAME, fp);
					else if (plr == 0)
						LoadNewsAnim((BW == 0) ? 9 : 8, FIRST_FRAME, fp);
#if 1

					sprintf(cYr, "%d", 1900 + Data->Year);
					if (Data->Season == 1)
						DispBig(42 + (BW * 200), 40 - (plr * 4), "FALL", 0, -1);
					else
						DispBig(37 + (BW * 200), 40 - (plr * 4), "SPRING", 0, -1);
					DispBig(48 + (BW * 200), 63 - (4 * plr), &cYr[0], 0, -1);
					VBlank();
#endif
					Status = 0;
					loc++;
					break;
				case 1:		   //: Intro
					if (plr == 1)
						LoadNewsAnim((BW == 0) ? 4 : 5, FIRST_FRAME, fp);
					else if (plr == 0)
						LoadNewsAnim((BW == 0) ? 1 : 0, FIRST_FRAME, fp);
					Status = 0;
					if (AnimIndex == 5)
					{

						RectFill(227, 108, 228, 108, grGetPixel(227, 108));

					}
					PlayNewsAnim(fp);
					PlayNewsAnim(fp);
					PlayNewsAnim(fp);
					PlayNewsAnim(fp);
					NGetVoice(plr, 0);
					PlayVoice();
					loc++;
					break;
				case 2:		   //: Event (sound)
					NGetVoice(plr, Data->Events[Data->Count] + 2);
					PlayVoice();
					Status = 0;
					loc++;
					if (IsChannelMute(AV_SOUND_CHANNEL))
						Status = 1;	//no sound klugge(skip over event picture)
					i = bline;
					ShowEvt(plr, Data->Events[Data->Count]);
					bline = i;
					break;
				case 3:		   //: Close
					if (plr == 1)
						LoadNewsAnim((BW == 0) ? 6 : 7, FIRST_FRAME, fp);
					else if (plr == 0)
						LoadNewsAnim((BW == 0) ? 3 : 2, FIRST_FRAME, fp);
					Status = 0;
					if (plr == 0)
					{			   // This is done to sync with audio
						PlayNewsAnim(fp);
						PlayNewsAnim(fp);
						PlayNewsAnim(fp);
						PlayNewsAnim(fp);
						PlayNewsAnim(fp);
					}
					NGetVoice(plr, 1);
					PlayVoice();
					if (plr == 1)
					{
						bzdelay(170);
					};
					loc++;
					break;
				case 4:		   //: Angle Out
					if (plr == 1)
						LoadNewsAnim((BW == 0) ? 10 : 11, FIRST_FRAME, fp);
					else if (plr == 0)
						LoadNewsAnim((BW == 0) ? 9 : 8, FIRST_FRAME, fp);
#if 1

					sprintf(cYr, "%d", 1900 + Data->Year);
					if (Data->Season == 1)
						DispBig(42 + (BW * 200), 40 - (plr * 4), "FALL", 0, -1);
					else
						DispBig(37 + (BW * 200), 40 - (plr * 4), "SPRING", 0, -1);
					DispBig(48 + (BW * 200), 63 - (4 * plr), &cYr[0], 0, -1);

#endif
					Status = 0;
					loc++;
					break;
				case 5:		   //: Event (no sound)
					Status = 0;
					loc++;
					if (AnimIndex == 9)
					{

						ShBox(240, 3, 316, 22);
						RectFill(315, 20, 317, 21, 3);
						RectFill(241, 2, 242, 4, 3);
						IOBox(243, 3, 316, 19);
						grSetColor(1);
						PrintAt(258, 13, "CONTINUE");

					};
					i = bline;
					ShowEvt(plr, Data->Events[Data->Count]);
					bline = i;
					break;
				default:
					break;
			};

		if (Frame != MaxFrame)
		{
			PlayNewsAnim(fp);
			if (Frame == MaxFrame)
				Status = 1;
		}
		else
		{
			idle_loop_secs(.125);
		}

		//: Repeat News Sequence
		if (key == 'R' && loc == 6)
		{
			KillVoice();
			loc = 0;
			Status = 1;
		}
		if (ctop > 0 && key == 0x4900)
		{						   // Page Up Key
			ctop -= 9;
			if (ctop < 0)
				ctop = 0;
			DrawNText(plr, ctop);
		}
		else if (ctop < bline && key == 0x5100)
		{						   // Page Down Key
			ctop += 9;
			if (ctop > bline)
				ctop = bline;
			DrawNText(plr, ctop);
		}
		else if (ctop > 0 && ((x >= 303 && y > 120 && x <= 313 && y <= 156
					&& mousebuttons > 0) || (key >> 8) == 72))
		{						   // Up Arrow
			InBox(303, 120, 313, 156);
			ctop--;
			DrawNText(plr, ctop);
			OutBox(303, 120, 313, 156);
		}
		else if ((x >= 245 && y >= 5 && x <= 314 && y <= 17
				&& mousebuttons > 0) || (key == K_ENTER))
		{						   // Continue
			InBox(245, 5, 314, 17);
			WaitForMouseUp();
			key = 0;
			i = 0;
			KillMusic();
			KillVoice();
			break;
		}
		else if (ctop < bline && ((x >= 303 && y > 158 && x <= 313
					&& y <= 194 && mousebuttons > 0) || (key >> 8) == 80))
		{						   // Down Arrow
			InBox(303, 158, 313, 194);
			ctop++;
			DrawNText(plr, ctop);
			OutBox(303, 158, 313, 194);
		}
//   gr_sync ();
	};
	if (fp)
		fclose(fp);
}

void
AIEvent(char plr)
{
	ResolveEvent(plr);
}

// ResolveEvent seems to set a flag in the BadCard array
// and return the index into that array
char
ResolveEvent(char plr)
{
	int bad, ctr = 0;

	bad = REvent(plr);
	if (bad)
	{
		// News event was bad, find an open slot to record the bad event
		do
		{
			ctr++;
			bad = random(14);
			if (ctr > 30)
			{
				// After 30 good faith random attemptes to find an open slot
				// reset the BadCard array
				memset(&Data->P[plr].BadCard[0], 0x00, sizeof(Data->P[plr].BadCard));
				ctr = 0;
			}
		} while (Data->P[plr].BadCard[bad] != 0);

		Data->P[plr].BadCard[bad] = 1;
		bad++;
	}
	return bad;					   // zero if card is good
}

void
Breakgrp(char plr)
{
	int i, j, k, l, temp;

	j = plr;
	if (plr == 4)
		for (k = 0; k < 6; k++)
		{
			for (l = 0; l < 8; l++)
			{
				temp = 0;
				if (Data->P[j].Gcnt[k][l] > 0)
				{
					for (i = 0; i < Data->P[j].Gcnt[k][l]; i++)
					{
						if (Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Status == 1
							|| Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Status == 2
							|| Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Status == 3)
							temp++;
					};			   /* for i */
					if (temp > 0)
					{
						for (i = 0; i < Data->P[j].Gcnt[k][l]; i++)
						{
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].oldAssign = 
							  Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Assign;
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Assign = 0;
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Crew = 0;
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Prime = 0;
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Task = 0;
							Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Moved = 0;
							if (Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Special == 0)
								Data->P[j].Pool[Data->P[j].Crew[k][l][i] - 1].Special = 6;
							Data->P[j].Crew[k][l][i] = 0;
						};		   /* for i */
						Data->P[j].Gcnt[k][l] = 0;
					};			   /* it temp */
				};				   /* if Gcnt */
			};					   /* for l */
		};						   /* for k */
}

void
PlayNewsAnim(FILE * fp)
{
	char *buf = NULL;
	int readlen = 0;
	GXHEADER local;
	double delta;
	int frames_realtime;
	int skip_frame;

	skip_frame = 0;
	delta = get_time() - load_news_anim_start;
	frames_realtime = delta * 15;
	if (Frame < frames_realtime)
		skip_frame = 1;

	GV(&local, Depth, Length);

	readlen = table[Frame % MaxFrame].size;
	buf = malloc(readlen);
	fseek(fp, table[Frame].offset + totnews_offset, SEEK_SET);
	fread(buf, 1, readlen, fp);
	RLED_img(buf, &local.vptr[0], readlen, Depth, Length);
	free(buf);

	gxPutImage(&local, gxXOR, X_Offset, Y_Offset, 0);

	DV(&local);
	Frame += 1;

	if (skip_frame == 0)
		idle_loop_secs(1 / 16.0);

	return;
}

FILE *
LoadNewsAnim(ui8 Index, ui8 Mode, FILE * fp)
{
	GXHEADER local;
	int aframe;
	unsigned MAX = 0, TOT = 0;

	struct aCHART
	{
		ui8 frames;
		ui8 x_off;
		ui8 y_off;
		ui8 width;
		ui8 height;
	} aChart;

	SimpleHdrW hdr[12];

	load_news_anim_start = get_time();

	if (Mode == 1)
	{
		switch (Index)
		{
			case 0:
				TOT = 118;
				MAX = 170;
				break;
			case 1:
				TOT = 135;
				MAX = 181;
				break;
			case 2:
				TOT = 58;
				MAX = 170;
				break;
			case 3:
				TOT = 75;
				MAX = 181;
				break;
			case 4:
				TOT = 151;
				MAX = 211;
				break;
			case 5:
				TOT = 167;
				MAX = 227;
				break;
			case 6:
				TOT = 66;
				MAX = 211;
				break;
			case 7:
				TOT = 68;
				MAX = 227;
				break;
			case 8:
				TOT = 0;
				MAX = 170;
				break;
			case 9:
				TOT = 0;
				MAX = 181;
				break;
			case 10:
				TOT = 0;
				MAX = 211;
				break;
			case 11:
				TOT = 0;
				MAX = 227;
				break;
			default:
				break;
		}
	};

	if (!fp)
		fp = sOpen("TOTNEWS.CDR", "rb", 0);
	fseek(fp, 0x00, SEEK_SET);
	fread_SimpleHdrW(hdr, ARRAY_LENGTH(hdr), fp);

	totnews_offset = hdr[Index].offset;

	fseek(fp, hdr[Index].offset, SEEK_SET);
	fread(&aChart, sizeof aChart, 1, fp);
	fread_SimpleHdr(table, aChart.frames, fp);
	fread(&pal[96], 1, 672, fp);

	Frame = 1;
	AnimIndex = Index;
	MaxFrame = aChart.frames;
	X_Offset = aChart.x_off;
	Y_Offset = aChart.y_off;
	Depth = aChart.width;
	Length = aChart.height;

	//Specs: calculate frames for allocation
	aframe = hdr[Index].size / 16384L;
	aframe += 2;				   //buffer between animations in EMS

	if (Mode == 0)
	{

		RectFill(4, 4, 239, 109, 0);
		RectFill(240, 23, 315, 109, 0);

	};
	if (Mode == 0)
		if (Index == 9 || Index == 1 || Index == 3)
		{

			ShBox(240, 3, 316, 22);
			RectFill(315, 20, 317, 21, 3);
			RectFill(241, 2, 242, 4, 3);
			IOBox(243, 3, 316, 19);
			grSetColor(1);
			PrintAt(258, 13, "CONTINUE");

		};
	//Specs: Display Single Frame
	if (Mode == 0)
	{
		gxSetDisplayPalette(pal);
		VBlank();
		fseek(fp, table[0].offset + hdr[Index].offset, SEEK_SET);
		fread(vhptr.vptr, table[0].size, 1, fp);
		gxCreateVirtual(gxCMM, &vhptr2, gxVGA_13, 312, 106);
		RLED_img(vhptr.vptr, vhptr2.vptr, table[0].size, vhptr2.w, vhptr2.h);
		GV(&local, 80, 24);

		gxGetImage(&local, 240, 0, 319, 23, 0);
		gxPutImage(&vhptr2, gxSET, 4, 4, 0);
		VBlank();
		gxPutImage(&local, gxSET, 240, 0, 0);

		gxDestroyVirtual(&vhptr2);
		DV(&local);

	};

	if (Mode == 69)
	{							   // *************** TCS001 my kludge (tom) 3/15/94

		RectFill(0, 0, MAX_X - 1, MAX_Y - 1, 0);	// clears screen

		//  RectFill(4,4,239,109,0);    //clears news anim space
		//  RectFill(240,23,315,109,0);

		gxSetDisplayPalette(pal);
		FadeOut(2, pal, 1, 0, 0);

		// DrawNews() conv
		gxClearDisplay(0, 0);	   // DrawNews() conv
		memset(screen, 0xff, 320 * 113);	// DrawNews() conv
		OutBox(0, 0, 319, 113);	   // DrawNews() conv
		grSetColor(3);			   // DrawNews() conv
		Box(1, 1, 318, 112);
		Box(2, 2, 317, 111);	   // DrawNews() conv
		InBox(3, 3, 316, 110);	   // DrawNews() conv
		ShBox(240, 3, 316, 22);	   // DrawNews() conv
		RectFill(315, 20, 317, 21, 3);	// DrawNews() conv
		RectFill(241, 2, 242, 4, 3);	// DrawNews() conv
		IOBox(243, 3, 316, 19);	   // DrawNews() conv
		grSetColor(1);
		PrintAt(258, 13, "CONTINUE");	// DrawNews() conv
		ShBox(0, 115, 319, 199);   // DrawNews() conv
		InBox(4, 118, 297, 196);   // DrawNews() conv
		if (Index == 9 || Index == 8)
			RectFill(5, 119, 296, 195, 7);	// If US
		else
			RectFill(5, 119, 296, 195, 10);	// else USSR

		InBox(301, 118, 315, 196);
		RectFill(302, 119, 314, 195, 0);	// DrawNews() conv
		ShBox(303, 120, 313, 156);
		ShBox(303, 158, 313, 194); // DrawNews() conv
		UPArrow(305, 126);
		DNArrow(305, 163);		   // DrawNews() conv

		if (Index == 9 || Index == 8)
			DrawNText(0, 0);	   // if US
		else
			DrawNText(1, 0);	   // else USSR      DRAWS THE TEXT

		fseek(fp, table[0].offset + hdr[Index].offset, SEEK_SET);
		fread(vhptr.vptr, table[0].size, 1, fp);
		gxCreateVirtual(gxCMM, &vhptr2, gxVGA_13, 312, 106);
		RLED_img(vhptr.vptr, vhptr2.vptr, table[0].size, vhptr2.w, vhptr2.h);
		GV(&local, 80, 24);
		gxGetImage(&local, 240, 0, 319, 23, 0);
		gxPutImage(&local, gxSET, 240, 0, 0);
		gxPutImage(&vhptr2, gxSET, 4, 4, 0);	// put under setpal
		VBlank();

		if (Index == 10 || Index == 11)	// Russian Whitespace in corner kludge
		{
			RectFill(240, 4, 315, 22, 0);	//clears news anim space
			ShBox(240, 3, 316, 22);
			RectFill(315, 20, 317, 21, 3);
			RectFill(241, 2, 242, 4, 3);
			IOBox(243, 3, 316, 19);
			grSetColor(1);
			PrintAt(258, 13, "CONTINUE");
		}
		else
		{
			ShBox(240, 3, 316, 22);
			RectFill(315, 20, 317, 21, 3);
			RectFill(241, 2, 242, 4, 3);
			IOBox(243, 3, 316, 19);
			grSetColor(1);
			PrintAt(258, 13, "CONTINUE");
		}

		FadeIn(2, pal, 50, 0, 0);

		gxDestroyVirtual(&vhptr2);
		DV(&local);

	}							   // end TCS kludge ************************************

	//fclose(fp);
	return fp;
}

void
ShowEvt(char plr, char crd)
{
	FILE *ffin;
	size_t i;
	struct mm
	{
		long off;
		long size;
	} MM;

	memset(&pal[96], 0, 672);
	VBlank();					   /* SetPal(pal); FIXME */
	if (plr == 0)
	{
		switch (crd)
		{
			case 41:
				crd = 45;
				break;
			case 50:
				crd = 67;
				break;
			case 8:
				crd = 98;
				break;
			case 105:
				crd = 112;
				break;
			case 33:
				crd = 111;
				break;
			case 109:
				crd = 108;
				break;
			default:
				break;
		}
	}
	ffin = sOpen("NEWS.CDR", "rb", 0);
	if (ffin == NULL)
		return;					   // file does not exist
	fseek(ffin, (plr * 115 + crd) * (sizeof MM), SEEK_SET);
	fread(&MM, sizeof MM, 1, ffin);

	Swap32bit(MM.off);
	Swap32bit(MM.size);

	if (MM.off != 0)
	{
		fseek(ffin, MM.off, SEEK_SET);
		fread(&pal[384], 384, 1, ffin);
		fread(vhptr.vptr, (size_t) MM.size, 1, ffin);

		for (i = 0; i < (unsigned int) MM.size; i++)
		{
			if (screen[i] >= 32)
				screen[i] = vhptr.vptr[i];
		}
	}
	VBlank();					   /* SetPal(pal); FIXME */

	fclose(ffin);
}

// EOF


