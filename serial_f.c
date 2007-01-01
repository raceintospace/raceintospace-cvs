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

#define DLL       0						  
#define IER       1						  
#define DLH       1						  
#define IIR       2 					  
#define LCR       3						  
#define MCR       4						  
#define LSR       5						  
#define MSR       6						  
#define RX_INT    1						  
#define DLAB      0x80					  
#define DTR       1						  
#define RTS       2						  
#define MC_INT    8						  
#define RX_RDY    0x01
#define TX_RDY    0x20
#define DCD       0x80
#define IMR       0x21						     
#define ICR       0x20						     
#define IRQ3      0xf7						  
#define IRQ4      0xef					  	  

int port_open = 0, irq;
extern char Option,FIN;
extern char *files[10];

void (_interrupt _far *ovect)();
void SaveFirst(char);
void CheckFirst(char);
void UpdateFirst(void);
void SendFirst(void);
void RecvFirst(void);
int e_handler(int c, long p, char *s);
void killgame(char *fName);

void SaveFirst(char mode)
{
 struct Need {
  char side;
  char Name1[20];
  char Name2[20];
  char Lev1,Ast1;
  char Lev2,Ast2;
  char Model;
  char pad[200];
 };

 int i,j,k,length;
 long size;
 FILE *fin;

 struct Need Temp;

 killgame("INIT.DAT");
 Temp.side=mode;
 Temp.Model=Data->Def.Input;
 memcpy(&Temp.Name1[0],&Data->P[0].Name[0],20);
 memcpy(&Temp.Name2[0],&Data->P[1].Name[0],20);
 Temp.Lev1=Data->Def.Lev1;
 Temp.Ast1=Data->Def.Ast1;
 Temp.Lev2=Data->Def.Lev2;
 Temp.Ast2=Data->Def.Ast2;
 memset(buffer,0x00,BUFFER_SIZE);
 for (i=0;i<200;i++) Temp.pad[i]=0x00;
 j=RLEC((char *)&Temp,buffer,sizeof(struct Need));
 fin=sOpen("INIT.DAT","wb",0);
 fwrite(&size,sizeof(size),1,fin);
 fwrite((char *)buffer,j,1,fin);
 fseek(fin,0L,SEEK_SET);
 size=filelength(fin->fd)-sizeof(size);
 fwrite(&size,sizeof(size),1,fin);
 memset(buffer,0x00,BUFFER_SIZE);
 fclose(fin);
 return;
}

void SendFirst(void)
{
 char init[]="INIT.DAT";

 memset(buffer,0x00,BUFFER_SIZE);
 Read_CRC();
 files[0] = init;
 files[1] = NULL;
 xmit_file(e_handler, files);
 return;
}

void UpdateFirst(void)
{
 struct Need {
  char side;
  char Name1[20];
  char Name2[20];
  char Lev1,Ast1;
  char Lev2,Ast2;
  char Model;
  char pad[200];
 };

 int i,j,k,length;
 char OldSide,OldModel;
 long size;
 FILE *f,*fin;

 struct Need Temp;

 memset(buffer,0x00,BUFFER_SIZE);
 for (i=0;i<200;i++) Temp.pad[i]=0x00;
 fin=sOpen("INIT.DAT","rb",0);
 fread(&size,sizeof(size),1,fin);
 fread((char *)buffer,size,1,fin);
 fclose(fin);
 RLED(buffer,(char *)&Temp,(unsigned int)size);
 OldSide=Option;
 OldModel=Data->Def.Input;
 Option=Temp.side;
 memcpy(&Data->P[0].Name[0],&Temp.Name1[0],20);
 memcpy(&Data->P[1].Name[0],&Temp.Name2[0],20);
 Data->Def.Lev1=Temp.Lev1;Data->Def.Ast1=Temp.Ast1;
 Data->Def.Lev2=Temp.Lev2;Data->Def.Ast2=Temp.Ast2;
 Data->Def.Input=Temp.Model;
 if (OldSide!=Temp.side || OldModel!=Temp.Model)
  {
   FadeOut(2,pal,10,0,0);
   MouseOff();gxClearDisplay(0,0);MouseOn();
   DrawPrefs(1,0,0);
   FadeIn(2,pal,10,0,0);
   if (OldSide==0 && Temp.side==1) Idiot("i800");
    else if (OldSide==1 && Temp.side==0) Idiot("i801");
   if ((OldModel==0 || OldModel==2) && (Temp.Model==1 || Temp.Model==3))
    Idiot("i802");
   if ((OldModel==1 || OldModel==3) && (Temp.Model==0 || Temp.Model==2))
    Idiot("i803");
   if ((OldModel==0 || OldModel==1) && (Temp.Model==2 || Temp.Model==3))
     Idiot("i804");
   if ((OldModel==2 || OldModel==3) && (Temp.Model==0 || Temp.Model==1))
    Idiot("i805");
  }
 //Specs: game modem Historical/Basic
 if (OldModel!=Temp.Model)
  {
   if (Data->Def.Input==2 || Data->Def.Input==3)
    {
    fin=sOpen("HIST.DAT","rb",0);
    fread(&Data->P[0].Probe[0],28*(sizeof (struct Equipment)),1,fin);
    fread(&Data->P[1].Probe[0],28*(sizeof (struct Equipment)),1,fin);
    fclose(fin);
   }
   else {
    fin=sOpen("RAST.DAT","rb",0);
    fread(&Data->P[0].Probe[0],28*(sizeof (struct Equipment)),1,fin);
    fread(&Data->P[1].Probe[0],28*(sizeof (struct Equipment)),1,fin);
    fclose(fin);
   }
  }

 //Specs: Historical/Basic rosters
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

 memset(buffer,0x00,BUFFER_SIZE);
 return;
}

