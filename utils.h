#ifndef _UTILS_H
#define _UTILS_H

#include "race.h"
#include "int_types.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

void *xmalloc (size_t size);
void *xcalloc (size_t nelems, size_t size);
void *xrealloc (void *ptr, size_t size);
char *xstrdup (char const *a);
int xstrcasecmp (char const *a, char const *b);
int xstrncasecmp (char const *a, char const *b, int len);
ssize_t fread_dyn (char **destp, size_t *n, FILE *stream);
double get_time (void);

#endif /* _UTILS_H */
