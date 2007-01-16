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
// All Administration Main Files

#include "Buzz_inc.h"
#include "externs.h"
#include "replay.h"

#define MODEM_ERROR 4
#define NOTSAME 2
#define SAME_REPLACE 1
#define SAME_ABORT 0
#define YES 1
#define NO 0

extern char Sounds,Option,MAIL;
extern int fOFF;

SaveFileHdr *SaveHdr;
SFInfo *FList;

extern struct mStr Mis;

void Admin(char plr)
{
   int i,beg;
   char AName[7][22]={"BUDGET OFFICE","HARDWARE PURCHASE","FUTURE MISSIONS",
      "ASTRONAUT RECRUITING","PREFERENCES","TIME CAPSULE","EXIT ADMINISTRATION"};
   char AImg[7]={1,2,3,4,6,7,0};

  if (plr==1) strncpy(&AName[3][0],"COSMO",5);
  if (Data->P[plr].AstroDelay>0) AImg[3]=5;

  FadeOut(2,pal,10,0,0);
  i=0;beg=1;
  
  do {
     if (i!=6) PreLoadMusic(M_GOOD);
     if (beg) beg=0;
     else {
        
        FadeOut(2,pal,10,0,0);

        DrawSpaceport(plr);
        PortPal(plr);
        RectFill(166,191,318,198,3);
        grSetColor(0);PrintAt(257,197,"CASH:");DispMB(285,197,Data->P[plr].Cash);
        grSetColor(11);PrintAt(256,196,"CASH:");DispMB(284,196,Data->P[plr].Cash);
        grSetColor(0);if (Data->Season==0) PrintAt(166,197,"SPRING 19");
        else PrintAt(166,197,"FALL 19");DispNum(0,0,Data->Year);
        grSetColor(11);if (Data->Season==0) PrintAt(165,196,"SPRING 19");
        else PrintAt(165,196,"FALL 19");DispNum(0,0,Data->Year);
        
     }

     PlayMusic(0);

     strncpy(IDT,(plr==0) ?"i702":"i703",4);
     strncpy(IKEY,(plr==0) ? "k601":"k602",4);

     i=BChoice(plr,7,(char *)AName,(char *) AImg);

     switch(i) {
        case 1: KillMusic();
              PreLoadMusic(M_DRUMSM);
              PlayMusic(0);
              strcpy(IDT,"i007");strcpy(IKEY,"k007");
              Budget(plr);key=0;
              KillMusic();
              break;

        case 2: KillMusic();
              strcpy(IDT,"i008");
              HPurc(plr);key=0;
              break;

        case 3: strcpy(IDT,"i010");strcpy(IKEY,"k010");
              KillMusic();
              PreLoadMusic(M_MISSPLAN);
              PlayMusic(0);
              Future(plr);
              KillMusic();key=0;
              break;

        case 4: KillMusic();
              strcpy(IDT,"i012");strcpy(IKEY,"k012");
              AstSel(plr);key=0;
              break;

        case 5: KillMusic();
              strcpy(IDT,"i013");strcpy(IKEY,"k013");
              Prefs(1);key=0;
              break;

        case 6: strcpy(IDT,"i128");strcpy(IKEY,"k128");
              FileAccess(0);key=0;
              break;

        case 7: break;

        default: key=0;break;

     }
     if (Data->P[plr].AstroDelay>0) AImg[3]=5;
  } while (!(i==7 || (i==6 && (QUIT || LOAD)) ));

  remove_savedat("PORT.TMP"); // remove temp file
  KillMusic();
  strcpy(IDT,"i000");strcpy(IKEY,"k000");
  WaitForMouseUp();
}

int GenerateTables(SaveGameType saveType)
{
  struct ffblk ffblk;
  int i,j,tFiles,done;
  FILE *fin;
  tFiles=0;

  done=first_saved_game(&ffblk);
  while (!done && tFiles<100) {tFiles++;done = next_saved_game(&ffblk);}  // Count Total Files

  memset(buffer,0x00,20480);

  // old code lacked parans, so pointed FList off into space - pace
  FList=(SFInfo *)(buffer+5000);
  SaveHdr=(SaveFileHdr *) buffer;

  if (tFiles>0) {
    tFiles=0;
    done=first_saved_game(&ffblk);
    while(!done && tFiles<100)  // Get All Files Names And save Dates.
    {
     if (strlen (ffblk.ff_name) > sizeof FList[tFiles].Name - 1)
	     goto next;
     memset (&FList[tFiles], 0, sizeof FList[tFiles]);
     strcpy(FList[tFiles].Name,ffblk.ff_name);
     FList[tFiles].time=ffblk.ff_ftime;
     FList[tFiles].date=ffblk.ff_fdate;
     fin=sOpen(ffblk.ff_name,"rb",1);
     if (fin == NULL)
	     goto next;
     fread(SaveHdr,1,sizeof (SaveFileHdr),fin);
     fclose(fin);
     if (saveType == SAVEGAME_Normal) {
      strcpy(FList[tFiles].Title,SaveHdr->Name);
      tFiles++;
     }
     else if (saveType == SAVEGAME_PlayByMail)
      {
       if (SaveHdr->Country[0]==8 || SaveHdr->Country[1]==9)
        {
         strcpy(FList[tFiles].Title,SaveHdr->Name);
         tFiles++;
        }
      }
     else if (saveType == SAVEGAME_Modem)
      {
       if (SaveHdr->Country[0]==6 || SaveHdr->Country[1]==7)
        {
         strcpy(FList[tFiles].Title,SaveHdr->Name);
         tFiles++;
        }
      }
    next:
     done=next_saved_game(&ffblk);
    }
    if (tFiles!=0) {
     {
      for (i=0;i<tFiles-1;i++)
       {
        for (j=i+1;j<tFiles;j++)
         {
	       if (stricmp(FList[j].Title,FList[i].Title)<0)
           {
	         memcpy(&FList[tFiles],&FList[i],sizeof(SFInfo));
	         memcpy(&FList[i],&FList[j],sizeof(SFInfo));
	         memcpy(&FList[j],&FList[tFiles],sizeof(SFInfo));
	        }
         }
       }
     }
   }

  } 
  return tFiles;
}

void GetSaveMouse(void)
{
  GetMouse();
}

