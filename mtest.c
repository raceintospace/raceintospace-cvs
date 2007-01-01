#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
struct TM {
	char ID[6];
	long offset;
	long size;
};


struct music_dir {
	struct TM dir;
	int nfiles;
	struct TM *files;
};

FILE *musicf;
#define MDIRS 4 
struct music_dir dirs[MDIRS];

int
open_music_cat (char *filename)
{
	int i;
	struct music_dir *dp;

	if ((musicf = fopen (filename, "r")) == NULL)
		return (-1);

	for (i = 0, dp = dirs; i < MDIRS; i++, dp++)
		fread (&dp->dir, 1, sizeof dp->dir, musicf);
		
	for (i = 0, dp = dirs; i < MDIRS; i++, dp++) {
		dp->nfiles = dp->dir.size / sizeof (struct TM);
		dp->files = calloc (dp->nfiles, sizeof *dp->files);
		fseek (musicf, dp->dir.offset, SEEK_SET);
		fread (dp->files, dp->nfiles, sizeof *dp->files, musicf);
	}

	return (0);
}

int
get_music (char *filetype, int idx, void *buf, int bufsize)
{
	int len;
	int i;
	struct music_dir *dp;

	len = strlen (filetype);

	for (i = 0, dp = dirs; i < MDIRS; i++, dp++) {
		if (strncasecmp (dp->dir.ID, filetype, len) == 0)
			break;
	}

	if (i == MDIRS)
		return (-1);

	return (-1);
}

	
		

void
do_display_cat (void)
{
	FILE *f;
	struct TM MusicIndex;
	int i;

	if ((f = fopen ("/l/baris/gamedat/music.cat", "r")) == NULL) {
		fprintf (stderr, "can't open music.cat\n");
		exit (1);
	}

	for (i = 0; i < 36; i++) {
		if (fread (&MusicIndex, 1, sizeof MusicIndex, f) == 0) {
			printf ("unexpected EOF\n");
			exit (1);
		}
		printf ("%6.6s 0x%08lx %8ld\n",
			MusicIndex.ID,
			MusicIndex.offset,
			MusicIndex.size);
	}

	fclose (f);
}

int display_cat;

void
usage (void)
{
	fprintf (stderr, "usage: mtest\n");
	exit (1);
}

int
main (int argc, char **argv)
{
	int c;

	while ((c = getopt (argc, argv, "c")) != EOF) {
		switch (c) {
		case 'c':
			display_cat = 1;
			break;
		default:
			usage ();
		}
	}

	if (optind != argc)
		usage ();

	if (display_cat) {
		do_display_cat ();
		exit (0);
	}

	return (0);
}


