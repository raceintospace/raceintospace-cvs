#include "Buzz_inc.h"
#include "externs.h"
#include "assert.h"
#include "pace.h"
#include "av.h"

char cdrom_dir[1000];
char savedat_dir[1000];
char music_dir[1000];
char movies_dir[1000];

extern GXHEADER vhptr;

void frm_init (void);

char
DoModem(int sel)
{
	printf ("DoModem not implemented\n");
	return (0);
}

char
MPrefs(char mode) 
{
	return (0);
}

struct Prest_Upd MP[3];


int put_serial(unsigned char n) {return 0;}
void MesCenter(void){}





int
AquireDrive (void)
{
	return 'K' - 'A';
}

int
getdisk (void) 
{
	return ('C' - 'A');
}

char *
slurp_gamedat (char *name)
{
	FILE *f;
	ssize_t len;
	char *p = NULL;
    size_t buflen = 0;

	if (NULL == (f = open_gamedat (name)))
        return NULL;

    len = fread_dyn(&p, &buflen, f); 

    if (len < 0)
    {
        perror("slurp_gamedat");
        exit(EXIT_FAILURE);
    }

	fclose (f);

	return (p);
}

char *letter_dat;

void
OpenEmUp(void)
{
	srand(clock());

	frm_init ();

	GV(&vhptr,320,200);     // Allocate only Virtual Buffer

	XMAS=1; /* we do have a mouse */

	letter_dat = slurp_gamedat ("letter.dat");
}

double
get_time (void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}

int
PCX_D (void *src_raw,void *dest_raw,unsigned src_size)
{
  char *src = src_raw;
  char *dest = dest_raw;
  char num;
  char *orig_dest = dest;
  do {
    if ((*src&0xc0)==0xc0) {
      num=*(src++)&0x3f; src_size--;
      while ((num--)>0) {*(dest++)=*src;}
      src++;src_size--;
    } else {
      (*dest++)=*(src++);src_size--;
    };
  }  while(src_size);
  return (dest - orig_dest);
}

int
RLED (void *src_raw, void *dest_raw, unsigned int src_size)
{
	signed char *src = src_raw;
	signed char *dest = dest_raw;
	unsigned int used;
	int count, val;
	int i;

	used = 0;
	while (used < src_size) {
		count = src[used++];
		
		if (count < 0) {
			count = -count + 1;
			val = src[used++];
			for (i = 0; i < count; i++)
				*dest++ = val;
		} else {
			count++;
			for (i = 0; i < count; i++)
				*dest++ = src[used++];
		}
	}

	if (0)
		printf ("total bytes %d\n", (void *)dest - (void *)dest_raw);

	return ((void *)dest - (void *)dest_raw);
}

int
RLED_img (void *src_raw, void *dest_raw, unsigned int src_size, int w, int h)
{
	signed char *src = src_raw;
	signed char *dest;
	unsigned int used;
	int count, val;
	int total;
	signed char buf[128 * 1024];
	int row;

	dest = buf;

	used = 0;
	while (used < src_size) {
		count = src[used++];
		
		if (count < 0) {
			count = -count + 1;
			val = src[used++];
            memset(dest, val, count);
            dest += count;
		} else {
			count++;
            memcpy(dest, &src[used], count);
            used += count;
            dest += count;
		}
	}

	total = dest - buf;

	if (total < w * h + h) {
		memcpy (dest_raw, buf, w * h);
		return (w * h);
	}

	dest = dest_raw;
	for (row = 0; row < h; row++) {
        memcpy(dest, &buf[row * (w+1)], w);
        dest += w;
	}

	return (w * h);
}

/*
 * Original sources say:
 * @param wh = 0 - fade colors from 0 to val,
 *           = 1 - fade colors from val to 3*256,
 *           = 2 - fade all colors
 * @param palx pointer to palette
 * @param steps
 * @param val pivot index in palette array
 * @param mode if mode == 1 then preserve non-faded colors, else make black
 */
void
FadeIn (char wh,char *palx,int steps,int val,char mode)
{
    int from = 0;
    int to = 256;
    if (wh == 0)
        to = val;
    else if (wh == 1)
        from = val;
    else
        assert(wh == 2);
    av_set_fading(AV_FADE_IN, from, to, steps, !!mode);
}

void
FadeOut(char wh,char *palx,int steps,int val,char mode)
{
    int from = 0;
    int to = 256;

    if (wh == 0)
        to = val;
    else if (wh == 1)
        from = val;
    else
        assert(wh == 2);
    av_set_fading(AV_FADE_OUT, from, to, steps, !!mode);
}

void
delay (int millisecs)
{
	idle_loop_secs (millisecs / 1000.0);
}

void
bzdelay (int ticks)
{
	idle_loop_secs (ticks / 100.0);
}

