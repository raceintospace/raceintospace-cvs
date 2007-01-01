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
#include "serial.h"

#define P8N1 1
#define P7E1 0
#define CONN 1
#define DISC 0
#define OFF 0
#define ON 1
#define C1 "COM1"
#define C2 "COM2"
#define C3 "COM3"
#define C4 "COM4"
#define B1 " 300"
#define B2 "1200"
#define B3 "2400"
#define B4 "9600"

char dial(void);
char DoModem(int);
void AAset(void);
void Toggle_Cfg(int,int);
void Read_Config(void);
void Write_Config(void);
void get_dial(char *);
void DialAt(int x,int y,char *s);
void DrawCenter(void);
void MesCenter(void);
void WriteMsg(void);
void Message(int,int);
void ReadMsg(void);
void Upload(void);
void Dnload(void);
char CheckCarrier(void);
char MPrefs(char);
void DrawMPrefs(char,char);
void TrackPict(char mode);
void killgame(char *fName);

struct CONFIG2 {
	i16 com_port;
	long baud_rate;
	i16 parity;
} fig2;

int C_Status=DISC,R_Status=OFF,OK=OFF;
extern char Option,AI[2];
char *files[10];
extern char Musics,tMusics,Sounds;

//save side for modem play
//remember <pad> klugge

void Message(int msg, int tr)
{
 MouseOff();
 switch(msg)
 {
  case 0:RectFill(233,41,316,53,3);grSetColor(9);
         PrintAt(235,49,"NO CARRIER");C_Status=DISC;
         break;
  case 1:RectFill(233,41,316,53,3);grSetColor(11);
         PrintAt(235,49,"CONNECTED ");C_Status=CONN;
         MouseOn();
         if (tr==300) Idiot("i820");
          else if (tr==1200) Idiot("i821");
           else if (tr==2400) Idiot("i822");
            else if (tr==9600) Idiot("i823");
         MouseOff();
         break;
  case 2:RectFill(107,72,211,81,3);grSetColor(11);
         PrintAt(113,78,"AUTO ANSWER: ");grSetColor(11);
         PrintAt(0,0,"OFF");
         break;
  case 3:RectFill(107,72,211,81,3);grSetColor(11);
         PrintAt(113,78,"AUTO ANSWER: ");grSetColor(9);
         PrintAt(0,0,"ON");R_Status=1;
         break;
  case 4:RectFill(111,61,209,69,3);grSetColor(1);
         PrintAt(116,67,"WAITING FOR CALL");
         break;
  case 5:RectFill(233,41,316,53,3);grSetColor(1);
         PrintAt(235,49,"INTERRUPTED");
         break;
  case 6:RectFill(233,41,316,53,3);grSetColor(1);
         PrintAt(235,49,"RECYCLE");
         break;
  case 9:RectFill(233,41,316,53,3);grSetColor(7);
         PrintAt(235,49,"RINGING");
         break;
  case 11:RectFill(233,41,316,53,3);grSetColor(9);
         PrintAt(235,49,"DIAL ABORTED");
         break;
  default:break;
 }
 MouseOn();
 return;
}

//Modem Play => auto answer
void AAset(void)
{
 MouseOff();
 ShBox(104,46,215,84);
 InBox(110,60,210,70);
 grSetColor(5);PrintAt(118,55,"SET AUTO ANSWER");
 MouseOn();
 return;
}

void DialAt(int x,int y,char *s)
{
  short i;
  if (x!=0 && y!=0) grMoveTo(x,y);
  if (strlen(s) > 100) return;
  for(i=0;i<strlen(s);i++)
   {
    DispChr(s[i]);
    if (s[0]=='1') 
     {
      if (i==0) DispChr('-');
      if (i==3) DispChr('-');
      if (i==6) DispChr('-'); 
     }
    else if (i==2) DispChr('-');
   }
  return;
}