void FileAccess(char mode)
// mode==0 if save allowed
{
  char Temp[4],sc=0;
  long size;
  int tFiles,i,now,done,BarB,temp,left;
  FILE *fin,*fout;
  char Name[12];
  extern char plr[2],AI[2];
  RPLY Rep;
	SaveGameType saveType = SAVEGAME_Normal;

  //sp. case -> no regular save off mail/modem game
  if ((mode==0 || mode==1) && (MAIL!=-1 || Option!=-1)) sc=2;

  if (mode==2) {
   mode=0;
   sc=1; //only allow mail save
  }

  
  strcpy(IDT,"i128");strcpy(IKEY,"k128");
  FadeOut(2,pal,10,0,0);
  gxClearDisplay(0,0);

	saveType = SAVEGAME_Normal;

  if (Option!=-1) 
		saveType = SAVEGAME_Modem;
	else if (mode==2) 
		saveType = SAVEGAME_PlayByMail;

  tFiles=GenerateTables(saveType);
  
  ShBox(34,32,283,159);
  InBox(37,35,204,45);RectFill(38,36,203,44,7);
  InBox(207,35,280,45);RectFill(208,36,279,44,10);
  InBox(37,48,204,128);RectFill(38,49,203,127,0);
  InBox(37,132,280,156);
  ShBox(191,50,202,87);UPArrow(194,55);
  ShBox(191,89,202,126);DNArrow(194,94);

  if (tFiles>0 && (sc==0 || sc==2)) IOBox(207,48,280,60);else InBox(207,48,280,60);

  if (mode==0)
   {
    if (sc==0 || sc==2) IOBox(207,62,280,74); //Regular Save game
     else InBox(207,62,280,74); 
    if (sc==1) IOBox(207,76,280,88); //Mail Save game
     else InBox(207,76,280,88);
   }   
   else {
    InBox(207,62,280,74);
    InBox(207,76,280,88); 
   }
  if (tFiles>0) IOBox(207,90,280,102); else InBox(207,90,280,102); //Delete
  IOBox(207,104,280,116); //Play
  IOBox(207,118,280,130); //Quit

  grSetColor(11);
  PrintAt(59,42,"TIME CAPSULE REQUEST");
  PrintAt(218,42,"FUNCTIONS");
  grSetColor(1);PrintAtKey(233,56,"LOAD",0);
  grSetColor(1);PrintAtKey(233,70,"SAVE",0);
  grSetColor(1);PrintAtKey(223,84,"MAIL SAVE",0);
  grSetColor(1);PrintAtKey(226,98,"DELETE",0);
  grSetColor(1);PrintAtKey(233,112,"PLAY",0);
  grSetColor(1);PrintAtKey(233,126,"QUIT",0);

  done=BarB=now=0;
  ShBox(39,52+BarB*8,189,60+BarB*8);
  DrawFiles(0,0,tFiles);
  if (tFiles)
	  FileText(&FList[now].Name[0]);
  FadeIn(2,pal,10,0,0);
  
  
  while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
  while(!done) {
    GetSaveMouse();
    
    for (i=0;i<9;i++)
     {  // Right Select Box
      if (x>=40 && y>=(53+i*8) && x<=188 && y<=(59+i*8) && mousebuttons>0 && (now-BarB+i)<=(tFiles-1))
       {
        
	      now-=BarB; now+=i; BarB=i;
        RectFill(38,49,190,127,0);
    	   ShBox(39,52+BarB*8,189,60+BarB*8);
        DrawFiles(now,BarB,tFiles);
	      FileText(&FList[now].Name[0]);
        while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
   	   
       }
    }  
    
  if ((sc==0 || sc==2) && tFiles>0 && ((x>=209 && y>=50 && x<=278 && y<=58 && mousebuttons>0)
   || (key=='L')))
   {
    InBox(209,50,278,58);
	  delay(250);
    if (mode==1) temp=1;
     else {
	    temp=Help("i101");
     }    

	 while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
    if (temp>=0) {
			// Read in Saved game data

	   fin=sOpen(FList[now].Name,"rb",1);
	   fread(SaveHdr,1,sizeof (SaveFileHdr),fin);
	   fread(vhptr.vptr,1,SaveHdr->fSize,fin);
	   if (SaveHdr->dSize==sizeof(struct Players))
       {
#ifdef OLD_DOS_ENCRYPT_SAVEDATA
				{
				int moo = 0;
        srand(SaveHdr->fSize);
        for(moo=0;moo<SaveHdr->fSize;moo++) vhptr.vptr[moo]^=random(256);
        srand(biostime(0,0L));
				}
#endif
 	      RLED((char *) vhptr.vptr,(char *)Data,SaveHdr->fSize);
        fread(vhptr.vptr,(sizeof Rep)*200,1,fin);
        fout=sOpen("REPLAY.DAT","wb",1);
        fwrite(vhptr.vptr,(sizeof Rep)*200,1,fout);
        fclose(fout);

        fout=sOpen("EVENT.TMP","wb",1);
        left=32000; // copy EVENT.TMP FILE
        while (left==32000) {
          left=fread(vhptr.vptr,1,vhptr.h*vhptr.w,fin);
          fwrite(vhptr.vptr,left,1,fout);
         }
         fclose(fout);
         fclose(fin);

         if (!(SaveHdr->Country[0]==6 || SaveHdr->Country[1]==7 || SaveHdr->Country[0]==8 || SaveHdr->Country[1]==9))
          {
	         plr[0]=Data->Def.Plr1;plr[1]=Data->Def.Plr2;
	         Data->plr[0]=Data->Def.Plr1;
	         Data->plr[1]=Data->Def.Plr2;
	         if (plr[0]==2 || plr[0]==3) AI[0]=1; else AI[0]=0;  // SET AI FLAGS
	         if (plr[1]==2 || plr[1]==3) AI[1]=1; else AI[1]=0;
          }


        if (SaveHdr->Country[0]==8 || SaveHdr->Country[1]==9)
         {
          //Play-By-Mail save game LOAD
          Option=-1;fOFF=-1;
          //save file offset
          fOFF=now;

          Data->Season=SaveHdr->Season;
          Data->Year=SaveHdr->Year;

          if (SaveHdr->Country[0]==8) {MAIL=plr[0]=0;plr[1]=1;}
           else {MAIL=plr[1]=1;plr[0]=0;}
          AI[0]=AI[1]=0;
          Data->Def.Plr1=0;Data->Def.Plr2=1;
	       Data->plr[0]=Data->Def.Plr1;
	       Data->plr[1]=Data->Def.Plr2;

          if (Data->Def.Input==0 || Data->Def.Input==2)
           { // Hist Crews
            fin=sOpen("CREW.DAT","rb",0);
            size=fread(buffer,1,BUFFER_SIZE,fin);
            fclose(fin);
            fin=sOpen("MEN.DAT","wb",1);
            fwrite(buffer,size,1,fin);
            fclose(fin);
	        }
          else if (Data->Def.Input==1 || Data->Def.Input==3)
           { // User Crews
	         fin=sOpen("USER.DAT","rb",0);
             size=fread(buffer,1,BUFFER_SIZE,fin);
             fclose(fin);
             fin=sOpen("MEN.DAT","wb",1);
             fwrite(buffer,size,1,fin);
             fclose(fin);
	        }
         } 
        else
         //Modem save game LOAd
        if (SaveHdr->Country[0]==6 || SaveHdr->Country[1]==7)
         {
          //modem connect up
          if (SaveHdr->Country[0]==6) {plr[0]=SaveHdr->Country[0];plr[1]=1;}
           else {plr[1]=SaveHdr->Country[1];plr[0]=0;}
          //Modem Play => reset the modem
          if (Option!=-1) DoModem(2);
          Option=MPrefs(1);
          //klugge
          if (Option==0 || Option==2) Option=0;
           else if (Option==1 || Option==3) Option=1;
         }
        else
         {
          //Regular save game LOAD
          if (Data->Def.Input==0 || Data->Def.Input==2) { // Hist Crews
	         fin=sOpen("CREW.DAT","rb",0);
             size=fread(buffer,1,BUFFER_SIZE,fin);
             fclose(fin);
             fin=sOpen("MEN.DAT","wb",1);
             fwrite(buffer,size,1,fin);
             fclose(fin);
	        }
          else if (Data->Def.Input==1 || Data->Def.Input==3) { // User Crews
	         fin=sOpen("USER.DAT","rb",0);
             size=fread(buffer,1,BUFFER_SIZE,fin);
             fclose(fin);
             fin=sOpen("MEN.DAT","wb",1);
             fwrite(buffer,size,1,fin);
             fclose(fin);
	        }
        }  
	      if (Option!=MODEM_ERROR) LOAD=done=1;
         else {Option=-1;LOAD=done=0;return;}
	     }   
      else {fclose(fin);BadFileType();};
      } // temp
	   OutBox(209,50,278,58);  // Button Out
	   key=0;

    } // LOAD
      else if ((sc==0 || sc==2) && mode==0 && ((x>=209 && y>=64 && x<=278 && y<=72 && mousebuttons>0)
	         || (key=='S')))
      {
	      InBox(209,64,278,72);
	      delay(250);

	      while(YES)  { av_block (); GetSaveMouse();if (mousebuttons==NO) break;}

        memset(SaveHdr->Name,0x00,23);
        done=GetBlockName(SaveHdr->Name);  // Checks Free Space
				SaveHdr->ID = RaceIntoSpace_Signature;
        SaveHdr->Name[22]=0x1A;
	      temp=NOTSAME;

	      for (i=0;(i<tFiles && temp==2);i++) 
	         if (strcmp(SaveHdr->Name,FList[i].Title)==0) {
	            temp=RequestX("REPLACE FILE",1);
              if (temp==SAME_ABORT) done=0;
	         }

	      if (done==YES) {
	         i--;  // decrement to correct for the FOR loop
	         strcpy(SaveHdr->PName[0],Data->P[plr[0]].Name);
	         strcpy(SaveHdr->PName[1],Data->P[plr[1]].Name);
           //modem save game hack
           if (Option!=-1)
            {
             if (Option==0) {plr[0]=6;plr[1]=1;}
              else if (Option==1) {plr[0]=0;plr[1]=7;}
             Data->Def.Plr1=plr[0];Data->Def.Plr2=plr[1];
  	          Data->plr[0]=Data->Def.Plr1;   
	          Data->plr[1]=Data->Def.Plr2;
	          AI[0]=0;AI[1]=0;
            }
	         SaveHdr->Country[0]=Data->plr[0];
	         SaveHdr->Country[1]=Data->plr[1];
	         SaveHdr->Season=Data->Season;
	         SaveHdr->Year=Data->Year;
	         SaveHdr->dSize=sizeof(struct Players);
	   
	         fin=sOpen("ENDTURN.TMP","rb",1);
	         if (fin) {
	            SaveHdr->fSize=fread(vhptr.vptr,1,vhptr.h*vhptr.w,fin);
	            fclose(fin);
	         } else SaveHdr->fSize=0;

	      if (temp==NOTSAME)
          { 
	        i=0;
           fin=NULL;
	        do {
            if (fin) fclose(fin);
	         i++;
	         memset(Name,0x00,sizeof Name);
	         memset(Temp,0x00,sizeof Temp);
	         strcpy(Name,"BUZZ");
	         sprintf(Temp, "%d", i);
	         strcat(Name,Temp);
	         strcat(Name,".SAV");
	         fin=sOpen(Name,"rb",1);
	        } while (fin!=NULL);  // Find unique name
          fin=sOpen(Name,"wb",1);
	      }
        else fin=sOpen(FList[i].Name,"wb",1);

	     fwrite(SaveHdr,sizeof (SaveFileHdr),1,fin);

      //----------------------------------
      //Specs: Special Modem Save Klugge |
      //----------------------------------
        if (Option!=-1)
         {
          if (Option==0) {plr[0]=0;plr[1]=1;}
           else if (Option==1) {plr[0]=0;plr[1]=1;}
          Data->Def.Plr1=plr[0];Data->Def.Plr2=plr[1];
          Data->plr[0]=Data->Def.Plr1;   
          Data->plr[1]=Data->Def.Plr2;
	       AI[0]=0;AI[1]=0;
         }

        fout=sOpen("ENDTURN.TMP","rb",1);
        size=16000L;
        fseek(fout,0,SEEK_SET);
        while (size==16000) {
          size=fread(vhptr.vptr,1,size,fout);
          fwrite(vhptr.vptr,size,1,fin); // save Replay File
        }
        fclose(fout);

        fout=sOpen("REPLAY.DAT","rb",1);
        fread(vhptr.vptr,(sizeof Rep)*200,1,fout);
        fclose(fout);
        fwrite(vhptr.vptr,(sizeof Rep)*200,1,fin); // save Replay File

        fout=sOpen("EVENT.TMP","rb",1);  // Save Event File
        left=32000; // copy EVENT.TMP FILE
        while (left==32000) {
           left=fread(vhptr.vptr,1,32000,fout);
           fwrite(vhptr.vptr,left,1,fin);
        }
        fclose(fout); // close EVENT.TMP
       fclose(fin);
	   }  // end done if
	   OutBox(209,64,278,72);
	   key=0;
  }
   else if (sc==1 && mode==0 && ((x>=209 && y>=78 && x<=278 && y<=86 && mousebuttons>0)
    || (key=='M')))  // PLAY-BY-MAIL SAVE GAME
    {
     InBox(209,78,278,86);
     delay(250);
     while(1) {GetSaveMouse();if (mousebuttons==0) break;}
     memset(SaveHdr->Name,0x00,23);
     done=GetBlockName(SaveHdr->Name);  // Checks Free Space
		 SaveHdr->ID = RaceIntoSpace_Signature;
     SaveHdr->Name[22]=0x1A;
	  temp=NOTSAME;
	  for (i=0;(i<tFiles && temp==2);i++) 
	   if (strcmp(SaveHdr->Name,FList[i].Title)==0)
       {
	     temp=RequestX("REPLACE FILE",1);
        if (temp==SAME_ABORT) done=0;
	    }
	  if (done==YES)
      {
	    i--;  // decrement to correct for the FOR loop
	    strcpy(SaveHdr->PName[0],Data->P[plr[0]].Name);
	    strcpy(SaveHdr->PName[1],Data->P[plr[1]].Name);

        // play-by-mail save game hack
        // starts US side
        plr[0]=0;plr[1]=9;MAIL=-1;
        Data->Def.Plr1=plr[0];Data->Def.Plr2=plr[1];
  	     Data->plr[0]=Data->Def.Plr1;   
	     Data->plr[1]=Data->Def.Plr2;
	     AI[0]=0;AI[1]=0;

	     SaveHdr->Country[0]=Data->plr[0];
	     SaveHdr->Country[1]=Data->plr[1];
	     SaveHdr->Season=Data->Season;
	     SaveHdr->Year=Data->Year;
	     SaveHdr->dSize=sizeof(struct Players);

		EndOfTurnSave((char *) Data, sizeof ( struct Players));

        fin=sOpen("ENDTURN.TMP","rb",1);
	     if (fin)
         {
	       SaveHdr->fSize=fread(vhptr.vptr,1,vhptr.h*vhptr.w,fin);
	       fclose(fin);
	      } else SaveHdr->fSize=0;

	      if (temp==NOTSAME)
          { 
	        i=0;
          fin=NULL;
	        do {
	         i++;
           if (fin) fclose(fin);
	         memset(Name,0x00,sizeof Name);
	         memset(Temp,0x00,sizeof Temp);
	         strcpy(Name,"BUZZ");
	         sprintf(Temp, "%d", i);
	         strcat(Name,Temp);
	         strcat(Name,".SAV");
	         fin=sOpen(Name,"rb",1);
	        } while (fin!=NULL);  // Find unique name
          fin=sOpen(Name,"wb",1);
	      }
        else fin=sOpen(FList[i].Name,"wb",1);
	   
	     fwrite(SaveHdr,sizeof (SaveFileHdr),1,fin);
        fout=sOpen("ENDTURN.TMP","rb",1);
        size=16000L;
        fseek(fout,0,SEEK_SET);
        while (size==16000)
         {
          size=fread(vhptr.vptr,1,size,fout);
          fwrite(vhptr.vptr,size,1,fin); // save Replay File
         }
        fclose(fout);
        fout=sOpen("REPLAY.DAT","rb",1);
        fread(vhptr.vptr,(sizeof Rep)*200,1,fout);
        fclose(fout);
        fwrite(vhptr.vptr,(sizeof Rep)*200,1,fin); // save Replay File

        fout=sOpen("EVENT.TMP","rb",1);  // Save Event File
        left=32000; // copy EVENT.TMP FILE
        while (left==32000)
         {
          left=fread(vhptr.vptr,1,32000,fout);
          fwrite(vhptr.vptr,left,1,fin);
         }
      fclose(fout); // close EVENT.TMP
	    fclose(fin);
	   }  
     OutBox(209,78,278,86);
     key=0;
    }
   else if (tFiles>0 && ((x>=209 && y>=92 && x<=278 && y<=100 && mousebuttons>0)
	 || (key=='D')))
    {
	 InBox(209,92,278,100);
	 delay(250);
	 while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
	 OutBox(209,92,278,100);
	 // perform delete
	 i=RequestX("DELETE FILE",1);
	 if (i==1) {
	   
	   remove_savedat(FList[now].Name);
      memset(Name,0x00,sizeof Name);
			saveType = SAVEGAME_Normal;

      if (Option!=-1) saveType = SAVEGAME_Modem;
       else if (mode==2) saveType = SAVEGAME_PlayByMail;

	   tFiles=GenerateTables(saveType);
	   now=0;BarB=0;
	   RectFill(38,49,190,127,0);
	   ShBox(39,52+BarB*8,189,60+BarB*8);
	   DrawFiles(now,BarB,tFiles);
	   FileText(&FList[now].Name[0]);
	   if (tFiles==0) {
	     InBox(207,48,280,60);RectFill(208,49,279,59,3);
	     grSetColor(1);PrintAtKey(233,56,"LOAD",0);
	     InBox(207,90,280,102);RectFill(208,91,279,101,3);
	     grSetColor(1);PrintAtKey(226,98,"DELETE",0);
	   }
	   
	 }
	 key=0;
      }
      else if ((x>=209 && y>=106 && x<=278 && y<=114 && mousebuttons>0) || (key=='P'))
       {
	      InBox(209,106,278,114);
	      delay(250);
	      while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
	      OutBox(209,106,278,114);
	      key=0;done=1;
      }
      else if ((x>=209 && y>=120 && x<=278 && y<=128 && mousebuttons>0) || (key=='Q'))
      {
	    InBox(209,120,278,128);
    	 delay(250);
    	 while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
	    OutBox(209,120,278,128);
 	    // perform quit
	    i=RequestX("QUIT",1);
       //Modem Play => reset the modem
       if (Option!=-1 && i==1) DoModem(2);
	    if (i==1)
         {
          MAIL=Option=-1;
          QUIT=done=1;
         }
	    key=0;
      }
      else if ((x>=191 && y>=50 && x<=202 && y<=87 && mousebuttons>0) || key==UP_ARROW)
      {
	 InBox(191,50,202,87);
	 if (BarB==0) {
	   if (now > 0) {
	     now--;
	     RectFill(38,49,190,127,0);
	     ShBox(39,52+BarB*8,189,60+BarB*8);
	     DrawFiles(now,BarB,tFiles);
	     FileText(&FList[now].Name[0]);
	   };
	 }
	 if (BarB>0) {
	   RectFill(38,49,190,127,0);
	   BarB--;now--;
	   ShBox(39,52+BarB*8,189,60+BarB*8);
	   DrawFiles(now,BarB,tFiles);
	   FileText(&FList[now].Name[0]);
	 };
	 
	 //while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
	 OutBox(191,50,202,87);
	 // perform Up Button
	 key=0;
    }
    else if (key==0x4900) {  // Page Up
   	 
   	 if (now>0) {
   	   RectFill(38,49,190,127,0);
   	   now-=9;
         if (now<0) {now=0;BarB=0;}
   	   ShBox(39,52+BarB*8,189,60+BarB*8);
   	   DrawFiles(now,BarB,tFiles);
   	   FileText(&FList[now].Name[0]);
   	 };
   	 
   	 // perform Up Button
  	   key=0;

      }
    else if (key==0x5100) {  // Page Down
   	 
   	   if (now<(tFiles-9)) {
   	     now+=9;
           if (now>(tFiles-1)) {
            now=tFiles-1;
            BarB=8;
            }
   	     RectFill(38,49,190,127,0);
   	     ShBox(39,52+BarB*8,189,60+BarB*8);
   	     DrawFiles(now,BarB,tFiles);
   	     FileText(&FList[now].Name[0]);
   	   }
   	 
   	 key=0;
      }
      else if ((x>=191 && y>=89 && x<=202 && y<=126 && mousebuttons>0) || key==DN_ARROW)
      {
	 InBox(191,89,202,126);
	 if (BarB==8) {
	   if (now<(tFiles-1)) {
	     now++;
	     RectFill(38,49,190,127,0);
	     ShBox(39,52+BarB*8,189,60+BarB*8);
	     DrawFiles(now,BarB,tFiles);
	     FileText(&FList[now].Name[0]);
	   }
	 }
	 if (BarB<8 && now<(tFiles-1)) {
	   RectFill(38,49,190,127,0);
	   BarB++;now++;
	   ShBox(39,52+BarB*8,189,60+BarB*8);
	   DrawFiles(now,BarB,tFiles);
	   FileText(&FList[now].Name[0]);
	 };
	 
   
	 //while(1)  { GetSaveMouse();if (mousebuttons==0) break;}
	 OutBox(191,89,202,126);
	 // perform Down Button
	 key=0;
      }; //
  } //while
  if (LOAD==1) {OutBox(209,50,278,60);}  // Button Out
  if (mode==1 && QUIT==1) FadeOut(2,pal,10,0,0);
}

