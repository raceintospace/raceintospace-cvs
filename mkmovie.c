#include <opus/opus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

int vflag;

int
RLED (void *src_raw, void *dest_raw, unsigned int src_size)
{
	signed char *src = src_raw;
	signed char *dest = dest_raw;
	short used, count, val;
	short i;

	used = 0;
	while (used < src_size) {
		count = src[used++];
		
		if (count < 0) {
			count = -(count - 1);
			val = src[used++];
			
			if (vflag)
				printf ("%6d: repeat %02x\n", count, val&0xff);

			for (i = 0; i < count; i++)
				*dest++ = val;
		} else {
			count++;

			if (vflag)
				printf ("%6d:", count);

			for (i = 0; i < count; i++) {
				if (vflag)
					printf (" %02x", src[used] & 0xff);
				*dest++ = src[used++];
			}

			if (vflag)
				printf ("\n");
		}
	}

	return ((void *)dest - (void *)dest_raw);
}

void
usage (void)
{
	fprintf (stderr, "usage: mkmovie filename\n");
	exit (1);
}

struct frm {
	FILE *fin;
	int next_frame_chunks;
	int frame_idx;
	int nframes;
	int frame_rate;
	unsigned char pal[768];
};

struct frm *
frm_open (char *filename)
{
	struct frm *frm;
	FILE *fin;

	if ((fin = fopen (filename, "r")) == NULL)
		return (NULL);

	if ((frm = calloc (1, sizeof *frm)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	frm->fin = fin;
	frm->next_frame_chunks = 8;
	frm->frame_idx = 0;

	return (frm);
}
		
int
frm_get (struct frm *frm, unsigned char *rgb)
{
	unsigned char raw[64 * 1024];
	unsigned char pbuf[64 * 1024];
	int n;
	int val0, val1;
	unsigned char *pixels;
	unsigned char *outp;
	int idx;
	int pixel;
	unsigned char *up;

	n = frm->next_frame_chunks * 2048;

	if (n == 0)
		return (0);

	if (n < 0 || n > sizeof raw)
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

	outp = rgb;
	for (idx = 0; idx < 160 * 100; idx++) {
		int r, g, b;

		pixel = pixels[idx];
		up = frm->pal + pixel * 3;

		r = up[0];
		g = up[1];
		b = up[2];

		*outp++ = r;
		*outp++ = g;
		*outp++ = b;
	}
			
	frm->next_frame_chunks = val1;
	frm->frame_idx++;

	return (1);
}

FILE *movief;

int
main (int argc, char **argv)
{
	int c;
	char *filename;
	struct frm *frm;
	int rc;
	unsigned char rgb[160 * 100 * 3];

	while ((c = getopt (argc, argv, "")) != EOF) {
		switch (c) {
		default:
			usage ();
		}
	}

	if (optind >= argc)
		usage ();

	filename = argv[optind++];

	if (optind != argc)
		usage ();

	if ((frm = frm_open (filename)) == NULL) {
		fprintf (stderr, "can't open %s\n", filename);
		exit (1);
	}

	if ((movief = fopen ("m.ppm", "w")) == NULL) {
		fprintf (stderr, "can't create m.ppm\n");
		exit (1);
	}

	while (1) {
		if ((rc = frm_get (frm, rgb)) < 0) {
			printf ("error reading frame\n");
			exit (1);
		}

		if (rc == 0)
			break;

		fprintf (movief, "P6\n160 100\n255\n");
		fwrite (rgb, 1, 160 * 100 * 3, movief);
	}

	return (0);
}