void get_dial(char *old)
{
 int i=0,j,k;

 MouseOff();
 ShBox(104,46,215,130);
 grSetColor(5);
 PrintAt(126,55,"PHONE NUMBER");
 IOBox(126,112,194,126);
 InBox(110,60,210,70);RectFill(111,61,209,69,0);
 for(j=0;j<5;j++)
  {
   IOBox(109+21*j,73,127+21*j,89);
   IOBox(109+21*j,92,127+21*j,108);
   grSetColor(11);
   DispNum(116+21*j,83,j);
   DispNum(116+21*j,102,j+5);
  }
 grSetColor(11);PrintAt(144,121,"ENTER");
 grSetColor(1);
 RectFill(111,61,209,69,0);
 MouseOn();
 key=0;memset(&buffer[1000],0x00,sizeof(15));
 while(1) {GetMouse();if (mousebuttons==0) break;}
 while(1)
  {
   key=0;GetMouse();
   if ((key==0x0d) || (x>=128 && y>=114 && x<=192 && y<=124 && mousebuttons>0))
    {
     MouseOff();InBox(128,114,192,124);MouseOn();
     while(1) {GetMouse();if (mousebuttons==0) break;}
     if (key>0) delay(1000);
     MouseOff();OutBox(128,114,192,124);MouseOn();
     return;
    }
   else 
   if ((key==K_ESCAPE) || (x>=236 && y>=114 && x<=312 && y<=124 && mousebuttons>0))
    {
     MouseOff();InBox(236,114,312,124);MouseOn();
     //modem set the clear
     if (Option!=-1) DoModem(2);
     memset(&buffer[1000],0x00,sizeof(15));
     //redraw prefs screen
     MouseOff();OutBox(236,114,312,124);MouseOn();
     return;
    }
   else
	 if ((i<11) && (key>='0' && key<='9'))
	  {
	   grSetColor(1);
     old[i++]=(char)key;
     MouseOff();
     RectFill(111,61,209,69,0);
     DialAt(115,67,&old[0]);
     MouseOn();
	   key=0;
	  }
   else
	 if (i>0 && key==0x08)
	  {
     grSetColor(1);
     old[--i]=0x00;
     MouseOff();
     RectFill(111,61,209,69,0);
     DialAt(115,67,&old[0]);
     MouseOn();
	  	key=0;
	  }
  if (i<11)
   for(j=0;j<5;j++)
    {
     if (x>=111+21*j && y>=75 && x<=125+21*j && y<=87 && mousebuttons>0)
      {
       MouseOff();InBox(111+21*j,75,125+21*j,87);MouseOn();
       while(1) {GetMouse();if (mousebuttons==0) break;}
       grSetColor(1);
       key=j+0x30;
       old[i++]=key;
       MouseOff();
       RectFill(111,61,209,69,0);
       DialAt(115,67,&old[0]);
       MouseOn();
	     key=0;
       MouseOff();OutBox(111+21*j,75,125+21*j,87);MouseOn();
      }
    }
  if (i<11)
   for(j=0;j<5;j++)
    {
     if (x>=111+21*j && y>=94 && x<=125+21*j && y<=106 && mousebuttons>0)
      {
       MouseOff();InBox(111+21*j,94,125+21*j,106);MouseOn();
       while(1) {GetMouse();if (mousebuttons==0) break;}
       grSetColor(1);
       key=j+0x35;
       old[i++]=key;
       MouseOff();
       RectFill(111,61,209,69,0);
       DialAt(115,67,&old[0]);
       MouseOn();
	     key=0;
       MouseOff();OutBox(111+21*j,94,125+21*j,106);MouseOn();
      }
    }
  }
}

void Read_Config(void)
{
	FILE *f;
                                                 
	if ((f = sOpen("MODUS.CFG", "rb",0)) == NULL) {
		if ((f = sOpen("MODUS.CFG", "wb",0)) != NULL) {
			fig2.com_port = 2;
			fig2.baud_rate = 2400;
			fig2.parity = P8N1;
			fwrite(&fig2,sizeof(fig2), 1, f);
        fclose(f);
			return;
		}
		return;
	}
	fread(&fig2, sizeof(fig2), 1, f);
  fclose(f);
}

int e_handler(int c, long p, char *s)
{
	int k;
  MouseOff();
	switch (c)
   {
	  case SENDING:RectFill(93,109,257,119,3);PrintAt(95,116,"SENDING DATA");break;
	  case RECEIVING:RectFill(93,109,257,119,3);PrintAt(95,116,"RECEIVING DATA");break;
     case SENT:p*=2;if (p>=196) p=200;RectFill(55,93,55+(int)p,105,11);break;
	  case RECEIVED:p*=2;if (p>=196) p=200;RectFill(55,93,55+(int)p,105,9);break;
	  case ERROR:break;
	  case COMPLETE:RectFill(93,109,257,119,3);grSetColor(9);PrintAt(95,116,"COMPLETE");break;
    default:break;
 	 }
  MouseOn();
 #if 0
	f (kbhit()) {				
		if (!(k = getch())) { 
			getch();
			return 1;
		}
		if (k == 27)
			return 0;
	}
#endif
	return 1;
}

void Dnload(void)
{
 int set=0;
 char ph[]="PLR0.DAT";
 char pr[]="PLR1.DAT";

  if (Option==0) killgame("PLR1.DAT");
   else killgame("PLR0.DAT");
  memset(buffer,0x00,BUFFER_SIZE);
  Read_CRC();
  if (Option==0) set=recv_file(e_handler, pr);
   else set=recv_file(e_handler, ph);
  if (set==0) {
   Idiot("i777");  //bad transmisssion
   Option=99;
  }
 return;
}

void Upload(void)
{
 int set=0;
 char ph[]="PLR0.DAT";
 char pr[]="PLR1.DAT";

  memset(buffer,0x00,BUFFER_SIZE);
  Read_CRC();
  if (Option==0) files[0] = ph;
   else files[0] = pr;
  files[1] = NULL;
  set=xmit_file(e_handler, files);
  if (set==0) {
   Idiot("i777");   //bad transmission
   Option=99;
  }

 return;
}