void
save_game (char *name)
{
	FILE *inf, *outf;
	SaveFileHdr hdr;
    size_t buflen = 0;
    ssize_t size = 0;
    char * buf = NULL;

	EndOfTurnSave((char *) Data, sizeof ( struct Players));

    if ((inf = sOpen("ENDTURN.TMP","rb",1)) == NULL) {
		printf ("save_game: can't open ENDTURN.TMP\n");
		return;
	}

	memset (&hdr, 0, sizeof hdr);

	hdr.ID = RaceIntoSpace_Signature;
	strcpy (hdr.Name, "AUTOSAVE");
	hdr.Name[sizeof hdr.Name - 1] = 0x1a;

	strcpy(hdr.PName[0], Data->P[plr[0]].Name);
	strcpy(hdr.PName[1], Data->P[plr[1]].Name);
	hdr.Country[0] = Data->plr[0];
	hdr.Country[1] = Data->plr[1];
	hdr.Season = Data->Season;
	hdr.Year = Data->Year;
	hdr.dSize = sizeof(struct Players);
	hdr.fSize = 0; //filelength (fileno (inf));
	   
	if ((outf = sOpen (name, "wb", 1)) == NULL) {
		printf ("save_game: can't create %s\n", name);
		return;
	}

    size = fread_dyn(&buf, &buflen, inf);
    if (size < 0) {
        perror("save_game");
	    fclose (inf);
        if (&buf)
            free(&buf);
        return;
    }
    hdr.fSize = size;
	
	fwrite(&hdr,sizeof hdr,1,outf);
    fwrite(buf, size, 1, outf);
	
	if ((inf = sOpen ("REPLAY.DAT", "rb", 1)) != NULL) {
        size = fread_dyn(&buf, &buflen, inf);
        if (size >= 0)
            fwrite(buf, size, 1, outf);
        else
            perror("save_game");
		fclose (inf);
	}
	
	if ((inf = sOpen ("EVENT.TMP", "rb", 1)) != NULL) {
        size = fread_dyn(&buf, &buflen, inf);
        if (size >= 0)
            fwrite(buf, size, 1, outf);
        else
            perror("save_game");
		fclose (inf);
	}

	fclose (outf);
    free(buf);
}

