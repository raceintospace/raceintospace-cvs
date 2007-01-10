#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <memory.h>
#include "SDL.h"

#include "av.h"

void dbg (char const *fmt, ...) __attribute__ ((format (printf, 1, 2)));

SDL_Surface *sur;

static struct audio_channel Channels[AV_NUM_CHANNELS];

void
intr (int sig)
{
	SDL_Quit ();
	exit (0);
}

static int have_audio;

static SDL_AudioSpec audio_desired, audio_obtained;

static void
audio_callback (void *userdata, Uint8 *stream, int len)
{
	int togo, thistime, i, left;
	Uint8 silence = audio_obtained.silence;
	int num_active = AV_NUM_CHANNELS;
	unsigned  sample, sum;
	struct audio_channel* chp;

	struct {
		Uint8* data;
		unsigned coeff;
	} arr[AV_NUM_CHANNELS];

	togo = len;

	while (togo > 0 && num_active > 0) {
		thistime = togo;
		// sum = 0;
		sum = AV_NUM_CHANNELS * AV_MAX_VOLUME;
		num_active = AV_NUM_CHANNELS;

		/* init mixer data */
		for (i=0; i<AV_NUM_CHANNELS; ++i) {
			arr[i].data = NULL;
			arr[i].coeff = 0;
			chp = &Channels[i];
			// sum += chp->volume;
			if (chp->chunk) {
				left = chp->chunk->size - chp->offset;
				if (left < 0) {
					chp->chunk = NULL;
					chp->chunk_tailp = &chp->chunk;
					chp->offset = 0;
					num_active--;
					continue;
				}
				if (left < thistime)
					thistime = left;
				arr[i].data = chp->chunk->data + chp->offset;
				if (!chp->mute)
					arr[i].coeff = chp->volume;
			} else
				num_active--;
		}

		if (!num_active)
			break;

		left = thistime;

		while (left--) {
			sample = 0;
			/* Make this outer loop? */
			for (i=0; i<AV_NUM_CHANNELS; ++i)
				sample += arr[i].coeff * (
						arr[i].data ? *(arr[i].data++) : silence);
			sample /= sum;
			if (sample > 255)
				sample = 255;
			*(stream++) = (Uint8) sample;
		}

		togo -= thistime;
		assert(togo >= 0);

		for (i=0; i<AV_NUM_CHANNELS; ++i) {
			chp = &Channels[i];
			if (chp->chunk) {
				chp->offset += thistime;
				if (chp->offset >= chp->chunk->size) {
					chp->offset = 0;		
					if (!chp->chunk->loop) {
						chp->chunk = chp->chunk->next;
						if (chp->chunk == NULL)
							chp->chunk_tailp = &chp->chunk;
							/* why this lastp?? */
					}
				}
			}
		}
	}

	memset (stream, silence, togo);
}

/* 0 means busy playing audio; 1 means idle */
char
AnimSoundCheck(void)
{
	/* assume sound channel */
	av_step ();
	if (Channels[AV_SOUND_CHANNEL].chunk)
		return (0);
	return (1);
}

int
IsChannelMute(int channel)
{
	assert(channel >= 0 && channel < AV_NUM_CHANNELS);
	return Channels[channel].mute;
}

void
play (struct audio_chunk *new_chunk, int channel)
{
	struct audio_chunk *cp;
	struct audio_channel *chp;

	assert(channel >= 0 && channel < AV_NUM_CHANNELS);

	chp = &Channels[channel];

	if (have_audio == 0)
		return;

	SDL_LockAudio ();
	for (cp = chp->chunk; cp; cp = cp->next) {
		if (cp == new_chunk) {
			printf ("play: attempt to do duplicate chunk add\n");
			av_silence (channel);
			break;
		}
	}

	new_chunk->next = NULL;
	*chp->chunk_tailp = new_chunk;
	SDL_UnlockAudio ();
}

void
av_silence(int channel)
{
	int i = channel;

	if (channel == AV_ALL_CHANNELS) {
		for (i=0; i<AV_NUM_CHANNELS; ++i)
			av_silence(i);
	} else {
		assert(channel >= 0 && channel < AV_NUM_CHANNELS);
		if (Channels[channel].chunk) {
			SDL_LockAudio ();
			Channels[channel].chunk = NULL;
			Channels[channel].chunk_tailp = &Channels[channel].chunk;
			Channels[channel].offset = 0;
			SDL_UnlockAudio ();
		}
	}
}


Uint32
sdl_timer_callback (Uint32 interval, void *param)
{
	static SDL_Event tick;

	tick.type = SDL_USEREVENT;
	SDL_PushEvent (&tick);
	return (interval);
}