void ReadMsg(void)
{
 char old[120];
 int i;
 FILE *f;
 
 if ((f = sOpen((Option==1) ? "SENDH.MSG" : "SENDR.MSG", "r+b",0)) == NULL) {
		if ((f = sOpen((Option==1) ? "SENDH.MSG" : "SENDR.MSG" , "w+b",0)) != NULL) {
        memset(old,0x00,sizeof old);
			fwrite(&old, sizeof(old), 1, f);
		}
	}
 fread(&old, sizeof(old), 1, f);

 RectFill(40,127,197,158,3);
 grSetColor(9);
 for (i=0;i<4;i++)
   PrintAt(44,134+(i*7),&old[i*27]);
 fclose(f);
 return;
}

void WriteMsg(void)
{
 int xlne=44,ylne=77,line=0,max=25,hold=0,i=0;
 char old[120],DN=0;
 FILE *f;
 killgame((Option==0) ? "SENDH.MSG" : "SENDR.MSG");
 if ((f = sOpen((Option==0) ? "SENDH.MSG" : "SENDR.MSG", "r+b",0)) == NULL) {
		if ((f = sOpen((Option==0) ? "SENDH.MSG" : "SENDR.MSG" , "w+b",0)) != NULL) {
        memset(old,0x00,sizeof old);
			fwrite(&old, sizeof(old), 1, f);
		}
	}
 fread(&old,sizeof(old),1,f);
 fclose(f);
 MouseOff();
 key=0;
 RectFill(40,70,197,101,3);
 grSetColor(9);PrintAt(xlne,ylne,"");DispChr(0x14);
 MouseOn();
 while(1)  { GetMouse();if (mousebuttons==0) break;}
 while(DN==0)
  {
   key=0;GetMouse();
   if (mousebuttons > 0 || key>0)
    {
     //BACKSPACE
     if ((i>0 && hold>0 && x>=211 && y>=50 && x<=268 && y<=60 && mousebuttons>0) || (i>0 && hold>0 && key==0x08))
	    {
       MouseOff();InBox(211,50,268,60);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
       if (key>0) delay(250);
       MouseOff();
       grSetColor(11);
       old[--i]=key;--hold;
       RectFill(42,70+line*8,197,77+line*8,3);
       PrintAt(xlne,ylne+(line*7),&old[line*27]);
       grSetColor(9);DispChr(0x14);
       MouseOn();
	  	  key=0;
       MouseOff();OutBox(211,50,268,60);MouseOn();
      }
     else //IOSTREAM 
		if ((line<4 && hold<max) && ((key>=0x21 && key<=0x5A) || key==K_SPACE))
	    {
       MouseOff();
		  grSetColor(11);
       old[i++]=key;hold++;
       RectFill(42,70+line*8,197,77+line*8,3);
       PrintAt(xlne,ylne+(line*7),&old[line*27]);
       //text wrap around
       if (hold==max && line<3) {
        ++line;i=line*27;hold=0;
        RectFill(42,70+line*8,197,77+line*8,3);
        PrintAt(xlne,ylne+(line*7),&old[line*27]);
       }
       grSetColor(9);DispChr(0x14);
       MouseOn();
		  key=0;
		 }
     else //NEXT LINE
     if (key==0x0d && line<3)
      {
       MouseOff();
       grSetColor(11);
       RectFill(42,70+line*8,197,77+line*8,3);
       PrintAt(xlne,ylne+(line*7),&old[line*27]);
       ++line;i=line*27;
       hold=0;
       PrintAt(xlne,ylne+(line*7),&old[line*27]);
       grSetColor(9);DispChr(0x14);
       MouseOn();
       key=0;
      }
     else //ERASE
     if ((x>=211 && y>=67 && x<=268 && y<=77 && mousebuttons>0) || key=='^')
      {
       MouseOff();InBox(211,67,268,77);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}
       i=0;memset(old,0x00,sizeof old);
       MouseOff();
       grSetColor(11);
       for (line=0;line<4;line++)
        RectFill(42,70+line*8,197,77+line*8,3);
       line=0;hold=0;
       grSetColor(9);PrintAt(xlne,ylne,"");DispChr(0x14);
       MouseOn();
       key=0;
       MouseOff();OutBox(211,67,268,77);MouseOn();
     }
    else //DONE
    if ((x>=211 && y>=84 && x<=268 && y<=94 && mousebuttons>0) || key==K_ESCAPE)
     {
      MouseOff();InBox(211,84,268,94);MouseOn();
      while(1)  { GetMouse();if (mousebuttons==0) break;}
      DN=1;
      MouseOff();OutBox(211,84,268,94);MouseOn();
     }
   }  	
 }
 MouseOff();
 grSetColor(11);
 RectFill(42,70+line*8,197,77+line*8,3);
 PrintAt(xlne,ylne+(line*7),&old[line*27]);

 if ((f = sOpen((Option==0) ? "SENDH.MSG" : "SENDR.MSG", "w+b",0)) != NULL)
  {
   old[26]=old[53]=old[80]=old[107]=0x00;
	 fwrite(&old,sizeof(old), 1, f);
	 fclose(f);
  }
 MouseOn();
 return;
}

