#ifndef _MM_FILE_H
#define _MM_FILE_H

#include <stdio.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <theora/theora.h>
#include <SDL/SDL.h>

enum stream_type
{
	MEDIA_AUDIO = 1,
	MEDIA_VIDEO = 2
};

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
} mm_file;

extern int mm_open(mm_file * mf, const char *fname);
extern int mm_close(mm_file * mf);
extern int mm_video_info(const mm_file * mf, int *width, int *height,
	float *fps);
extern int mm_audio_info(const mm_file * mf, int *channels, int *rate);
extern int mm_decode_video(mm_file * mf, SDL_Overlay * ovl);
extern int mm_decode_audio(mm_file * mf, void *buf, int buflen);

#endif /* _MM_FILE_H */
