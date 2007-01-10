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

// SERIAL_F.C
void (_interrupt  *oldvect)();
void SaveFirst(char);
void CheckFirst(char);
void UpdateFirst(void);
void SendFirst(void);
void RecvFirst(void);
int e_handler(int c, long p, char *s);
void SaveFirst(char mode);
void SendFirst(void);
void UpdateFirst(void);
void CheckFirst(char Hmode);
void RecvFirst(void);
int port_exist(int port);
void open_port(int port, int inlen);
void close_port(void);
void purge_in(void);
void set_baud(long baud);
long get_baud(void);
int get_bits(void);
int get_parity(void);
int get_stopbits(void);
void set_data_format(int bits, int parity, int stopbit);
void set_port(long baud, int bits, int parity, int stopbit);
int in_ready(void);
int carrier(void);
void set_dtr(int n);

// CDROM.C
int CDAccess(int drive,int track,char mode);
int AquireDrive(void);

// TRACK.C
void Message(int msg, int tr);
void AAset(void);
void DialAt(int x,int y,char *s);
void get_dial(char *old);
void Read_Config(void);
int e_handler(int c, long p, char *s);
void Dnload(void);
void Upload(void);
void ReadMsg(void);
void WriteMsg(void);
void Write_Config(void);
void DrawCenter();
void MesCenter(void);
char MPrefs(char mode);
void Toggle_Cfg(int opt,int old);
void TrackPict(char mode);

// PROTOCOL.C
void Read_CRC(void);
void SaveSide(char side);
void UpdPrestige();
void RecvSide(char side);
static int timed_get(int n);
static int build_block(int l, FILE *file);
static void abort_transfer(void);
static void parse(char *p, char *f);
int xmit_file(int (*error_handler)(int c, long p, char *s), char *files[]);
static int getblock(int block, int crc);
int recv_file(int(*error_handler)(int c, long p, char *s), char *path);


// MODEM.CPP
char dial(void);
char CheckCarrier(void);
void Modem_Config(void);
char *get_modem_string(char *s, char *d);
char DoModem(int sel);
void exit_program(void);
void hangup(void);
void modem_control_string(char *s);


