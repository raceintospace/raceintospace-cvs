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
#include "replay.h"

#define FRM_Delay     25

#define STL_OFF   26715
#define ANIM_PARTS  297

extern char STEPnum,loc[4];
extern struct MisEval Mev[60];
extern char MANNED[2],STEP,pal2[768],AI[2];
extern int tFrames,cFrame;
extern long aLoc;
extern GXHEADER dply;
extern struct AnimType AHead;
extern struct BlockHead BHead;

#if 0
void RLEF(char *dest, char *src, unsigned int src_size)
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

  asm cmp bx,100h;
  asm jg  sk;
  asm int   03h;

sk:
	asm cmp bx,0;         // see if finished
	asm jg loa;           // if not then loop

bot:                          // bottom of routine
	asm pop ds;           // restore ds
	asm pop es;           // restore es

  return;
}
#endif


void Replay(char plr,int num,int dx,int dy,int width,int height,char *Type)
{
 struct romKEYS {
  char Name[8];
  long offset;
  long size;
 } altKEY;  // for index based stuff

 struct oLIST {
   i16 aIdx; 
   i16 sIdx; 
 };

 struct oGROUP {
   char ID[10];  
   struct oLIST oLIST[5]; 
 };

 struct NTable {
   char ID[8];
 };

 struct oFGROUP {
   char ID[15];
   struct oLIST oLIST[5];
 };

 struct Table {
  char fname[8];
  long foffset;
   unsigned short size;
 }; 

 int wlen,i,j,kk,fres,fres1,mode,max;
 FILE *fin,*kfin;
 long offset;
 struct oGROUP *bSeq,aSeq;
 struct oFGROUP *dSeq,cSeq;
 struct NTable NT;
 struct Table *F;
 char *SEQ_DAT="SEQ.DAT\0";
 char *SEQ_KEY="ALTSEQ.KEY\0";
 char *FSEQ_DAT="FSEQ.DAT\0";
 char *FSEQ_KEY="ALTFSEQ.KEY\0";
 unsigned long tim;
 unsigned short fr;
 int *fcount,*frate,count;
 int *fsize;
 RPLY Rep;
 GXHEADER dopy,snzy;
 struct frm *frm = NULL;
 int update_map;

 if (strncmp("OOOO",Type,4)==0)
  {
   kfin=sOpen("REPLAY.DAT","rb",1);
   offset=(plr*100)+num;
   fseek(kfin,offset * (sizeof Rep),SEEK_SET);
   fread(&Rep,sizeof Rep,1,kfin);
   fclose(kfin);
  }
 else {  // Find correct Sequence
  bSeq=(struct oGROUP *)&vhptr.vptr[35000];
  fin=sOpen(SEQ_DAT,"rb",0);
  offset=filelength(fileno(fin));
  fread(&vhptr.vptr[35000],offset,1,fin);
  fclose(fin);
  mode=0;
  j=0;
  while (strncmp(bSeq[j].ID,"XXXX",4)!=0 && strncmp(&bSeq[j].ID[3],Type,strlen(&bSeq[j].ID[3]))!=0)
   j++;
  if (strncmp(bSeq[j].ID,"XXXX",4)==0) return;  // bad sequence
  Rep.Qty=1;
  Rep.Off[0]=j;
 };

 GV(&snzy,width,height);
 GV(&dopy,160,100);

 printf ("******\n");
 printf ("%d segments\n", Rep.Qty);
 for (kk=0;kk<Rep.Qty;kk++)
  {
   printf ("segment %d: %d\n", kk, Rep.Off[kk]);
   UpdateMusic();  
   i=0;
   if (Rep.Off[kk]<1000)    //Specs: success seq
    {
     bSeq=(struct oGROUP *)&vhptr.vptr[35000];
     fin=sOpen(SEQ_DAT,"rb",0);
     offset=filelength(fileno(fin));
     fread(&vhptr.vptr[35000],offset,1,fin);
     fclose(fin);
     mode=0;
    }
   else { //Specs: failure seq
    dSeq=(struct oFGROUP *)&vhptr.vptr[35000];
    F=(struct Table *)&vhptr.vptr[0];
    fin=sOpen(FSEQ_DAT,"rb",0);
    fseek(fin,0,SEEK_SET);
    fread(&vhptr.vptr[0],700,1,fin);
    i=Rep.Off[kk]/1000;
    Rep.Off[kk]%=1000;
    if (i==0 || i==50) {fclose(fin);goto done;}
    i--; //Specs: offset index klugge
    offset=F[i].foffset; 
    fseek(fin,offset,SEEK_SET);
    fread(&vhptr.vptr[35000],F[i].size,1,fin);
    fclose(fin);
    mode=1;
   };

  if (mode==0) {
   memcpy(&aSeq,&bSeq[Rep.Off[kk]].ID[0],sizeof aSeq);
   max=aSeq.ID[1]-0x30;
  }
  else {
   memcpy(&cSeq,&dSeq[Rep.Off[kk]].ID[0],sizeof cSeq);
   max=cSeq.ID[1]-0x30;
  };

  i=0;

  update_map = 0;
  while (i<max)
  {
   UpdateMusic();

   if ((frm = frm_open_seq (aSeq.oLIST[i].aIdx, mode)) == NULL)
	   goto done;

   update_map = 1;

   while (1) {
	   unsigned char map[384];

	   UpdateMusic();

	   if (frm_get2 (frm, dopy.vptr, map) <= 0)
		   break;

	   if (update_map) {
		   memcpy (&pal[384], map, 384);
		   update_map = 0;
	   }

	   gxVirtualScale(&dopy,&snzy);
	   MouseOff();VBlank();
	   gxPutImage(&snzy,gxSET,dx,dy,0);
	   MouseOn();
	   gr_sync ();

	   if (bioskey(1)) {
		   key=bioskey(0);
		   if (key>0)
			   goto done;
		   goto skip_step;
	   }

	   if (frm->frame_rate)
		   idle_loop_secs (1.0 / frm->frame_rate);
	   else
		   idle_loop_secs (1.0 / 8.0);

   }

   frm_close (frm);
   frm = NULL;

   i++;
  }

skip_step:;

 }
done:
 DV(&dopy);DV(&snzy);
 if (frm)
	 frm_close (frm);
 return;
}

