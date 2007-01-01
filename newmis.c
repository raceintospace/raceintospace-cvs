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
#include "externs.h"

char Month[12][11] = {
    "JANUARY ","FEBRUARY ","MARCH ","APRIL ","MAY ","JUNE ",
    "JULY ","AUGUST ","SEPTEMBER ","OCTOBER ","NOVEMBER ","DECEMBER "};

struct order Order[7] ;
unsigned int colss,VBlank;
extern char AI[2];
extern struct mStr Mis;
extern char dg[62][6];
extern char Option,MAIL,BIG;

char OrderMissions(void)
{
  int i,j,k;
  memset(Order,0x00,sizeof Order);
   // Sort Missions for Proper Order
   k=0;
   for (i=0;i<2;i++)
     for (j=0;j<3;j++)
       if (Data->P[i].Mission[j].MissionCode>0 && Data->P[i].Mission[j].part!=1) {
	 Order[k].plr=i;
	 Order[k].loc=j;
	 Order[k].budget=Data->P[i].Budget;
	 Order[k].date=Data->P[i].Mission[j].Month;
	 k++;
       };

   for (i=0;i<k-1;i++)
     for (j=i+1;j<k;j++) {
       if (Order[j].date<Order[i].date) {
	 Order[6].plr=Order[i].plr;
	 Order[6].loc=Order[i].loc;
	 Order[6].budget=Order[i].budget;
	 Order[6].date=Order[i].date;

	 Order[i].plr=Order[j].plr;
	 Order[i].loc=Order[j].loc;
	 Order[i].budget=Order[j].budget;
	 Order[i].date=Order[j].date;

	 Order[j].plr=Order[6].plr;
	 Order[j].loc=Order[6].loc;
	 Order[j].budget=Order[6].budget;
	 Order[j].date=Order[6].date;
       };
       if ((Order[j].date==Order[i].date) &&
	   (Order[j].budget<Order[i].budget)) {
	 Order[6].plr=Order[i].plr;
	 Order[6].loc=Order[i].loc;
	 Order[6].budget=Order[i].budget;
	 Order[6].date=Order[i].date;

	 Order[i].plr=Order[j].plr;
	 Order[i].loc=Order[j].loc;
	 Order[i].budget=Order[j].budget;
	 Order[i].date=Order[j].date;

	 Order[j].plr=Order[6].plr;
	 Order[j].loc=Order[6].loc;
	 Order[j].budget=Order[6].budget;
	 Order[j].date=Order[6].date;
       };
     };
  if (MAIL==-1 && Option==-1 && AI[0]==0 && AI[1]==0 && k!=0) MisOrd(k);
  return k;
}

void MisOrd(char num)
{
  int i,j=0,mo;
  MouseOff();
  ShBox(63,19,257,173);
  InBox(74,36,246,163);
  grSetColor(36);PrintAt(77,30,"       LAUNCH ORDER");

  for (i=0;i<num;i++) {
    InBox(78,39+21*j,105,55+21*j);
    FlagSm(Order[i].plr,79,40+21*j);
    grSetColor(34);
    PrintAt(110,45+21*j,"SCHEDULED LAUNCH");
	PrintAt(110,52+21*j,"DATE: ");
    grSetColor(1);

    PrintAt(0,0,
      Month[Data->P[Order[i].plr].Mission[Order[i].loc].Month]);

    PrintAt(0,0," 19");DispNum(0,0,Data->Year);
    j++;
  };
  FadeIn(2,pal,10,0,0);
  MouseOn();
  while(1)  { GetMouse();if (mousebuttons==0) break;}
  while (1)
  {
    key=0;GetMouse();
    if (mousebuttons > 0 || key > 0)
	 while(1)  { GetMouse();if (mousebuttons==0)
	   {FadeOut(2,pal,10,0,0);return;}}
  };
}


