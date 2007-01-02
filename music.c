#include <vorbis/vorbisfile.h>

#include "Buzz_inc.h"


struct music_file {
	struct music_file *next;
	char *name;
	char *buf;
	int size;
};
struct music_file *music_files;

struct music_file *
get_music_file (char *name)
{
	struct music_file *mp;
	FILE *inf;
	int togo, offset;
	OggVorbis_File vf;
	int ret;
	int bs;
	char fullname[1000];
	int chop;

	for (mp = music_files; mp; mp = mp->next) {
		if (strcasecmp (name, mp->name) == 0)
			return (mp);
	}

	sprintf (fullname, "%s/%s.ogg", music_dir, name);

	if ((inf = fopen (fullname, "r")) == NULL) {
		printf ("can't open music file %s\n", fullname);
		return (NULL);
	}

	if ((mp = calloc (1, sizeof *mp)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	if ((mp->name = strdup (name)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	mp->next = music_files;
	music_files = mp;

	if (ov_open (inf, &vf, NULL, 0) < 0) {
		fprintf(stderr, "ERROR: Failed to open input as vorbis\n");
		goto bad;
	}

	if (ov_info(&vf, 0)->channels != 1) {
		fprintf (stderr, "ERROR: ogg file must be mono\n");
		goto bad;
	}

        mp->size = ov_pcm_total(&vf, 0); /* output size in bytes */

	if ((mp->buf = calloc (1, mp->size)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	togo = mp->size;
	offset = 0;

	while (togo > 0) {
		if ((ret = ov_read (&vf, mp->buf + offset,
				    togo, 0, 1, 0, &bs)) < 0)
			break;

		offset += ret;
		togo -= ret;
	}

	ov_clear(&vf); /* closes inf */

	chop = 2 * 11025;
	if (mp->size > chop)
		mp->size -= chop;

	return (mp);
bad:
	fclose (inf);
	return (mp);
}

struct music_key {
	int idx;
	char *name;
} music_key[] = {
	{ M_ASSEMBLY, "ASSEMBLY" },
	{ M_ASTTRNG, "ASTTRNG" },
	{ M_BADNEWS, "BADNEWS" },
	{ M_DRUMSM, "DRUMSM" },
	{ M_ELEPHANT, "ELEPHANT" },
	{ M_FILLER, "FILLER" },
	{ M_FUTURE, "FUTURE" },
	{ M_GOOD, "GOOD" },
	{ M_HARDWARE, "HARDWARE" },
	{ M_HISTORY, "HISTORY" },
	{ M_INTEL, "INTEL" },
	{ M_INTELLEG, "INTELLEG" },
	{ M_INTERLUD, "INTERLUD" },
	{ M_LIFTOFF, "LIFTOFF" },
	{ M_MISSPLAN, "MISSPLAN" },
	{ M_NEW1950, "NEW1950" },
	{ M_NEW1970, "NEW1970" },
	{ M_PRES, "PRES" },
	{ M_PRGMTRG, "PRGMTRG" },
	{ M_RD, "R&D" },
	{ M_SOVTYP, "SOVTYP" },
	{ M_SUCCESS, "SUCCESS" },
	{ M_SVFUN, "SVFUN" },
	{ M_SVLOGO, "SVLOGO" },
	{ M_SVPORT, "SVPORT2" },
	{ M_THEME, "THEME" },
	{ M_UNSUCC, "UNSUCC" },
	{ M_USFUN, "USFUN" },
	{ M_USMIL, "USMIL" },
	{ M_USPORT, "USPORT2" },
	{ M_USSRMIL, "USSRMIL" },
	{ M_VICTORY, "VICTORY" },
	{ 0, NULL },
};

struct music_file *cur_music;

void
PreLoadMusic(char val)
{
	struct music_key *dp;

	for (dp = music_key; dp->name; dp++) {
		if (dp->idx == val)
			break;
	}

	if (dp->name == NULL) {
		printf ("PreLoadMusic: unknown idx %d\n", val);
		return;
	}

	printf ("PreLoadMusic(%s)\n", dp->name);

	cur_music = get_music_file (dp->name);
}

struct audio_chunk music_chunk;

void
PlayMusic(char mode) 
{
	KillMusic ();

	if (cur_music == NULL)
		return;
	music_chunk.data = cur_music->buf;
	music_chunk.size = cur_music->size;
	music_chunk.loop = 1;
	play (&music_chunk);
}

void
KillMusic(void)
{
	av_silence ();
}

void
UpdateMusic (void)
{
	av_step ();
}