int
CDAccess (int drive,int track,char op)
{
	return (0);
}


int
brandom (int limit)
{
	if (limit == 0)
		return (0);
	return (rand () % limit);
}

long RLEC (char *src, char *dest, unsigned int src_size)
{
	unsigned int src_i;
	int dest_i,cpr;

	for (src_i = dest_i=0; src_i< src_size; ) {
		int k;  /* holds the number of characters to copy or repeat. */
		k = 0;
		cpr= ((src_size-src_i-1)<128) ? src_size-src_i-1 : 128;
		while (k<cpr && src[src_i] != src[src_i+1]) {
			k++;       /* increment the number of characters to copy */
			src_i++;   /* move pointer to the next character */
		}
		if (k) {
			dest[dest_i++] = (k-1);
			memcpy (&dest[dest_i], &src[src_i-k], k);
			dest_i += k;
		} else {
			k = 2;   /* there are at least two characters to be repeated */
			src_i++; /* advance pointer beyond the first match*/
			while ( k<cpr && src[src_i] == src[src_i+1]) {
				k++;       /* increment the number of characters to copy */
				src_i++;   /* move pointer to the next character */
			}  /* while */
			dest[dest_i++] = (-k+1);
			dest[dest_i++] = src[src_i++];
		}
	}
	return (dest_i);
}

#if defined(__linux__) || defined(CONFIG_MACOSX)
int
biostime (int a, long b)
{
	struct timeval tv;

	gettimeofday (&tv, NULL);
	return (tv.tv_sec * 1000 * 1000 + tv.tv_usec);
}
#endif

void
StopAudio(char mode) 
{
}

void
getcurdir (int drive, char *buf)
{
	getcwd (buf, 100);
}

static DIR *f_dir;


int
first_saved_game (struct ffblk *ffblk)
{
	if (f_dir) {
		closedir (f_dir);
		f_dir = NULL;
	}

	if ((f_dir = opendir (savedat_dir)) == NULL)
		return (1);

	return (next_saved_game (ffblk));
}

int
next_saved_game (struct ffblk *ffblk)
{
	struct dirent *dp;
	int len;

	memset (ffblk, 0, sizeof *ffblk);

	if (f_dir == NULL)
		return (1);

	while ((dp = readdir (f_dir)) != NULL) {
		len = strlen (dp->d_name);
		if (len < 4)
			continue;
		if (xstrncasecmp (dp->d_name + len - 4, ".SAV", 4) != 0)
			continue;

		strncpy (ffblk->ff_name, dp->d_name, sizeof ffblk->ff_name);
		ffblk->ff_name[sizeof ffblk->ff_name - 1] = 0;

		return (0);
	}

	return (1);
}


void
CloseEmUp (unsigned char error,unsigned int value)
{
	printf ("CloseEmUp()\n");
	exit(EXIT_SUCCESS);
}

struct tblinfo {
	int count;
	char **strings;
};

void
frm_read_tbl (char *keyname, struct tblinfo *tbl)
{
	FILE *fin;
	int lo, hi;
	int idx;
    char *p;
	char name[100];

	if ((fin = sOpen (keyname, "rb", 0)) == NULL)
		return;

	lo = getc (fin);
	hi = getc (fin);
	tbl->count = (hi << 8) | lo;
	
	tbl->strings = xcalloc(tbl->count, sizeof *tbl->strings);

	idx = 0;
	while (fread (name, 1, 8, fin) == 8) {
		name[8] = '\0';
        for (p = name; *p; p++) {
            *p = tolower (*p);
            if (*p == '#')
                *p = '_';
        }
		tbl->strings[idx++] = xstrdup (name);
	}

	/* now idx is number of read strings */
	if (tbl->count != idx) {
		tbl->count = idx;
		tbl->strings = xrealloc(tbl->strings, sizeof *tbl->strings);
	}
		
	fclose (fin);
}

struct tblinfo frm_tbl, frm_ftbl;

void
frm_init (void)
{
	frm_read_tbl ("SEQ.KEY", &frm_tbl);
	frm_read_tbl ("FSEQ.KEY", &frm_ftbl);
}

char *
seq_filename (int seq, int mode)
{
	struct tblinfo *tp;

	if (mode == 0)
		tp = &frm_tbl;
	else
		tp = &frm_ftbl;

	if (seq < 0 || seq >= tp->count)
		return (NULL);

	return (tp->strings[seq]);
}

struct frm *
frm_open_seq (int seq, int mode)
{
	struct tblinfo *tp;
	char filename[1000];

	if (mode == 0)
		tp = &frm_tbl;
	else
		tp = &frm_ftbl;

	if (seq < 0 || seq >= tp->count)
		return (NULL);

	sprintf (filename, "%s/rom/%s.frm", cdrom_dir, tp->strings[seq]);
	return (frm_open (filename));
}

