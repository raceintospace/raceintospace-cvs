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
// Vehicle Assembly Building

#include "Buzz_inc.h"
#include "externs.h"
extern char AI[2];

  struct VInfo VAS[7][4];
  char VASqty,CV;
  extern struct mStr Mis;
  extern char Vab_Spot;
  extern char far *sbuf0,far *sbuf1;

// CAP,LM,SDM,DMO,EVA,PRO,INT,KIC

struct MDA {
  i16 x1,y1,x2,y2,o;
} far *MI; //  MI[2][28] = {
#if 0
  104,1,119,55,0,   // Atlas  0
  143,1,153,67,0,   // Titan  1
  177,1,217,133,0,  // Saturn 2
  219,1,262,139,0,  // Nova   3
  121,1,141,53,0,   // Atlas+B 4
  155,1,175,67,0,   // Titan+B 5
  177,1,217,133,0,  // Saturn+B 6
  83,1,102,57,0,    // Casing Small 7
  264,1,318,145,0,  // Casing Large 8
  59,48,75,67,0,    // Orbital 9
  59,69,73,84,0,    // InterPlan 10
  59,86,72,100,0,   // Lunar Probe 11
  130,55,141,59,0,  // Docking 12
  2,1,18,39,13,     // Merc 13
  79,69,111,117,2,  // Gemini 14
  20,1,57,59,12,    // Apollo 15
  59,1,81,46,7,     // MiniSh 16
  137,69,175,139,7, // Four Cap 17
  223,141,262,178,0,// Two Lem 18
  188,141,221,177,0,// One Lem 19
  138,141,160,166,0,// KickA 20
  162,141,186,172,0,// KickB 21
  0,0,0,0,0,        // KickC 22
  0,0,0,0,0,        // None : Zond 23
  0,0,0,0,0,        // Filler Sm
  20,61,57,141,0,   // Filler Lg
  113,69,132,98,0,  // smShroud
  81,119,135,153,0, // LgShroud

  86,1,113,74,0,     // A-Series 0
  115,1,137,82,0,    // Proton 1
  192,1,227,130,0,   // N-1      2
  229,0,263,131,0,   // Energia  3
  139,1,166,85,0,    // A-Series 4
  168,1,190,95,0,    // Proton+B   5
  192,1,227,130,0,   // N-1+B      6
  3,60,22,116,0,     // Casing Small 7
  277,1,318,145,0,   // Casing Large 8
  5,32,22,51,0,      // Orbital  9
  79,132,89,157,0,   // InterPlan 10
  3,1,20,30,0,       // Lunar Probe 11
  5,53,19,58,0,      // Docking 12
  91,132,118,166,15, // Vostok 13
  120,132,148,167,15,// Voshod 14
  176,132,211,194,15,// Soyuz 15
  249,132,275,193,11,// MiniSh 16
  213,132,247,196,11,// Four Cap 17
  25,1,55,36,0,      // Two Lem 18
  57,1,84,37,0,      // One Lem 19
  24,39,36,72,0,     // KicA 20
  38,39,58,75,0,     // KicB 21
  60,39,84,94,0,     // KicC 22
  150,132,174,177,22,// Zond 23
  0,0,0,0,0,         // Filler Sm
  24,77,48,183,0,    // Filler Lg
  3,118,22,147,0,    // smShroud
  149,97,190,124,0   // LgShroud
};
#endif

void GradRect2(int x1,int y1,int x2,int y2,char plr)
{
  register int i,j,val;

  val=3*plr+6;

  RectFill(x1,y1,x2,y2,7+3*plr);
  for (j=x1;j<=x2;j+=3)
    for (i=y1;i<=y2;i+=3)
      screen[j+320*i]=val;
   return;
}


