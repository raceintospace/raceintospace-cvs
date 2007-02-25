/*
    Copyright (C) 2007 Krzysztof Kosciuszkiewicz
    Copyright (C) 2005 Michael K. McCarty & Fritz Bronner

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
#include "fs.h"
#include "options.h"
#include "pace.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

/* see how do we call mkdir */
#if HAVE_MKDIR
# if MKDIR_TAKES_ONE_ARG
   /* MinGW32 */
#  define mkdir(a, b) mkdir(a)
# endif
#else
# if HAVE__MKDIR
   /* plain Windows 32 */
#  define mkdir(a, b) _mkdir(a)
# else
#  error "Don't know how to create a directory on this system."
# endif
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

static DIR *save_dir;

/*
 * gamedata & savedata access functions
 */

/* try to open base/xxx/name for xxx = arg4 ... */
static FILE *
s_open_helper(const char *base, const char *name, const char *mode, ...)
{
	FILE *f = NULL;
	char *p = NULL;
	char *cooked = xmalloc(1024);
	size_t len = 1024, len2 = 0;
	size_t len_base = strlen(base), len_name = strlen(name);
	va_list ap;

	assert(base);
	assert(name);
	assert(mode);

	va_start(ap, mode);
	for (p = va_arg(ap, char *); p; p = va_arg(ap, char *))
	{
		char *s = NULL;
		size_t len_p = strlen(p);

		len2 = len_base + len_name + len_p + 3;
		if (len2 > len)
			cooked = xrealloc(cooked, (len = len2));
		sprintf(cooked, "%s/%s/%s", base, p, name);
		f = fopen(cooked, mode);
		if (f)
			break;

		/* try lowercase version */
		for (s = cooked + len_base + len_p + 2; *s; ++s)
			if (isupper(*s))
				*s = tolower(*s);

		f = fopen(cooked, mode);
		if (f)
			break;
	}
	va_end(ap);
	free(cooked);
	return f;
}

FILE *
sOpen(const char *name, const char *mode, int type)
{
	FILE *f = NULL;
	char *gd = options.dir_gamedata;
	char *sd = options.dir_savegame;
	char *where = "";

	/* allow write access only to savegame files */
	if (type != FT_SAVE)
	{
		if (strchr(mode, 'b'))
			mode = "rb";
		else
			mode = "r";
	}

	switch (type)
	{
		case FT_DATA:
			f = s_open_helper(gd, name, mode,
					"gamedata",
					NULL);
			where = "game data";
			break;
		case FT_SAVE:
			f = s_open_helper(sd, name, mode,
					".",
					NULL);
			where = "save data";
			break;
		case FT_AUDIO:
			f = s_open_helper(gd, name, mode,
					"audio/mission",
					"audio/music",
					"audio/news",
					"audio/sounds",
					".",
					NULL);
			where = "audio";
			break;
		case FT_VIDEO:
			f = s_open_helper(gd, name, mode,
					"video/mission",
					"video/news",
					"video/training",
					".",
					NULL);
			where = "video";
			break;
		default:
			assert("Unknown FT_* specified");
	}

	if (!f)
	{
		int serrno = errno;
		/* WARN */ fprintf(stderr, "can't %s file %s in %s directory\n",
			strchr(mode, 'w') ? "create" : "access", name, where);
		errno = serrno;
	}
	return f;
}

int
remove_savedat(const char *name)
{
	size_t len_base = strlen(options.dir_savegame) + 1;
	size_t len_name = strlen(name) + 1;
	char *cooked = xmalloc(len_base + len_name);
	int rv = 0;

	sprintf(cooked, "%s/%s", options.dir_savegame, name);
	rv = remove(cooked);
	free(cooked);
	return rv;
}

FILE *
open_gamedat(const char *name)
{
	return sOpen(name, "rb", FT_DATA);
}

FILE *
open_savedat(const char *name, const char *mode)
{
	return sOpen(name, mode, FT_SAVE);
}

char *
slurp_gamedat(const char *name)
{
	FILE *f;
	ssize_t len;
	char *p = NULL;
	size_t buflen = 0;

	f = open_gamedat(name);
	if (!f)
		return NULL;

	len = fread_dyn(&p, &buflen, f);

	if (len < 0)
	{
		/* ERROR */ perror("slurp_gamedat");
		exit(EXIT_FAILURE);
	}

	fclose(f);

	return p;
}

int
create_save_dir(void)
{
	if (mkdir(options.dir_savegame, 0777) < 0 && errno != EEXIST)
		return -1;
	errno = 0;
	return 0;
}

int
first_saved_game(struct ffblk *ffblk)
{
	if (save_dir)
	{
		closedir(save_dir);
		save_dir = NULL;
	}

	if ((save_dir = opendir(options.dir_savegame)) == NULL)
		return (1);

	return (next_saved_game(ffblk));
}

int
next_saved_game(struct ffblk *ffblk)
{
	struct dirent *dp;
	int len;

	memset(ffblk, 0, sizeof *ffblk);

	if (save_dir == NULL)
		return (1);

	while ((dp = readdir(save_dir)) != NULL)
	{
		len = NAMLEN(dp);
		if (len < 4)
			continue;
		if (xstrncasecmp(dp->d_name + len - 4, ".SAV", 4) != 0)
			continue;

		strncpy(ffblk->ff_name, dp->d_name, sizeof ffblk->ff_name);
		ffblk->ff_name[sizeof ffblk->ff_name - 1] = 0;

		return (0);
	}

	return (1);
}
