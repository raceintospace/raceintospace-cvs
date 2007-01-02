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

extern struct mStr Mis;


int HardCrewAssign(char plr,char Pad,int MisType,char NewType)
{
 int M=0;
 GXHEADER local3;
 if (NewType<=2) Data->P[plr].Future[MisType].Joint=0;
   else Data->P[plr].Future[MisType].Joint=1;

 switch(NewType)
  {
   case 0: return 1;
   case 1: M= HardRequest(plr,0,MisType,Pad);
     	     if (M==0) return 0; else return 1;
   case 2: M= SecondHard(plr,0,MisType,Pad);
	        if (M!=0) M=AsnCrew(plr,Pad,0);
	         else return 0;
	        return (M!=0);
   case 3: M=SecondHard(plr,1,MisType,Pad);
	        Data->P[plr].Future[Pad].part=0;
	        Data->P[plr].Future[Pad].Joint=1;
	        Data->P[plr].Future[Pad].MissionCode=MisType;
	        if (M!=0) M=AsnCrew(plr,Pad+1,1);
	         else return 0;
	        Data->P[plr].Future[Pad+1].Joint=1;
	        Data->P[plr].Future[Pad+1].part=1;
	        Data->P[plr].Future[Pad+1].MissionCode=MisType;
	        if (M==0) return 0; else return 1;
   case 4: M= SecondHard(plr,0,MisType,Pad);
	        GV(&local3,171,188);
	        MouseOff();
	        gxGetImage(&local3,74,3,244,190,0);
	        MouseOn();
	        if (M!=0) M=AsnCrew(plr,Pad,0);
	         else {DV(&local3);return 0;};
	        Data->P[plr].Future[Pad].part=0;
	        Data->P[plr].Future[Pad].Joint=1;
	        Data->P[plr].Future[Pad].MissionCode=MisType;
	        MouseOff();
	        gxPutImage(&local3,gxSET,74,3,0);
	        MouseOn();
	        DV(&local3);
	        if (M!=0) M=SecondHard(plr,1,MisType,Pad);
	         else return 0;
	        if (M!=0) M=AsnCrew(plr,Pad+1,1);
		      else return 0;
	        Data->P[plr].Future[Pad+1].Joint=1;
	        Data->P[plr].Future[Pad+1].part=1;
	        Data->P[plr].Future[Pad+1].MissionCode=MisType;
	        if (M==0) return 0; else return 1;
   case 5: M= SecondHard(plr,0,MisType,Pad+1);
	        Data->P[plr].Future[Pad].part=0;
	        Data->P[plr].Future[Pad].MissionCode=MisType;
	        Data->P[plr].Future[Pad].Joint=1;
	        Data->P[plr].Future[Pad+1].Joint=1;
	        Data->P[plr].Future[Pad+1].part=1;
	        Data->P[plr].Future[Pad+1].MissionCode=MisType;
	        Data->P[plr].Future[Pad+1].Men=0;
	        if (M==0) return 0; else return 1;
   default: break;
  };
 return M; // all the proper hardware and crews have been assigned
}