void DispVAB(char plr,char pad)
{
  int i;
  FILE *fout;
  struct bImg {
    unsigned char pal[768];
    ui16 fSize;
  } M;


  // these were 4 lines added 5/1/93 for data reduction purposes
  MI=(struct MDA far *) sbuf0;
  fout=sOpen("VTABLE.DAT","rb",0);
  fread(MI,(sizeof (struct MDA))*56,1,fout);
  fclose(fout);

  strcpy(IDT,"i016");strcpy(IKEY,"k016");
  MouseOff();
  FadeOut(2,pal,10,0,0);

  fout=sOpen("VAB.IMG","rb",0);
  fread(&M,sizeof M,1,fout);
  if (plr==1) {fseek(fout,M.fSize,SEEK_CUR);fread(&M,sizeof M,1,fout);}
  memcpy(&pal[0],&M.pal[0],768);
  fread((char *)screen,(long)M.fSize,1,fout);
  fclose(fout);
  PCX_D((char *)screen,vhptr.vptr,M.fSize);

  gxClearDisplay(0,0);
  ShBox(0,0,319,22);ShBox(0,24,170,99);
  ShBox(0,101,170,199);ShBox(172,24,319,199);
  InBox(3,3,30,19);
  IOBox(243,3,316,19);
  IOBox(175,183,244,197);
  if (Data->P[plr].Mission[pad].MissionCode!=0) IOBox(247,183,316,197);
  else InBox(247,183,316,197);

  InBox(4,104,166,123);
  IOBox(62,127,163,177);  IOBox(62,179,163,193);
  
  InBox(177,28,314,180);
  RectFill(178,29,313,179,3);

  IOBox(4,84,165,96);

  grSetColor(1);
  PrintAt(13,92,"AUTO PURCHASE");
  PrintAt(198,192,"EXIT");
  PrintAt(268,192,"SCRUB");
  PrintAt(264,13,"ASSIGN");
  PrintAt(16,136,"PRIMARY:");
  PrintAt(24,148,"KICKER:");
  PrintAt(43,160,"L.M.:");
  PrintAt(15,172,"PAYLOAD:");
  PrintAt(23,188,"ROCKET:     ");

  Name[0]='A'+pad; Name[1]=0x00;
  InBox(4,27,166,37);RectFill(5,28,165,36,10);
  grSetColor(11);
  PrintAt(42,34,"LAUNCH FACILITY: ");PrintAt(0,0,Name);
  grSetColor(1);
  if (plr==0) DispBig(42,4,"VEHICLE ASSEMBLY",0,-1);
  else DispBig(37,4,"VEHICLE INTEGRATION",0,-1);

  grSetColor(5);
  PrintAt(5,45,"MISSION: ");PrintAt(0,0,Data->P[plr].Mission[pad].Name);

  grSetColor(1);
  PrintAt(5,61,"CREW: ");
  switch(Data->P[plr].Mission[pad].Men) {
    case 0:PrintAt(0,0,"UNMANNED");break;
    case 1:PrintAt(0,0,"ONE PERSON");break;
    case 2:PrintAt(0,0,"TWO PEOPLE");break;
    case 3:PrintAt(0,0,"THREE PEOPLE");break;
    case 4:PrintAt(0,0,"FOUR PEOPLE");break;
  }

  RectFill(5,105,165,122,7+plr*3);
  grSetColor(11);
  PrintAt(35,111,"MISSION HARDWARE:");
  PrintAt(10,119,"SELECT PAYLOADS AND BOOSTER");

  grSetColor(1);

  GetMisType(Data->P[plr].Mission[pad].MissionCode);

  PrintAt(5,52,Mis.Abbr);

  FlagSm(plr,4,4);
  MouseOn();
  return;
}