void Write_Config(void)
{
 FILE *f;

  if ((f = sOpen("MODUS.CFG", "w+b",0)) != NULL) {
   fwrite(&fig2, sizeof(fig2), 1, f);
   fclose(f);
  }
}

void DrawCenter()
{
 char name[20],str[10];
 FadeOut(2,pal,10,0,0);
 MouseOff();
 gxClearDisplay(0,0);
 ShBox(34,32,204,164);ShBox(206,32,273,164);
 InBox(39,49,66,65);InBox(39,106,66,122);
 FlagSm(Option,40,50);FlagSm(other(Option),40,107);
 InBox(39,69,198,102);InBox(39,126,198,159);
 IOBox(209,130,270,144);IOBox(209,147,270,161);
 InBox(210,98,269,127);

 InBox(38,35,200,45);RectFill(39,36,199,44,7);
 InBox(210,35,269,45);RectFill(211,36,268,44,10);

 grSetColor(11);PrintAt(77,42,"TRACKING STATION");
 if (Data->Season==0) strcpy(&name[0],"SPRING "); else strcpy(&name[0],"FALL ");
 itoa(Data->Year,&str[0],10);
 strcat(&name[0],&str[0]);
 if (Data->Season==0) PrintAt(217,42,&name[0]);
  else PrintAt(221,42,&name[0]);
 grSetColor(9);PrintAt(72,55,"OUTGOING MESSAGE");
 grSetColor(0);
 PrintAt(72,62,"DIRECTOR: ");
 grSetColor(1);
 if (Option==0) PrintAt(0,0,&Data->P[ Data->Def.Plr1 ].Name[0]);
  else PrintAt(0,0,&Data->P[ Data->Def.Plr2 ].Name[0]);
 grSetColor(7);PrintAt(72,112,"INCOMING MESSAGE");
 grSetColor(0);
 PrintAt(72,119,"DIRECTOR: ");
 grSetColor(1);
 if (Option==1) PrintAt(0,0,&Data->P[ Data->Def.Plr1 ].Name[0]);
  else PrintAt(0,0,&Data->P[ Data->Def.Plr2 ].Name[0]);
 grSetColor(7);
 grSetColor(7);
 PrintAt(216,139,"WRITE MSG");
 grSetColor(1);
 PrintAt(230,156,"EXIT");
 IOBox(209,48,270,62);
 IOBox(209,65,270,79);
 IOBox(209,82,270,96);
 grSetColor(7);
 PrintAt(215,57,"BACKSPACE");
 grSetColor(11);
 PrintAt(225,74,"ERASE");
 grSetColor(9);
 PrintAt(228,91,"DONE");
 FadeIn(2,pal,10,0,0);
 MouseOn();
 return;
}

void MesCenter(void)
{
 while(1)  { GetMouse();if (mousebuttons==0) break;}

 DrawCenter();
 MouseOff();
 ReadMsg();
 MouseOn();
 strncpy(IDT,"i811",4);
 strncpy(IKEY,"k807",4);
 OK=0;
 while(1)
   {
    key=0;GetMouse();
    if ((x>=211 && y>=149 && x<=268 && y<=159 && mousebuttons>0) || (key==K_ENTER))
     {
      MouseOff();InBox(211,149,268,159);MouseOn();

      delay(250);
	    while(1)  { GetMouse();if (mousebuttons==0) break;}
	    MouseOff();OutBox(211,149,268,159);MouseOn();
	    key=0;return;
     }; //Exit
    if ((x>=211 && y>=132 && x<=268 && y<=142 && mousebuttons>0) || (key=='W'))
     {
      MouseOff();InBox(211,132,268,142);MouseOn();
	    delay(250);
	    while(1)  { GetMouse();if (mousebuttons==0) break;}
      MouseOff();OutBox(211,132,268,142);MouseOn();
      WriteMsg();
	    key=0;
     }; //WRITE MESSAGE
   }
}