char GetBlockName(char *Nam)
{
  int i,key;
  GXHEADER local;
  
  GV(&local,164,77);
  gxGetImage(&local,39,50,202,126,0);
  ShBox(39,50,202,126);
  i=1;
  if (i==1) {
    InBox(43,67,197,77);RectFill(44,68,196,76,13);
    grSetColor(11); PrintAt(47,74,"ENTER FILE DESCRIPTION");
    InBox(51,95,190,105);RectFill(52,96,189,104,0);
  } else {
    InBox(43,67,197,77);RectFill(44,68,196,76,13);
    grSetColor(11);PrintAt(47,74,"NOT ENOUGHT DISK SPACE");
    delay(2000);gxPutImage(&local,gxSET,39,50,0);DV(&local);
    
    return 0;
  }
  
  gr_sync ();
  key=0;i=0;
  while(!(key==K_ENTER || key==K_ESCAPE)) {
    av_block ();
    gr_maybe_sync ();
    key = bioskey (0);
    if (key >= 'a' && key <= 'z')
	    key = toupper (key);
    if (key&0x00ff) {
      if ((i<21) && ((key==' ') || ((key>='A' && key<='Z')) ||
	 (key>='0' && key<='9'))) {  // valid key
	Nam[i++]=key;
	grSetColor(1);PrintAt(53,102,&Nam[0]);
	key=0;
      }
      if (i>0 && key==0x08) {
	Nam[--i]=0x00;
  
	RectFill(52,96,189,104,0);
	grSetColor(1);PrintAt(53,102,&Nam[0]);
  
	key=0;
      }
    }
  }
  
  gxPutImage(&local,gxSET,39,50,0);
  DV(&local);
  
  if(key==K_ENTER && i>=1) return 1; else return 0;
}