int FillVab(char plr,char f,char mode)
{
  int i,j,k,cost;
  cost=0;
  for (i=0;i<4;i++) {
    if ((VAS[f][i].qty-VAS[f][i].ac)<=0 && strncmp(VAS[f][i].name,"NONE",4)!=0) {
      switch(i) {
	      case 0: case 2:
	        if ((Data->P[plr].Manned[VAS[f][i].dex].Num-Data->P[plr].Manned[VAS[f][i].dex].Spok)==0) {
            if (mode==1) {
	            Data->P[plr].Cash-=maxx(1,Data->P[plr].Manned[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Manned[VAS[f][i].dex].UnitCost/2);
	            Data->P[plr].Manned[VAS[f][i].dex].Num++;
	          }
            else cost+=maxx(1,Data->P[plr].Manned[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Manned[VAS[f][i].dex].UnitCost/2);
          }
	        break;
	      case 1: 
	        if ((Data->P[plr].Misc[VAS[f][i].dex].Num-Data->P[plr].Misc[VAS[f][i].dex].Spok)==0)
            if (mode==1) {
	            Data->P[plr].Cash-=maxx(1,Data->P[plr].Misc[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Misc[VAS[f][i].dex].UnitCost/2);
	            Data->P[plr].Misc[VAS[f][i].dex].Num++;
	          }
            else cost+=maxx(1,Data->P[plr].Misc[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Misc[VAS[f][i].dex].UnitCost/2);
	        break;
    	   case 3:
	        if (VAS[f][i].dex!=4) {
	          if ((Data->P[plr].Probe[VAS[f][i].dex].Num-Data->P[plr].Probe[VAS[f][i].dex].Spok)==0) {
              if (mode==1) {
		           Data->P[plr].Cash-=maxx(1,Data->P[plr].Probe[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Probe[VAS[f][i].dex].UnitCost/2);
		           Data->P[plr].Probe[VAS[f][i].dex].Num++;
	            }
              else cost+=maxx(1,Data->P[plr].Probe[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Probe[VAS[f][i].dex].UnitCost/2);
            }
	        }
          else {
	          if ((Data->P[plr].Misc[VAS[f][i].dex].Num-Data->P[plr].Misc[VAS[f][i].dex].Spok)==0) {
              if (mode==1) {
		           Data->P[plr].Cash-=maxx(1,Data->P[plr].Misc[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Misc[VAS[f][i].dex].UnitCost/2);
		           Data->P[plr].Misc[VAS[f][i].dex].Num++;
	            }
              else cost+=maxx(1,Data->P[plr].Misc[VAS[f][i].dex].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Misc[VAS[f][i].dex].UnitCost/2);
	          }
          }
	        break;
      }
    }
  }

  return cost;
}


int ChkDelVab(char plr,char f)
{
  int i,j,k;
  for (i=0;i<4;i++) {
    if ((VAS[f][i].qty-VAS[f][i].ac)<=0 && strcmp(VAS[f][i].name,"NONE")!=0) {
      switch(i) {
	      case 0: case 2:
	        if (Data->P[plr].Manned[VAS[f][i].dex].Delay!=0)
           return 0;
	        break;
	      case 1: 
	        if (Data->P[plr].Misc[VAS[f][i].dex].Delay!=0) return 0;
	        break;
    	   case 3:
	        if (VAS[f][i].dex!=4) {
	          if (Data->P[plr].Probe[VAS[f][i].dex].Delay!=0) return 0;
	        }
          else {
	          if (Data->P[plr].Misc[VAS[f][i].dex].Delay!=0) return 0;
          }
	        break;
      }
    }
  }
  return 1;
}

int BuyVabRkt(char plr,int rk,int *q,char mode)
{
  int cost=0;
  if ((Data->P[plr].Rocket[rk%4].Num-Data->P[plr].Rocket[rk%4].Spok)==0) {
      if (mode==1) {
        Data->P[plr].Cash-=maxx(1,Data->P[plr].Rocket[rk%4].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Rocket[rk%4].UnitCost/2);
        Data->P[plr].Rocket[rk%4].Num++;
        q[rk%4]++;
      }
      else cost=maxx(1,Data->P[plr].Rocket[rk%4].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Rocket[rk%4].UnitCost/2);
  }
  if (rk>3 && (Data->P[plr].Rocket[4].Num-Data->P[plr].Rocket[4].Spok)==0)
  {
     if (mode==1) {
       Data->P[plr].Cash-=maxx(1,Data->P[plr].Rocket[4].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Rocket[4].UnitCost/2);
       Data->P[plr].Rocket[4].Num++;
       q[rk]++;
     }
     else cost+=maxx(1,Data->P[plr].Rocket[rk%4].UnitCost-(Data->P[plr].TurnOnly==3)*Data->P[plr].Rocket[rk%4].UnitCost/2);

  }
  return cost;
}

int ChkVabRkt(char plr,int rk,int *q)
{
  if (Data->P[plr].Rocket[rk%4].Delay!=0 && q[rk]==0)
   return 0;
  if (rk>3 && Data->P[plr].Rocket[4].Delay!=0 && q[rk]==0)
   return 0;
  return 1;
}

void ShowVA(char f)
{
  int i;
  MouseOff();
  RectFill(65,130,160,174,3);
  grSetColor(1);
  for (i=0;i<4;i++) {
    if (VAS[f][i].qty<0) grSetColor(9); else grSetColor(1);
    PrintAt(67,136+12*i,&VAS[f][i].name[0]);
    if ((VAS[f][i].qty-VAS[f][i].ac)<0) {
      DispNum(152,136+12*i,0);
      DispNum(128,136+12*i,0);
    } else {
      DispNum(152,136+12*i,VAS[f][i].qty-VAS[f][i].ac);
      DispNum(128,136+12*i,VAS[f][i].sf);
    };
    PrintAt(0,0,"%");
  };
  MouseOn();
 return;
}


void ShowRkt(char *Name,int sf,int qty,char mode)
{
  MouseOff();
  RectFill(65,182,160,190,3);
  if (qty<0 || mode==1) grSetColor(9); else grSetColor(1);
  PrintAt(67,188,&Name[0]);
  if (qty<0) {DispNum(152,188,0);DispNum(128,188,0); }
  else {
    DispNum(152,188,qty);
    DispNum(128,188,sf);
  };
  PrintAt(0,0,"%");
  MouseOn();
  return;
}


void DispVA(char plr,char f)
{
  int retcode,i,j,TotY,IncY;
  int w,h,x1,y1,x2,y2,w2,h2,cx,off=0;
  unsigned char *spix, *dpix, wh;
  GXHEADER local,local2;

  cx=0;
  for (i=0;i<4;i++) if (VAS[f][i].img>0) cx++;
  wh=((cx==1 && VAS[f][3].img>0) || cx==0) ? 7 : 8;
  if (VAS[f][0].img==13 && plr==0) wh=7;
  TotY=0;
  for (i=0;i<4;i++)
    if (VAS[f][i].img>1) TotY+=MI[plr*28+VAS[f][i].img].y2-MI[plr*28+VAS[f][i].img].y1+1;
                                                                     
    // Load Proper Background into Buffer
    x1=MI[plr*28+wh].x1;y1=MI[plr*28+wh].y1;
    x2=MI[plr*28+wh].x2;y2=MI[plr*28+wh].y2;
    w=x2-x1+1;h=y2-y1+1;

    if (plr==0 && VAS[f][0].img==13) {h+=13;off=13;};

    GV(&local,w,h);gxClearVirtual(&local,0);
    gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local,0,0+off,gxSET);
    spix=local.vptr;

    GV(&local2,w,h);
    MouseOff();
    RectFill(178,29,243,179,3);
    gxGetImage(&local2,210-w/2,103-h/2,210-w/2+w,103-h/2+h,0);
    MouseOn();

    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (local.vptr[i]==0x00) local.vptr[i]=local2.vptr[i];
    };
    DV(&local2);


    //for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
    //  if (*spix==0x00) *spix=0x03;
    //  spix++;
    //};


    GV(&local2,w,h);gxClearVirtual(&local2,0);

    cx=0;
    IncY=(h-TotY)/2;
    if (VAS[f][0].img>0) {
      if (!(VAS[f][0].img==13 && plr==0)) IncY=MI[plr*28+VAS[f][0].img].o;
      else IncY=0;
     }

    for (i=0;i<4;i++) {
      if (VAS[f][i].img>0) {
	wh=VAS[f][i].img;
	x1=MI[plr*28+wh].x1;y1=MI[plr*28+wh].y1;
	x2=MI[plr*28+wh].x2;y2=MI[plr*28+wh].y2;
	w2=x2-x1+1;h2=y2-y1+1;
	cx=w/2-w2/2-1;
	gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local2,cx,IncY,gxSET);
	IncY+=h2+1;
      }
    }
    spix=local.vptr; dpix=local2.vptr;

    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (*dpix!=0x00) *spix=*dpix;
      spix++;dpix++;
    };

    //spotxx

  cx=0;
  for (i=0;i<4;i++) if (VAS[f][i].img>0) cx++;
  wh=((cx==1 && VAS[f][3].img>0) || cx==0) ? 7 : 8;
  if (VAS[f][0].img==13 && plr==0) wh=7;


  if (wh==8) {
    x1=MI[plr*28+25].x1;y1=MI[plr*28+25].y1;x2=MI[plr*28+25].x2;y2=MI[plr*28+25].y2;
    y2=y1+TotY-IncY-1;
    w2=x2-x1+1;h2=y2-y1+1;cx=w/2-w2/2-1;
    gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local2,cx,IncY,gxSET);

    spix=local.vptr;  dpix=local2.vptr;
    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (*dpix!=0x00) *spix=*dpix;
      spix++;dpix++;
    };

    x1=MI[plr*28+27].x1;y1=MI[plr*28+27].y1;x2=MI[plr*28+27].x2;y2=MI[plr*28+27].y2;
    w2=x2-x1+1;h2=y2-y1+1;
    gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local2,0,h-h2,gxSET);
    spix=local.vptr;  dpix=local2.vptr;
    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (*dpix!=0x00) *spix=*dpix;
      spix++;dpix++;
    };
  } else {
    x1=MI[plr*28+26].x1;y1=MI[plr*28+26].y1;x2=MI[plr*28+26].x2;y2=MI[plr*28+26].y2;
    w2=x2-x1+1;h2=y2-y1+1;
    gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local2,0,h-h2,gxSET);
    spix=local.vptr;  dpix=local2.vptr;
    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (*dpix!=0x00) *spix=*dpix;
      spix++;dpix++;
    };
  }
  MouseOff();
  DV(&local2);
  gxPutImage(&local,gxSET,210-w/2,103-h/2,0);
  MouseOn();
  DV(&local);
  return;
}

