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
//#include "serial.h"
#include "replay.h"

#define LET_A   0x09
#define LET_M   0x0A
#define LET_O   0x0B
#define LET_V   0x0C
#define LET_R   0x08

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CPMEOF 0x1A
#define TIMED_OUT 3
#define BAD_BLOCK 5
#define CRC_ERROR 7
#define ABORT "LOSS OF CARRIER"
#define TCO "TRANSFER COMPLETED"
#define TCR "CANCELLED BY RECEIVER"
#define updcrc(d, s) (buffer[2000+((s >> 8) & 0xff)] ^ (s << 8) ^ d)

extern char Option,MAIL;
struct Prest_Upd MP[3];
extern RPLY Rep;
char StartRecv;

void killgame(char *fName);

void Read_CRC(void)
{
	FILE *f;
	f = sOpen("TABLE.CRC", "rb",0);
	fread(&buffer[2000], sizeof(256), 1, f);
  fclose(f);
}

void SaveSide(char side)
{
  //holds replay data
  struct Repdat {
   unsigned char Num;
   ui16 Off[35];
  };

 struct Need {
  char Name[20];
  char Lev,Ast;
  char SES,YER;
  struct Equipment P[7];
  struct Equipment R[7];
  struct Equipment C[7];
  struct Equipment M[7];
  char astlevel;
  char miscode[3];
  char futcode[3];
  char lfacil[3];
  char table[5][7];
  short PastMis;
  i16 Pst_H[3];
  i16 Pst_S[3];
  struct PrestType Presto[28];
  struct Prest_Upd tempMP[3];
  struct Repdat Reper[3];  //last three missions (REPLAY)
  char Msg[120];
 };
 int i,j,k,length;
 char old[120];
 char msc;
 long size;
 FILE *f,*fin;

 struct Need Temp;

 if (side==0)  killgame("PLR0.DAT");
  else killgame("PLR1.DAT");

 memset(old,0x00,sizeof(old));
 memcpy(&Temp.Name[0],&Data->P[side].Name[0],20);

 if (side==0) {Temp.Lev=Data->Def.Lev1;Temp.Ast=Data->Def.Ast1;}
  else {Temp.Lev=Data->Def.Lev2;Temp.Ast=Data->Def.Ast2;};

 Temp.SES=Data->Season;
 Temp.YER=Data->Year;

 Temp.PastMis=Data->P[side].PastMis;

 //update mission .History PRESTIGE
 for (i=0;i<3;i++)
  {
   //dont need to send HOST prestige values from SLAVE
   if (side==0 && Data->P[0].PastMis-i>0) Temp.Pst_H[i]=Data->P[0].History[Data->P[0].PastMis-i].Prestige;
    else Temp.Pst_H[i]=0;
   if (side==0 && Data->P[1].PastMis-i>0) Temp.Pst_S[i]=Data->P[1].History[Data->P[1].PastMis-i].Prestige;
    else Temp.Pst_S[i]=0;
  }

 //only slave needs to slave MP struct
 for (i=0;i<3;i++)
  memcpy(&Temp.tempMP[i],&MP[i],sizeof(struct Prest_Upd));

#if 1
 // host side has update prestige
 for (i=0;i<28;i++)
  memcpy(&Temp.Presto[i],&Data->Prestige[i],sizeof(struct PrestType));

 for (i=0;i<5;i++)
  for (k=0;k<7;k++)
   Temp.table[i][k]=Data->P[side].Table[i][k];

 Temp.astlevel=Data->P[side].AstroLevel;
 for (i=0;i<3;i++) Temp.miscode[i]=Data->P[side].Mission[i].MissionCode;
 for (i=0;i<3;i++) Temp.futcode[i]=Data->P[side].Future[i].MissionCode;
 for (i=0;i<3;i++) Temp.lfacil[i]=Data->P[side].LaunchFacility[i];
#endif

 for (i=0;i<7;i++) {
  memcpy(&Temp.P[i],&Data->P[side].Probe[i],sizeof(struct Equipment));
  memcpy(&Temp.R[i],&Data->P[side].Rocket[i],sizeof(struct Equipment));
  memcpy(&Temp.C[i],&Data->P[side].Manned[i],sizeof(struct Equipment));
  memcpy(&Temp.M[i],&Data->P[side].Misc[i],sizeof(struct Equipment));
 }

  //Modem Play => REPLAY hack for prestige first
  fin=sOpen("REPLAY.DAT","r+b",1);
  for(i=0;i<3;i++)
   {
    if (((Data->P[side].PastMis-3)+i)<0)  //Specs: was <=0)
     {
      Temp.Reper[i].Num=0;
      for(j=0;j<35;j++) Temp.Reper[i].Off[j]=0;
     }
    else
     {
      if (Data->P[side].History[Data->P[side].PastMis-3+i].MissionCode>=53)
       {
        msc=Data->P[side].History[Data->P[side].PastMis-3+i].MissionCode;
        Temp.Reper[i].Off[33]=msc;
        if (msc==55 || msc==56 || msc==57)
         Temp.Reper[i].Off[34]=Data->P[side].History[Data->P[side].PastMis-3+i].Hard[1][0];
          else Temp.Reper[i].Off[34]=Data->P[side].History[Data->P[side].PastMis-3+i].Hard[0][0];
        Temp.Reper[i].Off[32]=Data->P[side].History[Data->P[side].PastMis-3+i].Patch[side];
       }
      size=(side*100)+((Data->P[side].PastMis-3)+i);
      fseek(fin,size * (sizeof Rep),SEEK_SET);
      fread(&Rep,sizeof Rep,1,fin);
      Temp.Reper[i].Num=Rep.Qty;
      for(j=0;j<32;j++) Temp.Reper[i].Off[j]=Rep.Off[j];
     }
   }
  fclose(fin);


 if ((f = sOpen((side==0) ? "SENDH.MSG" : "SENDR.MSG", "r+b",0)) == NULL) {
		if ((f = sOpen((side==0) ? "SENDH.MSG" : "SENDR.MSG" , "w+b",0)) != NULL) {
       memset(old,0x00,sizeof old);
		 fwrite(&old, sizeof(old), 1, f);
		}
	}
 fread(&old,sizeof(old),1,f);
 fclose(f);

 killgame((side==0) ? "SENDH.MSG" : "SENDR.MSG");

 for (i=0;i<120;i++) Temp.Msg[i]=old[i];

 memset(buffer,0x00,BUFFER_SIZE);
 j=RLEC((char *)&Temp,buffer,sizeof(struct Need));

 if (side==0) fin=sOpen("PLR0.DAT","wb",0);
  else if (side==1) fin=sOpen("PLR1.DAT","wb",0);

 fwrite(&size,sizeof(size),1,fin);

 fwrite((char *)buffer,j,1,fin);
 fseek(fin,0L,SEEK_SET);
 size=filelength(fileno(fin))-sizeof(size);

 fwrite(&size,sizeof(size),1,fin);
 memset(buffer,0x00,BUFFER_SIZE);
 fclose(fin);
 return;
}

