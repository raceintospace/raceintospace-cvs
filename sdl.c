#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <memory.h>
#include <SDL/SDL.h>
#include "Buzz_inc.h"

#include "av.h"
#define MAX_X	320
#define MAX_Y	200

void dbg(char const *fmt, ...) __attribute__ ((format(printf, 1, 2)));

int av_mouse_cur_x, av_mouse_cur_y;
int av_mouse_pressed_x, av_mouse_pressed_y;
int av_mouse_pressed_cur;
int av_mouse_pressed_latched;

SDL_Surface *display;
SDL_Overlay *video_overlay;
SDL_Rect video_rect;
static SDL_Surface *screen_surf;
static SDL_Surface *screen_surf2x;

static SDL_Color pal_colors[256];

static struct audio_channel Channels[AV_NUM_CHANNELS];

unsigned char *screen;

// unsigned char* screen_2;

char pal[3 * 256];

int screen_dirty;

void
intr(int sig)
{
	SDL_Quit();
	exit(0);
}

static int have_audio;

static SDL_AudioSpec audio_desired, audio_obtained;

static void
audio_callback(void *userdata, Uint8 * stream, int len)
{
	int togo, thistime, i, left;
	Uint8 silence = audio_obtained.silence;
	int num_active = AV_NUM_CHANNELS;
	unsigned sample, sum;
	struct audio_channel *chp;

	struct
	{
		Uint8 *data;
		unsigned coeff;
	} arr[AV_NUM_CHANNELS];

	togo = len;

	while (togo > 0 && num_active > 0)
	{
		thistime = togo;
		// sum = 0;
		sum = AV_NUM_CHANNELS * AV_MAX_VOLUME;
		num_active = AV_NUM_CHANNELS;

		/* init mixer data */
		for (i = 0; i < AV_NUM_CHANNELS; ++i)
		{
			arr[i].data = NULL;
			arr[i].coeff = 0;
			chp = &Channels[i];
			// sum += chp->volume;
			if (chp->chunk)
			{
				left = chp->chunk->size - chp->offset;
				if (left < 0)
				{
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
			}
			else
				num_active--;
		}

		if (!num_active)
			break;

		left = thistime;

		while (left--)
		{
			sample = 0;
			/* Make this outer loop? */
			for (i = 0; i < AV_NUM_CHANNELS; ++i)
				sample +=
					arr[i].coeff * (arr[i].data ? *(arr[i].data++) : silence)
					+ (AV_MAX_VOLUME - arr[i].coeff) * silence;
			sample /= sum;
			if (sample > 255)
				sample = 255;
			*(stream++) = (Uint8) sample;
		}

		togo -= thistime;
		assert(togo >= 0);

		for (i = 0; i < AV_NUM_CHANNELS; ++i)
		{
			chp = &Channels[i];
			if (chp->chunk)
			{
				chp->offset += thistime;
				if (chp->offset >= chp->chunk->size)
				{
					chp->offset = 0;
					if (!chp->chunk->loop)
					{
						chp->chunk = chp->chunk->next;
						if (chp->chunk == NULL)
							chp->chunk_tailp = &chp->chunk;
						/* why this lastp?? */
					}
				}
			}
		}
	}

	memset(stream, silence, togo);
}

/* 0 means busy playing audio; 1 means idle */
char
AnimSoundCheck(void)
{
	/* assume sound channel */
	av_step();
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
play(struct audio_chunk *new_chunk, int channel)
{
	struct audio_chunk *cp;
	struct audio_channel *chp;

	assert(channel >= 0 && channel < AV_NUM_CHANNELS);

	chp = &Channels[channel];

	if (have_audio == 0)
		return;

	SDL_LockAudio();
	for (cp = chp->chunk; cp; cp = cp->next)
	{
		if (cp == new_chunk)
		{
			printf("play: attempt to do duplicate chunk add\n");
			av_silence(channel);
			break;
		}
	}

	new_chunk->next = NULL;
	*chp->chunk_tailp = new_chunk;
	SDL_UnlockAudio();
}

void
av_silence(int channel)
{
	int i = channel;

	if (channel == AV_ALL_CHANNELS)
	{
		for (i = 0; i < AV_NUM_CHANNELS; ++i)
			av_silence(i);
	}
	else
	{
		assert(channel >= 0 && channel < AV_NUM_CHANNELS);
		if (Channels[channel].chunk)
		{
			SDL_LockAudio();
			Channels[channel].chunk = NULL;
			Channels[channel].chunk_tailp = &Channels[channel].chunk;
			Channels[channel].offset = 0;
			SDL_UnlockAudio();
		}
	}
}

Uint32
sdl_timer_callback(Uint32 interval, void *param)
{
	static SDL_Event tick;

	tick.type = SDL_USEREVENT;
	SDL_PushEvent(&tick);
	return (interval);
}

void
av_setup(int *argcp, char ***argvp)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "SDL_Init error\n");
		exit(1);
	}

	atexit(SDL_Quit);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO < 0))
	{
		printf("no audio\n");
	}
	else
	{
		printf("audio initialized\n");
		have_audio = 1;
	}

	if ((display = SDL_SetVideoMode(640, 400, 24,
				SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
	{
		/* ERROR */ fprintf(stderr, "error in SDL_SetVideoMode: %s\n",
                SDL_GetError());
		exit(1);
	}

	screen = xcalloc(MAX_X * MAX_Y, 1);
	screen_surf = SDL_CreateRGBSurfaceFrom(screen, 320, 200, 8,
		MAX_X, 0, 0, 0, 0);
    if (!screen_surf)
    {
        /* ERROR */ fprintf(stderr, "can't create screen surface: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
	screen_surf2x = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 400, 8,
		~0, ~0, ~0, 0);
    if (!screen_surf2x)
    {
        /* ERROR */ fprintf(stderr, "can't create screen_2x surface: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

#ifdef CONFIG_THEORA_VIDEO
    /* XXX: Hardcoded video width & height */
    video_overlay = SDL_CreateYUVOverlay(160, 100, SDL_YV12_OVERLAY, display);
    if (!video_overlay)
    {
        /* ERROR */ fprintf(stderr, "can't create video_overlay: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
#endif

	SDL_EnableUNICODE(1);
	SDL_WM_SetCaption("Race Into Space", NULL);

	if (have_audio)
	{
		int i = 0;

		audio_desired.freq = 11025;
		audio_desired.format = AUDIO_U8;
		audio_desired.channels = 1;
		audio_desired.samples = 8192;
		audio_desired.callback = audio_callback;

		/* initialize audio channels */
		for (i = 0; i < AV_NUM_CHANNELS; ++i)
		{
			Channels[i].volume = AV_MAX_VOLUME;
			Channels[i].mute = 0;
			Channels[i].chunk = NULL;
			Channels[i].chunk_tailp = &Channels[i].chunk;
			Channels[i].offset = 0;
		}

		if (SDL_OpenAudio(&audio_desired, &audio_obtained) < 0)
		{
			fprintf(stderr, "error in SDL_OpenAudio\n");
			exit(1);
		}

		SDL_PauseAudio(0);
	}

	SDL_AddTimer(30, sdl_timer_callback, NULL);
}

#define KEYBUF_SIZE 256
int keybuf[KEYBUF_SIZE];
int keybuf_in_idx, keybuf_out_idx;

void
av_process_event(SDL_Event * evp)
{
	int c;

	switch (evp->type)
	{
		case SDL_QUIT:
			exit(0);
			break;

		case SDL_USEREVENT:
			break;

		case SDL_KEYDOWN:
			switch (evp->key.keysym.sym)
			{
				case SDLK_UP:
					c = 0x4800;
					break;
				case SDLK_DOWN:
					c = 0x5000;
					break;
				case SDLK_RIGHT:
					c = 0x4D00;
					break;
				case SDLK_LEFT:
					c = 0x4B00;
					break;
				default:
					c = evp->key.keysym.unicode;
					break;
			}
			if (c)
			{
				keybuf[keybuf_in_idx] = c;
				keybuf_in_idx = (keybuf_in_idx + 1) % KEYBUF_SIZE;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			av_mouse_pressed_cur = 1;
			av_mouse_pressed_latched = 1;
			av_mouse_pressed_x = evp->button.x;
			av_mouse_pressed_y = evp->button.y;
			printf("mouseclick(%d,%d) b = %d\n",
				av_mouse_pressed_x, av_mouse_pressed_y, evp->button.button);
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
			printf("got uknown event %d\n", evp->type);
			break;
	}
}

/* non-blocking */
void
av_step(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
		av_process_event(&ev);
}

void
av_block(void)
{
	SDL_Event ev;

	/* 
	 * block until an event comes in
	 *
	 * we have a 30ms timer going, so that is the
	 * maximum wait time
	 */
	if (SDL_WaitEvent(&ev))
	{
		av_process_event(&ev);
		av_step();				   /* soak up any other currently available events */
	}
}

int
bioskey(int peek)
{
	int c;

	av_step();

	if (peek)
	{
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
//  av_step ();
}

void
NUpdateVoice(void)
{
	av_step();
}

static SDL_Surface *
SDL_Scale2x(SDL_Surface * src, SDL_Surface * dst)
{
	int x, y, bpp;
	uint8_t *from, *to;
	SDL_Rect clp;
	SDL_PixelFormat *pf;

	assert(src);
	assert(src != dst);

	pf = src->format;

	if (!dst)
		dst = SDL_CreateRGBSurface(SDL_SWSURFACE,
			2 * src->w, 2 * src->h,
			pf->BitsPerPixel, pf->Rmask, pf->Gmask, pf->Bmask, pf->Amask);

	if (!dst)
		return NULL;

	bpp = pf->BytesPerPixel;

	if (2 * src->h != dst->h
		|| 2 * src->w != dst->w || bpp != dst->format->BytesPerPixel)
	{
		SDL_SetError("dst surface size or bpp mismatch (%d vs %d)",
			bpp, dst->format->BytesPerPixel);
		return NULL;
	}

	if (bpp == 1)
		SDL_SetColors(dst, pf->palette->colors, 0, pf->palette->ncolors);

	if (SDL_MUSTLOCK(src))
		SDL_LockSurface(src);
	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);

	SDL_GetClipRect(dst, &clp);

	for (y = clp.y / 2; y < clp.y / 2 + clp.h / 2; ++y)
	{
		for (x = clp.x / 2; x < clp.x / 2 + clp.w / 2; ++x)
		{
			from = ((uint8_t *) src->pixels) + y * src->pitch + x * bpp;
			to = ((uint8_t *) dst->pixels) + 2 * y * dst->pitch +
				2 * x * bpp;
			switch (bpp)
			{
#define ASSIGN do { \
					*(TYPE (to)) = *(TYPE from); \
					*(TYPE (to+bpp)) = *(TYPE from); \
					*(TYPE (to+dst->pitch)) = *(TYPE from); \
					*(TYPE (to+dst->pitch+bpp)) = *(TYPE from); \
				} while (0)

				case 1:
#define TYPE (uint8_t *)
					ASSIGN;
					break;
#undef TYPE
				case 2:
#define TYPE (uint16_t *)
					ASSIGN;
					break;
#undef TYPE
				case 3:
#define TYPE (uint8_t *)
					ASSIGN;
					to++;
					from++;
					ASSIGN;
					to++;
					from++;
					ASSIGN;
					to++;
					from++;
					break;
#undef TYPE
				case 4:
#define TYPE (uint32_t *)
					ASSIGN;
					break;
#undef TYPE
#undef ASSIGN
			}
		}
	}

	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
	if (SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);

	return dst;
}

void
av_sync(void)
{
	int i = 0;
    SDL_Rect r;

	/*
	 * int row, col;
	 * 
	 * int src_row, src_col, src_idx;
	 * int dest_row, dest_col, dest_idx;
	 * unsigned char pixel;
	 * 
	 * int min_x = MAX_X, min_y = MAX_Y;
	 * int max_x = -1, max_y = -1;
	 * char *p;
	 * char *outp;
	 */

#ifdef PROFILE_GRAPHICS
	Uint32 ticks = SDL_GetTicks();
#endif

	/* copy palette */
	for (i = 0; i < 256; ++i)
	{
		pal_colors[i].r = pal[3 * i] * 4;
		pal_colors[i].g = pal[3 * i + 1] * 4;
		pal_colors[i].b = pal[3 * i + 2] * 4;
	}
	SDL_Scale2x(screen_surf, screen_surf2x);
	SDL_SetColors(screen_surf2x, pal_colors, 0, 256);
	SDL_BlitSurface(screen_surf2x, NULL, display, NULL);
    if (video_rect.h && video_rect.w)
    {
        r.h = 2 * video_rect.h;
        r.w = 2 * video_rect.w;
        r.x = 2 * video_rect.x;
        r.y = 2 * video_rect.y;
        SDL_DisplayYUVOverlay(video_overlay, &r);
    }
	SDL_Flip(display);
	screen_dirty = 0;
	return;

#if 0
	if (SDL_MUSTLOCK(display))
		SDL_LockSurface(display);

	outp = display->pixels;

#   if 1
	for (row = 0; row < MAX_Y; ++row)
	{
		for (col = 0; col < MAX_X; ++col)
		{
			i = row * MAX_X + col;
			p = &pal[screen[i] * 3];
			if (screen_2[i] != screen[i])
			{
				screen_2[i] = screen[i];
				min_x = minn(min_x, col);
				max_x = maxx(max_x, col);
				min_y = minn(min_y, row);
				max_y = maxx(max_y, row);
			}
#      ifdef __BIG_ENDIAN__
#         define PAL_R	p[0]
#         define PAL_G	p[1]
#         define PAL_B	p[2]
#      else
#         define PAL_R	p[2]
#         define PAL_G	p[1]
#         define PAL_B	p[0]
#      endif

			i = 4 * row * MAX_X + 2 * col;
			outp[3 * i] = PAL_R * 4;
			outp[3 * i + 1] = PAL_G * 4;
			outp[3 * i + 2] = PAL_B * 4;

			i = 4 * row * MAX_X + 2 * col + 1;
			outp[3 * i] = PAL_R * 4;
			outp[3 * i + 1] = PAL_G * 4;
			outp[3 * i + 2] = PAL_B * 4;

			i = 4 * row * MAX_X + 2 * col + 2 * MAX_X;
			outp[3 * i] = PAL_R * 4;
			outp[3 * i + 1] = PAL_G * 4;
			outp[3 * i + 2] = PAL_B * 4;

			i = 4 * row * MAX_X + 2 * col + 2 * MAX_X + 1;
			outp[3 * i] = PAL_R * 4;
			outp[3 * i + 1] = PAL_G * 4;
			outp[3 * i + 2] = PAL_B * 4;
		}
	}
#   endif

#   if 0
	for (dest_row = 0; dest_row < 2 * MAX_Y; dest_row++)
	{
		src_row = dest_row / 2;
		for (dest_col = 0; dest_col < 2 * MAX_X; dest_col++)
		{
			src_col = dest_col / 2;

			src_idx = src_row * MAX_X + src_col;
			dest_idx = (dest_row * 2 * MAX_X + dest_col) * 3;

			if (screen_2[src_idx] != screen[src_idx])
			{
				screen_2[src_idx] = screen[src_idx];
				min_x = minn(min_x, src_col);
				max_x = maxx(max_x, src_col);
				min_y = minn(min_y, src_row);
				max_y = maxx(max_y, src_row);
			}

			pixel = screen[src_idx] & 0xff;
			p = pal + pixel * 3;

			outp = display->pixels + dest_idx;

			outp[0] = PAL_R * 4;
			outp[1] = PAL_G * 4;
			outp[2] = PRL_B * 4;
		}
	}
#   endif

	if (SDL_MUSTLOCK(display))
		SDL_UnlockSurface(display);

	if (max_x >= 0 && max_y > 0)
	{
		SDL_UpdateRect(display, min_x * 2, min_y * 2,
			2 * (max_x - min_x + 1), 2 * (max_y - min_y + 1));

#   ifdef PROFILE_GRAPHICS
		fprintf(stderr, "av_sync: ~%3d%% updated in ~%3ums\n",
			/* min_x, min_y, max_x, max_y, */
			100 * (max_x - min_x + 1) * (max_y - min_y +
				1) / (MAX_X * MAX_Y), SDL_GetTicks() - ticks);
#   endif
	}
	else
	{
#   ifdef PROFILE_GRAPHICS
		fprintf(stderr, "av_sync:	 no update	in ~%3ums\n",
			SDL_GetTicks() - ticks);
#   endif
	}

	screen_dirty = 0;
#endif
}

void
MuteChannel(int channel, int mute)
{
	int i;

	if (channel == AV_ALL_CHANNELS)
	{
		for (i = 0; i < AV_NUM_CHANNELS; ++i)
			MuteChannel(i, mute);
	}
	else
	{
		assert(channel >= 0 && channel < AV_NUM_CHANNELS);
		Channels[channel].mute = mute;
	}
}