void DispRck(char plr,char wh)
{
  int retcode,i;
  int w,h,x1,y1,x2,y2;
  GXHEADER local,local2;

    x1=MI[plr*28+wh].x1;y1=MI[plr*28+wh].y1;
    x2=MI[plr*28+wh].x2;y2=MI[plr*28+wh].y2;
    w=x2-x1+1;h=y2-y1+1;
    GV(&local,w,h); GV(&local2,w,h);
    gxVirtualVirtual(&vhptr,x1,y1,x2,y2,&local,0,0,gxSET);
    MouseOff();
    RectFill(247,29,313,179,3);
    gxGetImage(&local2,282-w/2,103-h/2,282-w/2+w,103-h/2+h,0);

    for (i=0;i<gxVirtualSize(gxVGA_13,w,h);i++) {
      if (local.vptr[i]==0x00) local.vptr[i]=local2.vptr[i];
    };

    gxPutImage(&local,gxSET,282-w/2,103-h/2,0);
    MouseOn();
    DV(&local2);DV(&local);
  return;
}

void DispWts(int two,int one)
{
  MouseOff();
  RectFill(5,65,140,83,3);

  grSetColor(1);
  PrintAt(5,77,"MAXIMUM PAYLOAD: ");DispNum(0,0,one);

  grSetColor(1);
  PrintAt(5,70,"CURRENT PAYLOAD: ");
  if (one<two) grSetColor(9);
  DispNum(0,0,two);
  MouseOn();
  return;
}

