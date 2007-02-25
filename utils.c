#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* taken from ffmpeg project */
#ifdef CONFIG_WIN32
#   include <sys/types.h>
#   include <sys/timeb.h>
#elif defined(CONFIG_OS2)
#   include <string.h>
#   include <sys/time.h>
#else
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/time.h>
#endif

double
get_time (void)
{
#ifdef CONFIG_WIN32
    /* mingw was complaining */
	struct _timeb tb;

	_ftime(&tb);
	return tb.time + tb.millitm / 1e3;
#else
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1e6;
#endif
}

int
xstrcasecmp (char const *a, char const *b)
{
	while (*a) {
		if (tolower (*a & 0xff) != tolower (*b & 0xff))
			break;
		a++;
		b++;
	}
	return (tolower(*a) - tolower(*b));
}

int
xstrncasecmp (char const *a, char const *b, int n)
{
	while (n && *a) {
		if (tolower (*a & 0xff) != tolower (*b & 0xff))
			break;
		a++;
		b++;
		n--;
	}
	return (tolower(*a) - tolower(*b));
}

void *
xmalloc (size_t n)
{
	void *p;

	if ((p = malloc (n)) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

void *
xcalloc (size_t a, size_t b)
{
	void *p;

	if ((p = calloc (a, b)) == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

char *
xstrdup (const char *s)
{
	void *p;

	p = xmalloc (strlen (s) + 1);
	strcpy (p, s);
	return (p);
}

void *
xrealloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);

	if (!p)
    {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	return (p);
}

ssize_t
fread_dyn(char **destp, size_t *n, FILE *stream)
{
    const unsigned bsize = 8192;
    size_t total = 0, cnt = 0; 

    assert(destp);
    assert(n);
    assert(stream);

    if (!*destp) 
        *destp = xmalloc(*n = bsize);

    while (1) {
        cnt = fread(*destp+total, 1, *n-total, stream);
        if (cnt != *n-total)
        {
            if (feof(stream))
                return total+cnt;
            else if (ferror(stream))
                return -1;
        }
        total += cnt;

        if (*n <= total)
            *destp = xrealloc(*destp, *n *= 2);
    }
}

/* vim: set noet ts=4 sw=4 tw=77: */
