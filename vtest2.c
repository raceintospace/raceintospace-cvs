#include "race.h"
#include "mmfile.h"
#include "int_types.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <memory.h>
#include <SDL/SDL.h>

#define xrealloc realloc
#define xmalloc  malloc
#ifndef min
#   define min(a, b) ((a) <= (b) ? (a) : (b))
#endif

/* taken from ffmpeg project */
#ifdef CONFIG_WIN32
#   include <sys/types.h>
#   include <sys/timeb.h>
#elif defined(CONFIG_OS2)
#   include <string.h>
#   include <sys/time.h>
#else
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/time.h>
#endif

/* TODO: make sure it works and move where it belongs (portability?) */
int64_t
xgettime(void)
{
#ifdef CONFIG_WIN32
	struct timeb tb;

	_ftime(&tb);
	return ((int64_t) tb.time * (int64_t) (1000)
		+ (int64_t) tb.millitm) * (int64_t) (1000);
#else
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}

void
eperror(const char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void
eprintf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
usage(const char *progname)
{
	if (strrchr(progname, '/') != NULL)
		progname = strrchr(progname, '/') + 1;
	fprintf(stderr, "usage: %s file.ogg\n", progname);
	exit(EXIT_SUCCESS);
}

struct audiobuf
{
	char *buf;
	int off, bytes, size;
};

static void
audio_cb(void *userdata, Uint8 * stream, int len)
{
	struct audiobuf *abuf = userdata;
	int to_copy = min(len, abuf->bytes);

	memcpy(stream, abuf->buf + abuf->off, to_copy);
	abuf->bytes -= to_copy;
	abuf->off += to_copy;
	memset(stream + to_copy, 0, len - to_copy);
}

int
main(int argc, char **argv)
{
	char *file = NULL;
	int have_video = 0, have_audio = 0;
	int h = 0, w = 0;
	float fps = 0.0;
	int ch = 0, hz = 0;
	mm_file media;

	if (argc > 1)
		file = argv[1];
	else
		usage(argv[0]);

	if (mm_open(&media, file) <= 0)
		printf("No audio or video in `%s'\n", file);

	if (mm_video_info(&media, &w, &h, &fps) >= 0)
	{
		printf("Video data: %dx%d, %gfps\n", w, h, fps);
		have_video = 1;
	}

	if (mm_audio_info(&media, &ch, &hz) >= 0)
	{
		printf("Audio data: %s, %dHz\n", (ch == 1) ? "mono" : "stereo", hz);
		have_audio = 1;
	}

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
		eprintf("Sdl init failed: %s\n", SDL_GetError());

	if (have_video)
	{
		SDL_Surface *display = NULL;
		SDL_Overlay *ovl = NULL;
		int64_t oldt, newt;

		if ((display = SDL_SetVideoMode(w, h, 24,
					SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
			eprintf("SDL_SetVideoMode: %s\n", SDL_GetError());

		if ((ovl = SDL_CreateYUVOverlay(w, h,
					SDL_YV12_OVERLAY, display)) == NULL)
			eprintf("SDL_CreateYUVOverlay: %s\n", SDL_GetError());

		/* XXX: Need a better way of timing sync */
		oldt = xgettime();
		while (mm_decode_video(&media, ovl) > 0)
		{
			SDL_Rect r = { 0, 0, w, h };
			newt = 1e6 / fps + oldt - xgettime();
			if (newt > 0)
				usleep(newt);
			SDL_DisplayYUVOverlay(ovl, &r);
			oldt = xgettime();
		}
		SDL_FreeYUVOverlay(ovl);
	}

	if (have_audio)
	{
		struct audiobuf abuf;
		int64_t tdiff;
		int bytes;
		SDL_AudioSpec desired;

		desired.channels = ch;
		desired.freq = hz;
		desired.format = AUDIO_U8;
		desired.samples = 4096;
		desired.callback = audio_cb;
		desired.userdata = &abuf;
		if (SDL_OpenAudio(&desired, NULL) < 0)
			eprintf("SDL_OpenAudio: %s\n", SDL_GetError());

		abuf.size = 4096;
		abuf.off = 0;
		abuf.bytes = 0;
		abuf.buf = xmalloc(abuf.size);

		tdiff = xgettime();
		while ((bytes = mm_decode_audio(&media,
					abuf.buf + abuf.bytes, abuf.size - abuf.bytes)) > 0)
		{
			abuf.bytes += bytes;
			if (abuf.size - abuf.bytes <= 4096)
				abuf.buf = xrealloc(abuf.buf, abuf.size *= 2);
		};
		if (bytes < 0)
			eperror("decode_audio");

		printf("Decoding: %.3fs\n", (xgettime() - tdiff) / 1e6);
		printf("Audio: %d samples, %.2fs\n", abuf.bytes / ch,
			(double) (abuf.bytes) / ch / hz);

		SDL_PauseAudio(0);

		while (abuf.bytes > 0)
			sleep(1);

		SDL_PauseAudio(1);

		free(abuf.buf);
	}

	SDL_Quit();
	mm_close(&media);

	return EXIT_SUCCESS;
}
