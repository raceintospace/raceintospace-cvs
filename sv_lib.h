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
///////////////////////////////////////////////////////////
//
// Strategic Visions Library Header
// Revision 0.5              
// 06/07/93

// Standard definitions

#ifndef BYTE 
#define BYTE unsigned char
#endif
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef LONG
#define LONG unsigned long
#endif

#ifndef UBYTE
#define UBYTE unsigned char
#endif

#ifndef SBYTE
#define SBYTE char
#endif

#ifndef UWORD
#define UWORD unsigned short;
#endif

#ifndef SWORD
#define SWORD short;
#endif

#define TXT 0x03
#define GFX 0x13

//////////////////////////////////////////////////////////
//    These ones are internal and are not needed
//
//int  GetPCX_Run(char *rbuf,int max);
//int  PCX_Encode_Line(char *ibuf,char *obuf,int line_size);
//void PCX_D (char *src,char *dest,unsigned src_size);

  int WritePCX(char *Name,char *pal);
  int ReadPCX(char *Name,char *pal);


////////////////////////////////////////////////////////
//
//  Prototypes for AMOVE.ASM file in Lib
//
#define GFX 0x13
#define TXT 0x03

void SetMode(char mode);
void BFill(void *buffer,char color);
void BxFill(char far *buffer,char c1,char c2);
void SMove(void *src,int x, int y);
void LMove(void *src);
void SMove2(char far *src,int x, int y);
void AMove(char far *, char far *,char mode);
void MemCopy(char far *,char far *);
void HLine(int X1,int Y1,int X2,char Col);
void VLine(int X1,int Y1,int Y2,char Col);
void oBox(int X1,int Y1,int X2,int Y2,char Col);
void shBox(int X1,int Y1,int X2,int Y2);
void RFill(int X1,int Y1,int X2,int Y2,char Col);
int RLED (void *src, void *dest, unsigned int src_size);
void SetPal(char *pal);


///////////////////////////////////////////////////////
//
//  AVI structures used within AVI.c
//

// Definitions

// AVI RIFF structures

typedef char ID[4];

struct FORM {
  char form[4];
  LONG size;
  char formType[4];
};

struct CHUNK {
  char ckID[4];
  LONG ckSize;
  // UBYTE ckData[ CHUNK.ckSize ];
};

struct AVIH {
  LONG uSecDelay;
  LONG target_rate;
  WORD a3a;
  WORD a3b;
  LONG a4;
  LONG frames;
  LONG a6;
  LONG a7;
  WORD a8a;
  WORD a8b;
  LONG width;
  LONG height;
  WORD a11a;
  WORD a11b;
  WORD a12a;
  WORD a12b;
  WORD a13a;
  WORD a13b;
  WORD a14a;
  WORD a14b;
};

struct STRL {
  LONG t1;
};

struct DIB {
  LONG mys1;
  LONG mys2;
  LONG mys3;
  LONG i_offset;
  LONG mys4;
  LONG mys5;
  LONG frames;
  LONG a_offset;
  LONG mys6;
  LONG mys7;
  LONG mys8;
  LONG mys9;
};

struct VSTRF {
  LONG size;  // header length
  LONG width;   // image width
  LONG height;  // image height
  WORD planes;  
  WORD bits_per_plane;
  LONG compress;
  LONG framesize;
  LONG XPelsPerMeter;
  LONG YPelsPerMeter;
  LONG num_colors;
  LONG Useful;
};

struct ASTRF {
  WORD formatTag;
  WORD nChannels;
  LONG nSamplesPerSec;
  LONG nAvgBytesPerSec;
  WORD nBlockAlign;
  WORD nBitsPerSample;
};

struct IDX1 {
   char ID[4];
   long val1;
   long val2;
   long val3;
};

struct AVInfo {  // built from in DecodeRIFF
  char ID[8];
  i16 fcount;
  i16 frate;
  i16 w,h;
  long fOff[400];
};