void ClrFut(char plr,char pad)
{
 char prime,back,men,i,prg;
 prg=Data->P[plr].Future[pad].Prog;
 if (Data->P[plr].Future[pad].PCrew!=0)
  prime=Data->P[plr].Future[pad].PCrew-1; else prime=-1;
 if (Data->P[plr].Future[pad].BCrew!=0)
  back=Data->P[plr].Future[pad].BCrew-1; else back=-1;
 men= Data->P[plr].Future[pad].Men;
 if (prime!=-1)
  for (i=0;i<men;i++)
 	Data->P[plr].Pool[Data->P[plr].Crew[prg][prime][i]-1].Prime=0;
 if (back!=-1)
  for (i=0;i<men;i++)
   Data->P[plr].Pool[Data->P[plr].Crew[prg][back][i]-1].Prime=0;
  if (Data->P[plr].Future[pad].Joint==1 && Data->P[plr].Future[pad+1].part==1)
   {	
    prg=Data->P[plr].Future[pad+1].Prog;
	 if (Data->P[plr].Future[pad+1].PCrew!=0)
	  prime=Data->P[plr].Future[pad+1].PCrew-1; else prime=-1;
	 if (Data->P[plr].Future[pad+1].BCrew!=0)
	  back=Data->P[plr].Future[pad+1].BCrew-1; else back=-1;
	 men=Data->P[plr].Future[pad+1].Men;
	 if (prime!=-1)
	  for (i=0;i<men;i++)
	   Data->P[plr].Pool[Data->P[plr].Crew[prg][prime][i]-1].Prime=0;
	 if (back!=-1)
	  for (i=0;i<men;i++)
	 Data->P[plr].Pool[Data->P[plr].Crew[prg][back][i]-1].Prime=0;
	 Data->P[plr].Future[pad+1].part=0;
	 Data->P[plr].Future[pad+1].Prog=0;
	 Data->P[plr].Future[pad+1].PCrew=0;
	 Data->P[plr].Future[pad+1].BCrew=0;
    Data->P[plr].Future[pad+1].Duration=0;
	 Data->P[plr].Future[pad+1].Joint=0;
	 Data->P[plr].Future[pad+1].Men=0;
	 Data->P[plr].Future[pad+1].MissionCode=0;
	}
 Data->P[plr].Future[pad].Prog=0;
 Data->P[plr].Future[pad].PCrew=0;
 Data->P[plr].Future[pad].BCrew=0;
 Data->P[plr].Future[pad].Men=0;
 Data->P[plr].Future[pad].Duration=0;
 Data->P[plr].Future[pad].Joint=0;
 Data->P[plr].Future[pad].MissionCode=0;
 return;
}

