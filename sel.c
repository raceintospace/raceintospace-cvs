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
#include "Buzz_inc.h"

extern char Musics,Sounds,Name[20];

void DFrame(int x1, int y1, int x2, int y2)
{
   int i;
   gotoxy(x1,y1);cprintf("É");
   gotoxy(x1,y2);cprintf("È");
   gotoxy(x2,y1);cprintf("»");
   gotoxy(x2,y2);cprintf("¼");

   for (i=x1+1;i<x2;i++) {
      gotoxy(i,y1);cprintf("Í");
      gotoxy(i,y2);cprintf("Í");
      }

   for (i=y1+1;i<y2;i++) {
      gotoxy(x1,i);cprintf("º");
      gotoxy(x2,i);cprintf("º");
      }

   return;
}

void SFrame(int x1, int y1, int x2, int y2)
{
   int i;
   gotoxy(x1,y1);cprintf("Ú");
   gotoxy(x1,y2);cprintf("À");
   gotoxy(x2,y1);cprintf("¿");
   gotoxy(x2,y2);cprintf("Ù");

   for (i=x1+1;i<x2;i++) {
      gotoxy(i,y1);cprintf("Ä");
      gotoxy(i,y2);cprintf("Ä");
      }

   for (i=y1+1;i<y2;i++) {
      gotoxy(x1,i);cprintf("³");
      gotoxy(x2,i);cprintf("³");
      }


   return;
}

void SFWin(int x1, int y1, int x2, int y2,char *txt)
{
   int i,dx,dy;
   dx=x2-x1+1;
   dy=y2-y1+1;

   window(x1,y1,x2,y2);
   textbackground(BLUE);clrscr();
   textcolor(YELLOW);

   for (i=2;i<dx;i++) {
      gotoxy(i,1);cprintf("Í");
      gotoxy(i,dy);cprintf("Í");
      }

   for (i=2;i<dy;i++) {
      gotoxy(1,i);cprintf("º");
      gotoxy(dx,i);cprintf("º");
      }
   gotoxy(1,1);cprintf("É");
   gotoxy(1,dy);cprintf("È");
   gotoxy(dx,1);cprintf("»");
   gotoxy(dx,dy);cprintf("¼");

  window(x1,y1,x2,y1);textbackground(15);clrscr();
  Print((dx>>1)-(strlen(txt)>>1) , 1 ,YELLOW,txt);
  textbackground(BLUE);window(1,2,80,25);
   return;
}


void Print(int x, int y,int col,char *txt)
{
   gotoxy(x,y);textcolor(col);
   cprintf("%s",txt);
   return;
}

void ScreenSetup(void)
{
  clrscr();
  _wscroll=0;
  _setcursortype(_NOCURSOR);
  window(1,1,80,1);textbackground(WHITE);clrscr();
  Print(12,1,YELLOW,"Race into Space");
  window(1,2,80,25);textbackground(1);clrscr();
  return;
}

#define SCARDS 10
#define DCARDS 5

