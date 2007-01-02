void av_step (void);
void av_setup (int *argcp, char ***argvp);
void av_silence (void);
void av_sync (void);

unsigned char far *screen;
int screen_dirty;
char pal[768];

struct audio_chunk {
	struct audio_chunk *next;
	void *data;
	int size;
	int loop;
};
void play (struct audio_chunk *cp);

int av_dirty;

char AnimSoundCheck(void);
void UpdateAudio (void);


double get_time (void);
int bioskey (int peek);

int av_mouse_cur_x, av_mouse_cur_y;
int av_mouse_pressed_x, av_mouse_pressed_y;
int av_mouse_pressed_cur;
int av_mouse_pressed_latched;