void UpdPrestige()
{
 int i,j,which;
 FILE *gork;

 // update prestige based on month/year
 // host machine always has pristine version
 // .Mth is the month of the mission

 //play-by-mail hack
 if (MAIL==1)
  for (i=0;i<3;i++)
   memcpy(&MP[i],&Data->P[1].Udp[i],sizeof(struct Prest_Upd));

#if 0    //write out Soviet prestige values
 gork=sOpen("pbm.xx","at",1);
 fprintf(gork,"PRESTIGE UPDATE\n");
 for (i=0;i<3;i++)
  {
   fprintf(gork,"Index: %d\n",i);
   for (j=0;j<MP[i].Qty;j++)
    fprintf(gork,"Poss: %d Value: %d\n",MP[i].Poss[j],MP[i].PossVal[j]);
   fprintf(gork,"HInd: %d\n",MP[i].HInd);
   fprintf(gork,"Qty: %d\n",MP[i].Qty);
   fprintf(gork,"Mnth: %d\n",MP[i].Mnth);
  }
 fclose(gork);
#endif

 //score play-by-mail USA firsts
 if (MAIL!=-1) 
  for(i=0;i<3;i++)
   for(j=0;j<Data->P[0].Udp[i].Qty;j++)
    {
     which=Data->P[0].Udp[i].Poss[j];
     Data->Prestige[which].Place=0;
     Data->Prestige[which].Points[0]+=Data->Prestige[which].Add[0];
     Data->Prestige[which].Year=Data->Year;
     Data->Prestige[which].Month=Data->P[0].Udp[i].Mnth;
     Data->P[0].History[Data->P[0].Udp[i].HInd].Prestige+=Data->Prestige[which].Add[0];
     Data->Prestige[which].Indec=Data->P[0].Udp[i].HInd;
     Data->P[0].Prestige+=Data->Prestige[which].Add[0];
     Data->Prestige[which].Goal[0]++;
    }

   for(i=0;i<3;i++)
    for(j=0;j<MP[i].Qty;j++)
     {
      which=MP[i].Poss[j];
      switch(MP[i].PossVal[j])
       {
        case 0://check for first already set
               if (Data->Prestige[which].Place!=-1)
                {
                 //check your prestige first
                 if (Data->Prestige[which].Place==1)
                  {
                  //check if lower month then replace
                  //and score subsequent
                  if (MP[i].Mnth<Data->Prestige[which].Month && Data->Year==Data->Prestige[which].Year)
                   {
                    Data->Prestige[which].Month=MP[i].Mnth;
                    Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[2];
                    Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[2];
                    Data->P[1].Prestige+=Data->Prestige[which].Add[2];
                    Data->Prestige[which].Indec=MP[i].HInd;
                    Data->Prestige[which].Goal[1]++;
                   }
                  //else score subsequent
                  else if (Data->Year==Data->Prestige[which].Year)
                   {
                    Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[2];
                    Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[2];
                    Data->P[1].Prestige+=Data->Prestige[which].Add[2];
                    Data->Prestige[which].Goal[1]++;
                   }
                 }
                else
                 {
                  //host first now check lower month of slave
                  if (MP[i].Mnth<Data->Prestige[which].Month && (Data->Year==Data->Prestige[which].Year))
                   {
                    //slave first
                    Data->Prestige[which].Place=1;
                    Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[0];
                    Data->Prestige[which].Year=Data->Year;
                    Data->Prestige[which].Month=MP[i].Mnth;
                    Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[0];
                    Data->Prestige[which].Indec=MP[i].HInd;
                    Data->P[1].Prestige+=Data->Prestige[which].Add[0];
                    Data->Prestige[which].Goal[1]++;

                    //host given second
                    Data->Prestige[which].mPlace=0;
                    Data->Prestige[which].Points[0]-=Data->Prestige[which].Add[0];
                    Data->Prestige[which].Points[0]+=Data->Prestige[which].Add[1];

                    //Adjust host player prestige
                    Data->P[0].Prestige-=Data->Prestige[which].Add[0];
                    Data->P[0].Prestige+=Data->Prestige[which].Add[1];

                    //Adjust host History[???].Prestige
                    Data->P[0].History[Data->Prestige[which].Indec].Prestige-=Data->Prestige[which].Add[0];
                    Data->P[0].History[Data->Prestige[which].Indec].Prestige+=Data->Prestige[which].Add[1];
                   }
                  else 
                   {
                    //slave is second
                    //check second not set
                    if (Data->Prestige[which].mPlace==-1)
                     { //score second to slave
                      Data->Prestige[which].mPlace=1;
                      Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[1];
                      Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[1];
                      Data->P[1].Prestige+=Data->Prestige[which].Add[1];
                      Data->Prestige[which].Goal[1]++;
                     }
                    else
                     {
                      //else score subsequent
                      Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[2];
                      Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[2];
                      Data->P[1].Prestige+=Data->Prestige[which].Add[2];
                      Data->Prestige[which].Goal[1]++;
                     }
                   }
                 }
               }
              else  
               {
                //set first
                Data->Prestige[which].Place=1;
                Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[0];
                Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[0];
                Data->Prestige[which].Indec=MP[i].HInd;
                Data->P[1].Prestige+=Data->Prestige[which].Add[0];
                Data->Prestige[which].Year=Data->Year;
                Data->Prestige[which].Month=MP[i].Mnth;
                Data->Prestige[which].Goal[1]++;
               }
              break;
       case 1:if (Data->Prestige[which].mPlace==-1 && Data->Prestige[which].Place!=1)
               {
                //score second to slave
                Data->Prestige[which].mPlace=1;
                Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[1];
                Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[1];
                Data->P[1].Prestige+=Data->Prestige[which].Add[1];
                Data->Prestige[which].Goal[1]++;
               }
              else 
               {
                //score subsequent to slave
                Data->Prestige[which].mPlace=1;
                Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[2];
                Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[2];
                Data->P[1].Prestige+=Data->Prestige[which].Add[2];
                Data->Prestige[which].Goal[1]++;
               }
              break;
       case 2:Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[2];
              Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[2];
              Data->P[1].Prestige+=Data->Prestige[which].Add[2];
              Data->Prestige[which].Goal[1]++;
              break;
       case 3:Data->Prestige[which].Points[1]+=Data->Prestige[which].Add[3];
              Data->P[1].History[MP[i].HInd].Prestige+=Data->Prestige[which].Add[3];
              Data->P[1].Prestige+=Data->Prestige[which].Add[3];
              Data->Prestige[which].Goal[1]++;
              break;
       default:break;
      }    
    }

 if (MAIL!=-1)
  for (i=0;i<3;i++)
   {
    memset(&Data->P[0].Udp[i],0x00,sizeof(struct Prest_Upd));
    memset(&Data->P[1].Udp[i],0x00,sizeof(struct Prest_Upd));
   };
 return;
}

