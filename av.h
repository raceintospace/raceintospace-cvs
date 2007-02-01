#ifndef _AV_H

#define _AV_H

#include <SDL/SDL.h>

#define AV_NUM_CHANNELS     2
#define AV_ALL_CHANNELS     -1
#define AV_SOUND_CHANNEL    0
#define AV_MUSIC_CHANNEL    1
#define AV_MIN_VOLUME       0
#define AV_MAX_VOLUME       127

void av_step (void);
void av_block (void);
void av_setup (int *argcp, char ***argvp);
void av_silence (int channel);
void av_sync (void);

struct audio_chunk {
	struct audio_chunk *next;
	void *data;
	int size;
	int loop;
};
void play (struct audio_chunk *cp, int channel);

struct audio_channel {
    unsigned                volume;
    unsigned                mute;
#if 0
    unsigned                fade;            /* actual fade value */
    int                     fade_inc;        /* fade inc or dec */
#endif
    struct audio_chunk*     chunk;           /* played chunk */
    struct audio_chunk**    chunk_tailp;     /* tail of chunk list? */
    unsigned                offset;          /* data offset in chunk */
};

char AnimSoundCheck(void);
void UpdateAudio (void);
void MuteChannel(int channel, int mute);

double get_time (void);
int bioskey (int peek);

int av_mouse_cur_x, av_mouse_cur_y;
int av_mouse_pressed_x, av_mouse_pressed_y;
int av_mouse_pressed_cur;
int av_mouse_pressed_latched;

#endif /* ifndef _AV_H */