void DrawFiles(char now,char loc,char tFiles)
{
  int i,j,start;
  start=now-loc;
  grSetColor(1);
  j=0;

  for (i=start;i<start+9;i++) {
    if (i<tFiles) {
      PrintAt(40,58+j*8,FList[i].Title);
      j++;
    }
  }
}


void BadFileType(void)
{
  GXHEADER local;
  GV(&local,164,77);
  gxGetImage(&local,39,50,202,126,0);
  ShBox(39,50,202,126);
    InBox(43,67,197,77);RectFill(44,68,196,76,13);
    grSetColor(11);PrintAt(47,74,"CORRUPT SAVE FILE");
    delay(2000);gxPutImage(&local,gxSET,39,50,0);
	PauseMouse();
  gxPutImage(&local,gxSET,39,50,0);
  DV(&local);
}


void FileText(char *name)
{
  FILE *fin;

  RectFill(38,133,279,155,3);
  grSetColor(1);
  fin=sOpen(name,"rb",1);
  if (fin==NULL) {
    grSetColor(11);
    PrintAt(70,147,"NO HISTORY RECORDED");
    return;
  };
  fread(SaveHdr,sizeof (SaveFileHdr),1,fin);
  fclose(fin);
  grMoveTo(40,139);
  //if (((char)SaveHdr->Country[0])&0x02) grSetColor(7+(SaveHdr->Country[1]-2)*3); 

  grSetColor(5);

  //grSetColor(6+(SaveHdr->Country[0]%2)*3); 
  if (SaveHdr->Country[0]==6 || SaveHdr->Country[1]==7) PrintAt(0,0,"MODEM DIRECTOR ");
   else if (SaveHdr->Country[0]==8 || SaveHdr->Country[1]==9) PrintAt(0,0,"MAIL DIRECTOR ");
    else if (SaveHdr->Country[0]==2) PrintAt(0,0,"COMPUTER DIRECTOR ");
     else PrintAt(0,0,"HUMAN DIRECTOR ");
  PrintAt(0,0,&SaveHdr->PName[0][0]);
  PrintAt(0,0," OF THE U.S.A.");

  grMoveTo(40,147);
  //if (((char)SaveHdr->Country[1])&0x02) grSetColor(7+(SaveHdr->Country[1]-2)*3); 

  grSetColor(9);

  if (SaveHdr->Country[0]==6 || SaveHdr->Country[1]==7) PrintAt(0,0,"VS. MODEM DIRECTOR ");
   else if (SaveHdr->Country[0]==8 || SaveHdr->Country[1]==9) PrintAt(0,0,"VS. MAIL DIRECTOR ");
    else if (SaveHdr->Country[1]==3) PrintAt(0,0,"VS. COMPUTER DIRECTOR ");
     else PrintAt(0,0,"VS. HUMAN DIRECTOR ");

  PrintAt(0,0,&SaveHdr->PName[1][0]);
  PrintAt(0,0," OF THE U.S.S.R.");

  grMoveTo(40,154);grSetColor(11);
  if (SaveHdr->Season==0)
   {
    if (SaveHdr->Country[0]==8) PrintAt(0,0,"U.S.A. TURN IN THE SPRING OF ");
     else if (SaveHdr->Country[1]==9) PrintAt(0,0,"SOVIET TURN IN THE SPRING OF ");
      else PrintAt(0,0,"THE SPRING OF ");
   }
  else {
    if (SaveHdr->Country[0]==8) PrintAt(0,0,"U.S.A. TURN IN THE FALL OF ");
     else if (SaveHdr->Country[1]==9) PrintAt(0,0,"SOVIET TURN IN THE FALL OF ");
      else PrintAt(0,0,"THE FALL OF ");
   }   
  DispNum(0,0,19);DispNum(0,0,SaveHdr->Year);PrintAt(0,0,".");
}