int AsnCrew(char plr,char pad,char part)
{
 int count=0,i,prg=0,grp=-1,prime=-1,men=0,back=-1,t=0,s=0,k=0,yes=0,stflag=0,bug;
 char temp;
 MouseOff();
 strcpy(IKEY,"k200");
 men=Data->P[plr].Future[pad].Men;
 prg=Data->P[plr].Future[pad].Prog;
 if (Data->P[plr].Future[pad].PCrew==0) prime=-1;
   else prime=Data->P[plr].Future[pad].PCrew-1;
 if (Data->P[plr].Future[pad].BCrew==0) back=-1;
   else back=Data->P[plr].Future[pad].BCrew-1;
  if (prime!=-1)
  for (i=0;i<men;i++)
	 Data->P[plr].Pool[Data->P[plr].Crew[prg][prime][i]-1].Prime=0;
  if (back!=-1)
	for (i=0;i<men;i++)
	   Data->P[plr].Pool[Data->P[plr].Crew[prg][back][i]-1].Prime=0;
  Data->P[plr].Future[pad].PCrew=0;
  Data->P[plr].Future[pad].BCrew=0;
  prime=-1;back=-1;
  bug=0;count=0;
   for (i=0;i<8;i++)  // Flight Crew Settings
	{
	 stflag=0;
	 if (Data->P[plr].Crew[prg][i][0]==0 || Data->P[plr].Pool[Data->P[plr].Crew[prg][i][0]-1].Moved==0
	     || Data->P[plr].Pool[Data->P[plr].Crew[prg][i][0]-1].Prime>0) stflag=0;
		   else stflag=1;
	 if (stflag==0) count++; // increment the counter
	}
   if (count>=7) 
    { 
     MouseOn();
     if (temp)EMPTY_BODY;
     temp=Idiot("i107"); 
     bug=1;
     return 0; 
     } 
    else bug=0;
  ShBox(74,3,244,190);IOBox(80,102,157,116);IOBox(161,102,238,116); // primary,secondary
  IOBox(80,7,154,21); IOBox(164,7,238,21); // assign,cancel
  InBox(80,36,238,99); // center screen
  RectFill(81,37,237,98,7+3*plr);
  ShBox(80,24,238,33);
  for (i=0;i<4;i++) {
    IOBox(78,121+16*i,156,139+16*i);
    FutFltsTxt(i,1);
  };
  for (i=0;i<4;i++) {
    IOBox(162,121+16*i,240,139+16*i);
    FutFltsTxt(i+4,1);
  };
  FutSt(plr,prg,prime,back);
  grSetColor(1);
  PrintAt(97,15,"CANCEL");
  grSetColor(9);PrintAt(181,15,"A");grSetColor(1);PrintAt(0,0,"SSIGN");
  PrintAt(86,111,"MAKE ");grSetColor(9);PrintAt(0,0,"P");grSetColor(1);PrintAt(0,0,"RIMARY");
  PrintAt(168,111,"MAKE ");grSetColor(9);PrintAt(0,0,"B");grSetColor(1);PrintAt(0,0,"ACKUP");
  grSetColor(11);
  if (part==0) PrintAt(105,31,"SELECT PRIMARY CREW");
    else PrintAt(100,31,"SELECT SECONDARY CREW");
  grSetColor(1); // reset the color
  count=0;
  MouseOn();
  while(1)  { GetMouse();if (mousebuttons==0) break;}
  while(1)
   {
    key=0;GetMouse();
    for (i=0;i<8;i++)  // Flight Crew Settings
     {
      stflag=0;
      if (Data->P[plr].Crew[prg][i][0]==0 || Data->P[plr].Pool[Data->P[plr].Crew[prg][i][0]-1].Moved==0
	     || Data->P[plr].Pool[Data->P[plr].Crew[prg][i][0]-1].Prime>0) stflag=0;
	       else stflag=1;
      if (stflag==0) ++count; // increment the counter
      t = (i<4) ? 0 : 1;
      yes=0;
      if (t==1) k=i-4; else k=i; // adjust for second side
	    if (!t && x>=80 && y>=123+k*16 && x<=154 && y<=137+k*16 && grp!=i && mousebuttons!=0 && stflag==1)
         yes=1;
	     else if (t && x>=164 && y>=123+k*16 && x<=238 && y<=137+k*16 && (grp!=i) && mousebuttons!=0 && stflag==1)
         yes=1;
          else if (grp!=i && stflag==1 && key=='1'+i) yes=1;
      if (yes==1 && bug==0)
       {
        MouseOff();
	     if (grp!=-1)
         {
          if (grp<4) OutBox(80,123+grp*16,154,137+grp*16);
	        else
            {
             s=grp-4;  OutBox(164,123+s*16,238,137+s*16);
            };
         }
      if (i<4) InBox(80,123+i*16,154,137+i*16);
	     else InBox(164,123+k*16,238,137+k*16);
      grp=i;
      FutAstList(plr,men,Data->P[plr].Crew[prg][grp][0],
		 Data->P[plr].Crew[prg][grp][1],
		 Data->P[plr].Crew[prg][grp][2],
		 Data->P[plr].Crew[prg][grp][3]);
      MouseOn();
      while(1)  { GetMouse();if (mousebuttons==0) break;}
     }
    }
   if (((x>=166 && y>=9 && x<=236 && y<=19 && mousebuttons>0) || key==K_ENTER || key=='A') && prime!=-1 && back!=-1 && bug==0)
	 {   // Assign
	  MouseOff();
	  InBox(166,9,236,19);
	  for (i=0;i<men;i++)
	  	Data->P[plr].Pool[Data->P[plr].Crew[prg][prime][i]-1].Prime=4;
	  Data->P[plr].Future[pad].PCrew=prime+1;
	  Data->P[plr].Future[pad].BCrew=back+1;
	  for (i=0;i<men;i++)
	   Data->P[plr].Pool[Data->P[plr].Crew[prg][back][i]-1].Prime=2;
	  MouseOn();
	  while(1)  { GetMouse();if (mousebuttons==0) break;}
     if (key>0) delay(150);
	  return 1;
	 }
   else
   if ((x>=82 && y>=9 && x<=152 && y<=19 && mousebuttons!=0) || key==K_ESCAPE)
	 {
	  MouseOff();InBox(82,9,152,19);MouseOn();
	  while(1)  { GetMouse();if (mousebuttons==0) break;}
     if (key>0) delay(150);
	  ClrFut(plr,pad);
	  return 0;  // Abort - Redo Mission
	 }
   else
   if (((x>=82 && y>=104 && x<=155 && y<=114 && mousebuttons>0) || key=='P') && prime!=grp && back!=grp && grp!=-1 && bug==0)
	 {
	  MouseOff();InBox(82,104,155,114); prime=grp;MouseOn();
	  FutSt(plr,prg,prime,back);
	  while(1)  { GetMouse();if (mousebuttons==0) break;}
     if (key>0) delay(110);
     MouseOff();OutBox(82,104,155,114);MouseOn();
    } // End Primary Set
   else
   if (((x>=163 && y>=104 && x<=236 && y<=114 && mousebuttons>0) || key=='B')  && prime!=grp && back!=grp && grp!=-1 && bug==0)
     {
      MouseOff();InBox(163,104,236,114); back=grp;MouseOn();
      FutSt(plr,prg,prime,back);
      while(1)  { GetMouse();if (mousebuttons==0) break;}
      MouseOff();OutBox(163,104,236,114);MouseOn();
      if (key>0) delay(110);
     }; // End Backup Set
 }; // end-while
}

