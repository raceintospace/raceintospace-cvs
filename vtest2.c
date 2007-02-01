#include <int_types.h>
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
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <theora/theora.h>
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

enum stream_type
{
	STREAM_AUDIO,
	STREAM_VIDEO
};

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

typedef struct
{
	FILE *file;
	ogg_sync_state sync;
	ogg_stream_state *audio;
	vorbis_info *audio_info;
	vorbis_dsp_state *audio_ctx;
	vorbis_block *audio_blk;
	ogg_stream_state *video;
	theora_info *video_info;
	theora_state *video_ctx;
} media_file;

/* return -1 on error, 0 on end of file, 1 on successful page read */
static int
get_page(media_file * mf, ogg_page * pg)
{
	const int bufsize = 8192;
	char *p = NULL;
	int n = 0;
	int res = 0;

	assert(mf);

	while (0 == (res = ogg_sync_pageout(&mf->sync, pg)))
	{
		p = ogg_sync_buffer(&mf->sync, bufsize);
		if (!p)
			eperror("ogg_sync_buffer");

		if (0 == (n = fread(p, 1, bufsize, mf->file)))
			return (feof(mf->file)) ? 0 : -1;

		if (ogg_sync_wrote(&mf->sync, n))
			eprintf("ogg_sync_wrote, buffer overflow\n");
	}
	/* XXX: following may segfault if non-ogg file is read */
	if (res < 0 || ogg_page_version(pg) != 0)
		return -1;
	return 1;
}

int
media_close(media_file * mf)
{
	assert(mf);
	if (mf->file)
	{
		fclose(mf->file);
		mf->file = NULL;
	}
	if (mf->audio)
	{
		ogg_stream_destroy(mf->audio);
		mf->audio = NULL;
	}
	if (mf->video)
	{
		ogg_stream_destroy(mf->video);
		mf->video = NULL;
	}
	if (mf->video_ctx)
	{
		theora_clear(mf->video_ctx);
		free(mf->video_ctx);
		mf->video_ctx = NULL;
	}
	if (mf->video_info)
	{
		theora_info_clear(mf->video_info);
		free(mf->video_info);
		mf->video_info = NULL;
	}
	if (mf->audio_blk)
	{
		vorbis_block_clear(mf->audio_blk);
		free(mf->audio_blk);
		mf->audio_ctx = NULL;
	}
	if (mf->audio_ctx)
	{
		vorbis_dsp_clear(mf->audio_ctx);
		free(mf->audio_ctx);
		mf->audio_ctx = NULL;
	}
	if (mf->audio_info)
	{
		vorbis_info_clear(mf->audio_info);
		free(mf->audio_info);
		mf->audio_info = NULL;
	}
	ogg_sync_clear(&mf->sync);
	return 0;
}

static int
get_packet(media_file * mf, ogg_packet * pkt, enum stream_type type)
{
	ogg_stream_state *stream, *other;
	ogg_page pg;
	int rv = 0;

	assert(mf);
	assert(pkt);
	switch (type)
	{
		case STREAM_VIDEO:
			assert(mf->video);
			stream = mf->video;
			other = mf->audio;
			break;
		case STREAM_AUDIO:
			assert(mf->audio);
			stream = mf->audio;
			other = mf->video;
			break;
		default:
			eprintf("bad stream type\n");
			return -1;			   /* not reached */
	}
	while (0 == ogg_stream_packetout(stream, pkt))
	{
		rv = get_page(mf, &pg);
		if (rv < 0)
			return -1;
		if (rv == 0)
			return 0;
		if (ogg_stream_pagein(stream, &pg) < 0
			&& !(other && ogg_stream_pagein(other, &pg) == 0))
		{
			/* DEBUG */ printf("got page not associated with any stream, "
				"serial %x\n", ogg_page_serialno(&pg));
			/* drop page */
		}
	}
	return 1;
}

static int
yuv_to_overlay(const media_file * mf, const yuv_buffer * yuv,
	SDL_Overlay * ovl)
{
	unsigned i, h, w, xoff, yoff;
	uint8_t *yp, *up, *vp;

	assert(mf);
	assert(yuv);
	assert(ovl);

	h = min(mf->video_info->frame_height, (unsigned) ovl->h);
	w = min(mf->video_info->frame_width, (unsigned) ovl->w);
	xoff = mf->video_info->offset_x;
	yoff = mf->video_info->offset_y;

	switch (ovl->format)
	{
		case SDL_IYUV_OVERLAY:
			up = yuv->u;
			vp = yuv->v;
			break;
		case SDL_YV12_OVERLAY:
			up = yuv->v;
			vp = yuv->u;
			break;
		default:
			/* DEBUG */ printf("only IYUV and YV12 SDL overlay"
				" formats supported\n");
			return -1;
	}
	yp = yuv->y;

	switch (mf->video_info->pixelformat)
	{
		case OC_PF_420:
			break;
		case OC_PF_422:
		case OC_PF_444:
		default:
			/* DEBUG */ printf("unknown/unsupported theora pixelformat!\n");
			return -1;
	}

	if (SDL_LockYUVOverlay(ovl) < 0)
	{
		/* DEBUG */ printf("unable to lock overlay!\n");
		return -1;
	}
	/* luna goes first */
	for (i = 0; i < h; ++i)
	{
		memcpy(ovl->pixels[0] + i * ovl->pitches[0],
			yp + (i + yoff) * yuv->y_stride + xoff, w);
	}
	xoff /= 2;
	yoff /= 2;
	/* round up */
	w = w / 2 + w % 2;
	h = h / 2 + h % 2;
	/* handle 2x2 subsampled u and v planes */
	for (i = 0; i < h; ++i)
	{
		memcpy(ovl->pixels[1] + i * ovl->pitches[1],
			up + (i + yoff) * yuv->uv_stride + xoff, w);
		memcpy(ovl->pixels[2] + i * ovl->pitches[2],
			vp + (i + yoff) * yuv->uv_stride + xoff, w);
	}
	SDL_UnlockYUVOverlay(ovl);
	return 0;
}