void CheckFirst(char Hmode)
{
 struct Need {
  char side;
  char Name1[20];
  char Name2[20];
  char Lev1,Ast1;
  char Lev2,Ast2;
  char Model;
  char pad[200];
 };

 int i,j,k,length;
 char Smode,Smodel;
 long size;
 FILE *f,*fin;

 struct Need Temp;

 memset(buffer,0x00,BUFFER_SIZE);
 for (i=0;i<200;i++) Temp.pad[i]=0x00;
 fin=sOpen("INIT.DAT","rb",0);
 fread(&size,sizeof(size),1,fin);
 fread((char *)buffer,size,1,fin);
 fclose(fin);
 RLED(buffer,(char *)&Temp,(unsigned int)size);
 Smode=Temp.side;
 Smodel=Temp.Model;
 if (Hmode==Smode) {
  if (Hmode==0) {
   Smode=1;
   memcpy(&Data->P[Smode].Name[0],&Temp.Name1[0],20);
   Data->Def.Lev2=Temp.Lev1;
   Data->Def.Ast2=Temp.Ast1;
  }
  else if (Hmode==1) {
   Smode=0;
   memcpy(&Data->P[Smode].Name[0],&Temp.Name2[0],20);
   Data->Def.Lev1=Temp.Lev2;
   Data->Def.Ast1=Temp.Ast2;
  }
 }
 else
  {
   if (Smode==0) memcpy(&Data->P[Smode].Name[0],&Temp.Name1[0],20);
    else memcpy(&Data->P[Smode].Name[0],&Temp.Name2[0],20);
   if (Smode==0) {Data->Def.Lev1=Temp.Lev1;Data->Def.Ast1=Temp.Ast1;}
    else {Data->Def.Lev2=Temp.Lev2;Data->Def.Ast2=Temp.Ast2;}
  }
 if (Data->Def.Input!=Smodel)
  {
   Smodel=Data->Def.Input;
  }
 memset(buffer,0x00,BUFFER_SIZE);
 return;
}

void RecvFirst(void)
{
 char init[]="INIT.DAT";

 killgame("INIT.DAT");
 memset(buffer,0x00,BUFFER_SIZE);
 Read_CRC();
 recv_file(e_handler, init);
 return;
}

int port_exist(int port)
{
 return mpeek(0, 0x400 + (port - 1) * 2);
}

void open_port(int port, int inlen)
{
	long i;

	if (port_open) {
		//printf("Unable to open port: A port is already open!\n");
		FIN=97;
      Idiot("i808");
      return;
	}

	/* make sure the port if valid */
	if (port < 1 || port > 4 || !port_exist(port)) {
		//printf("Unable to open port: Invalid port number!\n");
      FIN=98;
      Idiot("i809");
      return;
	}

	/* allocate the space for the buffers */
	ilen = inlen;
	if ((inbuff = farmalloc(ilen)) == NULL) {
		//printf("Unable to open port: Not enough memory for the buffer!\n");
      FIN=99;
      return;
	}

	/* calculate the flow control limits */
	foff = (int)((long)ilen * 90L / 100L);
	fon = (int)((long)ilen * 80L / 100L);
	rx_flow = 0;

	switch (port)
    {
	  case 1:base=0x3f8;irq=0x0c;break;
	  case 2:base=0x2f8;irq=0x0b;break;
	  case 3:base=0x3e8;irq=0x0c;break;
	  case 4:base=0x2e8;irq=0x0b;break;
	 }

	/* set up the interrupt handler */
	disable();                         		/* disable the interupts */
	ovect = getvect(irq);					/* save the current vector */
	setvect(irq, handler);					/* set the new vector */
	sibuff = eibuff = 0;					/* set the buffer pointers */
	outportb(base + MCR,
		inportb(base + MCR) | MC_INT | DTR | RTS); /* assert GPO2, RTS, and DTR */
	outportb(base + IER, RX_INT);			/* set received data interrupt */
	outportb(IMR,
		inportb(IMR) & (irq == 0x0b ? IRQ3 : IRQ4)); /* set the interrupt */
	enable();								/* enable the interrupts */
  //FIFO routine
  outportb(base + IIR, 0xc1);

  tx_rts=0;
  tx_dtr=0;
  tx_xon=0;tx_xonoff=0;
  rx_rts=0;
  rx_dtr=0;
  rx_xon=0;

	port_open = 1;						/* flag port is open */
}