//Modem preference screen
void DrawMPrefs(char mode,char a1)
{
 int i,j;
 FILE *fin;
  MouseOff();
  FadeOut(2,pal,10,0,0);
  strcpy(IDT,"i013");strcpy(IKEY,"k806");
  killgame("SENDH.MSG");killgame("SENDR.MSG");
  killgame("PLR0.DAT");killgame("PLR1.DAT");
  fin=sOpen("PREFS.BUT","rb",0);
  fread(&pal[0],768,1,fin);
  i=filelength(fin->fd)-768;
  fread((char *)screen,i,1,fin);
  fclose(fin);
  RLED_img((char *)screen,vhptr.vptr,i,vhptr.w,vhptr.h);

  gxClearDisplay(0,0);
  ShBox(0,0,319,22);ShBox(0,24,89,199);ShBox(91,24,228,107);
  IOBox(98,28,137,63); 
  IOBox(98,68,137,103); 
  IOBox(144,28,221,63); 
  IOBox(144,68,221,103);
  
  ShBox(91,109,228,199);
  InBox(95,113,224,195);
  RectFill(96,114,223,194,0);

  ShBox(230,24,319,56);
  ShBox(230,58,319,130);
  ShBox(230,132,319,199);

  //PreLoadMusic(M_SOVTYP);
  if (mode==0) IOBox(6,105,83,140); else InBox(6,105,83,140);
  if (mode==0) IOBox(6,158,83,193); else InBox(6,158,83,193);

  if (mode==0)
   {
    InBox(6,52,83,87);
    RectFill(7,53,82,86,0);
    OutBox(8,77,18,85);
   }
   else InBox(8,77,18,85);
  BinT(8,54,1);
  RectFill(7,35,82,41,0);

  DispBig(45,5,"MODEM GAME SELECTIONS",0,-1);
  InBox(6,34,83,42);

  PLevels(0,(a1==0) ? Data->Def.Plr1 : Data->Def.Plr2);
  CLevels(0,0);
  Levels(0,(a1==0) ? Data->Def.Lev1 : Data->Def.Lev2,1);
  Levels(0,(a1==0) ? Data->Def.Ast1 : Data->Def.Ast2,0);
  grSetColor(34);
  PrintAt(23,30,"PLAYER 1");
  grSetColor(5);
  PrintAt(20,49,"COUNTRY");
  PrintAt(17,101,"GAME LEVEL");
  PrintAt(20,148,"ASTRONAUT");PrintAt(20,155,"SELECTION");
  grSetColor(1);
  if (a1==0) PrintAt(8,40,&Data->P[Data->Def.Plr1].Name[0]);
   else PrintAt(8,40,&Data->P[Data->Def.Plr2].Name[0]);

  gxVirtualDisplay(&vhptr,153+34*(Data->Def.Music),0,101,31,134,60,0);
  gxVirtualDisplay(&vhptr,221+34*(Data->Def.Sound),0,101,71,134,100,0);
  gxVirtualDisplay(&vhptr,216,30,147,31,218,60,0);
  gxVirtualDisplay(&vhptr,72*(Data->Def.Anim),90,147,71,218,100,0);
  HModel(Data->Def.Input,1);

 InBox(234,28,314,38);RectFill(235,29,313,37,7);
 InBox(234,62,314,72);RectFill(235,63,313,71,7);
 InBox(234,136,314,146);RectFill(235,137,313,145,7);

 grSetColor(11);PrintAt(249,143,"CONFIGURE");
 PrintAt(255,69,"OPTION");
 PrintAt(253,35,"STATUS");

 IOBox(272,150,314,164); //Port
 IOBox(272,166,314,180); //Baud
 IOBox(272,182,314,196); //Parity

 IOBox(234,76,314,90); 
 IOBox(234,94,314,108); 
 IOBox(234,112,314,126); 

 grSetColor(5);PrintAt(243,85,"AUTO ANSWER");
 grSetColor(11);PrintAt(243,103,"DIAL NUMBER");
 grSetColor(9);PrintAt(255,121,"CANCEL");

 Message(0,0);//Message(2,0);

 grSetColor(5);
 PrintAt(235,159,"PORT");
 PrintAt(235,175,"BAUD");
 PrintAt(235,191,"PARITY");

 MouseOn();
 MouseOn();
 return;
}