void FutFltsTxt(char nw,char col)
{
  int temp=0;
  MouseOff();
  grSetColor(col);
  if (nw < 4) PrintAt(83,129+nw*16,"FLT. CREW ");
    else { temp=nw-4; PrintAt(167,129+temp*16,"FLT. CREW ");};
  switch(nw) {
    case 0: PrintAt(0,0,"I");break;
	case 1: PrintAt(0,0,"II");break;
    case 2: PrintAt(0,0,"III");break;
    case 3: PrintAt(0,0,"IV");break;
    case 4: PrintAt(0,0,"V");break;
    case 5: PrintAt(0,0,"VI");break;
    case 6: PrintAt(0,0,"VII");break;
    case 7: PrintAt(0,0,"VIII");break;
    default: break;
  };
  MouseOn();
  return;
}

void FutSt(char plr,int pr,int p,int b)
{
  int i;
  for (i=0;i<8;i++) {
    if (i==p) FutSt2(i,3);
    if (i==b) FutSt2(i,4);
    if (i!=p && i!=b)
      if (Data->P[plr].Crew[pr][i][0]==0) FutSt2(i,2);
      else if (Data->P[plr].Pool[Data->P[plr].Crew[pr][i][0]-1].Moved==0)
	 FutSt2(i,1);
      else if (Data->P[plr].Pool[Data->P[plr].Crew[pr][i][0]-1].Prime>0)
	FutSt2(i,5);
      else FutSt2(i,0);
  };
  return;
}

void FutSt2(int num,int type)
{
 int temp=0;
 if (num < 4) {  RectFill(84,130+num*16,151,136+num*16,3);
  grMoveTo(84,135+num*16); }
	else { temp=num-4; RectFill(168,130+temp*16,235,136+temp*16,3);
	     grMoveTo(168,135+temp*16); };
  switch(type) {
    case 0: grSetColor(6);PrintAt(0,0,"UNASSIGNED");break;
    case 1: grSetColor(9);PrintAt(0,0,"TRAINING");break;
    case 2: grSetColor(9);PrintAt(0,0,"VACANT");break;
    case 3: grSetColor(11);PrintAt(0,0,"PRIMARY");break;
    case 4: grSetColor(11);PrintAt(0,0,"BACKUP");break;
    case 5: grSetColor(10);PrintAt(0,0,"ASSIGNED");break;
    default: break;
  };
  return;
}

void FutAstList(char plr,char men,int M1,int M2,int M3,int M4)
{
  int i=0,m[4];

  m[0]=M1;m[1]=M2;m[2]=M3;m[3]=M4;
  RectFill(82,38,236,97,7+plr*3); // center screen
  grSetColor(1);
  for (i=0;i<men;i++)
   if (m[i]>0) {
    PrintAt(87,45+i*14,&Data->P[plr].Pool[m[i]-1].Name[0]);
    PrintAt(87,51+i*14,"CP:");
    DispNum(0,0,Data->P[plr].Pool[m[i]-1].Cap);
    PrintAt(0,0,"  LM:");
    DispNum(0,0,Data->P[plr].Pool[m[i]-1].LM);
    PrintAt(0,0,"  EV:");
    DispNum(0,0,Data->P[plr].Pool[m[i]-1].EVA);
    PrintAt(0,0,"  DO:");
    DispNum(0,0,Data->P[plr].Pool[m[i]-1].Docking);
    PrintAt(0,0,"  EN:");
    DispNum(0,0,Data->P[plr].Pool[m[i]-1].Endurance);
   };
  return;
}