void MisAnn(char plr,char pad)
{
  int i,j,bud;
  struct mStr Mis2;
  char temp,k,hold,Digit[4],IdiotFlag=0;
  MouseOff();
  for (i=0;i<768;i++) pal[i]=0;
  gxSetDisplayPalette(pal);
  gxClearDisplay(0,0);

  PortPal(plr);
  ShBox(41,20,281,184);InBox(46,25,276,179);
  InBox(46,25,117,65);Flag(47,26,plr);
  InBox(122,25,276,65);
  grSetColor(34);
  PrintAt(154,33,"SCHEDULED LAUNCH");
  PrintAt(127,40,"LAUNCH FACILITY: ");
  grSetColor(1);
  PrintAt(0,0,"PAD ");
  switch(pad) {
	case 0: PrintAt(0,0,"A");break;
	case 1: PrintAt(0,0,"B");break;
	case 2: PrintAt(0,0,"C");break;
  };
  grSetColor(34);
  PrintAt(127,47,"DATE: ");
  grSetColor(1);

  PrintAt(0,0,Month[Data->P[plr].Mission[pad].Month]);

  PrintAt(0,0," 19");DispNum(0,0,Data->Year);
  grSetColor(1);

  GetMisType(Data->P[plr].Mission[pad].MissionCode);
  memcpy(&Mis2,&Mis,sizeof Mis);


  if ((Mis.mVab[0]&0x10)==0x10 && Data->P[plr].DMod<=0) {
     i=0;
     while (dg[Mis2.Index][i]!=0 && Mis.Doc==1) {
        GetMisType(dg[Mis2.Index][i]);
        i++;
     }
     if (dg[Mis2.Index][i]==0) Data->P[plr].Mission[pad].MissionCode=4;  // new mission
     else Data->P[plr].Mission[pad].MissionCode=Mis.Index;  // new mission
     GetMisType(Data->P[plr].Mission[pad].MissionCode);
     IdiotFlag=1;
  }

  PrintAt(127,54,Mis.Abbr);

  IOBox(57,68,118,84);IOBox(131,68,197,84);IOBox(205,68,266,84);
  grSetColor(1);PrintAt(65,78,"CONTINUE");PrintAt(137,78,"PLAY FULL");
  PrintAt(221,78,"SCRUB");
  grSetColor(9);
  PrintAt(65,78,"C");PrintAt(137,78,"P");PrintAt(221,78,"S");

  //IOBox(85,68,158,84);IOBox(172,68,245,84);
  //grSetColor(1);PrintAt(102,78,"CONTINUE");PrintAt(189,78,"SCRUB");
  //grSetColor(9);
  //PrintAt(102,78,"C");PrintAt(189,78,"S");

  if (Data->P[plr].Mission[pad].Joint==0) PrintAt(129,91,"SINGLE LAUNCH");
    else 
     {
      PrintAt(129,91,"JOINT LAUNCH");
      PrintAt(49,101,"PART: ONE");
      PrintAt(102,101,"PAD: ");
      //DispNum(0,0,pad);
      switch(pad) {
	      case 0: PrintAt(0,0,"A");break;
	      case 1: PrintAt(0,0,"B");break;
	      case 2: PrintAt(0,0,"C");break;
      };

      PrintAt(160,101,"PART: TWO");
      PrintAt(213,101,"PAD: ");
      //DispNum(0,0,pad+1);
      switch(pad+1) {
	      case 0: PrintAt(0,0,"A");break;
	      case 1: PrintAt(0,0,"B");break;
	      case 2: PrintAt(0,0,"C");break;
      };

     }
for (i=0;i<Data->P[plr].Mission[pad].Joint+1;i++)
  {
  k=0;if (i==0) bud=49; else bud=160;
  for(j=0;j<5;j++)
   {
    hold=Data->P[plr].Mission[pad+i].Hard[j];
    switch(j)
     {
      case 0:if (hold>-1) 
	       {
		grSetColor(7);PrintAt(bud,109+14*k,"CAPSULE: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Manned[hold].Name[0]);
		grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
     itoa(Data->P[plr].Manned[hold].Safety,&Digit[0],10);
     PrintAt(0,0,&Digit[0]);
     PrintAt(0,0,"%");
	 //	PrintAt(144+i*111,116+14*k,"%");
		++k;
	       };
	     break;
      case 1:if (hold>-1)
	       {
		grSetColor(7);PrintAt(bud,109+14*k,"KICKER: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Misc[hold].Name[0]);
     grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
     itoa(Data->P[plr].Misc[hold].Safety,&Digit[0],10);
     PrintAt(0,0,&Digit[0]);
     PrintAt(0,0,"%");
    // DispNum(0,0,Data->P[plr].Misc[hold].Safety);
	  //	PrintAt(144+i*111,116+14*k,"%");  
		++k;
	       };
	      break;
       case 2:if (hold>-1)          
		{
		 grSetColor(7);PrintAt(bud,109+14*k,"LM: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Manned[hold].Name[0]);
		 grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
      itoa(Data->P[plr].Manned[hold].Safety,&Digit[0],10);
      PrintAt(0,0,&Digit[0]);
      PrintAt(0,0,"%");
      //DispNum(0,0,Data->P[plr].Manned[hold].Safety);
		 //PrintAt(144+i*111,116+14*k,"%");  
		 ++k;
		};
	       break;
	case 3:if (hold>-1)
		 {
		  if (hold<3)
		    {
		     grSetColor(7);PrintAt(bud,109+14*k,"PROBE: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Probe[hold].Name[0]);
		     grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
          itoa(Data->P[plr].Probe[hold].Safety,&Digit[0],10);
          PrintAt(0,0,&Digit[0]);
          PrintAt(0,0,"%");
          //DispNum(0,0,Data->P[plr].Probe[hold].Safety);
		     //PrintAt(144+i*111,116+14*k,"%");  
		     ++k;
		    }
		   else if (hold==4)
		    {
		     grSetColor(7);PrintAt(bud,109+14*k,"DOCKING: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Misc[hold].Name[0]);
		     grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
          itoa(Data->P[plr].Misc[hold].Safety,&Digit[0],10);
          PrintAt(0,0,&Digit[0]);
          PrintAt(0,0,"%");
          //DispNum(0,0,Data->P[plr].Misc[hold].Safety);
		     //PrintAt(144+i*111,116+14*k,"%");    
		     ++k;
		    }
		 };
	       break;
	case 4:if (hold>-1)
		 {
		  if (hold<5)
		    {
		     grSetColor(7);PrintAt(bud,109+14*k,"ROCKET: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Rocket[hold-1].Name[0]);
		     grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
          itoa(Data->P[plr].Rocket[hold-1].Safety,&Digit[0],10);
          PrintAt(0,0,&Digit[0]);
          PrintAt(0,0,"%");
          //DispNum(0,0,Data->P[plr].Rocket[hold-1].Safety);
		     //PrintAt(144+i*111,116+14*k,"%");  
		     ++k;
		    }
		  else 
		   {
		    grSetColor(7);PrintAt(bud,109+14*k,"ROCKET: ");grSetColor(1);PrintAt(0,0,&Data->P[plr].Rocket[hold-5].Name[0]);PrintAt(0,0," W/B");
		    grSetColor(11);PrintAt(bud,116+14*k,"SAFETY FACTOR: ");grSetColor(1);
         itoa((Data->P[plr].Rocket[hold-5].Safety+Data->P[plr].Rocket[4].Safety)/2,&Digit[0],10);
         PrintAt(0,0,&Digit[0]);
         PrintAt(0,0,"%");
        // DispNum(0,0,(Data->P[plr].Rocket[hold-5].Safety+Data->P[plr].Rocket[4].Safety)/2);
		   // PrintAt(144+i*111,116+14*k,"%");  
		    ++k;
		   }
		 }
	       break;
	default:break;
    }
   }
  }
  FadeIn(2,pal,10,0,0);
  MouseOn();

  while(1)  { GetMouse();if (mousebuttons==0) break;}
  if (IdiotFlag) Idiot("i156");  // Notification of being an idiot

  while (1)
  {
	key=0;GetMouse();
	if ((x>=59 && y>=70 && x<=116 && y<=82 && mousebuttons > 0) || key==0x0d || key=='C')
	  {
	   MouseOff();InBox(59,70,116,82);MouseOn();
	   while(1)  { GetMouse();if (mousebuttons==0)break;}
	   MouseOff();OutBox(59,70,116,82);MouseOn();
	   FadeOut(2,pal,10,0,0);BIG=0;return;
	  }
	else if ((x>=133 && y>=70 && x<=195 && y<=82 && mousebuttons > 0) || key==0x0d || key=='P')
	  {
	   MouseOff();InBox(133,70,195,82);MouseOn();
	   while(1)  { GetMouse();if (mousebuttons==0)break;}
	   MouseOff();OutBox(133,70,195,82);MouseOn();
	   FadeOut(2,pal,10,0,0);BIG=1;return;
	  }

	 else
	if ((x>=207 && y>=70 && x<=264 && y<=82 && mousebuttons > 0) || key=='S')
	  {
	   MouseOff();InBox(207,70,264,82);MouseOn();
	   while(1)  { GetMouse();if (mousebuttons==0) break;}
	   MouseOff();OutBox(207,70,264,82);MouseOn();
		{
		 ClrMiss(plr,pad);
		}
	   if (Data->P[plr].Mission[pad].MissionCode==0) {FadeOut(2,pal,10,0,0);return;}
	  }
  };
}


void AI_Begin(char plr)
{
  int i,j;
  FILE *fin;
  long len[2];

  MouseOff();
  for (i=0;i<768;i++) pal[i]=0;
  gxSetDisplayPalette(pal);

  fin=sOpen("TURN.BUT","rb",0);
  len[0]=filelength(fileno(fin))-768;
  fread(&pal,768,1,fin);
  fread((char *)screen,len[0],1,fin);
  fclose(fin);
  RLED_img((char *)screen,vhptr.vptr,(unsigned int)len[0],
	   vhptr.w,vhptr.h);

  gxClearDisplay(0,0); ShBox(0,60,319,80);
  grSetColor(6+plr*3);
  if (plr==0) DispBig(15,64,"DIRECTOR OF THE UNITED STATES",0,-1);
  else DispBig(30,64,"CHIEF DESIGNER OF THE USSR",0,-1);
  grSetColor(11);
  grMoveTo(175,122);
  if (Data->Season==0) PrintAt(0,0,"SPRING 19");
  else PrintAt(0,0,"FALL 19");
  DispNum(0,0,Data->Year);
  gxVirtualDisplay(&vhptr,1+110*plr,1,30,85,30+107,85+93,0);
  grSetColor(11);
  PrintAt(60,58,"COMPUTER TURN:  THINKING...");
  PreLoadMusic(M_SOVTYP);
  PlayMusic(0);
  FadeIn(2,pal,10,0,0);
  colss=0;
}

void AI_Done(void)
{
  KillMusic();
  FadeOut(2,pal,10,0,0);
  MouseOff();memset(screen,0x00,64000);MouseOn();
  MouseOn();
}