struct frm *
frm_open (char *filename)
{
	struct frm *frm;
	FILE *fin;

	printf ("frm_open(\"%s\")\n", filename);

	if ((fin = fopen (filename, "rb")) == NULL)
		return (NULL);

	frm = xcalloc(1, sizeof *frm);

	frm->fin = fin;
	frm->next_frame_chunks = 8;
	frm->frame_idx = 0;

	return (frm);
}
		
void
frm_close (struct frm *frm)
{
	if (frm) {
		if (frm->fin)
			fclose (frm->fin);
		free (frm);
	}
}

int
frm_get2 (struct frm *frm, void *pixels_arg, void *map)
{
	unsigned char raw[64 * 1024];
	unsigned char pbuf[64 * 1024];
	unsigned int n;
	int val0, val1;
	unsigned char *pixels;

	n = frm->next_frame_chunks * 2048;

	if (n == 0)
		return (0);

	if (n > sizeof raw)
		return (-1);

	fread (raw, 1, n, frm->fin);
	val0 = raw[0] | (raw[1] << 8);
	val1 = raw[2];

	if (frm->next_frame_chunks == 8) {
		if (frm->frame_idx == 0) {
			frm->nframes = val0;
			frm->frame_rate = raw[15998] | (raw[15999] << 8);
			memcpy (frm->pal + 384, raw + 16000, 384);
		}
		pixels = raw;
	} else {
		int compressed_size = val0;
		RLED (raw + 3, pbuf, compressed_size);
		pixels = pbuf;
	}

	memcpy (pixels_arg, pixels, 160 * 100);
	memcpy (map, frm->pal + 384, 384);
			
	frm->next_frame_chunks = val1;
	frm->frame_idx++;

	return (1);
}

void
SMove (void *p, int x, int y)
{
	GXHEADER local;

	GV (&local, 160, 100);
	memcpy (local.vptr, p, 160 * 100);
	gxPutImage (&local, gxSET, x, y, 0);
	DV (&local);
}

void
LMove (void *p)
{
	GXHEADER local;

	memset (screen, 0, 320 * 200);

	GV (&local, 160, 100);
	memcpy (local.vptr, p, 160 * 100);
	gxPutImage (&local, gxSET, 320 / 4, 200 / 4, 0);
	DV (&local);
}	

void
randomize (void)
{
	srand (clock ());
}

void
idle_loop_secs (double secs)
{
	double start;

	gr_sync ();

	start = get_time ();

	while (1) {
		av_block ();

		if (get_time () - start >= secs)
			break;
	}
}

void
idle_loop (int ticks)
{
	idle_loop_secs (ticks / 2000.0);
}

long VoiceOff;

char *soundbuf;
size_t soundbuf_size = 0;
size_t soundbuf_used = 0;
struct audio_chunk news_chunk;
	
void
soundbuf_alloc(size_t size)
{
	if (size > soundbuf_size)
		soundbuf = xrealloc(soundbuf, soundbuf_size = size);
}

void
NGetVoice(char plr,char val)
{
	struct TM {
		int32_t offset;
		int32_t size;
	} ABSnd;

	FILE *mvfile;

	printf ("**** NGetVoice(%d,%d)\n", plr, val);

	if (VoiceOff!=-1) KillVoice();
	VoiceOff=val;

	mvfile = sOpen( (plr==0) ? "UNEWS.CDR" : "SNEWS.CDR" ,"rb",0);
	if (mvfile==NULL) {VoiceOff=-1;return;}  // file not here

	fseek(mvfile,val*(sizeof ABSnd),SEEK_SET);
	fread(&ABSnd,sizeof ABSnd,1,mvfile);
	
	Swap32bit(ABSnd.offset);
	Swap32bit(ABSnd.size);

	fseek(mvfile,ABSnd.offset,SEEK_SET);

	printf ("offset %d; size %d\n", ABSnd.offset, ABSnd.size);

	soundbuf_alloc (ABSnd.size);

	fseek (mvfile, ABSnd.offset, SEEK_SET);
	fread (soundbuf, 1, ABSnd.size, mvfile);

	fclose (mvfile);

	soundbuf_used = ABSnd.size;
}

void
PlayVoice (void)
{
	news_chunk.data = soundbuf;
	news_chunk.size = soundbuf_used;
	play (&news_chunk, AV_SOUND_CHANNEL);
}

void
KillVoice (void)
{
	av_silence(AV_SOUND_CHANNEL);
}

void
StopVoice (void)
{
	av_silence(AV_SOUND_CHANNEL);
}