//function returns Option => Host/Slave
char MPrefs(char mode)
{
 int i,j,fout,num,hum1=0,c_set,p_set,oldc=0;
 char ch,Name[20],ksel=0,lck=0,side=0,gok=0;
 GXHEADER local2;
 long size,b_set;
 FILE *fin;

 C_Status=DISC;
 R_Status=0;
 if (mode==0)
  {
   plr[0]=0;
   Data->Def.Plr1=0;
   hum1=0;
   Data->Def.Lev1=Data->Def.Lev2=0;
   Data->Def.Input=0;
   Data->Def.Sound=Data->Def.Music=1;
   if (Data->Def.Plr1>1) {Data->Def.Plr1-=2;hum1=1;}
  }
 else { //loaded saved game
  if (plr[0]==6 && plr[1]==1) hum1=0;
   else if (plr[0]==0 && plr[1]==7) hum1=1;
  plr[0]=0;plr[1]=1;
  Data->Def.Plr1=plr[0];Data->Def.Plr2=plr[1];
  Data->plr[0]=Data->Def.Plr1;   
	Data->plr[1]=Data->Def.Plr2;
	AI[0]=0;AI[1]=0;
  side=(char)hum1;
 }
 DrawMPrefs(mode,hum1);
 Read_Config();
 Toggle_Cfg(0,fig2.com_port-1);c_set=fig2.com_port-1;
 switch(fig2.baud_rate)
  {
   case 300:b_set=0;break;
   case 1200:b_set=1;break;
   case 2400:b_set=2;break;
   case 9600:b_set=3;break;
   default:b_set=2;break;
  }
 Toggle_Cfg(1,b_set);
 Toggle_Cfg(2,fig2.parity);p_set=fig2.parity;

 MouseOff();grSetColor(34);PrintAt(23,30,"PLAYER");MouseOn();

 FadeIn(2,pal,10,0,0);
 //PlayMusic(0);
 //modem startup help
 Idiot("i806");
 strcpy(IDT,"i806");
 //strcpy(IKEY,"k806");
 while(1)  { GetMouse();if (mousebuttons==0) break;}
 while (1)
  {
   oldc=C_Status;
   if (CheckCarrier()) {if (oldc==0) Message(1,fig2.baud_rate);lck=1;}
    else {if (oldc==1) Message(0,0);if (lck!=3) lck=0;}
   key=0;
   if (lck==1 || lck==2) key=0x0d;
    else GetMouse();
   if (mousebuttons > 0 || key>0)  /* Game Play */
    {
     if ((lck==0 && x>=274 && y>=152 && x<=312 && y<=162 && mousebuttons>0)
         || (lck==0 && key=='1'))
      {
       MouseOff();InBox(274,152,312,162);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}; key=0;
       if (c_set==3) c_set=0;
        else ++c_set;
       fig2.com_port=c_set+1;
       Toggle_Cfg(0,c_set);
       MouseOff();OutBox(274,152,312,162);MouseOn();
      }
    else
    if ((lck==0 && x>=274 && y>=168 && x<=312 && y<=178 && mousebuttons>0)
        || (lck==0 && key=='2'))
      {
       MouseOff();InBox(274,168,312,178);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}; key=0;
       if (b_set==3) b_set=0;
        else ++b_set;
       switch(b_set)
        {
         case 0:fig2.baud_rate=300;break;
         case 1:fig2.baud_rate=1200;break;
         case 2:fig2.baud_rate=2400;break;
         case 3:fig2.baud_rate=9600;break;
         default:fig2.baud_rate=2400;break;
        }
       Toggle_Cfg(1,b_set);
       MouseOff();OutBox(274,168,312,178);MouseOn();
      }
    else
    if ((lck==0 && x>=274 && y>=184 && x<=312 && y<=194 && mousebuttons>0)
        || (lck==0 && key=='3'))
      {
       MouseOff();InBox(274,184,312,194);MouseOn();
       while(1)  { GetMouse();if (mousebuttons==0) break;}; key=0;
       p_set = (p_set==0) ? 1 : 0;
       fig2.parity=p_set;
       Toggle_Cfg(2,p_set);
       MouseOff();OutBox(274,184,312,194);MouseOn();
      }
    else
     //Modem Play => slave/receive side
     if ((R_Status==0 && lck==0 && x>=236 && y>=78 && x<=312 && y<=88 && mousebuttons>0)
         || (R_Status==0 &&  lck==0 && key=='R'))
      {
       MouseOff();InBox(236,78,312,88);MouseOn();
       delay(250);
       while(1)  { GetMouse();if (mousebuttons==0) break;}; key=0;
       Write_Config();
       delay(250);
       Option=1;gok=0;
       gok=DoModem(Option);
       if (gok==0 || gok==2 || gok==3) {
        if (gok==0 || gok==3) DoModem(2);
        lck=0;
        if (gok==3) Idiot("i810");
        if (mode==0) FadeOut(2,pal,10,0,0);
        return 4;
       }
       else {
        lck=3;
        if (R_Status==1) Message(3,0);
        MouseOff();OutBox(236,78,312,88);MouseOn();
       }
      }
     else
      //Modem Play => host/dial out side
     if ((R_Status==0 && lck==0 && x>=236 && y>=96 && x<=312 && y<=106 && mousebuttons>0)
         || (R_Status==0 && lck==0 && key=='D'))
      {
       MouseOff();InBox(236,96,312,106);MouseOn();
       delay(250);
       while(1)  { GetMouse();if (mousebuttons==0) break;}; key=0;
       Write_Config();
       delay(250);
       Option=0;gok=0;
       gok=DoModem(Option);
       if (gok==0 || gok==2 || gok==3)
        {
         if (gok==0 || gok==3) {
          DoModem(2);
          if (gok==0) Message(11,0);
         }
         lck=0;
         if (gok==3) Idiot("i810");
         if (mode==0) FadeOut(2,pal,10,0,0);
         return 4; 
        }
       else
        {
         lck=2;
         if (R_Status==1) Message(3,0);
         MouseOff();OutBox(236,96,312,106);MouseOn();
        }
      }
     else
      //Modem Play => cancel also in function get_dial
     if ((key==K_ESCAPE) || (x>=236 && y>=114 && x<=312 && y<=124 && mousebuttons>0))
      {
       MouseOff();InBox(236,114,312,124);MouseOn();
       //modem set the clear
       if (Option!=-1) DoModem(2);
       if (mode==0) FadeOut(2,pal,10,0,0);
       return 4;
       //redraw prefs screen
       //MouseOff();OutBox(236,114,312,124);MouseOn();
      } else
     if (((lck==1 || lck==2)  && x>=245 && y>=5 && x<=314 && y<=17 && mousebuttons>0) || (key==0x0d && (lck==1 || lck==2)))
      {
	    //  MouseOff();InBox(245,5,314,17);MouseOn();
	    while(1)  { GetMouse();if (mousebuttons==0) break;}
      if (key>0) delay(150); 
	    //  MouseOff();OutBox(245,5,314,17);MouseOn();
      Write_Config();
      if (side==0 && mode==0)
       {
        Data->Def.Plr1=0;
        strcpy(&Data->P[1].Name[0],"OPPONENT");
        Data->Def.Plr2=1;
       }
      else
       {
        if (mode==0) {
         Data->Def.Plr2=1;
         Data->Def.Plr1=0;
         Data->Def.Lev2=Data->Def.Lev1;
         Data->Def.Ast2=Data->Def.Ast1;
         strcpy(&Data->P[1].Name[0],&Data->P[0].Name[0]);
         strcpy(&Data->P[0].Name[0],"OPPONENT");
        }
       }
	    FadeOut(2,pal,10,0,0);
       key=0;
       if (mode==0 && (Data->Def.Input==2 || Data->Def.Input==3))
        {
	      fin=sOpen("HIST.DAT","rb",0);
	      fread(&Data->P[0].Probe[0],28*(sizeof (struct Equipment)),1,fin);
	      fread(&Data->P[1].Probe[0],28*(sizeof (struct Equipment)),1,fin);
	      fclose(fin);
	     }
        if (Data->Def.Input==0 || Data->Def.Input==2)
         { // Hist Crews
	       fin=sOpen("CREW.DAT","rb",0);
          size=filelength(fin->fd);
          fread(buffer,size,1,fin);
          fclose(fin);
	       fin=sOpen("MEN.DAT","wb",1);
         fwrite(buffer,size,1,fin);
         fclose(fin);
	      }
        else
         if (Data->Def.Input==1 || Data->Def.Input==3)
          { // User Crews
	        fin=sOpen("USER.DAT","rb",0); size=filelength(fin->fd);
	        fread(buffer,size,1,fin);fclose(fin);
	        fin=sOpen("MEN.DAT","wb",1);fwrite(buffer,size,1,fin);fclose(fin);
	       }
         //KillMusic();
         if (Data->Def.Music==0 && Musics!=0) 
          {i=tMusics;tMusics=Musics;Musics=i;}
         if (Data->Def.Music==1 && Musics==0)
          {i=tMusics;tMusics=Musics;Musics=i;}

       //Return Values
       // 0 host/us
       // 1 host/sv
       // 2 slave/us
       // 3 slave/sv
       // error

	     return (Option*2+side);
      }
     else
      if ((lck==0 && x>=146 && y>=30 && x<=219 && y<=61 && mousebuttons>0) || (key=='E' && lck==0))
       {
	     MouseOff();InBox(146,30,219,61);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
        if (key>0) delay(150);
	     MouseOff();OutBox(146,30,219,61);MouseOn();
	     GV(&local2,319,199);
	     MouseOff();
	     gxGetImage(&local2,0,0,319,199,0);
	     MouseOn();
        PreLoadMusic(M_FILLER);PlayMusic(0);
	     EditAst();
        strcpy(IDT,"i806");strcpy(IKEY,"K806");
        KillMusic();
	     MouseOff();
	     gxPutImage(&local2,gxSET,0,0,0);
	     MouseOn();
	     DV(&local2);
	     FadeIn(2,pal,10,0,0); 
       }
      else
      if ((mode==0 && x>=96 && y>=114 && x<=223 && y<=194 && mousebuttons>0) || (mode==0 && key==K_SPACE))  // Hist
       {
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
	     Data->Def.Input++;
        if (Data->Def.Input>3) Data->Def.Input=0;
	     HModel(Data->Def.Input,0);
      }
      else
      if ((lck==0 && x>=146 && y>=70 && x<=219 && y<=101 && mousebuttons>0) || (key=='A' && lck==0))
       {
	     MouseOff();InBox(146,70,219,101);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
	     Data->Def.Anim=abs(Data->Def.Anim-1);
	     MouseOff();gxVirtualDisplay(&vhptr,72*(Data->Def.Anim),90,147,71,218,100,0);
	     OutBox(146,70,219,101);MouseOn();
	     /* Anim Level */
       }
      else
      if ((lck==0 && x>=100 && y>=30 && x<=135 && y<=61 && mousebuttons>0) || (key=='M' && lck==0))
       {
	     MouseOff();InBox(100,30,135,61);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
        Data->Def.Music=abs(Data->Def.Music-1);
        MouseOff();gxVirtualDisplay(&vhptr,153+34*(Data->Def.Music),0,101,31,134,60,0);
	     OutBox(100,30,135,61);MouseOn();
	     /* Music Level */
       }
      else
      if ((lck==0 && x>=100 && y>=70 && x<=135 && y<=101 && mousebuttons>0) || (key=='S' && lck==0))
       {
	     MouseOff();InBox(100,70,135,101);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
        Data->Def.Sound=abs(Data->Def.Sound-1);
        MouseOff();gxVirtualDisplay(&vhptr,221+34*(Data->Def.Sound),0,101,71,134,100,0);
	     OutBox(100,70,135,101);MouseOn();
	     /* Sound Level */
       }
      else
      if ((lck==0 && mode==0 && x>=8 && y>=54 && x<=81 && y<=85 && mousebuttons>0) || (key=='C' && mode==0 && lck==0))
       {
        BinT(8,54,0);
	      while(1)  { GetMouse();if (mousebuttons==0) break;}
        side=(side==0) ? 1 : 0;
        PLevels(0,side);
        BinT(8,54,1);
       }
      if ((lck==0 && mode==0 && x>=8 && y>=107 && x<=81 && y<=138 && mousebuttons>0) ||
          (lck==0 && mode==0 && ksel==0 && key=='G'))
       {
	     MouseOff();InBox(8,107,81,138);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
	     MouseOff();OutBox(8,107,81,138);MouseOn();
	     Data->Def.Lev1++;
	     if (Data->Def.Lev1>2) Data->Def.Lev1=0;
	     Levels(0,Data->Def.Lev1,1);
	     /* P1: Game Level */
       }
      else
      if ((lck==0 && mode==0 && x>=8 && y>=160 && x<=81 && y<=191 && mousebuttons>0) ||
          (lck==0 && mode==0 && ksel==0 && key=='L'))
       {
	     MouseOff();InBox(8,160,81,191);MouseOn();
	     while(1)  { GetMouse();if (mousebuttons==0) break;}
	     MouseOff();OutBox(8,160,81,191);MouseOn();
	     Data->Def.Ast1++;
	     if (Data->Def.Ast1>2) Data->Def.Ast1=0;
	     Levels(0,Data->Def.Ast1,0);
	     /* P1: Astro Level */
       }
     else
     if ((lck==0 && mode==0 && x>=6 && y>=34 && x<=83 && y<=42 && mousebuttons>0) ||
       (lck==0 && mode==0 && ksel==0 && key=='N'))
      {
	    MouseOff();RectFill(7,35,82,41,0);
	    for (i=0;i<20;i++) Data->P[0].Name[i]=0x00;
	    num=0;ch=0;
	    grSetColor(1);grMoveTo(8,40);DispChr(0x14);
	    while(ch!=0x0d)
        {
	      ch=getch();
        if (ch!=(ch&0xff)) ch=0x00;
        if (ch>='a' && ch<='z') ch-=0x20;
	      if (ch==0x08 && num>0) Data->P[0].Name[--num]=0x00;
	      else if (num<12 && ( isupper(ch) || isdigit(ch) || ch==0x20))
           Data->P[0].Name[num++]=ch;
	      RectFill(7,35,82,41,0);
	      grSetColor(1);PrintAt(8,40,&Data->P[0].Name[0]);
	      DispChr(0x14);
	   }
	   Data->P[0].Name[num]=0x00;
	   RectFill(7,35,82,41,0);
	   grSetColor(1);PrintAt(8,40,&Data->P[0].Name[0]);
	   MouseOn();
	   /* P1: Director Name */
     }
   }
  };
}