int FutureCheck(char plr,char type)
{
  int i,pad,p[3],m[3],t=0,tx[3]={0,0,0};
  FILE *fin;
  
  for (i=0;i<3;i++) {
    p[i]=Data->P[plr].LaunchFacility[i];
    if (type==0) m[i]=Data->P[plr].Future[i].MissionCode;
    if (type==1) m[i]=(Data->P[plr].Mission[i].Hard[4] >0 ) ? 1: 0;
  };
  FadeOut(2,pal,10,0,0);


  PortPal(plr);

  fin=sOpen("LPADS.BUT","rb",0);
  i=fread(screen, 1, MAX_X*MAX_Y, fin);
  fclose(fin);
  RLED_img(screen,vhptr.vptr,i, vhptr.w,vhptr.h);
  if (type==0) {strcpy(IDT,"i010");strcpy(IKEY,"k010");}
   else{strcpy(IDT,"i015");strcpy(IKEY,"k015");}
  gxClearDisplay(0,0);
  ShBox(59,12,269,186);InBox(64,17,213,29);
  RectFill(65,18,212,28,7);
  ShBox(217,17,264,29);IOBox(217,17,264,29);grSetColor(9);PrintAt(231,25,"EXIT");
  grSetColor(11);
  if (type==0) PrintAt(74,25,"FUTURE ");
  else if (plr==0) PrintAt(83,25,"VAB "); else PrintAt(83,25,"VIB ");
  PrintAt(0,0,"LAUNCH SELECTION");
  for (i=0;i<3;i++) {
    InBox(64,35+51*i,104,66+51*i);
    InBox(64,69+51*i,104,79+51*i);
    InBox(108,35+51*i,264,64+51*i);
    IOBox(108,67+51*i,264,79+51*i);
    if (p[i]>1) {
      grSetColor(5);
      PrintAt(111,50+i*51,"THIS FACILITY IS DAMAGED");
      PrintAt(111,57+i*51,"IT WILL COST ");DispNum(0,0,abs(p[i]));
      PrintAt(0,0,"MB TO REPAIR");
      if (type==0) PrintAt(113,75+i*51,"REPAIR LAUNCH FACILITY");
      else InBox(110,69+i*51,262,77+i*51); t=2;
    };
    if (p[i]==1) {
      grSetColor(1);
      if (type==1) {
        GetMisType(Data->P[plr].Mission[i].MissionCode);
        PrintAt(111,41+i*51,Mis.Abbr);

	      if (i<2)
	         if (Data->P[plr].Mission[i+1].part==1) {
	            PrintAt(111,61+i*51,"PRIMARY MISSION PART");
	            PrintAt(111,61+(i+1)*51,"SECONDARY MISSION PART");
	         }
        }
        else {
           if (!Data->P[plr].Future[i].MissionCode) grSetColor(1);
           GetMisType(Data->P[plr].Future[i].MissionCode);

           PrintAt(111,41+i*51,Mis.Abbr);
	         if (i<2)
	            if (Data->P[plr].Future[i+1].part==1) {
	               PrintAt(111,61+i*51,"PRIMARY MISSION PART");
	               PrintAt(111,61+(i+1)*51,"SECONDARY MISSION PART");
	            }
        }

  if (type==0) {
	if (Data->P[plr].Future[i].part==0) {
	  if (m[i]==0) {PrintAt(113,75+i*51,"ASSIGN FUTURE MISSION");t=1;}
	  else {PrintAt(113,75+i*51,"REASSIGN FUTURE MISSION");t=0;};
	} else {
	  PrintAt(113,75+i*51,"SECOND PART OF JOINT MISSION");t=0;tx[i]=1;
	  InBox(110,69+i*51,262,77+i*51);
	}
      } else {
       if (m[i]==0) {PrintAt(113,75+i*51,"ASSEMBLE HARDWARE");t=1;}
       else {PrintAt(113,75+i*51,"REASSEMBLE HARDWARE");t=0;};
      };
    };
    if (p[i]==-1) {
      grSetColor(9);  // Changed from 5
      PrintAt(111,41+i*51,"NO FACILITY EXISTS");
      if (type==0) {
	PrintAt(111,49+i*51,"PURCHASE LAUNCH FACILITY");
	PrintAt(111,57+i*51,"FOR: 20 MB'S");
	PrintAt(111,75+i*51,"PURCHASE FACILITY");
      }
      else InBox(110,69+i*51,262,77+i*51);
      t=3;
    };
    grSetColor(11);PrintAt(72,76+i*51,"PAD ");DispChr(0x41+i);
    gxVirtualDisplay(&vhptr,156*plr+t*39,i*30,65,36+i*51,103,65+i*51,0);
  }

  FadeIn(2,pal,10,0,0);
  
  WaitForMouseUp();
  pad=-1;x=y=mousebuttons=key=0;
  while(pad==-1) {
   key=0;GetMouse();
     if (x>=219 && y>=19 && x<=262 && y<=27 && mousebuttons>0) 
      {
       InBox(219,19,262,27);
       WaitForMouseUp();
       pad=5;key=0;
      }
     if (((!(x>=59 && y>=12 && x<=269 && y<=186))&&mousebuttons>0) || key==K_ESCAPE || key==K_ENTER || key=='E')
      {
       InBox(219,19,262,27);
       pad=5;key=0;
      };
     for (i=0;i<3;i++)
      {
       if ((x>=110 && y>=69+i*51 && x<=262 && y<=77+i*51 && tx[i]!=1 && mousebuttons>0) || (tx[i]!=1 && key=='A'+i)) {
	     InBox(110,69+i*51,262,77+i*51);
	 WaitForMouseUp();
	 key=0;
	 delay(300);
	 if (p[i]==1) pad=i;
	 if (p[i]==-1 && Data->P[plr].Cash>=20 && type==0) {
     
	   gxVirtualDisplay(&vhptr,156*plr+39,i*30,65,36+i*51,103,65+i*51,0);
	   Data->P[plr].Cash-=20;
	   Data->P[plr].Spend[0][3]+=20;
	   Data->P[plr].LaunchFacility[i]=1; p[i]=1;
	   ShBox(110,69+i*51,262,77+i*51);
	   RectFill(109,36+51*i,263,63+51*i,3);
	   grSetColor(5);Missions(plr,111,41+i*51,m[i],0);
	   PrintAt(113,75+i*51,"ASSIGN FUTURE MISSION");
     
	 }
   else if (p[i]==-1 && Data->P[plr].Cash<20 && type==0)
    {Help("i129");}


	 if (p[i]>4 && Data->P[plr].Cash>=abs(Data->P[plr].LaunchFacility[i])
	    && type==0) {
     
	   gxVirtualDisplay(&vhptr,156*plr+39,i*30,65,36+i*51,103,65+i*51,0);
	   Data->P[plr].Cash-=Data->P[plr].LaunchFacility[i];
	   Data->P[plr].Spend[0][3]+=Data->P[plr].LaunchFacility[i];
	   Data->P[plr].LaunchFacility[i]=1; p[i]=1;
	   RectFill(109,36+51*i,263,63+51*i,3);
	   ShBox(110,69+i*51,262,77+i*51);
	   grSetColor(5);Missions(plr,111,41+i*51,m[i],0);
	   PrintAt(113,75+i*51,"ASSIGN FUTURE MISSION");
     
	 }
	 else if (p[i]>4 && Data->P[plr].Cash<abs(Data->P[plr].LaunchFacility[i])
	    && type==0) {Help("i129");}
       };
     };
   };
  return pad;
};