void RecvSide(char side)
{
  //holds replay data
  struct Repdat {
   unsigned char Num;
   ui16 Off[35];
  };

  struct Need {
  char Name[20];
  char Lev,Ast;
  char SES,YER;
  struct Equipment P[7];
  struct Equipment R[7];
  struct Equipment C[7];
  struct Equipment M[7];
  char astlevel;
  char miscode[3];
  char futcode[3];
  char lfacil[3];
  char table[5][7];
  short PastMis;
  i16 Pst_H[3];
  i16 Pst_S[3];
  struct PrestType Presto[28];
  struct Prest_Upd tempMP[3];
  struct Repdat Reper[3];
  char Msg[120];
 };
 int i,j,k;
 FILE *f,*fin;
 char old[120],p;
 char msc;
 long size;

 struct Need Temp;

 p=other(side);
 memset(buffer,0x00,20480);
 memset(old,0x00,sizeof(old));

 if (side==0) fin=sOpen("PLR1.DAT","rb",0);
  else if (side==1) fin=sOpen("PLR0.DAT","rb",0);

 fread(&size,sizeof(size),1,fin);
 fread((char *)buffer,size,1,fin);
 fclose(fin);

 RLED(buffer,(char *)&Temp,(unsigned int)size);
 
 memcpy(&Data->P[p].Name[0],&Temp.Name[0],20);
 if (p==0) {Data->Def.Lev1=Temp.Lev;Data->Def.Ast1=Temp.Ast;}
  else {Data->Def.Lev2=Temp.Lev;Data->Def.Ast2=Temp.Ast;}

 if (side==0) Data->P[1].PastMis=Temp.PastMis;
  else Data->P[0].PastMis=Temp.PastMis;

 //update mission .History prestige
if (side==1) 
 for (i=0;i<3;i++)
  {
   if (Data->P[0].PastMis-i>0) 
    Data->P[0].History[Data->P[0].PastMis-i].Prestige=Temp.Pst_H[i];
   if (Data->P[1].PastMis-i>0)
    Data->P[1].History[Data->P[1].PastMis-i].Prestige=Temp.Pst_S[i]; 
  };

 //slave side gets updated prestige
 if (side==1)
  {  
   for (i=0;i<28;i++)
    memcpy(&Data->Prestige[i],&Temp.Presto[i],sizeof(struct PrestType));
  }

 for (i=0;i<5;i++)
  for (k=0;k<7;k++)
   Data->P[side].Table[i][k]=Temp.table[i][k];

 Data->P[p].AstroLevel=Temp.astlevel;
 for (i=0;i<3;i++) Data->P[p].Mission[i].MissionCode=Temp.miscode[i];
 for (i=0;i<3;i++) Data->P[p].Future[i].MissionCode=Temp.futcode[i];
 for (i=0;i<3;i++) Data->P[p].LaunchFacility[i]=Temp.lfacil[i];

 for (i=0;i<7;i++)
  {
   memcpy(&Data->P[p].Probe[i],&Temp.P[i],sizeof(struct Equipment));
   memcpy(&Data->P[p].Rocket[i],&Temp.R[i],sizeof(struct Equipment));
   memcpy(&Data->P[p].Manned[i],&Temp.C[i],sizeof(struct Equipment));
   memcpy(&Data->P[p].Misc[i],&Temp.M[i],sizeof(struct Equipment));
  };

 // if host read in MP value for UpdatePrestige
 for(i=0;i<3;i++)
  memset(&MP[i],0x00,sizeof(struct Prest_Upd));

 if (side==0)
  {
   for (i=0;i<3;i++)
    memcpy(&MP[i],&Temp.tempMP[i],sizeof(struct Prest_Upd));
  }

 for (i=0;i<120;i++) old[i]=Temp.Msg[i];

  //REPLAY hack for prestige first
  fin=sOpen("REPLAY.DAT","r+b",1);
  for(i=0;i<3;i++)
   {
    if (((Data->P[other(side)].PastMis-3)+i)<0)  //Specs: was <=0)
     {
      Temp.Reper[i].Num=0;
      for(j=0;j<35;j++) Temp.Reper[i].Off[j]=0;
     }
    else
     {
      if (Temp.Reper[i].Off[33]>=53)
       {
        msc=Data->P[other(side)].History[Data->P[other(side)].PastMis-3+i].MissionCode=Temp.Reper[i].Off[33];
        if (msc==55 || msc==56 || msc==57)
         Data->P[other(side)].History[Data->P[other(side)].PastMis-3+i].Hard[1][0]=Temp.Reper[i].Off[34];
          else Data->P[other(side)].History[Data->P[other(side)].PastMis-3+i].Hard[0][0]=Temp.Reper[i].Off[34];
        Data->P[other(side)].History[Data->P[other(side)].PastMis-3+i].Patch[other(side)]=Temp.Reper[i].Off[32];
       }
      size=(other(side)*100)+((Data->P[other(side)].PastMis-3)+i);
      fseek(fin,size * (sizeof Rep),SEEK_SET);
      Rep.Qty=Temp.Reper[i].Num;
      for(j=0;j<32;j++) Rep.Off[j]=Temp.Reper[i].Off[j];
      Rep.Off[32]=Rep.Off[33]=Rep.Off[34]=0;
      fwrite(&Rep,sizeof Rep,1,fin);
     }
   }
  fclose(fin);

 killgame((side==0) ? "SENDR.MSG" : "SENDH.MSG");
 if ((f = sOpen((side==0) ? "SENDR.MSG" : "SENDH.MSG", "w+b",0)) != NULL)
  {
	 fwrite(&old,sizeof(old), 1, f);
	 fclose(f);
  }

 // Year/Season mismatch
 if (Temp.YER!=Data->Year || Temp.SES!=Data->Season)
  Option=99;
 return;
}

