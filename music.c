#include "race.h"
#include "Buzz_inc.h"
#include "mmfile.h"
#include "pace.h"
#include "utils.h"


struct music_file {
	struct music_file *next;
	char *name;
	char *buf;
	size_t size;
};
struct music_file *music_files;

struct music_file *
get_music_file (char *name)
{
	struct music_file *mp;
	char fname[20];
	int chop;
    ssize_t bytes;

	for (mp = music_files; mp; mp = mp->next) {
		if (xstrcasecmp (name, mp->name) == 0)
			return (mp);
	}

	snprintf (fname, sizeof(fname), "%s.ogg", name);

	mp = xcalloc(1, sizeof *mp);
    mp->size = 0;

    bytes = load_audio_file(fname, &mp->buf, &mp->size);

    if (bytes < 0)
    {
        free(mp->buf);
        return NULL;
    }

    mp->name = xstrdup (name);
    mp->next = music_files;
    music_files = mp;

	/* XXX: last ~2s of our music files is just garbage, get rid of it */
	chop = 2 * 11025;
	if (bytes > chop)
		bytes -= chop;

	/* Shorten the mem to actual size */
	mp->buf = xrealloc(mp->buf, (mp->size = bytes));

    return mp;
}

struct music_key {
	int idx;
	char *name;
} music_key[] = {
	{ M_ASSEMBLY, "assembly" },
	{ M_ASTTRNG, "asttrng" },
	{ M_BADNEWS, "badnews" },
	{ M_DRUMSM, "drumsm" },
	{ M_ELEPHANT, "elephant" },
	{ M_FILLER, "filler" },
	{ M_FUTURE, "future" },
	{ M_GOOD, "good" },
	{ M_HARDWARE, "hardware" },
	{ M_HISTORY, "history" },
	{ M_INTEL, "intel" },
	{ M_INTELLEG, "intelleg" },
	{ M_INTERLUD, "interlud" },
	{ M_LIFTOFF, "liftoff" },
	{ M_MISSPLAN, "missplan" },
	{ M_NEW1950, "new1950" },
	{ M_NEW1970, "new1970" },
	{ M_PRES, "pres" },
	{ M_PRGMTRG, "prgmtrg" },
	{ M_RD, "r&d" },
	{ M_SOVTYP, "sovtyp" },
	{ M_SUCCESS, "success" },
	{ M_SVFUN, "svfun" },
	{ M_SVLOGO, "svlogo" },
	{ M_SVPORT, "svport2" },
	{ M_THEME, "theme" },
	{ M_UNSUCC, "unsucc" },
	{ M_USFUN, "usfun" },
	{ M_USMIL, "usmil" },
	{ M_USPORT, "usport2" },
	{ M_USSRMIL, "ussrmil" },
	{ M_VICTORY, "victory" },
	{ 0, NULL },
};

static struct music_file *cur_music;
static int same_music = 0;

void
PreLoadMusic(char val)
{
	struct music_key *dp;
	struct music_file *mf;

	for (dp = music_key; dp->name; dp++) {
		if (dp->idx == val)
			break;
	}

	if (dp->name == NULL) {
		/* WARN */ fprintf (stderr, "PreLoadMusic: unknown idx %d\n", val);
		return;
	}

	/* DEBUG */ /* printf ("PreLoadMusic(%s)\n", dp->name); */

	mf = get_music_file (dp->name);

	if (mf == cur_music)
		same_music = 1;
	else
		same_music = 0;

	cur_music = mf;
}

struct audio_chunk music_chunk;

void
PlayMusic(char mode) 
{
	if (cur_music == NULL)
		return;

	if (same_music)
		return;

    /*
    if (IsChannelMute(AV_MUSIC_CHANNEL))
        return;
    */

	KillMusic ();

	music_chunk.data = cur_music->buf;
	music_chunk.size = cur_music->size;
	music_chunk.loop = 1;
	play (&music_chunk, AV_MUSIC_CHANNEL);
}

void
KillMusic(void)
{
	av_silence (AV_MUSIC_CHANNEL);
}

void
UpdateMusic (void)
{
	av_step ();
}
