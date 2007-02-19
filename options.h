#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

typedef struct {
    char *dir_savegame;
    char *dir_gamedata;
    int want_audio;
    int want_intro;
    int want_fade;
    int want_cheats;
} game_options;

extern game_options options;
extern int setup_options(int argc, char ** argv);

#endif /* _ENVIRONMENT_H */