static int timed_get(int n)
{
 unsigned long start;

 start = BzTimer;
 while (1)
  {
	if (in_ready())	return get_serial();	 
 	if (!carrier() || ((BzTimer-start) >= n*200)) break;
  }
 return -1;			 
}

static int build_block(int l, FILE *file)
{
 int i, j;

 memset(buffer,0x00,1024);
 i = fread(buffer, 1, l, file);
 if (!i) return 0;
 for (j = i; j < l; j++)
 	buffer[j] = CPMEOF;
 return 1;
}

static void abort_transfer(void)
{
 int i;

 for (i = 0; i < 8; i++) put_serial(CAN);
}

int xmit_file(int (*error_handler)(int c, long p, char *s), char *files[])
{
 int i, tries, block = 0, bsize, retrans, crc, batch, nfiles = 0;
 int obits, oparity, ostop, oxon;
 unsigned int sum;
 long length, bsent,jpe;
 char *fname;
 FILE *file;
 char path[13];
 static int cancel=0;

  bsize = 128;
  batch = 0;
  obits = get_bits();					/* save current settings */
  oparity = get_parity();
  ostop = get_stopbits();
  oxon=tx_xon;
  set_data_format(8, NO_PARITY, 1);	/* set for 8-N-1 */
  tx_xon=0;
  tx_xonoff=0;

  fname=files[nfiles];

   //Specs: purge line klugge
  if (Option==1 || (strncmp(fname,"INIT.DAT",8)==0))
   for(i=0;i<128;i++)
    if (in_ready()) get_serial();

 //added to purge outgoing line
  purge_in();

	while (1)
   {
	 fname = files[nfiles];  
	 files++;
	 if (fname == NULL && !batch)
     {
      set_data_format(obits, oparity, ostop);
      tx_xon=oxon;
      tx_xonoff=0;
 	    return 1;
	   }
	 cancel=0;				  
	 while (cancel<3)
    {
	  if (!carrier())
      {
	    (*error_handler)(ERROR, block, ABORT);
	  	 set_data_format(obits, oparity, ostop);
       tx_xon=oxon;tx_xonoff=0;
		 return 0;
		}
	  if (in_ready())
      {
	    switch (get_serial())
        {
	      case 'C':crc=1;cancel+=1;break;
         case LET_A:Progress(3);break;
         case LET_M:Progress(4);break;
         case LET_O:Progress(5);break;
         case LET_V:Progress(6);break;
         case LET_R:Progress(7);break;
	      default:break;
	     }
	   }
     }

		if (fname != NULL)
      {
	     if ((file = sOpen(fname, "rb",0)) == NULL)
        {
		    abort_transfer();
			 set_data_format(obits, oparity, ostop);
         tx_xon=oxon;tx_xonoff=0;
			 return 0;
			}
        length = filelength(file->fd);
			fseek(file, 0L, SEEK_SET);
			if (!(*error_handler)(SENDING, length, fname))
         {
		     abort_transfer();
			  fclose(file);
			  set_data_format(obits, oparity, ostop);
          tx_xon=oxon;tx_xonoff=0;
			  return 0;
			 } 
		   bsent = 0L;
	    }

     block = 1;					  
     bsize=128;
     retrans = 0;

		while (1)
      {
	     if (!retrans) {
    	   if (!build_block(bsize, file)) break;
		  }
			for (i = sum = 0; i < bsize; i++) {
				if (crc)
					sum = updcrc(buffer[i], sum);
				else
					sum += buffer[i];
			}
			put_serial(SOH);

			put_serial((unsigned char)(block & 0xff));
			put_serial((unsigned char)((255 - block) & 0xff));
			for (i = 0; i < bsize; i++)
				put_serial(buffer[i]);

			if (crc)
         {
			  sum = updcrc(0, sum);
			  sum = updcrc(0, sum);
			  put_serial((unsigned char)((sum >> 8) & 0xff));
			  put_serial((unsigned char)(sum & 0xff));
			 }
			else
				put_serial((unsigned char)(sum & 0xff));
			cancel = retrans = 0;

			while (1)
         {
		     if (!carrier())
            {
				 (*error_handler)(ERROR, block, ABORT);
             fclose(file);
				 set_data_format(obits, oparity, ostop);
             tx_xon=oxon;tx_xonoff=0;
				 return 0;
				}
			  if (in_ready()) {
  		   switch (get_serial())
            {
			     case NAK: 
				   if (!(*error_handler)(ERROR, block, "NAK")) {
				  	 abort_transfer();
					 fclose(file);
 					 set_data_format(obits, oparity, ostop);
               tx_xon=oxon;tx_xonoff=0;
					 return 0;
					}
				   cancel = 0;
				   retrans = 1;
				   break;
		 	     case ACK:		/* block received ok */
				  	 bsent += bsize;
               jpe=0L;
               jpe=bsent*100/length;
					 if (!(*error_handler)(SENT, jpe, "ACK")) {
					  abort_transfer();
					  fclose(file);
					  set_data_format(obits, oparity, ostop);
                tx_xonoff=0;tx_xon=oxon;
					  return 0;
				  	 }
				    cancel = 0;
					 break;
				 case CAN:
				  	/* abort if second CAN */
				   if (cancel) {
					 (*error_handler)(ERROR, block, TCR);
					 fclose(file);
					 set_data_format(obits, oparity, ostop);
               tx_xonoff=0;tx_xon=oxon;
					 return 0;
					}
					cancel = 1;
					continue;
					default:
					cancel = 0;
					continue;
				  }
				  break;
				}
			}
	    if (retrans) continue;
 		 block = (block + 1) & 0xff;
		}
		fclose(file);
		put_serial(EOT);
		while (1)
      {
	     if (!carrier())
        {
		    (*error_handler)(ERROR, block, ABORT);
			 set_data_format(obits, oparity, ostop);
         tx_xonoff=0;tx_xon=oxon;
			 return 0;
			}
 		  if (in_ready())
        {
  	    if (get_serial() != ACK)
          {
			   put_serial(EOT);
			   continue;
			  }
			 break;
			}
		}
		(*error_handler)(COMPLETE, block, TCO);
		if (!batch) {
		 set_data_format(obits, oparity, ostop);
      tx_xonoff=0;tx_xon=oxon;
		 return 1;
		}
	}
}

