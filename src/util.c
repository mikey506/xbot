/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void eprint(char *fmt, ...)
{
	char bufout[4096];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(bufout, sizeof bufout, fmt, ap);
	va_end(ap);

	fprintf(stderr, "%s", bufout);
	if (fmt[0] && fmt[strlen(fmt) - 1] == ':')
	{
		fprintf(stderr, "%s\n", strerror(errno));
	}
}

void strlcpy(char *to, const char *from, int len)
{
	memccpy(to, from, '\0', len);
	to[len-1] = '\0';
}

char *skip(char *s, char c)
{
	while (*s != c && *s != '\0')
	{
		s++;
	}

	if (*s != '\0')
	{
		*s++ = '\0';
	}

	return s;
}

void trim(char *s)
{
	char *e;

	e = s + strlen(s) - 1;
	while (isspace(*e) && e > s)
	{
		e--;
	}

	*(e + 1) = '\0';
}