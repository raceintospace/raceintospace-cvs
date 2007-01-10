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

void
intr (int sig)
{
	SDL_Quit ();
	exit (0);
}

int have_audio;

SDL_AudioSpec audio_desired, audio_obtained;


struct audio_chunk *cur_chunk, **cur_chunk_tailp = &cur_chunk;
int cur_offset;

void
audio_callback (void *userdata, Uint8 *stream, int len)
{
	int togo, thistime;

	togo = len;

	while (togo && cur_chunk) {
		thistime = cur_chunk->size - cur_offset;
		if (thistime < 0) {
			cur_chunk = NULL;
			cur_chunk_tailp = &cur_chunk;
			cur_offset = 0;
			break;
		}
			
		if (thistime > togo)
			thistime = togo;
		memcpy (stream, cur_chunk->data + cur_offset, thistime);

		stream += thistime;
		togo -= thistime;
		cur_offset += thistime;

		if (cur_offset >= cur_chunk->size) {
			if (cur_chunk->loop == 0) {
				if ((cur_chunk = cur_chunk->next) == NULL)
					cur_chunk_tailp = &cur_chunk;
			}
			cur_offset = 0;
		}
	}

	memset (stream, audio_obtained.silence, togo);
}

/* 0 means busy playing audio; 1 means idle */
char
AnimSoundCheck(void)
{
	av_step ();
	if (cur_chunk)
		return (0);
	return (1);
}

void
play (struct audio_chunk *new_chunk)
{
	struct audio_chunk *cp;

	if (have_audio == 0)
		return;
	SDL_LockAudio ();
	for (cp = cur_chunk; cp; cp = cp->next) {
		if (cp == new_chunk) {
			printf ("play: attempt to do duplicate chunk add\n");
			av_silence ();
			break;
		}
	}
	new_chunk->next = NULL;
	*cur_chunk_tailp = new_chunk;
	SDL_UnlockAudio ();
}

void
av_silence (void)
{
	SDL_LockAudio ();
	cur_chunk = NULL;
	cur_chunk_tailp = &cur_chunk;
	cur_offset = 0;
	SDL_UnlockAudio ();
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
		audio_desired.freq = 11025;
		audio_desired.format = AUDIO_U8;
		audio_desired.channels = 1;
		audio_desired.samples = 8192;
		audio_desired.callback = audio_callback;
		
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
PlayAudio(char *name,char mode)
{
	printf ("PlayAudio(%s)\n", name);
	/* need to sum this sound with the music */
}

