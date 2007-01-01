#include <opus/opus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <alsa/asoundlib.h>

#include "Buzz_inc.h"

char Sounds;
long VoiceOff, VoiceStart;

FILE *mvfile;

int soundbuf_size;
unsigned char *soundbuf;
int soundbuf_used;
int soundbuf_off;
int soundbuf_playing;

int dsp_fd;

static int buffer_size;
static int period_size;
static char *sound_start_buf;
static int xrun_count;

static void
set_hwparams(snd_pcm_t *handle)
{
        unsigned int rrate;
        snd_pcm_uframes_t size;
        int err, dir;
        snd_pcm_hw_params_t *hwparams;
	unsigned int uval;

        snd_pcm_hw_params_malloc(&hwparams);

        snd_pcm_hw_params_any(handle, hwparams);

        snd_pcm_hw_params_set_rate_resample(handle, hwparams, 1);

        snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_U8);
        snd_pcm_hw_params_set_channels(handle, hwparams, 1);

	uval = 11025;
        snd_pcm_hw_params_set_rate_near(handle, hwparams, &uval, 0);

	uval = 500 * 1000; /* usecs */
        snd_pcm_hw_params_set_buffer_time_near(handle, hwparams, &uval, &dir);
        snd_pcm_hw_params_get_buffer_size(hwparams, &size);
	buffer_size = size;

	uval = 100 * 1000;
        snd_pcm_hw_params_set_period_time_near(handle, hwparams, &uval, &dir);
        snd_pcm_hw_params_get_period_size(hwparams, &size, &dir);
	period_size = size;

        snd_pcm_hw_params(handle, hwparams);

	printf ("buffer_size %d; period_size %d\n", buffer_size, period_size);
}

static void
set_swparams(snd_pcm_t *handle)
{
        int err;
        snd_pcm_sw_params_t *swparams;

        snd_pcm_sw_params_malloc(&swparams);

        snd_pcm_sw_params_current(handle, swparams);

        /* start the transfer when the buffer is almost full: */
        /* (buffer_size / avail_min) * avail_min */
        snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);

        /* allow the transfer when at least period_size samples can be processed */
        snd_pcm_sw_params_set_avail_min(handle, swparams, period_size);

        /* align all transfers to 1 sample */
        snd_pcm_sw_params_set_xfer_align(handle, swparams, 1);

        snd_pcm_sw_params(handle, swparams);
}

snd_pcm_t *pcm;

void
sound_init (void)
{
	int err;
	char *device;

	Sounds = 0;

	if (Sounds == 0)
		return;

	device = "plughw:0,0";

	printf ("opening pcm device %s\n", device);
        if ((err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0) {
                printf("Playback open error: %s\n", snd_strerror(err));
		exit (1);
        }

	set_hwparams(pcm);
	set_swparams(pcm);

	sound_start_buf = malloc (buffer_size);
	memset (sound_start_buf, 0x80, buffer_size);

}

/* return 0 when there is nothing left to play */
int
sound_step (void)
{
	int avail, n;

	if (Sounds == 0)
		return (1);

	if (soundbuf_playing == 0)
		return (0);

	if ((avail = soundbuf_used - soundbuf_off) == 0) {
		soundbuf_playing = 0;

		snd_pcm_drain (pcm);
		return (0);
	}

	if (avail > period_size)
		avail = period_size;

	n = snd_pcm_writei (pcm, soundbuf + soundbuf_off, avail);

	if (n < 0) {
		if (n == -EAGAIN) {
			usleep (1);
			return (1);
		}
		if (n == -EPIPE) {
			xrun_count++;
			snd_pcm_prepare (pcm);
			return (1);
		}
		
		dbg ("sound write error: %s\n", 
		     snd_strerror(n));
		exit (1);
	}

//	dbg ("used %d (xrun %d)\n", n, xrun_count);


	soundbuf_off += n;

	return (1);
}


void
sound_play (FILE *f, int offset, int size)
{
	unsigned char *start_buf;

	if (Sounds == 0)
		return;

	xrun_count = 0;
	soundbuf_playing = 1;

	snd_pcm_start (pcm);
	snd_pcm_prepare (pcm);

	snd_pcm_writei (pcm, sound_start_buf, buffer_size);
	sound_step ();
}

void
PlayVoice(void) 
{
	printf ("**** PlayVoice\n");

	if (Sounds == 0)
		return;

	soundbuf_off = 0;
	soundbuf_playing = 1;

	snd_pcm_start (pcm);
	snd_pcm_prepare (pcm);

	snd_pcm_writei (pcm, sound_start_buf, buffer_size);

	sound_step ();

}

void
NUpdateVoice(void) 
{
	if (Sounds == 0)
		return;

//	printf ("NUpdateVoice\n");
	sound_step ();
}




void
KillVoice(void) 
{
	if (Sounds == 0)
		return;

	printf ("**** KillVoice\n");
}

void StopVoice(void)
{
	if (Sounds == 0)
		return;

	printf ("**** StopVoice\n");
}

char
AnimSoundCheck(void)
{
	if (soundbuf_playing)
		return (0);
	return (1);
}

