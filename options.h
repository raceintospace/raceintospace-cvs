#ifndef _OPTIONS_H
#define _OPTIONS_H

typedef struct {
    char *dir_savegame;
    char *dir_gamedata;
    unsigned want_audio;
    unsigned want_fullscreen;
    unsigned want_intro;
    unsigned want_cheats;
    unsigned want_debug;
} game_options;

extern game_options options;
extern int setup_options(int argc, char ** argv);

#endif /* _OPTIONS_H */