void Toggle_Cfg(int opt,int old)
{
 while(1)  { GetMouse();if (mousebuttons==0) break;}
 MouseOff();
 RectFill(275,153+(opt*16),311,161+(opt*16),3);
 grSetColor(11);
 switch(old)
  {  
   case 0:if (opt==0) PrintAt(283,159,C1); else if (opt==1) PrintAt(283,175,B1);
           else  PrintAt(283,191,"7E1"); 
          break;
   case 1:if (opt==0) PrintAt(283,159,C2); else if (opt==1) PrintAt(283,175,B2);
           else  PrintAt(283,191,"8N1"); 
          break;
   case 2:if (opt!=2)
           {
            if (opt==0) PrintAt(283,159,C3); else if (opt==1) PrintAt(283,175,B3);
           }
          break;
   case 3:if (opt!=2)
           {
            if (opt==0) PrintAt(283,159,C4); else if (opt==1) PrintAt(283,175,B4);
           }
          break;
   default:break;
  };
 MouseOn();key=0;
 return;
}


void TrackPict(char mode)
{
 struct Patch {i16 w,h;ui16 size;long offset;} P;
  GXHEADER local,local2;
  long size;
  unsigned char *buf;
  unsigned int starty;
  int i,j;
  char poff;
  FILE *in;

 MouseOff();
 ShBox(0,0,319,199);
 InBox(7,7,312,192);
 if (mode);
 in=sOpen("TRACKER.BUT","rb",0);
 poff=0;
 fread(&pal[384],384,1,in);
 fseek(in,(poff)*(sizeof P),SEEK_CUR);
 fread(&P,sizeof(P),1,in);

 fseek(in,P.offset,SEEK_SET);
 GV(&local2,P.w,P.h);
 fread(local2.vptr,55936,1,in);
 fclose(in);
 gxPutImage(&local2,gxSET,8,8,0);
 DV(&local2);
 MouseOn();
 return;
}


