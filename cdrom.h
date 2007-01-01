#define IOCTL_READ 	3
#define IOCTL_WRITE 	12

#define CD_GETDISCINFO	10
#define CD_GETTRACKINFO	11
#define CD_GETQCHANINFO	12
#define CD_GETUPCCODE	14
#define CD_GETAUDIOSTAT	15

#define CD_CMD_EJECT	0
#define CD_CMD_RESET	2
#define CD_CMD_SEEK	131
#define CD_CMD_PLAY	132
#define CD_CMD_STOP 133
#define CD_CMD_RESUME 136

#define CDISHERE 		0x01
#define CDISPLAYING	0x02
#define CDISPAUSED	0x04

typedef unsigned char uchar;

struct cdromdrivestruct
{
	char unitcode;
	char far *ddheader;
};

struct cdreqheader
{
	char len;
	char unit;
	char cmd;
	int stat;
	char reserved0[8];
};

struct ioctlread
{
	struct cdreqheader cdh;
	char mdb;
	void far *buffer;
	int size;
	int ssn;
	void far *errbuf;
};
struct ioctlwrite
{
	struct cdreqheader cdh;
	char mdb;
	void far *buffer;
	int size;
	int ssn;
	void far *errbuf;
};
struct ioctlseek
{
	struct cdreqheader cdh;
	char addrmode;
	void far *buffer;
	int sectorcount;
	long startsector;
};
struct ioctlplay
{
	struct cdreqheader cdh;
	char addrmode;
	long startsector;
	long sectorcount;
};
struct ioctlstop
{
	struct cdreqheader cdh;
};
struct ioctlresume
{
	struct cdreqheader cdh;
};
struct ioctlstat
{
	char cmd;
	int status;
	long startloc;
	long endloc;
};

struct discinfo
{
	char cmd;
	char strk;
	char ltrk;
	long eodisc;
};

struct trackinfo
{
	char cmd;
	char track;
	uchar frame;
	uchar sec;
	uchar min;
	char dummy;
	char control;
};

struct qchaninfo
{
	char cmd;
	char caa;
	char track;
	char index;
	uchar min;
	uchar sec;
	uchar frame;
	char reserved1;
	uchar amin;
	uchar asec;
	uchar aframe;
};

struct upccode
{
	char cmd;
	char adr;
	char code[7];
	char zero;
	char aframe;
};

/* prototypes */
/* mscdex.asm */ int ismscdex(void);
/* mscdex.asm */ int getnumcdroms(void);
/* mscdex.asm */ int getfirstcdrom(void);
/* mscdex.asm */ void getcdromlist(struct cdromdrivestruct far *cdromlist);
/* mscdex.asm */ int getcopyrightfname(int drive, char far *copyrightfname);
/* mscdex.asm */ int getabstractfname(int drive, char far *abstractfname);
/* mscdex.asm */ int getbibliofname(int drive, char far *bibliofname);
/* mscdex.asm */ int readvtoc(int drive, int index, char far *dscbuf);
/* mscdex.asm */ int absdiscread(int drive, int count, long sector, char far *buffer);
/* mscdex.asm */ int absdiscwrite(int drive, int count, long sector, char far *buffer);
/* mscdex.asm */ int chkdrive(int drive);
/* mscdex.asm */ int getmscdexversion();
/* mscdex.asm */ void getcdromunits(char far *cdromunits);
/* mscdex.asm */ int getvdescpref(int drive);
/* mscdex.asm */ int setvdescpref(int drive, int pref);
/* mscdex.asm */ int getdirentry(int drive, char far *name, char far *buffer);
/* mscdex.asm */ void senddevreq(int drive, void far *cdh);
/* mscdex.asm */ int getlasterror(void);
/* mscdex.asm */ int clearlasterror(void);

/* cdrom.c */ cdplay(int drive, long frame, long lframe);
/* cdrom.c */ cdstop(int drive);
/* cdrom.c */ cdpause(int drive);
/* cdrom.c */ cdresume(int drive);
/* cdrom.c */ cdseek(int drive, long frame);
/* cdrom.c */ cdreset(int drive);
/* cdrom.c */ cdeject(int drive);
/* cdrom.c */ cdstatus(int drive);
/* cdrom.c */ cdaudiostatus(int drive, long *nextstart, long *nextend);
/* cdrom.c */ cdmediachanged(int drive, int *yesorno);
/* cdrom.c */ int cddiscinfo(int drive, struct discinfo *di);
/* cdrom.c */ int cdtrackinfo(int drive, int track, struct trackinfo *ti);
/* cdrom.c */ int cdqchaninfo(int drive, struct qchaninfo *qi);
/* cdrom.c */ isanaudiocd(int drive);
/* cdrom.c */ long redtolong(long redaddress);
/* cdrom.c */ long longtored(long longval);
/* cdrom.c */ long msftolong(long msfvalue);
/* cdrom.c */ inttobcd(int data);
/* cdrom.c */ bcdtoint(int data);
/* cdrom.c */ fixmsf(int *min, int *sec, int *frame);
/* cdrom.c */ cdseekmsf(int drive, int min, int sec, int frame);
/* cdrom.c */ cdplaymsf(int drive, int min, int sec, int frame, int lmin, int lsec, int lframe);


