/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
#define USE_MY_STRLCPY 1
#endif

void eprint(char *fmt, ...);

#if defined(__GLIBC__) && (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 38)) || defined(_WIN32)
void strlcpy(char *to, const char *from, int len);
#endif

char *skip(char *s, char c);
void trim(char *s);

#endif