void GetSoundDevice(char cfg_set)
{
  int i,j,fin;
  FILE *fxin;
  char M[2],good,ch,*env;
  struct SndN {
     char Name[33];
     char val;
  } nF[SCARDS]={
   "None                         \0",mNONE,
   "PC Speaker                   \0",mSPEAKER,
   "Adlib                        \0",mADLIB,
   "Sound Blaster / Thunder Board\0",mADLIB,
   "Sound Blaster Pro Series     \0",mSBP,
   "Pro Audio Spectrum Original  \0",mADLIB,
   "Pro Audio Spectrum Plus/16   \0",mPAS,
   "Gravis Ultrasound            \0",mGUS,
   "Roland MT-32 or LAPC-1       \0",mMT32,
   "Roland SCC-1                 \0",mSCC1
   },nD[DCARDS]= {
   "None                         \0",sNONE,
   "Sound Blaster / Thunder Board\0",sBLASTER,
   "Sound Blaster Pro Series     \0",sSBP,
   "Pro Audio Spectrum Series    \0",sPAS,
   "Gravis Ultrasound            \0",sGUS,
   };

  {
	  int mode;
	  mode = O_RDWR;
#ifdef O_BINARY
	  mode |= O_BINARY;
#endif
	  fin=open("CONFIG",mode);
  }
  if (fin==-1 || cfg_set==0) {
     if (fin!=-1) {
        read(fin,M,sizeof M);
        Musics=nF[M[0]].val;
        Sounds=nD[M[1]].val;
        close(fin);
     }
     else Musics=Sounds=M[0]=M[1]=0;
     fin=creat("CONFIG",666);
  }
  else {
     read(fin,M,sizeof M);
     Musics=nF[M[0]].val;
     Sounds=nD[M[1]].val;
     close(fin);
     return;
  };

  ScreenSetup();
  SFWin(2,20,38,24,"Music Selection");Print(4,21,WHITE,nF[M[0]].Name);
  SFWin(42,20,78,24,"Sound Selection");Print(44,21,WHITE,nD[M[1]].Name);

  SFWin(2,3,79,14,"Music Device Selection");
  window(2,3,79,14);
  for (i=0;i<SCARDS;i++) {
     gotoxy(2,2+i);textcolor(WHITE);
     cprintf("%d: %s",i,nF[i].Name);
  }
  _setcursortype(_NORMALCURSOR);
  SFWin(2,16,40,18,"Enter New Selection");
  good=0;Musics=0;
  do {
     Print(4,16,WHITE,nF[M[0]].Name);
     gotoxy(4,16);
     ch=getch();
     if ((ch-0x30)<SCARDS && (ch-0x30)>=0) M[0]=ch-0x30;
     else if (M[0]<SCARDS && M[0]>=0 && ch==0x0d) good=1;
  } while (!good);
  Musics=nF[M[0]].val;
  _setcursortype(_NOCURSOR);
  SFWin(2,20,38,24,"Music Selection");Print(4,21,WHITE,nF[M[0]].Name);

  SFWin(2,3,79,14,"Digitized Sound Device Selection");
  window(2,3,79,14);
  for (i=0;i<DCARDS;i++) {
     gotoxy(2,2+i);textcolor(WHITE);
     cprintf("%d: %s",i,nD[i].Name);
  }

  SFWin(2,16,40,18,"Enter New Selection");
  _setcursortype(_NORMALCURSOR);
  good=0;Sounds=0;
  do {
     Print(4,16,WHITE,nD[M[1]].Name);
     gotoxy(4,16);
     ch=getch();
     if ((ch-0x30)<DCARDS && (ch-0x30)>=0) M[1]=ch-0x30;
     if (M[1]<DCARDS && M[1]>=0 && ch==0x0d) good=1;
  } while (!good);
  Sounds=nD[M[1]].val;
  _setcursortype(_NOCURSOR);
  SFWin(42,20,78,24,"Sound Selection");Print(44,21,WHITE,nD[M[1]].Name);

  lseek(fin,0L,SEEK_SET);
  write(fin,M,sizeof M);
  close(fin);

     if (Musics==mBLASTER || Musics==mSBP1 || Musics==mSBP2 || Sounds==sBLASTER || Sounds==sSBP) {
//getaddr:
       env = getenv("BLASTER");
       if (env==NULL) {// 0123456789012345678901
         char bString[30] = "BLASTER=A220 I5 D1 T1\0";
         int ll=0,good;
         ScreenSetup();

         // Address
         gotoxy(1,1);cprintf("The BLASTER envoronment variable is not currently set.");clreol();
         textcolor(WHITE);
         gotoxy(2,2);cprintf("Select your Soundcard's Base Address:");clreol();
         gotoxy(2,3);cprintf("  1: 210  (SB Only)");clreol();
         gotoxy(2,4);cprintf("  2: 220 *(SB / SBPro)");clreol();
         gotoxy(2,5);cprintf("  3: 230  (SB Only)");clreol();
         gotoxy(2,6);cprintf("  4: 240  (SB / SBPro)");clreol();
         gotoxy(2,7);cprintf("  5: 250  (SB Only)");clreol();
         gotoxy(2,8);cprintf("  6: 260  (SB Only)");clreol();
         gotoxy(2,9);cprintf("Select One:");clreol();
         good=0;
         while (!good) {
            ll=getch()-0x30;
            if (ll>0 && ll<=6) {
               good=1;
               switch(ll) {
                 case 1: ll=1;break;
                 case 2: ll=2;break;  //Pro
                 case 3: ll=3;break;
                 case 4: ll=4;break;  //Pro
                 case 5: ll=5;break;
                 case 6: ll=6;break;
               }
            }
         }
         bString[10]=(char) ll+ 0x0030;

         // IRQ
         gotoxy(2,2);cprintf("Select your Soundcard's IRQ:");clreol();
         gotoxy(2,3);cprintf("  1: IRQ 2 ");clreol();
         gotoxy(2,4);cprintf("  2: IRQ 3 ");clreol();
         gotoxy(2,5);cprintf("  3: IRQ 5 *");clreol();
         gotoxy(2,6);cprintf("  4: IRQ 7 ");clreol();
         gotoxy(2,7);clreol();
         gotoxy(2,8);clreol();
         gotoxy(2,9);cprintf("Select One:");clreol();
         good=0;
         while (!good) {
            ll=getch()-0x30;
            if (ll>0 && ll<=4) {
               good=1;
               switch(ll) {
                 case 1: ll=2;break;
                 case 2: ll=3;break; 
                 case 3: ll=5;break;
                 case 4: ll=7;break;
               }
            }
         }
         bString[14]=(char) ll+0x30;

         // DMA
         gotoxy(2,2);cprintf("Select your Soundcard's Base Address:");clreol();
         gotoxy(2,3);cprintf("  1: DMA 0  (SBPro Only)");clreol();
         gotoxy(2,4);cprintf("  2: DMA 1 *(SB / SBPro)");clreol();
         gotoxy(2,5);cprintf("  3: DMA 3  (SBPro Only)");clreol();
         gotoxy(2,6);clreol();
         gotoxy(2,7);clreol();
         gotoxy(2,8);clreol();
         gotoxy(2,9);cprintf("Select One:");clreol();
         good=0;
         while (!good) {
            ll=getch()-0x30;
            if (ll>0 && ll<=3) {
               good=1;
               switch(ll) {
                 case 1: ll=0;break;
                 case 2: ll=1;break;
                 case 3: ll=3;break;
               }
            }
         }
         bString[17]=(char) ll+0x30;
         gotoxy(2,9);cprintf("Running Buzz...");clreol();

         //putenv(bString);
         fxin=fopen("BUZZ.BAT","w+t");
         fprintf(fxin,"@SET %s\n@BARIS.EXE\n",bString);
         fclose(fxin);
         exit(0);
       }
     }
    gotoxy(1,1);textcolor(WHITE);
    textbackground(BLACK);clrscr();
    _setcursortype(_NORMALCURSOR);
  exit(0);
}
