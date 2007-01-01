#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <memory.h>
#include "SDL.h"

SDL_Surface *sur;

double
get_time (void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}

void
intr (int sig)
{
	SDL_Quit ();
	exit (0);
}

int have_audio;

SDL_AudioSpec audio_desired, audio_obtained;

struct audio_chunk {
	struct audio_chunk *next;
	unsigned char *data;
	int size;
};

struct audio_chunk *cur_chunk, **cur_chunk_tailp = &cur_chunk;
int cur_offset;

void
audio_callback (void *userdata, Uint8 *stream, int len)
{
	int togo, thistime;

	togo = len;

	while (togo && cur_chunk) {
		thistime = cur_chunk->size - cur_offset;
		if (thistime > togo)
			thistime = togo;
		memcpy (stream, cur_chunk->data + cur_offset, thistime);

		stream += thistime;
		togo -= thistime;
		cur_offset += thistime;

		if (cur_offset >= cur_chunk->size) {
			if ((cur_chunk = cur_chunk->next) == NULL)
				cur_chunk_tailp = &cur_chunk;
			cur_offset = 0;
		}
	}

	memset (stream, audio_obtained.silence, togo);
}

void
play (struct audio_chunk *new_chunk)
{
	SDL_LockAudio ();
	*cur_chunk_tailp = new_chunk;
	SDL_UnlockAudio ();
}

void
test_audio (void)
{
	struct audio_chunk *cp;
	FILE *f;

	cp = calloc (1, sizeof *cp);
	cp->data = malloc (1000 * 1000);

	if ((f = fopen ("/l/baris/gamedat/unews.cdr", "r")) == NULL) {
		fprintf (stderr, "can't open unews.cdr\n");
		exit (1);
	}

	cp->size = fread (cp->data, 1, 1000 * 1000, f);

	fclose (f);

	play (cp);
}


int
main ()
{
	SDL_Rect r;
	SDL_Event ev;

	if (SDL_Init (SDL_INIT_VIDEO) < 0) {
		fprintf (stderr, "SDL_Init error\n");
		exit (1);
	}

	signal (SIGINT, intr);

	if (SDL_InitSubSystem (SDL_INIT_AUDIO < 0)) {
		printf ("no audio\n");
	} else {
		printf ("audio initialized\n");
		have_audio = 1;
	}

	if ((sur = SDL_SetVideoMode (320, 200, 24, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
		fprintf (stderr, "error in SDL_SetVideoMode\n");
		exit (1);
	}

	SDL_EnableUNICODE (1);

	audio_desired.freq = 11025;
	audio_desired.format = AUDIO_U8;
	audio_desired.channels = 1;
	audio_desired.samples = 8192;
	audio_desired.callback = audio_callback;

	if (SDL_OpenAudio (&audio_desired, &audio_obtained) < 0) {
		fprintf (stderr, "error in SDL_OpenAudio\n");
		exit (1);
	}

	test_audio ();
	SDL_PauseAudio (0);

	while (1) {
		while (SDL_PollEvent (&ev)) {
			switch (ev.type) {
			case SDL_KEYDOWN:
				printf ("got key %d\n", ev.key.keysym.unicode);
				break;
			case SDL_KEYUP:
				break;
			default:
				printf ("got uknown event %d\n", ev.type);
				break;
			}
		}

		r.x = 0; r.y = 0; r.w = 320; r.h = 200;
		SDL_FillRect (sur, &r, 0);

		r.x = 160 + 120 * cos (get_time () * .5 * M_PI);
		r.y = 100;
		r.w = 20;
		r.h = 20;
		SDL_FillRect (sur, &r, 0xff0000);
	
		SDL_Flip (sur);

		usleep (33 * 1000);

	}

	return (0);
}