int
media_decode_video(media_file * mf, SDL_Overlay * ovl)
{
	int rv = 0;
	ogg_packet pkt;
	yuv_buffer yuv;

	assert(mf);
	if (!mf->video)
		return -1;
	for (;;)
	{
		rv = get_packet(mf, &pkt, STREAM_VIDEO);
		if (rv <= 0)
			return rv;
		/* we got packet, decode */
		if (theora_decode_packetin(mf->video_ctx, &pkt) == 0)
			break;
		else
		{
			/* DEBUG */ printf("packet does not contain theora frame!\n");
			/* get next packet */
		}
	}
	theora_decode_YUVout(mf->video_ctx, &yuv);
	if (yuv_to_overlay(mf, &yuv, ovl) < 0)
		return -1;
	return 1;
}

/* for now just 8bit unsigned values, mono channels FIXME
 * maybe use SDL_AudioConvert() for this */
int
media_decode_audio(media_file * mf, void *buf, int buflen)
{
	int rv = 0, samples = 0, left = buflen, total = 0;
	ogg_packet pkt;
	float **pcm;

	assert(mf);
	if (!mf->audio)
		return -1;
	for (;;)
	{
		/* output any samples left from last conversion */
		while ((samples = vorbis_synthesis_pcmout(mf->audio_ctx, &pcm)) > 0)
		{
			int i = 0;
			float *mono = pcm[0];	/* just left channel, or mono */

			if (left <= 0)
				return total;

			samples = min(samples, left);

			/* conv floats to uint8_t */
			for (i = 0; i < samples; ++i)
			{
				int val = ((mono[i] + 1) / 2) * UCHAR_MAX;

				if (val > UCHAR_MAX)
					val = UCHAR_MAX;
				if (val < 0)
					val = 0;
				*((uint8_t *) buf + total + i) = val;
			}

			total += samples;
			left -= samples;
			vorbis_synthesis_read(mf->audio_ctx, samples);
		}
		/* grab new packets if we need more */
		for (;;)
		{
			rv = get_packet(mf, &pkt, STREAM_AUDIO);
			if (rv < 0)
				return rv;
			else if (rv == 0)
				return total;

			/* have packet, synthesize */
			if (vorbis_synthesis(mf->audio_blk, &pkt) == 0)
			{
				vorbis_synthesis_blockin(mf->audio_ctx, mf->audio_blk);
				break;
			}
			else
			{
				/* DEBUG */
					printf("packet does not a valid vorbis frame!\n");
				/* get next packet */
			}
		}
	}
	/* NOT REACHED */
	return 0;
}

static int
media_init_theora(media_file * mf, ogg_page * pg)
{
	int pkts = 0;
	int res = 0;
	int rval = 0;
	theora_info *th_info = NULL;
	theora_comment th_comm;
	ogg_packet pkt;
	ogg_stream_state stream;

	assert(mf);
	th_info = xmalloc(sizeof(*mf->video_info));
	theora_info_init(th_info);
	theora_comment_init(&th_comm);
	ogg_stream_init(&stream, ogg_page_serialno(pg));

	if (ogg_page_packets(pg) != 1 || ogg_page_granulepos(pg) != 0)
		goto end;

	if (ogg_stream_pagein(&stream, pg))
		/* should not happen */
		goto end;

	/* Three first packets must go successfully through the loop. */
	for (pkts = 0; pkts < 3; ++pkts)
	{
		while ((res = ogg_stream_packetpeek(&stream, &pkt)) != 1)
		{
			if (res < 0
				|| get_page(mf, pg) <= 0
				|| ogg_stream_pagein(&stream, pg) < 0)
			{
				rval = -1;
				goto end;
			}
		}
		switch (theora_decode_header(th_info, &th_comm, &pkt))
		{
			case 0:
				break;
			case OC_VERSION:
			case OC_NEWPACKET:
				/* DEBUG */ fprintf(stderr, "incompatible theora file\n");
				/* fall through */
			case OC_BADHEADER:
			default:
				goto end;
		}

		/* decode successful so grab packet */
		ogg_stream_packetout(&stream, &pkt);
	}

	mf->video_ctx = xmalloc(sizeof(*mf->video_ctx));
	mf->video = xmalloc(sizeof(*mf->video));
	memcpy(mf->video, &stream, sizeof(stream));
	theora_decode_init(mf->video_ctx, th_info);
	mf->video_info = th_info;
	rval = 1;
  end:
	theora_comment_clear(&th_comm);
	if (rval <= 0)
	{
		ogg_stream_clear(&stream);
		theora_info_clear(th_info);
		mf->video_info = NULL;
	}
	return rval;
}