char RequestX(char *s,char md)
{
  char i;
  GXHEADER local;

  
  if (md==1) {  // Save Buffer
    GV(&local,196,84);
    gxGetImage(&local,85,52,280,135,0);
  }
  i=strlen(s)>>1;
  grSetColor(0);
  ShBox(85,52,249,135);
  IOBox(170,103,243,130);
  IOBox(91,103,164,130);
  InBox(92,58,243,97);
  grSetColor(1);
  DispBig(109,110,"YES",0,0);
  DispBig(196,110,"NO",0,0);
  grSetColor(11);
  DispBig(166-i*10,65,&s[0],0,-1);
  PrintAt(138,94,"ARE YOU SURE");
  
  WaitForMouseUp();
  i=2;
  while(i==2) {
    GetMouse();
     if ((x>=172 && y>=105 && x<=241 && y<=128 && mousebuttons!=0)||(key=='N')) {
       InBox(172,105,241,128);i=0;
       delay(50);key=0;
     };
     if ((x>93 && y>=105 && x<=162 && y<=128&& mousebuttons!=0)||(key=='Y')) {
       InBox(93,105,162,128);i=1;
       delay(50);key=0;
     };
  };

  if (md==1) {
    
    WaitForMouseUp();
    gxPutImage(&local,gxSET,85,52,0);
    DV(&local);
  }
  WaitForMouseUp();
  return i;
}