void
PlayAudio(char *name, char mode)
{
	FILE* file;
    int size = 16 * 1024;
    int total = 0;
    int bytes = 0;

	printf ("PlayAudio(%s)\n", name);
	file = open_gamedat(name);
	if (!file)
		return;

    av_silence(AV_SOUND_CHANNEL);

    do {
        soundbuf_alloc (size+total);
        bytes = fread(soundbuf+total, 1, size, file);
        total += bytes;
    } while (bytes);

	fclose (file);

	soundbuf_used = total;

	PlayVoice();
}

int
getch (void)
{
	int c;

	while (1) {
		av_block ();
		if ((c = bioskey (0)) != 0)
			return (c);
	}
}

void
play_audio (int sidx, int mode)
{
	char *raw_name;
	char filename[1000];
	FILE *f;
	ssize_t size;

	if ((raw_name = seq_filename (sidx, mode)) == NULL) {
		printf ("can't find audio file for %d,%d\n",
			sidx, mode);
		return;
	}

	sprintf (filename, "%s.raw", raw_name);

	printf ("play %s\n", filename);

	if ((f = open_gamedat (filename)) == NULL) {
		printf ("can't open sound %d,%d: %s\n", 
			sidx, mode, filename);
		return;
	}

    size = fread_dyn(&soundbuf, &soundbuf_size, f);
    if (size < 0)
    {
        perror("play_audio");
        exit(EXIT_FAILURE);
    }

	fclose (f);

	soundbuf_used = size;

	PlayVoice ();
}

void *
xmalloc (size_t n)
{
	void *p;

	if ((p = malloc (n)) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

void *
xcalloc (size_t a, size_t b)
{
	void *p;

	if ((p = calloc (a, b)) == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

char *
xstrdup (const char *s)
{
	void *p;

	p = xmalloc (strlen (s) + 1);
	strcpy (p, s);
	return (p);
}

void *
xrealloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);

	if (!p)
    {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

ssize_t
fread_dyn(char **destp, size_t *n, FILE *stream)
{
    const unsigned bsize = 8192;
    size_t total = 0, cnt = 0; 

    assert(destp);
    assert(n);
    assert(stream);

    if (!*destp) 
        *destp = xmalloc(*n = bsize);

    while (1) {
        cnt = fread(*destp+total, 1, *n-total, stream);
        if (cnt != *n-total)
        {
            if (feof(stream))
                return total+cnt;
            else if (ferror(stream))
                return -1;
        }
        total += cnt;

        if (*n <= total)
            *destp = xrealloc(*destp, *n += bsize);
    }
}

#define debug_file stdout
void
vdbg (char const *fmt, va_list args)
{
	char buf[10000];
	char *p;
	struct tm tm;
	struct timeval tv;
	time_t t;
	int binchars;
	static unsigned long last_millitime;
	unsigned long this_millitime;
	int delta;

	gettimeofday (&tv, NULL);
	t = tv.tv_sec;
	tm = *localtime (&t);

	p = buf;

	sprintf (p, "%02d:%02d:%02d.%03d ", tm.tm_hour, tm.tm_min, tm.tm_sec,
		 (int)(tv.tv_usec / 1000));
	p += strlen (p);

	this_millitime = (tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec) 
		* 1000 + tv.tv_usec / 1000;

	if (last_millitime == 0)
		last_millitime = this_millitime;

	delta = this_millitime - last_millitime;
	last_millitime = this_millitime;

	if (delta < 0)
		delta = 0;

	sprintf (p, "%5d ", delta);
	p += strlen (p);

	vsprintf (p, fmt, args);

	p += strlen (p);
	while (p != buf && (p[-1] == '\n' || p[-1] == '\r'))
		*--p = 0;

	binchars = 0;
	for (p = buf; *p && binchars < 20; p++) {
		int c = *p;
		if ((c >= ' ' && c < 0177) || c == '\t' || c == '\n') {
			putc (c, debug_file);
		} else {
			binchars++;
			putc ('.', debug_file);
		}
	}
	putc ('\n', debug_file);
	fflush (debug_file);
}

void
dbg (char const *fmt, ...)
{
	va_list args;

	va_start (args, fmt);
	vdbg (fmt, args);
	va_end (args);
}

#ifdef _WIN32
#include <sys/timeb.h>
int
gettimeofday (struct timeval *tv, struct timezone *tz)
{
	struct _timeb t;

	_ftime (&t);
	tv->tv_sec = t.time;
	tv->tv_usec = t.millitm * 1000;
	return (0);
}
#endif

int
xstrcasecmp (char const *a, char const *b)
{
	while (*a) {
		if (tolower (*a & 0xff) != tolower (*b & 0xff))
			break;
		a++;
		b++;
	}
	return (*a - *b);
}

int
xstrncasecmp (char const *a, char const *b, int n)
{
	while (n && *a) {
		if (tolower (*a & 0xff) != tolower (*b & 0xff))
			break;
		a++;
		b++;
		n--;
	}
	return (*a - *b);
}