/* rval < 0: no video in file */
int
media_video_info(const media_file * mf, int *width, int *height, float *fps)
{
	assert(mf);
	if (!mf->video)
		return -1;
	if (width)
		*width = mf->video_info->frame_width;
	if (height)
		*height = mf->video_info->frame_height;
	if (fps)
		*fps = mf->video_info->fps_numerator
			/ mf->video_info->fps_denominator;
	return 0;
}

static int
media_init_vorbis(media_file * mf, ogg_page * pg)
{
	int pkts = 0;
	int res = 0;
	int rval = 0;
	vorbis_block *vo_blk = NULL;
	vorbis_info *vo_info = NULL;
	vorbis_comment vo_comm;
	ogg_packet pkt;
	ogg_stream_state stream;

	assert(mf);
	vo_info = xmalloc(sizeof(*vo_info));
	vorbis_info_init(vo_info);
	vorbis_comment_init(&vo_comm);
	ogg_stream_init(&stream, ogg_page_serialno(pg));

	if (ogg_page_packets(pg) != 1 || ogg_page_granulepos(pg) != 0)
		goto end;

	if (ogg_stream_pagein(&stream, pg) < 0)
		/* should not happen */
		goto end;

	/* 
	 * Three first packets must go successfully through the loop.
	 */
	for (pkts = 0; pkts < 3; ++pkts)
	{
		while ((res = ogg_stream_packetpeek(&stream, &pkt)) != 1)
		{
			if (res < 0
				|| get_page(mf, pg) <= 0
				|| ogg_stream_pagein(&stream, pg) < 0)
			{
				rval = -1;
				goto end;
			}
		}

		switch (vorbis_synthesis_headerin(vo_info, &vo_comm, &pkt))
		{
			case 0:
				break;
			case OV_EBADHEADER:
			case OV_ENOTVORBIS:
			default:
				goto end;
		}

		/* decode successful so grab packet */
		ogg_stream_packetout(&stream, &pkt);
	}
	/* maybe print something about comment or etc? */

	mf->audio_ctx = xmalloc(sizeof(*mf->audio_ctx));
	mf->audio_blk = xmalloc(sizeof(*mf->audio_blk));
	mf->audio = xmalloc(sizeof(*mf->audio));
	vo_blk = xmalloc(sizeof(*vo_blk));
	memcpy(mf->audio, &stream, sizeof(stream));
	vorbis_synthesis_init(mf->audio_ctx, vo_info);
	vorbis_block_init(mf->audio_ctx, vo_blk);
	mf->audio_info = vo_info;
	mf->audio_blk = vo_blk;
	rval = 1;
  end:
	vorbis_comment_clear(&vo_comm);
	if (rval <= 0)
	{
		ogg_stream_clear(&stream);
		vorbis_info_clear(vo_info);
		free(vo_info);
	}
	return rval;
}

/* rval < 0: no audio in file */
int
media_audio_info(const media_file * mf, int *channels, int *rate)
{
	assert(mf);
	if (!mf->audio)
		return -1;
	if (channels)
		*channels = mf->audio_info->channels;
	if (rate)
		*rate = mf->audio_info->rate;
	return 0;
}

/* rval < 0: error, > 0: have audio or video */
int
media_open(media_file * mf, const char *fname)
{
	int retval = -1;
	int res = 0;
	int have_vorbis = 0;
	int have_theora = 0;
	ogg_page pg;

	assert(mf);
	assert(fname);
	memset(mf, 0, sizeof(*mf));

	mf->file = fopen(fname, "rb");
	if (!mf->file)
		return retval;
	ogg_sync_init(&mf->sync);

	/* get first page to start things up */
	if (get_page(mf, &pg) <= 0)
		goto err;

	res = media_init_theora(mf, &pg);
	if (res < 0)
		goto err;
	else
		have_theora = res;

	res = media_init_vorbis(mf, &pg);
	if (res < 0)
		goto err;
	else
		have_vorbis = res;

	return have_vorbis || have_theora;
  err:
	media_close(mf);
	return retval;
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
	media_file media;

	if (argc > 1)
		file = argv[1];
	else
		usage(argv[0]);

	if (media_open(&media, file) <= 0)
		printf("No audio or video in `%s'\n", file);

	if (media_video_info(&media, &w, &h, &fps) >= 0)
	{
		printf("Video data: %dx%d, %gfps\n", w, h, fps);
		have_video = 1;
	}

	if (media_audio_info(&media, &ch, &hz) >= 0)
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
		while (media_decode_video(&media, ovl) > 0)
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
		while ((bytes = media_decode_audio(&media,
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
	media_close(&media);

	return EXIT_SUCCESS;
}
