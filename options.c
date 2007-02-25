/*
    Copyright (C) 2007 Krzysztof Kosciuszkiewicz

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "race.h"
#include "options.h"
#include "macros.h"
#include "pace.h"
#include "fs.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#define ENVIRON_DATADIR ("BARIS_DATA")
#define ENVIRON_SAVEDIR ("BARIS_SAVE")

#if CONFIG_WIN32
#  define DEFAULT_DATADIR ("c:/" PACKAGE_TARNAME )
#  define DEFAULT_SAVEDIR ("c:/" PACKAGE_TARNAME "/savedat")
#elif CONFIG_LINUX
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

#if !HAVE_GETENV
#  if HAVE_SDL_GETENV
#    define getenv SDL_getenv
#  else
#    warn I don't know a way to read environment on this system
#    define getenv(a) (NULL)
#  endif
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

static struct {
	char *name;
	void *dest;
	char *type;
	int need_alloc;
	char *comment;
} config_strings[] = {
	{"datadir", &options.dir_gamedata, "%1024[^\n\r]", 1025,
		"Path to directory with game data files." },
	{"audio", &options.want_audio, "%d", 0,
		"Set to 0 if you don't want audio in game." },
	{"nofail",  &options.want_cheats, "%d", 0,
		"Set to 1 if you want every mission step check to succeeed." },
	{"intro", &options.want_intro, "%d", 0,
		"Set to 0 if do not want intro displayed at startup." },
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

static int
skip_past_newline(FILE * f)
{
	assert(f);
	return fscanf(f, "%*[^\r\n] ");
}

int
parse_var_value(FILE * f, int index)
{
	char format[128];
	int need_alloc;
	int res = 0, chars = 0, i = index;
	void **target;

	assert(f);
	assert(i >= 0 && i < (int) ARRAY_LENGTH(config_strings));

	target = config_strings[i].dest;
	need_alloc = config_strings[i].need_alloc;
	if (need_alloc)
		*target = xrealloc(*target, need_alloc);

	sprintf(format, " %s%%n", config_strings[i].type);
	res = fscanf(f, format, *target, &chars);

	if (res < 1)
		return -1;

	if (need_alloc && chars < need_alloc)
		*target = xrealloc(*target, chars + 1);

	return 0;
}

int
read_config_file(void)
{
	FILE *f = open_savedat("config", "rt");
	char config_word[32 + 1];
	int err = 0, res = 0, i = 0;
	char c[2];

	if (!f)
		return -1;

	while (1)
	{
		/* skip comments */
		if ((res = fscanf(f, " %1[#]", c)) == 1)
			goto skip_newline;

		if (res == EOF)
			break;

		/* get configuration variable name */
		res = fscanf(f, "%32[a-zA-Z_-]", config_word);

		/* do we have a match? */
		if (res == 1)
		{
			for (i = 0; i < (int) ARRAY_LENGTH(config_strings); ++i)
			{
				if (strcmp(config_word, config_strings[i].name) == 0)
				{
					res = parse_var_value(f, i);
					if (res != 0 && feof(f))
						goto skip_newline;
					else if (res != 0)
					{
						/* WARN */ fprintf(stderr,
							"config: wrong value type for variable '%s'\n",
							config_word);
						goto skip_newline;
					}
					else
						break;
				}
			}

			/* none matched */
			if (i == (int) ARRAY_LENGTH(config_strings))
			{
				/* WARN */
				fprintf(stderr, "config: unknown variable '%s'\n",
						config_word);
				goto skip_newline;
			}
		}
		else if (res == EOF)
			break;
		else
		{
			/* WARN */ fprintf(stderr,
				"config: expecting variable name!\n");
			goto skip_newline;
		}

	  skip_newline:
		if (EOF == skip_past_newline(f))
			break;
	}

	err = !feof(f);

	fclose(f);

	return -err;
}

int
write_default_config(void)
{
	int i = 0;
	int err = 0;
	FILE *f = NULL;

	create_save_dir();
	f = open_savedat("config", "wt");
	if (!f)
	{
		/* WARN */ fprintf(stderr,
				"can't write default configuration to %s/%s\n",
				options.dir_savegame, "config");
		return -1;
	}
	else
		/* INFO */ fprintf(stderr,
				"writing default configuration file to %s/%s\n",
				options.dir_savegame, "config");

	fprintf(f, "# This is template configuration file for %s\n",
		PACKAGE_STRING);
	fprintf(f, "# Comments start with '#' sign and span whole line.\n");
	fprintf(f, "# Non comment lines should look like:\n");
	fprintf(f, "# variable_name variable_value\n\n");
	for (i = 0; i < (int) ARRAY_LENGTH(config_strings); ++i)
		fprintf(f, "# %s\n# %s\n\n",
			config_strings[i].comment, config_strings[i].name);
	err = ferror(f);
	fclose(f);
	return err;
}

/* returns length of modified argv */

/* possibly maintain a list of dirs to search?? */
int
setup_options(int argc, char *argv[])
{
	char *str = NULL;
	int pos, i;

	/* first set up defaults */
	for (i = 0; i < (int) ARRAY_LENGTH(env_vars); ++i)
	{
		if ((str = getenv(env_vars[i].name)))
			*env_vars[i].dest = xstrdup(str);
		else if (strcmp(env_vars[i].name, ENVIRON_SAVEDIR) == 0
			&& ((str = getenv("HOME"))
#if CONFIG_WIN32
				|| (str = getenv("HOMEPATH"))
				|| (str = getenv("USERPROFILE"))
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
	options.want_intro = 1;
	options.want_cheats = 0;

	/* now try to read config file, if it exists */
	if (read_config_file() < 0)
		/* if not, then write default config template */
		write_default_config();

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

		for (; *str; ++str)
		{
			if (*str == 'h')
				usage(0);
			else if (*str == 'i')
				options.want_intro = 0;
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
	shift_argv(argv + 1, argc - 1, pos - 1);
	argc -= (pos - 1);

	/* filter the non-option arguments for variable assignments */
	/* pos contains index of first non-option element, if any */
	for (pos = 1; pos < argc; ++pos)
	{
		/* TODO should use PATH_MAX or something similar here */
		char name[32 + 1], *value;
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
		shift_argv(argv + pos, argc - pos, 1);

		/*
		 * now we have one less arg, pos points to the next arg, 
		 * keep it this way after incrementing it on top of the loop
		 */
		pos--;
		argc--;
	}

	return argc;
}