void DrawHard(char mode,char pad,char mis,char plr)
{
 strcpy(IKEY,"k201");
 ShBox(75,43,244,173);InBox(81,60,238,95);IOBox(81,154,238,167);// continue
 InBox(81,47,238,56);RectFill(82,61,237,94,6+3*plr);
 grSetColor(11);
 if (mode==0) PrintAt(90,54,"SELECT PRIMARY VEHICLE");
	else PrintAt(90,54,"SELECT SECONDARY VEHICLE");
 grSetColor(1);
 GetMisType(mis);
 PrintAt(85,70,Mis.Abbr);
 //Missions(plr,85,70,mis,0);

 PrintAt(85,85,"PAD: ");DispNum(0,0,pad+1);
 PrintAt(94,109,Data->P[plr].Manned[0].Name);
 PrintAt(174,109,Data->P[plr].Manned[1].Name);
 PrintAt(94,126,Data->P[plr].Manned[2].Name);
 PrintAt(174,126,Data->P[plr].Manned[3].Name);
 PrintAt(137,143,Data->P[plr].Manned[4].Name);
 PrintAt(144,163,"CANCEL");
 return;
}

int HardRequest(char plr,char mode,char mis,char pad)
{
  int i=0,pr[5],t=0;
  for (i=0;i<5;i++)
	if (Data->P[plr].Manned[i].Num>=0) {pr[i]=1;t++;}
	  else pr[i]=0;
  // special case 1-man capsule can't go to the moon
  GetMisType(mis);
  // exceptions
  if (Mis.Lun==1 || Mis.Doc==1 || Mis.mEq>1 || Mis.Days>1 ||
       Data->P[plr].Future[pad].Duration>2) pr[0]=0;

  if (Data->P[plr].Future[pad].Duration>5 || Mis.Days>5) pr[1]=0;

  if (Data->P[plr].Future[pad].Duration>3) pr[3]=0;
  if (mis==54) {
    pr[0]=pr[1]=pr[2]=pr[3]=0;
   }
  if (mis==57 && plr==1) {
    pr[0]=pr[1]=pr[3]=pr[4]=0;
   }
  if (Mis.Doc==1) pr[4]=0;
//  if (t==0) {Idiot("i126"); return 0;};
  MouseOff();

  DrawHard(mode,pad,mis,plr);
   if (pr[0]==0) InBox(81,100,158,114); else IOBox(81,100,158,114);
  if (pr[1]==0) InBox(161,100,238,114); else IOBox(161,100,238,114);
  if (pr[2]==0) InBox(81,117,158,131); else IOBox(81,117,158,131);
  if (pr[3]==0) InBox(161,117,238,131); else IOBox(161,117,238,131);
  if (pr[4]==0) InBox(121,134,198,148); else IOBox(121,134,198,148);
  MouseOn();
  i=0;
  while(1)  { GetMouse();if (mousebuttons==0) break;}
  while(i==0) {
  key=0;GetMouse();
   if (mousebuttons!=0 || key>0) {
     if (((x>=83 && y>=102 && x<=156 && y<=112) || key=='1') && pr[0]) {
       MouseOff();InBox(83,102,156,112);i=1;MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
     } // ONE
     else
       if (((x>=163 && y>=102 && x<=236 && y<=112) || key=='2') && pr[1]) {
       MouseOff();InBox(163,102,236,112);i=2;MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
     } // TWO
     else
     if (((x>=83 && y>=119 && x<=156 && y<=129) || key=='3') && pr[2]) {
       MouseOff();InBox(83,119,156,129);i=3;MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
     } // THREE
     else
     if (((x>=163 && y>=119 && x<=236 && y<=129) || key=='4') && pr[3]) {
       MouseOff();InBox(163,119,236,129);i=4;MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
     } // FOUR
     else
     if (((x>=123 && y>=136 && x<=196 && y<=146) || key=='5') && pr[4]) {
       MouseOff();InBox(123,136,196,146);i=5;MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
     } // FIVE
     else
     if ((x>=83 && y>=156 && x<=236 && y<=165 && mousebuttons!=0) || key==K_ENTER || key==K_ESCAPE) {
     MouseOff();InBox(83,156,236,165);MouseOn();
     while(1)  { GetMouse();if (mousebuttons==0) break;}
     if (key>0) delay(150);
     return 0;  // Abort - Redo Mission
     };
   };
  }; /* End while */
  Data->P[plr].Future[pad].Prog=i;
  return i;
}