void SaveMail(void)
{
 char Temp[4];
 long size;
 int tFiles,i,done=0,temp,left;
 FILE *fin,*fout;
 char Name[12];
 extern char plr[2],AI[2];
 RPLY Rep;


  tFiles=GenerateTables(0);
  if (tFiles)EMPTY_BODY;
  memset(SaveHdr->Name,0x00,23);
  // done=GetBlockName(SaveHdr->Name);  // Checks Free Space
  strcpy(SaveHdr->Name,FList[fOFF].Title);
	SaveHdr->ID = RaceIntoSpace_Signature;
  SaveHdr->Name[22]=0x1A;
  temp=NOTSAME;
  done=YES;
  for (i=0;(i<tFiles && temp==2);i++) 
   if (strcmp(SaveHdr->Name,FList[i].Title)==0)
    {
     temp=1;
     if (temp==SAME_ABORT) done=0;
    }
  if (done==YES)
   {
	 i--;  // decrement to correct for the FOR loop
	 strcpy(SaveHdr->PName[0],Data->P[plr[0]].Name);
	 strcpy(SaveHdr->PName[1],Data->P[plr[1]].Name);

    // play-by-mail save game hack
    // switch sides

    if (MAIL==0) {plr[0]=0;plr[1]=9;}
     else if (MAIL==1) {plr[0]=8;plr[1]=1;}
    Data->Def.Plr1=plr[0];Data->Def.Plr2=plr[1];
  	 Data->plr[0]=Data->Def.Plr1;   
	 Data->plr[1]=Data->Def.Plr2;
	 AI[0]=0;AI[1]=0;

	 SaveHdr->Country[0]=Data->plr[0];
	 SaveHdr->Country[1]=Data->plr[1];

	 SaveHdr->Season=Data->Season;
	 SaveHdr->Year=Data->Year;
	 SaveHdr->dSize=sizeof(struct Players);

		EndOfTurnSave((char *) Data, sizeof ( struct Players));

    MAIL=-1; //Reset Mail
	 fin=sOpen("ENDTURN.TMP","rb",1);
	 if (fin)
     {
	   SaveHdr->fSize=fread(vhptr.vptr,1,vhptr.w*vhptr.h,fin);
	   fclose(fin);
	  } else SaveHdr->fSize=0;

 	 if (temp==NOTSAME)
     { 
	   i=0;
     fin=NULL;
	   do {
	    i++;
      if (fin) fclose(fin);
	    memset(Name,0x00,sizeof Name);
	    memset(Temp,0x00,sizeof Temp);
	    strcpy(Name,"BUZZ");
	    sprintf(Temp, "%d", i);
	    strcat(Name,Temp);
	    strcat(Name,".SAV");
	    fin=sOpen(Name,"rb",1);
	   } while (fin!=NULL);  // Find unique name
     fin=sOpen(Name,"wb",1);
	  }
     else fin=sOpen(FList[i].Name,"wb",1);
	   
	  fwrite(SaveHdr,sizeof (SaveFileHdr),1,fin);
     fout=sOpen("ENDTURN.TMP","rb",1);
     size=16000L;
     fseek(fout,0x00,SEEK_SET);
     while (size==16000)
      {
       size=(long) fread(vhptr.vptr,1,size,fout);
       fwrite(vhptr.vptr,size,1,fin); // save Replay File
      }
     fclose(fout);
     fout=sOpen("REPLAY.DAT","rb",1);
     fread(vhptr.vptr,(sizeof Rep)*200,1,fout);
     fclose(fout);
     fwrite(vhptr.vptr,(sizeof Rep)*200,1,fin); // save Replay File
     fout=sOpen("EVENT.TMP","rb",1);  // Save Event File
     left=32000; // copy EVENT.TMP FILE
     while (left==32000)
      {
       left=fread(vhptr.vptr,1,32000,fout);
       fwrite(vhptr.vptr,left,1,fin);
      }
     fclose(fout); // close EVENT.TMP
	  fclose(fin);
    }	 
}

// Save Game related functions
void EndOfTurnSave(char *inData, int dataLen)
{
	FILE *fout = NULL;
	int compressedLen = 0;
	char buffer[dataLen * 2];

	// Remove old save data
	remove_savedat("ENDTURN.TMP");

	// Create new save data
	fout = sOpen("ENDTURN.TMP","wb",1);  
	compressedLen = RLEC(inData, buffer, dataLen);
#ifdef OLD_DOS_ENCRYPT_SAVEDATA
	{
		int moo = 0;
		srand(compressedLen);		// Seed the random number generator with file length
		for(moo=0;moo<j;moo++) 
		{
			buffer[moo]^=random(256);	//
		}
		// Reseed the random number generator 
		// -- this may have been the source of created complaints about randomness
		srand(biostime(0,0L));
	}
#endif

	fwrite(buffer, compressedLen, 1, fout);
	fclose(fout);
}


// EOF

