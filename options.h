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
    unsigned feat_shorter_advanced_training;
    unsigned feat_random_nauts;  //Naut Randomize, Nikakd, 10/8/10
    unsigned feat_compat_nauts;  //Naut Compatibility, Nikakd, 10/8/10
    unsigned feat_no_cTraining;  //No Capsule Training, Nikakd, 10/8/10
    unsigned cheat_no_damage;    //Damaged Equipment Cheat, Nikakd, 10/8/10
} game_options;

extern game_options options;
extern int setup_options(int argc, char ** argv);

#endif /* _OPTIONS_H */