static int getblock(int block, int crc)
{
 int c, l=0, i, sum;

 if ((c = timed_get(10)) == -1) return TIMED_OUT;

 switch (c)
  {
   case CAN:if ((c = timed_get(1)) == CAN) return CAN;
            return TIMED_OUT;
   case SOH:l = 128;break;
   case LET_A:if (StartRecv==0) return LET_A;break;
   case LET_M:if (StartRecv==0) return LET_M;break;
   case LET_O:if (StartRecv==0) return LET_O;break;
   case LET_V:if (StartRecv==0) return LET_V;break;
   case LET_R:if (StartRecv==0) return LET_R;break;
   case EOT:return EOT;
   default:return TIMED_OUT;		  
  }
 if ((c = timed_get(1)) == -1) return TIMED_OUT;
 if (c != block) return BAD_BLOCK;
 if ((c = timed_get(1)) == -1) return TIMED_OUT;
 if (c != ((255 - block) & 0xff)) return BAD_BLOCK;
 memset(buffer,0x00,1024);
 for (i = 0; i < l; i++)
  (char)buffer[i]=(unsigned char)timed_get(1);

 if (!crc)
  {
   if ((c = timed_get(1)) == -1) return TIMED_OUT;
	 for (i = sum = 0; i < l; i++) sum += buffer[i];
  }
 else
  {
   if ((sum = timed_get(1)) == -1) return TIMED_OUT;
 	 if ((c = timed_get(1)) == -1) return TIMED_OUT;
	 c |= sum << 8;
	 for (i = sum = 0; i < l; i++)
	  sum = updcrc(buffer[i], sum);
	 sum = updcrc(0, sum);
   sum = updcrc(0, sum);
  }
  if (c != sum) return CRC_ERROR;
  if (l == 128) return SOH;
 return SOH;
}

