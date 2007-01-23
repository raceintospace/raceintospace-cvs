#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <memory.h>
#include <SDL.h>
#include <avcodec.h>
#include <avformat.h>

struct MediaFile {
    AVFormatContext *file;
    AVStream        *v_stream;
    AVStream        *a_stream;
};

void eperror(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void eprintf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void usage(const char *progname)
{
    if (strrchr(progname, '/') != NULL)
        progname = strrchr(progname, '/')+1;
    fprintf(stderr, "usage: %s video_file\n", progname);
    exit(EXIT_SUCCESS);
}

void
close_media_file(struct MediaFile *mf)
{
    int i = 0;
    AVCodecContext *cc = NULL;

    if (!mf)
        return;

    if (mf->file)
    {
        for (i = 0; i < mf->file->nb_streams; ++i)
        {
            cc = mf->file->streams[i]->codec;
            if (cc && cc->codec)
                avcodec_close(cc);
        }

        av_close_input_file(mf->file);
    }

    free(mf);
}

struct MediaFile *
open_media_file(const char *filename, int open_codec_type)
{
    char ident_string[100];
    AVCodecContext *cc = NULL;
    AVCodec *codec = NULL;
    struct MediaFile *mf = NULL;
    int i = 0, err = 0;
    
    mf = malloc(sizeof (struct MediaFile));
    memset(mf, 0, sizeof (struct MediaFile));

    err = av_open_input_file(&mf->file, filename, NULL, 0, NULL);
    if (err < 0)
        goto end;

    err = av_find_stream_info(mf->file);
    if (err < 0)
        goto end;

    if (open_codec_type == CODEC_TYPE_AUDIO
            || open_codec_type == CODEC_TYPE_VIDEO)
    {
        for (i = 0; i < mf->file->nb_streams; ++i)
        {
            cc = mf->file->streams[i]->codec;
            if (cc->codec_type == open_codec_type)
            {
                codec = avcodec_find_decoder(cc->codec_id);
                if (!codec || avcodec_open(cc, codec) < 0)
                    goto end;

                avcodec_string(ident_string, sizeof(ident_string), cc, 0);
                fprintf(stderr, "CODEC FOUND %s\n", ident_string);

                if (open_codec_type == CODEC_TYPE_AUDIO)
                    mf->a_stream = mf->file->streams[i];
                else
                    mf->v_stream = mf->file->streams[i];
            }
        }
    }

    return mf;
end:
    if (mf) 
        close_media_file(mf);
    return NULL;
}

int
display_video_frame(struct MediaFile *mf, SDL_Overlay *ovl)
{
    AVCodecContext *cc = NULL;
    AVStream *st = NULL;
    AVPacket pkt;
    AVFrame frm;
    AVPicture spict;
    AVPicture dpict;
    int len = 0, got_picture = 0;

    avcodec_get_frame_defaults(&frm);
    for (;;)
    {
        /* misleading, reads AVPacket not AVFrame */
        if (av_read_frame(mf->file, &pkt) < 0)
            return -1;
        st = mf->file->streams[pkt.stream_index];
        if (st->codec->codec_type == CODEC_TYPE_VIDEO)
            break;
        av_free_packet(&pkt);
    }

    cc = st->codec;

    len = avcodec_decode_video(cc, &frm, &got_picture,
            pkt.data, pkt.size);

    if (got_picture)
    {
        SDL_Rect r = {0, 0, 320, 200};

        spict.data[0] = frm.data[0];
        spict.data[1] = frm.data[1];
        spict.data[2] = frm.data[2];
        spict.linesize[0] = frm.linesize[0];
        spict.linesize[1] = frm.linesize[1];
        spict.linesize[2] = frm.linesize[2];

        SDL_LockYUVOverlay(ovl);

        dpict.data[0] = ovl->pixels[0];
        dpict.data[1] = ovl->pixels[2];
        dpict.data[2] = ovl->pixels[1];
        dpict.linesize[0] = ovl->pitches[0];
        dpict.linesize[1] = ovl->pitches[2];
        dpict.linesize[2] = ovl->pitches[1];

        img_convert(&dpict, PIX_FMT_YUV420P, &spict,
                cc->pix_fmt, ovl->w, ovl->h);

        SDL_UnlockYUVOverlay(ovl);

        SDL_DisplayYUVOverlay(ovl, &r);
    }

    /* XXX: fix to include decoding delays */
    SDL_Delay((int)(0.5+av_q2d(cc->time_base)*1000));

    av_free_packet(&pkt);

    return 0;
}

int
main (int argc, char ** argv)
{
    SDL_Surface *display;
    SDL_Overlay *ovl;
	SDL_Event ev;
    struct MediaFile *stream = NULL;
    int res = 0;
    char * file = NULL;

    if (argc > 1)
        file = argv[1];
    else
        usage(argv[0]);

    av_register_all();

    stream = open_media_file(file, CODEC_TYPE_VIDEO);
    if (!stream)
        eprintf("error opening video file");

    if (!stream->v_stream)
        eprintf("no video streams found in file");

	if ((display = SDL_SetVideoMode (320, 200, 24, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
		eprintf ("SDL_SetVideoMode: %s\n", SDL_GetError());

    if ((ovl = SDL_CreateYUVOverlay(160, 100, SDL_YV12_OVERLAY, display)) == NULL)
		eprintf ("SDL_CreateYUVOverlay: %s\n", SDL_GetError());

	while (1) {
		while (SDL_PollEvent (&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				exit (0);
				break;

				/* ignore these events */
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_ACTIVEEVENT:
			case SDL_MOUSEMOTION:
				break;
			default:
				fprintf (stderr, "got uknown event %d\n", ev.type);
				break;
			}
		}

        res = display_video_frame(stream, ovl); 
        if (res < 0)
            break;
	}

    close_media_file(stream);

	return (0);
}

