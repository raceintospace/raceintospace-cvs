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
// Location for Any C-ASM hybrid code

#include "Buzz_Inc.h"
#include "externs.h"

extern int FadeVal;

void toGrey(int start)
{
   int i;
    i=256-start;

   asm mov ah,10h;
   asm mov al,1Bh;
   asm mov bx,start;
   asm mov cx,i;
   asm int 10h;
//   for (i=start;i<256;i++)
//      pal[3*i+0]=pal[3*i+1]=pal[3*i+2]=(pal[3*i+0]*30+pal[3*i+1]*59+pal[3*i+2]*11)/100;
   return;
}

void PCX_D (char *src,char *dest,unsigned src_size)
{
  char num;
  do {
    if ((*src&0xc0)==0xc0) {
      num=*(src++)&0x3f; src_size--;
      while ((num--)>0) {*(dest++)=*src;}
      src++;src_size--;
    } else {
      (*dest++)=*(src++);src_size--;
    };
  }  while(src_size);
  return;
}


long RLEC (char *src, char *dest, unsigned int src_size)
{
   int src_i, dest_i,cpr;

 for (src_i = dest_i=0; src_i< src_size; ) {
   int k;  /* holds the number of characters to copy or repeat. */
  k = 0;
  cpr= ((src_size-src_i-1)<128) ? src_size-src_i-1 : 128;
     while (k<cpr && src[src_i] != src[src_i+1]) {
       k++;       /* increment the number of characters to copy */
       src_i++;   /* move pointer to the next character */
     }
  if (k) {
       dest[dest_i++] = (k-1);
       memcpy (&dest[dest_i], &src[src_i-k], k);
       dest_i += k;
     }
     else {
       k = 2;   /* there are at least two characters to be repeated */
       src_i++; /* advance pointer beyond the first match*/
     while ( k<cpr && src[src_i] == src[src_i+1]) {
       k++;       /* increment the number of characters to copy */
       src_i++;   /* move pointer to the next character */
     }  /* while */
       dest[dest_i++] = (-k+1);
       dest[dest_i++] = src[src_i++];
     }
 } /* for */
 return (dest_i);
}

void FadeIn(char wh,char *palx,int steps,int val,char mode)
{
  int i,j;
  char xtra[768];
  char *pp1,*pp2;
  pp1=xtra;
  pp2=palx;
  j=val*3;

  FADE=0;

  switch(wh) {
    case 0: for (i=j;i<768;i++) xtra[i]=(mode==1) ? palx[i]: 0;
	    i=0;
	    break;
    case 1: for (i=0;i<j;i++) xtra[i]=(mode==1) ? palx[i]: 0;
	    i=j; j=768-j;
	    break;
     case 2: j=768;i=0;break;
  }

  if (steps<30) steps=2;
  else steps=(FadeVal*2)/3;

  asm push es;
  asm push ds;
  asm mov bx,0;
loop1:
  asm les di,pp2;
  asm lds si,pp1;
  asm push si;
  asm mov cx,i;
loop3:
  asm inc di;
  asm inc si;
  asm loop loop3;
  asm mov cx,j;
loop2:
  asm xor ah,ah;
  asm mov al,[es:di];
  asm mul bx;
  asm div steps;
  asm mov [ds:si],al;
  asm inc di;
  asm inc si;
  asm loop loop2;
  asm mov dx,3DAh;

retraceloop2:
  asm in al,dx;
  asm and al,08h;
  asm jz retraceloop2;

  asm pop si;
  asm mov dx,3C8h;
  asm xor al,al;
  asm out dx,al;
  asm mov cx,768;
  asm mov dx,3C9h;
  asm cld;
  asm rep outsb;

  asm inc bx;
  asm cmp bx,steps;
  asm jng loop1;
  asm pop ds;
  asm pop es;

 return;
}

/*
  wh   :: 0 = Fade First Bit of Palette
          1 = Fade Last Bit of Palette
          2 = Fade Entire Palette

  mode :: 0 = Make Non Faded Colors with Black
          1 = Preserve Non Faded Colors 

*/

void FadeOut(char wh,char *palx,int steps,int val,char mode)
{
  int i,j;
  char *pp1,*pp2;
  char xtra[768];
  pp1=xtra;
  pp2=palx;
  j=val*3;
  FADE=1;
  switch(wh) {
    case 0: for (i=j;i<768;i++) xtra[i]=(mode==1) ? palx[i]: 0;
	    i=j;j=0;
	    break;
    case 1: for (i=0;i<j;i++) xtra[i]=(mode==1) ? palx[i]: 0;
	    i=768-j;
	    break;
    case 2: j=0;i=768;break;
  }

  if (steps<30) steps=3;
  else steps=(FadeVal*2)/3;

  asm push es;
  asm push ds;
  asm mov bx,steps;
loop1:
  asm les di,pp2;
  asm lds si,pp1;
  asm push si;
  asm mov cx,j;
loop3:
  asm inc di;
  asm inc si;
  asm loop loop3;
  asm mov cx,i;
loop2:
  asm xor ah,ah;
  asm mov al,[es:di];
  asm mul bx;
  asm div steps;
  asm mov [ds:si],al;
  asm inc di;
  asm inc si;
  asm loop loop2;
  asm mov dx,3Dah;

retraceloop2:
  asm in al,dx;
  asm and al,08h;
  asm jz retraceloop2;

  asm pop si;
  asm mov dx,3C8h;
  asm xor al,al;
  asm out dx,al;
  asm mov cx,768;
  asm mov dx,3C9h;
  asm cld;
  asm rep outsb;

  asm dec bx;
  asm jnl loop1;
  asm pop ds;
  asm pop es;

  return;
}


void RLED (char *src, char *dest, unsigned int src_size)
{
	asm push es;          // preserve ES
	asm push ds;          // preserve DS
	asm les di,dest;      // move dest into ES:DI
	asm lds si,src;       // move src into DS:SI
	asm mov cx,0;         // clear CX
	asm mov bx,src_size;  // move counter into BX
loa:
	asm lodsb;            // move byte into AL
	asm dec bx;           // decrement CX
	asm cmp al,0;         // compare AL to 0
	asm jl repeat;        // if al < 0 jump to repeat
			      // copy bytes
	asm mov ah,0;         // clear AH
	asm inc al;           // increment AL
	asm mov cl,al;        // put value of AL into CL
	asm rep movsb;        // move CX bytes from DS:SI to ES:DI
	asm sub bx,ax;        // increment BX by approp value
	asm cmp bx,0;         // see if finished
	asm jg loa;           // if not then loop
	asm jmp bot;          // else jump to bottom

repeat:
	asm neg al;           // negate AL
	asm inc al;           // increment AL by 1
	asm mov cl,al;        // move counter value to CX
	asm lodsb;            // load value to copy
lob:
	asm stosb;            // copy AL into ES:DI
	asm loop lob;         // do while CX >0
	asm dec bx;           // decrement bx;
	asm cmp bx,0;         // see if finished
	asm jg loa;           // if not then loop

bot:                          // bottom of routine
	asm pop ds;           // restore ds
	asm pop es;           // restore es

  return;
}