void VAB(char plr)
{
  int i,j,j2,v,val,mis,cnt,sf[8],qty[8],wt[8],wgt,pay[8],tmp,ccc,rk,cwt,ab,ac;
  char Name[8][12],ButOn,temp;
  PreLoadMusic(M_HARDWARE);
  PlayMusic(0);
  CV=0;
begvab:
  mis=FutureCheck(plr,1);
  if (mis==5)
  {
     Vab_Spot=(Data->P[plr].Mission[0].Hard[4] > 0) ? 1 : 0;
     KillMusic();return;
  };

  temp=CheckCrewOK(plr,mis);
   if (temp==1) //found mission no crews
   {
    ClrMiss(plr,mis+3);
    goto begvab;
   }

  strcpy(IDT,"i016");
  if (Data->P[plr].Mission[mis].Hard[4]>0) {
	 for (i=0;i<4;i++) {
	   switch(i) {
	     case 0: case 2:  // Manned+LM
	       Data->P[plr].Manned[Data->P[plr].Mission[mis].Hard[i] ].Spok--;
	       break;
	     case 1:  // Kicker
	       Data->P[plr].Misc[Data->P[plr].Mission[mis].Hard[i]].Spok--;
	       break;
	     case 3:  // DM+Probes
	       if (Data->P[plr].Mission[mis].Hard[i]==4) Data->P[plr].Misc[4].Spok--;
	       else Data->P[plr].Probe[ Data->P[plr].Mission[mis].Hard[i]].Spok--;
	       break;
	   }
	 }

	 Data->P[plr].Rocket[(Data->P[plr].Mission[mis].Hard[4]-1)%4].Spok--;
	 if (Data->P[plr].Mission[mis].Hard[4]>3) Data->P[plr].Rocket[4].Spok--;
     
  };
  BuildVAB(plr,mis,0,0,0);  // now holds the mission info
  // Rocket Display Data --------------------------
  for (i=0;i<7;i++) {
    if (i>3) {
      sf[i]=(Data->P[plr].Rocket[i-4].Safety+
		Data->P[plr].Rocket[4].Safety)>>1;
      strcpy(&Name[i][0],"B/");
      strcat(&Name[i][0],&Data->P[plr].Rocket[i-4].Name[0]);
      qty[i]=Data->P[plr].Rocket[i-4].Num-Data->P[plr].Rocket[i-4].Spok;
      tmp=Data->P[plr].Rocket[4].Num-Data->P[plr].Rocket[4].Spok;
      if (tmp<qty[i]) qty[i]=tmp;
      pay[i]=(Data->P[plr].Rocket[i-4].MaxPay+Data->P[plr].Rocket[4].MaxPay);
    } else {
       sf[i]=Data->P[plr].Rocket[i].Safety;
       strcpy(&Name[i][0],&Data->P[plr].Rocket[i].Name[0]);
       qty[i]=Data->P[plr].Rocket[i].Num-Data->P[plr].Rocket[i].Spok;
       pay[i]=Data->P[plr].Rocket[i].MaxPay;
    }
  }

  DispVAB(plr,mis);
  if (Data->P[plr].Mission[mis].MissionCode>0) ButOn=1;
  else {ButOn=0;InBox(245,5,314,17);};
  wgt=0;ccc=(VASqty>0)? 1:0;
  for (i=0;i<4;i++) wgt+=VAS[1][i].wt;
  rk=0;while(pay[rk]<wgt) rk++;
  ccc=1;
  ShowVA(ccc);
  ShowRkt(&Name[rk][0],sf[rk],qty[rk],pay[rk]<wgt);
  DispRck(plr,rk);
  DispVA(plr,ccc);
  cwt=0;for (i=0;i<4;i++) cwt+=VAS[ccc][i].wt;
  DispWts(cwt,pay[rk]);
  MouseOn();
  FadeIn(2,pal,10,0,0);
  while(1)  { GetMouse();if (mousebuttons==0) break;}
  while (1)
  {
    key=0;GetMouse();
    if (mousebuttons > 0 || key>0)  /* Game Play */
    {

      // AUTO PURCHASE
      if ((x>=6 && y>=86 && x<=163 && y<=94 && mousebuttons>0) || key=='A')
      {
        MouseOff();InBox(6,86,163,94);MouseOn();
        key=0;
        // NEED A DELAY CHECK
        ac=0;
        ac=ChkDelVab(plr,ccc);
        if (ac!=0) ac=ChkVabRkt(plr,rk,&qty[0]);
        ab=FillVab(plr,ccc,0);
        ab+=BuyVabRkt(plr,rk,&qty[0],0);

        if (Data->P[plr].Cash>=ab && ac!=0) {
          FillVab(plr,ccc,1);
          BuyVabRkt(plr,rk,&qty[0],1);
          BuildVAB(plr,mis,0,0,1);
          
    	 // Rocket Display Data --------------------------
	    for (i=0;i<7;i++) {
	     if (i>3) {
	      sf[i]=(Data->P[plr].Rocket[i-4].Safety+
		      Data->P[plr].Rocket[4].Safety)>>1;
	      strcpy(&Name[i][0],"B/");
	      strcat(&Name[i][0],&Data->P[plr].Rocket[i-4].Name[0]);
	      qty[i]=Data->P[plr].Rocket[i-4].Num-Data->P[plr].Rocket[i-4].Spok;
	      tmp=Data->P[plr].Rocket[4].Num-Data->P[plr].Rocket[4].Spok;
	      if (tmp<qty[i]) qty[i]=tmp;
	       pay[i]=(Data->P[plr].Rocket[i-4].MaxPay+Data->P[plr].Rocket[4].MaxPay);
	     } else {
	      sf[i]=Data->P[plr].Rocket[i].Safety;
	      strcpy(&Name[i][0],&Data->P[plr].Rocket[i].Name[0]);
	      qty[i]=Data->P[plr].Rocket[i].Num-Data->P[plr].Rocket[i].Spok;
	      pay[i]=Data->P[plr].Rocket[i].MaxPay;
	     }
	    }
	   }
	   else if (ac==0) Idiot("i135");  // delay on purchase
	    else Idiot("i137");  //not enough money
     ShowVA(ccc);ShowRkt(&Name[rk][0],sf[rk],qty[rk],pay[rk]<wgt);
     MouseOff();OutBox(6,86,163,94);MouseOn();
    }

     if ((x>=177 && y>=185 && x<=242 && y<=195 && mousebuttons>0) || (key==K_ESCAPE || key=='E'))
      {
	      MouseOff();InBox(177,185,242,195);MouseOn();
	      while(1)  { GetMouse();if (mousebuttons==0) break;}
        if (key>0) delay(150);
	      MouseOff();OutBox(177,185,242,195);MouseOn();
	      for (i=0;i<5;i++) Data->P[plr].Mission[mis].Hard[i]=0;
        goto begvab; /* CONTINUE/EXIT/DO NOTHING */
      }
      else
      if (((x>=249 && y>=185 && x<=314 && y<=195 && mousebuttons>0) || key=='S') && Data->P[plr].Mission[mis].MissionCode!=0)
	    {
  	     MouseOff();InBox(249,185,314,195);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
        if (key>0) delay(100);
	     MouseOff();OutBox(249,185,314,195);MouseOn();
       ClrMiss(plr,mis);
	     if (Data->P[plr].Mission[mis].MissionCode==0) goto begvab;  // SCRUB The whole mission
	   }
     else
	  if (((x>=245 && y>=5 && x<=314 && y<=17 && mousebuttons>0) || key==K_ENTER) && ccc!=0 && ButOn==1 && cwt<=pay[rk])
	   {
	    j=0;
      if (Mis.EVA==1 && Data->P[plr].Misc[3].Num==-1) Idiot("i118");
      else if (Mis.Doc==1 && Data->P[plr].Misc[4].Num==-1) Idiot("i119");
      else
      {
      if ((Mis.mVab[0]&0x10)==0x10 && Data->P[plr].DMod<=0)
        Idiot("i155");  // No docking module in orbit

      j=j2=0;
      if (strncmp((char *)VAS[ccc][0].name,"NONE",4)!=0) {
        j++;
        if ((Data->P[plr].Manned[VAS[ccc][0].dex].Num-Data->P[plr].Manned[VAS[ccc][0].dex].Spok)>0) j2++;
      }
      if (strncmp((char *)VAS[ccc][1].name,"NONE",4)!=0) {
        j++;
        if ((Data->P[plr].Misc[VAS[ccc][1].dex].Num-Data->P[plr].Misc[VAS[ccc][1].dex].Spok)>0) j2++;
      }
      if (strncmp((char *)VAS[ccc][2].name,"NONE",4)!=0) {
        j++;
        if ((Data->P[plr].Manned[VAS[ccc][2].dex].Num-Data->P[plr].Manned[VAS[ccc][2].dex].Spok)>0) j2++;
      }
      if (strncmp((char *)VAS[ccc][3].name,"NONE",4)!=0) {
        j++;
        if  (((Data->P[plr].Probe[VAS[ccc][3].dex].Num-Data->P[plr].Probe[VAS[ccc][3].dex].Spok)>0) 
            || ((Data->P[plr].Misc[VAS[ccc][3].dex].Num-Data->P[plr].Misc[VAS[ccc][3].dex].Spok)>0))
           j2++;
        }
      j++;
      if (qty[rk]>0) j2++;

	      if (j==j2) {
	         MouseOff();InBox(245,5,314,17);MouseOn();
	         while(1)  { GetMouse();if (mousebuttons==0) break;}
              if (key>0) delay(150);
	         MouseOff();OutBox(245,5,314,17);MouseOn();
	         for (i=0;i<4;i++) {
	            Data->P[plr].Mission[mis].Hard[i]=VAS[ccc][i].dex;
              if (VAS[ccc][i].dex>=0) {
	              switch(i) {
		             case 0: case 2:  // Manned+LM
		               Data->P[plr].Manned[VAS[ccc][i].dex].Spok++;
	                  break;
	                case 1:  // Kicker
	                  Data->P[plr].Misc[VAS[ccc][i].dex].Spok++;
	                  break;
	                case 3:  // DM+Probes
	                  if (VAS[ccc][i].dex==4) Data->P[plr].Misc[4].Spok++;
	                  else Data->P[plr].Probe[VAS[ccc][i].dex].Spok++;
	                  break;
                 }
	            }
	         }
	         Data->P[plr].Mission[mis].Hard[4]=rk+1;
	         Data->P[plr].Rocket[rk%4].Spok++;
	         if (rk>3) Data->P[plr].Rocket[4].Spok++;
           goto begvab;
	      }
     }
    }
    else
    if ((x>=64 && y>=181 && x<=161 && y<=191 && mousebuttons>0) || key=='R')
     {
    	MouseOff();InBox(64,181,161,191);MouseOn();
      #define Misdef(a)     Data->P[plr].Mission[(a)].MissionCode 
      rk++;
      if (rk>6) rk=0;
      if ((Misdef(mis)>=42 && Misdef(mis)<=57 || Misdef(mis)>=7 && Misdef(mis)<=13) && (rk==4 || rk==0))
        rk++;
      
	   ShowRkt(&Name[rk][0],sf[rk],qty[rk],pay[rk]<wgt);
	   DispWts(cwt,pay[rk]);
	   DispRck(plr,rk);
	   while(1)  { GetMouse();if (mousebuttons==0) break;}
      if (key>0) delay(100);
	   MouseOff();OutBox(64,181,161,191);MouseOn();
	   /* Rocket Choose */
     }
    else
    if ((x>=64 && y>=129 && x<=161 && y<=175 && mousebuttons>0) || key=='P')
     {
	   MouseOff();InBox(64,129,161,175);MouseOn();
	   ccc++; if (ccc>VASqty) ccc=0;
	   cwt=0;for (i=0;i<4;i++) cwt+=VAS[ccc][i].wt;
	   ShowVA(ccc);DispWts(cwt,pay[rk]);
	   DispVA(plr,ccc);
	   while(1)  { GetMouse();if (mousebuttons==0) break;}
      if (key>0) delay(100);
	   MouseOff();OutBox(64,129,161,175);MouseOn();
	   /* RIGHT Choose */
     };
    }
  }
}


