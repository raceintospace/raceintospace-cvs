#include "race.h"
#include "options.h"
#include "macros.h"
#include "pace.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#define ENVIRON_DATADIR ("BARIS_DATA")
#define ENVIRON_SAVEDIR ("BARIS_SAVE")

#if CONFIG_WIN32
#  define DEFAULT_DATADIR ("c:/" PACKAGE_TARNAME )
#  define DEFAULT_SAVEDIR ("c:/" PACKAGE_TARNAME "/savedat")
#elif CONFIG_LINUX
/* TODO: make this depend on PREFIX */
#  define DEFAULT_DATADIR CONFIG_DATADIR
#  define DEFAULT_SAVEDIR (".")
#elif CONFIG_MACOSX
#  warn TODO: fill something in here...
#endif

/* and provide defaults for unspecified OS */
#ifndef DEFAULT_DATADIR
#  define DEFAULT_DATADIR (".")
#endif

#ifndef DEFAULT_SAVEDIR
#  define DEFAULT_SAVEDIR (".")
#endif

game_options options;

/*set-up array for environment vars */
static struct {
    char *name;
    char **dest;
    char *def_val;
} env_vars[] = {
    {ENVIRON_DATADIR, &options.dir_gamedata, DEFAULT_DATADIR},
    {ENVIRON_SAVEDIR, &options.dir_savegame, DEFAULT_SAVEDIR},
};

static void
usage (int fail)
{
	fprintf(stderr, "usage: raceintospace [-i] [-n] [-a]\n");
	exit((fail) ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void
shift_argv(char ** argv, int len, int shift)
{
    int i = 0;
    assert(shift >= 0);
    assert(len >= 0);
    if (!shift)
        return;
    for (i = shift; i < len; ++i)
    {
        argv[i - shift] = argv[i];
        argv[i] = NULL;
    }
}

/* returns length of modified argv */
/* possibly maintain a list of dirs to search?? */
int
setup_options(int argc, char * argv[])
{
    char *str = NULL;
    int pos, i;

    /* first set up defaults */
    for (i = 0; i < (int)ARRAY_LENGTH(env_vars); ++i)
    {
        if ((str = SDL_getenv(env_vars[i].name)))
            *env_vars[i].dest = xstrdup(str);
        else if (strcmp(env_vars[i].name, ENVIRON_SAVEDIR) == 0
                &&
                ((str = SDL_getenv("HOME"))
#if CONFIG_WIN32
                  || (str = SDL_getenv("HOMEPATH"))
                  || (str = SDL_getenv("USERPROFILE"))
#endif

                  ))
        {

            size_t len = strlen(str) + strlen(PACKAGE_TARNAME) + 3;
            *env_vars[i].dest = xmalloc(len);
            sprintf(*env_vars[i].dest, "%s/.%s", str, PACKAGE_TARNAME);
        }
        else
            *env_vars[i].dest = xstrdup(env_vars[i].def_val);
    }

    options.want_audio = 1;
    options.want_intro = 0;
    options.want_fade = 1;
    options.want_cheats = 0;

    /* now check cmdline arguments */
    for (pos = 1; pos < argc; ++pos)
    {
        str = argv[pos];

        if (*str != '-') 
            break;

        str++;

        if (*str == '-')
        {
            pos++;
            break;
        }

        if (!*str)
            break;

        for ( ; *str; ++str)
        {
            if (*str == 'h')
                usage(0);
            else if (*str == 'i')
                options.want_intro = 1;
            else if (*str == 'n')
                options.want_cheats = 1;
            else if (*str == 'a')
                options.want_audio = 0;
            else
            {
                /* ERROR */ fprintf(stderr, "unknown option -%c\n", *str);
                usage(1);
            }
        }
    }

    /* delete options from list */
    shift_argv(argv+1, argc-1, pos-1);
    argc -= (pos - 1);

    /* filter the non-option arguments for variable assignments */
    /* pos contains index of first non-option element, if any */
    for (pos = 1; pos < argc; ++pos)
    {
        /* TODO should use PATH_MAX or something similar here */
        char name[32+1], *value;
        int offset = 0;
        int fields = 0;

        fields = sscanf(argv[pos], "%32[A-Z_]=%n", name, &offset);

        /* it is unclear whether %n increments return value */
        if (fields < 1)
            continue;

        value = argv[pos] + offset;

        for (i = 0; i < (int) ARRAY_LENGTH(env_vars); ++i)
        {
            if (strcmp(name, env_vars[i].name) == 0)
            {
                free(*env_vars[i].dest);
                *env_vars[i].dest = xstrdup(value);
                break;
            }
        }

        if (i == (int) ARRAY_LENGTH(env_vars))
            /* WARN */ fprintf(stderr,
                    "unsupported command line variable %s\n", name);

        /* remove matched string from argv */
        shift_argv(argv+pos, argc-pos, 1);

        /*
         * now we have one less arg, pos points to the next arg, 
         * keep it this way after incrementing it on top of the loop
         */
        pos--;
        argc--;
    }

    return argc;
}

