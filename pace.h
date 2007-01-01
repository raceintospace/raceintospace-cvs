#ifndef __PACE_H__
#define __PACE_H__
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>

#include "av.h"

#define stricmp strcasecmp
#define strncmpi strncasecmp

#define far

typedef struct {
	unsigned char *vptr;
	int x1, y1, x2, y2;
	int w, h;
} GXHEADER;

typedef struct {
	int dummy;
} drvr_desc;

struct ffblk {
	char ff_name[64];
	int ff_ftime;
	int ff_fdate;
};

int findfirst (char *pattern, struct ffblk *ffblk, int flag);
int findnext (struct ffblk *ffblk);

#define grTRACK 1
#define grCARROW 2

unsigned char XMAS;
void grTrackMouse(int how);
void grSetMouseStyle (int style, int size);

int grInitMouse (void);
void grSetColor (int);
void grMoveTo (int x, int y);
void grLineTo (int x, int y);
void grPutPixel (int x, int y, int val);
void grGetMouseBounds(int *x1p, int *y1p, int *x2p, int *y2p);
void grSetMouseBounds (int x1, int y1, int x2, int y2);
void grSetMousePos (int x, int y);
int grGetPixel (int x, int y);
int grGetMousePos (int *xp, int *yp);
#define grLSOLID 1
void grSetLineStyle (int style, int width);
int grGetMouseButtons (void);
#define grHIDE 1
#define grSHOW 1
void grDisplayMouse (int mode);
void grDrawLine (int x1, int y1, int x2, int y2);
void grSetBkColor (int col);
void grClearArea (int x1, int y1, int x2, int y2);
#define grOUTLINE 1
void grDrawRect (int x1, int y1, int x2, int y2, int mode);
void grLineRel (int x, int y);
void grMoveRel (int x, int y);

#define grLBUTTON 1
#define grRBUTTON 2

void gxClearDisplay (int, int);
void gxGetImage (GXHEADER *hp, int a, int b, int c, int d, int e);


#define gxSET 1 /* mode */
#define gxXOR 2
void gxPutImage (GXHEADER *hp, int mode, int a, int b, int c);

void gxVirtualDisplay (GXHEADER *hp, int a, int b, int c,
		       int d, int e, int f, int g);
void gxDisplayVirtual (int a, int b, int c, int d, int e,
		       GXHEADER *hp, int f, int g);
void gxClearVirtual (GXHEADER *hp, int a);
void gxSetDisplayPalette (char *pal);
void gxVirtualVirtual (GXHEADER *hp, int a, int b, int c, int d,
		       GXHEADER *hp2, int x, int y, int mode);
#define gxVGA_13 1 /* 320x200 ... note: non-square pixels */
int gxVirtualSize (int mode, int w, int h);
void gxSetUserMalloc (void *(*malloc)(unsigned long),
		      int (*free)(void *),
		      unsigned long (*avail)(void));
#define gxCMM 1
int gxVirtualFree (int mode);
int gxCreateVirtual (int mode, GXHEADER *hp, int gxVGA_mode, int w, int h);
void gxDestroyVirtual (GXHEADER *hp);
void gxVirtualScale (GXHEADER *a, GXHEADER *b);

#define gxSUCCESS 0

void delay (int millisecs);
int brandom (int limit);
int biostime (int, long);
int filelength (int fd);
void itoa (int val, char *buf, int len);
void ltoa (long val, char *buf, int len);
int bioskey (int wait);
int inp (int port);

#define EMPTY_BODY {}

void _fmemcpy (void *dest, void *src, int count);

typedef struct {
	int dummy;
} *HTIMER;

void *farmalloc (int len);
void farfree (void *p);
long farcoreleft (void);
int getdisk (void);
void getcurdir (int drive, char *buf);

struct diskfree_t {
	int dummy;
};

#define _NOCURSOR 1
#define _NORMALCURSOR 2
void _setcursortype (int type);

void gotoxy (int x, int y);
#define BLUE 1
#define RED 2
#define WHITE 3
#define YELLOW 4
#define BLACK 5
void textbackground (int color);
void clrscr (void);
void clreol (void);
void textcolor (int color);
void cprintf (char *fmt, ...);

void *MK_FP (int seg, int off);
#define P_WAIT 1
void spawnl (int mode, char *cmd, ...);

void setcbrk (int val);

HTIMER AIL_register_timer (void (*func)(void));
void AIL_set_timer_period (HTIMER t, long ticks);
void AIL_start_timer (HTIMER t);

/* free disk space */
struct dfree {
	int df_sclus;
	int df_avail;
	int df_bsec;
};
void getdfree (int drdive, struct dfree *fp);

void randomize (void);

void SMove (void *p, int x, int y);
void LMove (void *p);

void absdiscread (int disk, long op, int offset, unsigned char *buf);
int getch (void);
void BFill(void *buffer,char color);

void window (int x1, int y1, int x2, int y2);
int _wscroll;

int setup_gtk (int *argcp, char ***argvp);
void step_gtk (void);
void display_gtk_img (unsigned char *rgb);
double get_time (void);

int show_intro_flag;

FILE *open_gamedat (char *name);
char *slurp_gamedat (char *name);

char *letter_dat;

void gr_sync (void);

void gr_set_color_map (unsigned char *map);
int gr_slow;

int RLED_img (void *src, void *dest, unsigned int src_size, int w, int h);

struct frm {
	FILE *fin;
	int next_frame_chunks;
	int frame_idx;
	int nframes;
	int frame_rate;
	unsigned char pal[768];
};

struct frm *frm_open (char *filename);
struct frm *frm_open_seq (int seq, int mode);
int frm_get2 (struct frm *frm, void *pixels_arg, void *map);
void frm_close (struct frm *frm);


char cdrom_dir[1000];

void remove_savedat (char *name);

void idle_loop (int ticks);
void idle_loop_secs (double secs);
void unimp (void);

void env_setup (void);

#endif /* __PACE_H__ */