// VAB Auto Build Functions

void BuildVAB(char plr,char mis,char ty,char pa,char pr)
{
  char i,j,part,mcode,prog,ext=0,fin;
  unsigned char VX;

  memset(VAS,0x00,sizeof VAS);

  if (ty==0) {
    part=Data->P[plr].Mission[mis].part;
    mcode=Data->P[plr].Mission[mis].MissionCode;
    prog=Data->P[plr].Mission[mis].Prog-1;

    if (part==0) {
      if (Data->P[plr].Mission[mis].Joint==1)
	      ext=Data->P[plr].Mission[mis+1].Prog-1;
      }
    else ext=prog;
  } else {                              // ty>0 for AI code
    part=pa;                            // Mission Part to Check
    mcode=mis;                          // Mission to Check
    prog=ext=pr;                        // Manned Program to Check
  }

  GetMisType(mcode);

  VX=Mis.mVab[part];
  
  for (i=0;i<7;i++)
    for (j=0;j<5;j++) {
      strcpy(&VAS[i][j].name[0],"NONE");
      VAS[i][j].qty=VAS[i][j].sf=VAS[i][j].wt=0;
      VAS[i][j].dex=VAS[i][j].img=-1;
    };
#if 0
  if (Mis.EVA==1) {
    for (i=1;i<7;i++) {
      VAS[i][4].qty=1;  // Set Flag to signal EVA on Mission
      VAS[i][4].dex=Data->P[plr].Misc[3].Num; // Copy qty if have stuff
      VAS[i][4].sf=Data->P[plr].Misc[3].Safety; // EVA safety factor
    }
  }
#endif

  VASqty=0;
  if (VX&0x80) { // Capsule
    j=(part==0) ? prog:ext;
    for (i=1;i<6;i++) {  // Fill all parts with CAP
      VASqty++;
      VVals(plr,0,&Data->P[plr].Manned[j],j,13+j);
    };
  }

  VASqty=0;

  if (VX == 0x20 && part==0 && mcode==1) { // P:Sxx XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Probe[0],0,9);
  }
  if (VX == 0x20 && part==0 && mcode!=1) { // P:xDM XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
  }
  else if (VX == 0x04 && part==0) { // P:INTER XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Probe[1],1,10);
  }
  else if (VX == 0x02 && part==0) { // P:PRO XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Probe[2],2,11);
  }
  else if (VX == 0x60 && part==0) { // P:LM+SDM XX
    LMAdd(plr,ext,-1,1); 
  }
  else if (VX == 0xe8 && part==0) { // P:LM+SDM+EVA XX
    LMAdd(plr,prog,-1,1);
  }
  else if (VX == 0x61 && part==0) { // P:LM+SDM+KIC XX
    LMAdd(plr,ext,0,1);
    LMAdd(plr,ext,1,1);
  }
  else if (VX == 0x21 && part==0) { // P:SDM+KIC-C XX
    VASqty++;
    VVals(plr,1,&Data->P[plr].Misc[2],2,22);
  }

  else if (VX == 0x80) VASqty=1;  // P/S:CAP XX

  else if (VX == 0x88) { // P/S:CAP+EVA XX
    VASqty=1;
    // EVA Check
  }

  else if (VX == 0xa0 && part==0) { // P:CAP+SDM XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
  }

  else if (VX == 0x90 && part==0) { // P:CAP+DMO XX
    VASqty=1;   // DMO Check
  }

  else if (VX == 0xa8 && part==0) { // P:CAP+SDM+EVA XX
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
    // EVA Check
  }

  else if (VX == 0x98 && part==0) { // P:CAP+DMO+EVA XX
    VASqty=1;
    // EVA Check
    // DMO Check
  }

  else if (VX == 0xe0 && part==0) { // P:CAP+LM+SDM XX
    LMAdd(plr,prog,-1,0);
  }

  else if (VX == 0x81) { // P/S:CAP+KIC XX
    if (prog==1 || prog==3) {
      VASqty++;
      VVals(plr,1,&Data->P[plr].Misc[0],0,20);
      VASqty++;
      VVals(plr,1,&Data->P[plr].Misc[1],1,21);
    } else VASqty=1;
  }

  else if (VX == 0xe1 && part==0) { // P:CAP+LM+SDM+KIC XX
   LMAdd(plr,prog,1,0);
  }

  else if (VX == 0xe9 && part==0) { // P:CAP+LM+SDM+EVA XX
    LMAdd(plr,prog,1,0);
    // EVA Check
  }

  else if (VX == 0x89 && part==1) { // S:CAP+EVA+KIC
   if (prog!=2) {
    VASqty++;
    VVals(plr,1,&Data->P[plr].Misc[0],0,20);
    VASqty++;
    VVals(plr,1,&Data->P[plr].Misc[1],1,21);
   } else VASqty=1;
    // EVA Check
  }
  return;
};