int recv_file(int(*error_handler)(int c, long p, char *s), char *path)
{
	int i,l,block,crc;
	int obits, oparity, ostop, oxon;
	long length, received, moddate, jpe;
	char *fname, line[80];
	FILE *file;
	#ifdef __TURBOC__
    struct ftime ft;
	#else
	 unsigned fd, ft;
	#endif
	struct tm *t;

   obits = get_bits();	  
	oparity = get_parity();
	ostop = get_stopbits();
   oxon=tx_xon;
	set_data_format(8, NO_PARITY, 1); 
   tx_xonoff=0;tx_xon=0;
   StartRecv=0;

  if (Option==0 || Option==1)
   for(i=0;i<128;i++)
    if (in_ready()) get_serial();

	while (1)
   {
    crc = 1;					 
    put_serial('C');
    block = 1;
    while(1)
     {
      l = getblock(block, crc);
 	    if (l == CAN || !carrier())
       {
	      set_data_format(obits, oparity, ostop);
         tx_xonoff=0;tx_xon=oxon;
	      return 0;
	     }
       if (l==LET_A) Progress(3);
        else if (l==LET_M) Progress(4);
        else if (l==LET_O) Progress(5);
        else if (l==LET_V) Progress(6);
        else if (l==LET_R) Progress(7);
       if (l != SOH && l != EOT) put_serial('C');
       if (l==SOH) break;
      }

	   fname = path;
	   length = received = 0L;
      StartRecv=1;

		if ((file = sOpen(fname, "wb",0)) == NULL)
       {
		  abort_transfer();
		  set_data_format(obits, oparity, ostop);
        tx_xonoff=0;tx_xon=oxon;
		  return 0;
		 }

		if (!(*error_handler)(RECEIVING, length, fname))
       {
		  abort_transfer();
 		  fclose(file);
		  set_data_format(obits, oparity, ostop);
        tx_xonoff=0;tx_xon=oxon;
		  return 0;
		 } 

		while (1)
      {
		  if (l == EOT)
        {
			 (*error_handler)(COMPLETE, 0, TCO);
			 break;
			}
       fwrite(buffer,1,128, file);
	    received += 128;
       if (block==1) length= *(long *)buffer;
       jpe=0L;
       if (length!=0) jpe=received*100/length;
  	  if (!(*error_handler)(RECEIVED, jpe, "BLOCK RECEIVED"))
        {
		    abort_transfer();
		    set_data_format(obits, oparity, ostop);
          tx_xonoff=0;tx_xon=oxon;
		    fclose(file);
		    return 0;
		  }
		  block = (block + 1) & 255;
		  put_serial(ACK);
		  for (i = 0; i < 10000; i++) {
		  	l = getblock(block, crc);
		  	if (l == SOH || l == EOT) break;
			if (!carrier()) {
			 (*error_handler)(ERROR, block, ABORT);
			 set_data_format(obits, oparity, ostop);
         tx_xonoff=0;tx_xon=oxon;
			 fclose(file);
			 return 0;
			}
			switch (l) {
				case CAN:			/* abort if CAN */
					(*error_handler)(ERROR, block, "CANCELLED BY SENDER");
					set_data_format(obits, oparity, ostop);
               tx_xonoff=0;tx_xon=oxon;
   				fclose(file);
					return 0;
				case TIMED_OUT:		/* TIMED OUT */
					if (!(*error_handler)(ERROR, block, "SHORT BLOCK")) {
	   			 abort_transfer();
					 set_data_format(obits, oparity, ostop);
               tx_xonoff=0;tx_xon=oxon;
					 fclose(file);
					 return 0;
					}
					break;
 			   case BAD_BLOCK:		/* bad block number */
					if (!(*error_handler)(ERROR, block, "BAD BLOCK NUMBER")) {
					 abort_transfer();
					 set_data_format(obits, oparity, ostop);
               tx_xonoff=0;tx_xon=oxon;
					 fclose(file);
					 return 0;
					}
				  	break;
				case CRC_ERROR:		/* CRC error */
					if (!(*error_handler)(ERROR, block, crc ? "CRC ERROR" : "CHECKSUM ERROR")) {
				 	 abort_transfer();
					 set_data_format(obits, oparity, ostop);
               tx_xonoff=0;tx_xon=oxon;
					 fclose(file);
					 return 0;
				  	}
				  	break;
				}
			 put_serial(NAK);
			}
			if (i > 10000) {
				abort_transfer();
				set_data_format(obits, oparity, ostop);
           tx_xonoff=0;tx_xon=oxon;
           fclose(file);
				return 0;
			}   
		}
	 put_serial(ACK);
	 fclose(file);
	 return 1;
  }    	
}