void
av_setup (int *argcp, char ***argvp)
{
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf (stderr, "SDL_Init error\n");
		exit (1);
	}

	if (SDL_InitSubSystem (SDL_INIT_AUDIO < 0)) {
		printf ("no audio\n");
	} else {
		printf ("audio initialized\n");
		have_audio = 1;
	}

	if ((sur = SDL_SetVideoMode (640, 400, 24,
					 SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
		fprintf (stderr, "error in SDL_SetVideoMode\n");
		exit (1);
	}

	SDL_EnableUNICODE (1);
	SDL_WM_SetCaption("Race Into Space", NULL);

	if (have_audio) {
		int i = 0;

		audio_desired.freq = 11025;
		audio_desired.format = AUDIO_U8;
		audio_desired.channels = 1;
		audio_desired.samples = 8192;
		audio_desired.callback = audio_callback;

		/* initialize audio channels */
		for (i=0; i<AV_NUM_CHANNELS; ++i) {
			Channels[i].volume = AV_MAX_VOLUME;
			Channels[i].mute = 0;
			Channels[i].chunk = NULL;
			Channels[i].chunk_tailp = &Channels[i].chunk; 
			Channels[i].offset = 0;
		}
		
		if (SDL_OpenAudio (&audio_desired, &audio_obtained) < 0) {
			fprintf (stderr, "error in SDL_OpenAudio\n");
			exit (1);
		}

		SDL_PauseAudio (0);
	}

	SDL_AddTimer (30, sdl_timer_callback, NULL);
}

#define KEYBUF_SIZE 256
int keybuf[KEYBUF_SIZE];
int keybuf_in_idx, keybuf_out_idx;

void
av_process_event (SDL_Event *evp)
{
	int c;

	switch (evp->type) {
	case SDL_QUIT:
		exit (0);
		break;
		
	case SDL_USEREVENT:
		break;

	case SDL_KEYDOWN:
		switch (evp->key.keysym.sym) {
		case SDLK_UP: c = 0x4800; break;
		case SDLK_DOWN: c = 0x5000; break;
		case SDLK_RIGHT: c = 0x4D00; break;
		case SDLK_LEFT: c = 0x4B00; break;
		default:
			c = evp->key.keysym.unicode;
			break;
		}
		if (c) {
			keybuf[keybuf_in_idx] = c;
			keybuf_in_idx = (keybuf_in_idx + 1)
				% KEYBUF_SIZE;
		}
		break;
		
	case SDL_MOUSEBUTTONDOWN:
		av_mouse_pressed_cur = 1;
		av_mouse_pressed_latched = 1;
		av_mouse_pressed_x = evp->button.x;
		av_mouse_pressed_y = evp->button.y;
		printf ("mouseclick(%d,%d)\n", 
			av_mouse_pressed_x,
			av_mouse_pressed_y);
		break;
		
	case SDL_MOUSEBUTTONUP:
		av_mouse_pressed_cur = 0;
		break;
		
	case SDL_MOUSEMOTION:
		av_mouse_cur_x = evp->motion.x;
		av_mouse_cur_y = evp->motion.y;
		break;
		
		/* ignore these events */
	case SDL_KEYUP:
	case SDL_ACTIVEEVENT:
		break;
	default:
		printf ("got uknown event %d\n", evp->type);
		break;
	}
}

/* non-blocking */
void
av_step (void)
{
	SDL_Event ev;
	
	while (SDL_PollEvent (&ev))
		av_process_event (&ev);
}

void
av_block (void)
{
	SDL_Event ev;

	/* 
	 * block until an event comes in
	 *
	 * we have a 30ms timer going, so that is the
	 * maximum wait time
	 */
	if (SDL_WaitEvent (&ev)) {
		av_process_event (&ev);
		av_step (); /* soak up any other currently available events */
	}
}

int
bioskey (int peek)
{
	int c;

	av_step ();

	if (peek) {
		if (keybuf_in_idx != keybuf_out_idx)
			return (1);
		return (0);
	}

	if (keybuf_in_idx == keybuf_out_idx)
		return (0);

	c = keybuf[keybuf_out_idx];
	keybuf_out_idx = (keybuf_out_idx + 1) % KEYBUF_SIZE;

	return (c);
}

void
UpdateAudio(void)
{
//	av_step ();
}

void
NUpdateVoice(void) 
{
	av_step ();
}

void
av_sync (void)
{
	int dest_row, dest_col, dest_idx;
	int src_row, src_col, src_idx;
	int pixel;
	char *p;
	char *outp;

	SDL_LockSurface (sur);

	for (dest_row = 0; dest_row < 400; dest_row++) {
		src_row = dest_row / 2;
		for (dest_col = 0; dest_col < 640; dest_col++) {
			src_col = dest_col / 2;

			src_idx = src_row * 320 + src_col;
			dest_idx = (dest_row * 640 + dest_col) * 3;

			pixel = screen[src_idx] & 0xff;
			p = pal + pixel * 3;

			outp = sur->pixels + dest_idx;

			outp[0] = p[2] * 4;
			outp[1] = p[1] * 4;
			outp[2] = p[0] * 4;
		}
	}

	SDL_UnlockSurface (sur);

	SDL_Flip (sur);

	screen_dirty = 0;
}

void
MuteChannel(int channel, int mute)
{
	int i;

	if (channel == AV_ALL_CHANNELS) {
		for (i=0; i<AV_NUM_CHANNELS; ++i)
			MuteChannel(i, mute);
	} else {
		assert(channel >= 0 && channel < AV_NUM_CHANNELS);
		Channels[channel].mute = mute;
	}
}