void DispBaby(int x, int y, int loc,char neww)
{
   int i;
   FILE *fin;
   GXHEADER boob;
   ui16 *bot,off=0;
   long locl;

   off=224;

   GV(&boob,68,46);
   bot=(ui16 *) boob.vptr;

   fin=sOpen("BABYPICX.CDR","rb",0);
   locl=(long) 1612*loc;  // First Image
   fseek(fin,locl,SEEK_SET);
   for (i=0;i<48;i++) pal[off*3+i]=0;
   if (neww) gxSetDisplayPalette(pal);
   fread(&pal[off*3],48,1,fin);
   fread(boob.vptr,1564,1,fin);
   fclose(fin);
   
   for (i=0;i<782;i++) {
      bot[i+782]=((bot[i]&0xF0F0)>>4);
      bot[i]=(bot[i]&0x0F0F);
      }
   for (i=0;i<1564;i++) {
      boob.vptr[i]+=off;
      boob.vptr[1564+i]+=off;
      }

   gxPutImage(&boob,gxSET,x,y,0);
   if (neww) gxSetDisplayPalette(pal);
   DV(&boob);

   return;
}

void AbzFrame(char plr,int num,int dx,int dy,int width,int height,char *Type,char mde)
{
 struct romKEYS {
  char Name[8];
  long offset;
  long size;
 } altKEY;  // for index based stuff

 struct oLIST {
   i16 aIdx; 
   i16 sIdx; 
 };

 struct oGROUP {
   char ID[10];  
   struct oLIST oLIST[5]; 
 };

 struct NTable {
   char ID[8];
 };

 int wlen,i,j;
 FILE *fin,*kfin;
 long offset;
 struct oGROUP *bSeq,aSeq;
 struct NTable NT;
 char *SEQ_DAT="SEQ.DAT\0";
 char *SEQ_KEY="ALTSEQ.KEY\0";
 unsigned long tim;
 unsigned short fr;
 int *fcount,*frate,count;
 int *fsize;
 RPLY Rep;
 GXHEADER dopy,snzy;
 struct frm *frm;

 bSeq=(struct oGROUP *)&vhptr.vptr[35000];
 j = 0;

 GV(&snzy,width,height);
 GV(&dopy,160,100);

 if (mde)EMPTY_BODY;

 fin=sOpen(SEQ_DAT,"rb",0);
 offset=filelength(fileno(fin));
 fread(&vhptr.vptr[35000],offset,1,fin);
 fclose(fin);

   if (strncmp("OOOO",Type,4)==0) {
      kfin=sOpen("REPLAY.DAT","rb",1);
      offset=(plr*100)+num;
      fseek(kfin,offset * (sizeof Rep),SEEK_SET);
      fread(&Rep,sizeof Rep,1,kfin);
      fclose(kfin);
     }
   else {  // Find correct Sequence
     j=0;
     while ( strncmp(bSeq[j].ID,"XXXX",4)!=0 && strncmp(&bSeq[j].ID[3],Type,strlen(&bSeq[j].ID[3]))!=0 )
       j++;
     if (strncmp(bSeq[j].ID,"XXXX",4)==0) goto done;  // bad sequence
     Rep.Qty=1;
     Rep.Off[0]=j;
   }

 if (strncmp(bSeq[j].ID,"XXXX",4)==0) goto done;
 memcpy(&aSeq,&bSeq[Rep.Off[0]].ID[0],sizeof aSeq);



 if ((frm = frm_open_seq (aSeq.oLIST[0].aIdx, 0)) != NULL) {
	 frm_get2 (frm, dopy.vptr, &pal[384]);
	 frm_close (frm);
 } else {
	 memset (dopy.vptr, 0, 160 * 100);
	 memset (&pal[384], 0, 384);
 }

 gxVirtualScale(&dopy,&snzy);

 MouseOff();VBlank();
 RectFill(dx,dy,dx+width,dy+height-1,0);
 gxPutImage(&snzy,gxSET,dx,dy,0);
 MouseOn();

done:
 DV(&dopy);DV(&snzy);
 return;
}