void close_port(void)
{
	if (!port_open) return;
	port_open = 0;					  
	disable();								
	outportb(IMR,inportb(IMR) | (irq == 0x0b ? ~IRQ3 : ~IRQ4)); 
	outportb(base + IER, 0);			  
	outportb(base + MCR,inportb(base + MCR) & ~MC_INT); 
	setvect(irq, ovect);				  
	enable();							  
	outportb(base + MCR,
	inportb(base + MCR) & ~RTS); 
  //TR fix
  outportb(base+MCR,inportb(base+MCR)&0x0c);
}

void purge_in(void)
{
	disable();                         		/* disable the interrupts */
	sibuff = eibuff = 0;					/* set the buffer pointers */
	enable();								/* enable the interrupts */
}

void set_baud(long baud)
{
 int c, n;

	if (baud == 0L) return;
	n = (int)(115200L / baud);				/* figure the divisor */
	disable();								/* disable the interrupts */
	c = inportb(base + LCR);				/* get line control reg */
	outportb(base + LCR, (c | DLAB));		/* set divisor latch bit */
	outportb(base + DLL, n & 0x00ff);		/* set LSB of divisor latch */
	outportb(base + DLH, (n >> 8) & 0x00ff); /* set MSB of divisor latch */
	outportb(base + LCR, c);				/* restore line control reg */
	enable();								/* enable the interrupts */
}

long get_baud(void)
{
 int c, n;

	disable();								/* disable the interrupts */
	c = inportb(base + LCR);				/* get line control reg */
	outportb(base + LCR, (c | DLAB));		/* set divisor latch bit */
	n = inportb(base + DLH) << 8;			/* get MSB of divisor latch */
	n |= inportb(base + DLL);				/* get LSB of divisor latch */
	outportb(base + LCR, c);				/* restore the line control reg */
	enable();								/* enable the interrupts */
	return 115200L / (long)n;				/* return the baud rate */
}

int get_bits(void)
{
 return (inportb(base + LCR) & 3) + 5;
}

int get_parity(void)
{
 switch ((inportb(base + LCR) >> 3) & 7)
  {
 	 case 0:return NO_PARITY;
	 case 1:return ODD_PARITY;
	 case 3:return EVEN_PARITY;
	}
 return -1;
}

int get_stopbits(void)
{
	if (inportb(base + LCR) & 4)
		return 2;
	return 1;
}

void set_data_format(int bits, int parity, int stopbit)
{
	int n;

	if (parity < NO_PARITY || parity > ODD_PARITY) return;
	if (bits < 5 || bits > 8) return;
	if (stopbit < 1 || stopbit > 2) return;
	n = bits - 5;			 
	n |= ((stopbit == 1) ? 0 : 4); 
	switch (parity)
   {
	  case NO_PARITY:break;
	  case ODD_PARITY:n |= 0x08;break;
	  case EVEN_PARITY:n |= 0x18;break;
   }
	disable();		  
	outportb(base + LCR, n); 
	enable();					 
}

void set_port(long baud, int bits, int parity, int stopbit)
{
	if (!port_open) return;
	set_baud(baud); 
	set_data_format(bits, parity, stopbit); 
}

int in_ready(void)
{
	return !(sibuff == eibuff);
}

int carrier(void)
{
	return inportb(base + MSR) & DCD ? 1 : 0;
}

void set_dtr(int n)
{
 if (n) outportb(base + MCR, inportb(base + MCR) | DTR); /* assert DTR */
	else outportb(base + MCR, inportb(base + MCR) & ~DTR); /* unassert RTS */
}