int SecondHard(char plr,char mode,char mis,char pad)
{

  int i=0,men=0,prg=0,prog[5],t=0;
  for (i=0;i<5;i++) if (Data->P[plr].Manned[i].Num>=0) men++;
  if (men==0) {Idiot("i126"); return 0;};

  if (mode==1) pad++; // if second part of Joint mission
  for (i=0;i<5;i++)
	if (Data->P[plr].Manned[i].Num>=0) {prog[i]=1;t++;}
	else prog[i]=0;
  GetMisType(mis);
  // exceptions
  if (Mis.Lun==1 || Mis.Doc==1 || Mis.mEq>1 || Mis.Days>2 ||
       Data->P[plr].Future[pad].Duration>2) prog[0]=0;

  if (Data->P[plr].Future[pad].Duration>5 || Mis.Days>5) prog[1]=0;

  if (Data->P[plr].Future[pad].Duration>4) prog[3]=0;
  if (mis==54) {
    prog[0]=prog[1]=prog[2]=prog[3]=0;
   }
  if (Mis.Doc==1) prog[4]=0;
  if (mis==57 && plr==1) {
    prog[0]=prog[1]=prog[3]=prog[4]=0;
   }
  MouseOff();
  DrawHard(mode,pad,mis,plr);
  if (prog[0]==0) InBox(81,100,158,114); else IOBox(81,100,158,114);
  if (prog[1]==0) InBox(161,100,238,114); else IOBox(161,100,238,114);
  if (prog[2]==0) InBox(81,117,158,131); else IOBox(81,117,158,131);
  if (prog[3]==0) InBox(161,117,238,131); else IOBox(161,117,238,131);
  if (prog[4]==0) InBox(121,134,198,148); else IOBox(121,134,198,148);
  MouseOn();
  i=0;
  while(1)  { GetMouse();if (mousebuttons==0) break;}
  while(i==0)
    {
     key=0;GetMouse();
     if (mousebuttons>0 || key>0) {
     if (((x>=83 && y>=102 && x<=156 && y<=112 && mousebuttons!=0) || key=='1') && prog[0]!=0 && prg!=1)
      {
       men=1; MouseOff();InBox(83,102,156,112); MouseOn(); i=1;
       while(1) {GetMouse();if (mousebuttons==0) break;}
      } // One Man Program
      else
     if (((x>=163 && y>=102 && x<=236 && y<=112 && mousebuttons!=0) || key=='2') && prog[1]!=0 && prg!=3)
      {
       men=2; MouseOff();InBox(163,102,236,112); MouseOn();i=2;
       while(1)  { GetMouse();if (mousebuttons==0) break;}
      } // Two Man Program
     else
     if (((x>=83 && y>=119 && x<=156 && y<=129 && mousebuttons!=0) || key=='3') && prog[2]!=0 && prg!=2)
      {
       men=3; MouseOff();InBox(83,119,156,129); MouseOn(); i=3;
       while(1)  { GetMouse();if (mousebuttons==0) break;}
      } // Three Man Program
     else
     if (((x>=163 && y>=119 && x<=236 && y<=129 && mousebuttons!=0) || key=='4') && prog[3]!=0 && prg!=4)
      {
       men=3; MouseOff();InBox(163,119,236,129); MouseOn(); i=4;
       while(1)  { GetMouse();if (mousebuttons==0) break;}
      } // Minishuttle Man Program
     else
     if (((x>=123 && y>=136 && x<=196 && y<=146 && mousebuttons!=0) || key=='5') && prog[4]!=0 && prg!=5)
      {
       men=4; MouseOff();InBox(123,136,196,146); MouseOn(); i=5;
       while(1)  { GetMouse();if (mousebuttons==0) break;}
      } // Four Man Program
     else
     if ((x>=83 && y>=156 && x<=236 && y<=165 && mousebuttons!=0) || key==K_ENTER)
      {
       MouseOff();InBox(83,156,236,165);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
       if (key>0) delay(150);
       return 0;  // Abort - Redo Mission
      };
    }
  }
  Data->P[plr].Future[pad].Prog=i;
  Data->P[plr].Future[pad].Men=men;
  return i;
}


// EOF
