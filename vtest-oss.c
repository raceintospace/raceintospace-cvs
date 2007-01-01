#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/soundcard.h>

int dsp_fd;

void
sound_init (void)
{
	int val;
	struct audio_buf_info ai;

	if ((dsp_fd = open ("/dev/dsp", O_WRONLY | O_NONBLOCK)) < 0) {
		fprintf (stderr, "can't open /dev/dsp\n");
		exit (1);
	}

	val = 11025;
	ioctl (dsp_fd, SNDCTL_DSP_SPEED, &val);

	val = 0;
	ioctl (dsp_fd, SNDCTL_DSP_STEREO, &val);

	val = 1;
	ioctl (dsp_fd, SNDCTL_DSP_CHANNELS, &val);

	val = AFMT_U8;
	ioctl (dsp_fd, SNDCTL_DSP_SETFMT, &val);

	ioctl (dsp_fd, SNDCTL_DSP_GETOSPACE, &ai);
	printf ("avail frags %d; frags total %d; fragsize %d; bytes %d\n",
		ai.fragments, ai.fragstotal, ai.fragsize, ai.bytes);

	val = 0x04000a;
	if (ioctl (dsp_fd, SNDCTL_DSP_SETFRAGMENT, &val) < 0) {
		perror ("ioctl SETFRAGMENT");
		exit (1);
	}

	ioctl (dsp_fd, SNDCTL_DSP_GETOSPACE, &ai);
	printf ("avail frags %d; frags total %d; fragsize %d; bytes %d\n",
		ai.fragments, ai.fragstotal, ai.fragsize, ai.bytes);


}

int soundbuf_size;
unsigned char *soundbuf;
int soundbuf_used;
int soundbuf_off;
int soundbuf_playing;

int
sound_step (void)
{
	int avail, thistime;
	fd_set wset;
	struct timeval tv;

	if (soundbuf_playing == 0)
		return (0);

	if ((avail = soundbuf_used - soundbuf_off) == 0) {
		soundbuf_playing = 0;
		ioctl (dsp_fd, SNDCTL_DSP_POST, 0);
		ioctl (dsp_fd, SNDCTL_DSP_SYNC, 0);
		return (1);
	}

	FD_ZERO (&wset);
	FD_SET (dsp_fd, &wset);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (select (dsp_fd + 1, NULL, &wset, NULL, &tv) < 0)
		return (1);
	if (FD_ISSET (dsp_fd, &wset) == 0)
		return (1);

	thistime = write (dsp_fd, soundbuf + soundbuf_off, avail);

	//printf ("%5d %5d %5d\n", thistime, avail, soundbuf_used);

	if (thistime < 0) {
		fprintf (stderr, "sound error %s\n",
			 strerror (errno));
		exit (1);
	}
	soundbuf_off += thistime;
	return (1);
}

void
sound_play (unsigned char *buf, int size)
{
	if (size > soundbuf_size) {
		if (soundbuf)
			free (soundbuf);
		soundbuf_size = size;
		if ((soundbuf = malloc (soundbuf_size)) == NULL) {
			fprintf (stderr, "out of memory\n");
			exit (1);
		}
	}

	memcpy (soundbuf, buf, size);

	soundbuf_used = size;
	soundbuf_off = 0;
	soundbuf_playing = 1;
	sound_step ();
}

double
get_time (void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}

void
busywait (double secs)
{
	double start;
	start = get_time ();
	while (get_time () - start < secs)
		;
}

void
usage (void)
{
	fprintf (stderr, "usage: vtest idx\n");
	exit (1);
}

int
main (int argc, char **argv)
{
	int c;
	char *filename;
	int idx;
	FILE *f;
	char *rawbuf;
	FILE *outf;
	int i;

	struct TM {
		long offset;
		long size;
	} ABSnd;

	filename = "/l/baris/gamedat/unews.cdr";

	while ((c = getopt (argc, argv, "")) != EOF) {
		switch (c) {
		default:
			usage ();
		}
	}

	if (optind >= argc)
		usage ();

	idx = atoi (argv[optind++]);

	if (optind != argc)
		usage ();

	if ((f = fopen (filename, "r")) == NULL) {
		fprintf (stderr, "can't open %s\n", filename);
		exit (1);
	}

	fseek (f, idx * sizeof (ABSnd), SEEK_SET);
	fread (&ABSnd, sizeof ABSnd, 1, f);

	printf ("offset %ld; size %ld\n", ABSnd.offset, ABSnd.size);

	if ((rawbuf = malloc (ABSnd.size)) == NULL) {
		fprintf (stderr, "out of memory\n");
		exit (1);
	}

	fseek (f, ABSnd.offset, SEEK_SET);
	fread (rawbuf, 1, ABSnd.size, f);

	outf = fopen ("x", "w");
	fwrite (rawbuf, 1, ABSnd.size, outf);
	fclose (outf);

	outf = fopen ("y", "w");
	for (i = 0; i < ABSnd.size; i++)
		fprintf (outf, "%d\n", rawbuf[i] & 0xff);
	fclose (outf);

	sound_init ();
	sound_play ((unsigned char *)rawbuf, ABSnd.size);

	while (sound_step ())
		;
	usleep (250 * 1000);
	return (0);
}

