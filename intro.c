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

#include "Buzz_inc.h"
#include "externs.h"
GXHEADER local2;

void Credits(void)
{
  struct CREDIT {
     char page;
     i16 col;
     i16 x,y;
     char Txt[35];
  } *XX;
  int k,i;
  FILE *fin;
  long len[2],length;
  strcpy(IKEY,"i999\0");

  fin=sOpen("FIRST.IMG","rb",0);
  FadeOut(2,pal,30,0,0);
  MouseOff();
  fread(&pal[0],768,1,fin);
  fread(&len[0],4,1,fin);
  fseek(fin,len[0],SEEK_CUR);
  fread(&pal[0],768,1,fin);
  fread(&len[0],4,1,fin);
  fread(vhptr.vptr,len[0],1,fin);
  fclose(fin);

  fin=sOpen("VISION.DAT","rb",0);
  length=filelength(fileno(fin));
  fread(buffer,length,1,fin);
  fclose(fin);

  srand(1);
  XX=(struct CREDIT *) buffer;
  for(i=0;i<length;i++) buffer[i]^=random(256);
  srand(biostime(0,0L));
  MouseOn();

  for (k=0;k<5;k++) {
    MouseOff();
    if (k!=0) FadeOut(2,pal,30,0,0);  // Screen #2
    PCX_D((char *)vhptr.vptr,screen,(unsigned) len[0]);
    memset(&pal[431],0x00,96);
    screen[63999]=0;
    i=0;
    while (XX[i].page<9) {
       if (XX[i].page==k) {
         grSetColor(XX[i].col);
         PrintAt(XX[i].x,XX[i].y,XX[i].Txt);
       }
       i++;
    }
    
    FadeIn(2,pal,30,0,0);
    MouseOn();

    while(1)  {
      GetMouse();
      if (mousebuttons==0) break;
    }
    key=0;
    while (mousebuttons==0 && key==0) {
      GetMouse();
      //UpdateMusic();
    }
    key=0;
  }


#if 0
  MouseOff();
  PCX_D((char *)vhptr.vptr,screen,(unsigned) len[0]);
  memset(&pal[431],0x00,96);
  screen[63999]=0;
  for (i=0;i<35;i++) {
     grSetColor(XX[i+37].col);
     PrintAt(XX[i+37].x,XX[i+37].y,XX[i+37].Txt);
  }
  MouseOn();

  FadeIn(2,pal,30,0,0);
  while(1)  {
    GetMouse();
    if (mousebuttons==0) break;
  }
  key=0;
  while (mousebuttons==0 && key==0) {
    GetMouse();
    UpdateMusic();
  }
  key=0;

#endif

  FadeOut(2,pal,30,0,0);
  memset(screen,0x00,64000);
  strcpy(IKEY,"k000\0");
  return;
}


void MScale(int ssx,int ssy,int ddx,int ddy,char *sbuf,char *dbuf)
{
  unsigned int sx,sy,dx,dy;
  char *dest,*src;
  dest=dbuf;

  for (dy=0;dy<ddy;dy++) {
    sy=(dy*ssy)/ddy;
    for (dx=0;dx<ddx;dx++) {
      sx=(dx*ssx)/ddx;
      src=sbuf+sy*ssx+sx;
      *dest=*src;
      dest++;
    }
  }
  return;
}

void DBlk(int x,int y)
{
  int i,j;

  for (i=0;i<16;i++)
     for (j=0;j<16;j++)
       if (!( (i==0 && j==0) || (i==0 && j==15) || (i==15 && j==0) || (i==15 && j==15)))
         if (ems[320*(y+j)+x+i] > screen[320*(y+j)+x+i]) screen[320*(y+j)+x+i]++;

}

#define INTRO_IMAGE_COUNT 15

struct intro_image {
	unsigned char map[256 * 3];
	unsigned char pixels[320 * 200];
};
struct intro_image intro_images[INTRO_IMAGE_COUNT];

int
read_img_frame (FILE *inf, struct intro_image *ip)
{
	int len;
	char compressed[64 * 1024];
	int ilen;
	int idx;
	unsigned char *up;

	len = fread (&ip->map, 1, sizeof ip->map, inf);

	if (len == 0)
		return (-1);

	if (len != sizeof ip->map)
		goto bad;
	
	if (fread (&len, 4, 1, inf) != 1)
		goto bad;

	if (len > sizeof compressed) {
		fprintf (stderr, "frame too big\n");
		goto bad;
	}

	if (fread (compressed, 1, len, inf) != len)
		goto bad;
		
	PCX_D (compressed, ip->pixels, len);

	return (0);

bad:
	fprintf (stderr, "bad img file\n");
	exit (1);
}

void
read_intro_images (void)
{
	FILE *fin;
	int i;

	fin = sOpen ("FIRST.IMG","rb",0);

	for (i = 0; i < 15; i++) {
		if (read_img_frame (fin, &intro_images[i]) < 0) {
			fprintf (stderr, "error reading first.img\n");
			exit (1);
		}
	}

	fclose (fin);
}

void Introd(void)
{
  int k,j,i;
  FILE *fin;
  extern int cdROM;
  long len[2];
  int cdstat;
  struct intro_image *ip;
  double start;

  read_intro_images ();

  for (k=0;k<INTRO_IMAGE_COUNT;k++) {
    ip = &intro_images[k];

    gr_set_color_map (ip->map);
    memcpy (screen, ip->pixels, 320*200);
    gr_sync ();

    start = get_time ();
    while (get_time () - start < 3) {
	    av_step ();

	    key = 0;
	    PauseMouse();
	    if (key==K_ESCAPE)
		    goto done;

	    if (show_intro_flag == 0)
		    goto done;

    }
    key=0;
  }

done:
  memset(screen,0x00,64000);
}

void NextTurn(char plr)
{
  int i,j;
  FILE *fin;
  long len[2];

   MouseOff();
  for (i=0;i<768;i++) pal[i]=0;
  gxSetDisplayPalette(pal);
  strcpy(IDT,"i000");strcpy(IKEY,"k000");
    
  fin=sOpen("TURN.BUT","rb",0);
  len[0]=filelength(fileno(fin))-768;
  fread(&pal,768,1,fin);
  fread((char *)screen,len[0],1,fin);
  fclose(fin);

  RLED_img((char *)screen,vhptr.vptr,(unsigned int)len[0],
	   vhptr.w,vhptr.h);

  gxClearDisplay(0,0);

  ShBox(0,60,319,80);
  grSetColor(6+plr*3);
  if (plr==0) DispBig(15,64,"DIRECTOR OF THE UNITED STATES",0,-1);
  else DispBig(30,64,"CHIEF DESIGNER OF THE USSR",0,-1);
  grSetColor(11);
  grMoveTo(175,122);
  if (Data->Season==0) PrintAt(0,0,"SPRING 19");
  else PrintAt(0,0,"FALL 19");
  DispNum(0,0,Data->Year);
  gxVirtualDisplay(&vhptr,110*plr,0,30,85,31+107,85+94,0);
  
  FadeIn(2,pal,10,0,0);
  PreLoadMusic( (plr==0) ? M_GOOD : M_FUTURE );
  PlayMusic(1);
  MouseOn();

  while(1)  {
      GetMouse();
      if (mousebuttons==0) break;
      }
  while (mousebuttons==0 && key==0) GetMouse();
  key=0;

  KillMusic();
  FadeOut(2,pal,10,0,0);
  MouseOff();memset(screen,0x00,64000);MouseOn();
  return;
}




/* EOF */