void LMAdd(char plr,char prog,char kic,char part)
{
  if (prog==1) {
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
    VVals(plr,2,&Data->P[plr].Manned[6],6,19);
    if (kic>=0) VVals(plr,1,&Data->P[plr].Misc[kic],kic,20+kic);

    }

  else if (prog==2) {
    VASqty++;
    VVals(plr,2,&Data->P[plr].Manned[5],5,18);
    if (part==1 && kic>=0) VVals(plr,1,&Data->P[plr].Misc[kic],kic,20+kic);

    VASqty++;
    VVals(plr,2,&Data->P[plr].Manned[6],6,19);
    if (part==1 && kic>=0) VVals(plr,1,&Data->P[plr].Misc[kic],kic,20+kic);
    }

  else if (prog==3) {  // Minishuttle
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
    VVals(plr,2,&Data->P[plr].Manned[5],5,18);
    if (kic>=0) VVals(plr,1,&Data->P[plr].Misc[kic],kic,20+kic);
    VASqty++;
    VVals(plr,3,&Data->P[plr].Misc[4],4,12);
    VVals(plr,2,&Data->P[plr].Manned[6],6,19);
    if (kic>=0) VVals(plr,1,&Data->P[plr].Misc[kic],kic,20+kic);
    }
  return;
}

void VVals(char plr,char tx,struct Equipment *EQ,char v4,char v5)
{
  strcpy(&VAS[VASqty][tx].name[0],&EQ->Name[0]); 
  VAS[VASqty][tx].qty=EQ->Num;
  VAS[VASqty][tx].ac=EQ->Spok;
  VAS[VASqty][tx].wt=EQ->UnitWeight; 
  if (tx==3 && v4==4 && AI[plr]==1) VAS[VASqty][tx].sf=EQ->MSF;
  else VAS[VASqty][tx].sf=EQ->Safety;
  VAS[VASqty][tx].dex=v4;VAS[VASqty][tx].img=v5;
  return;
}

// EOF
