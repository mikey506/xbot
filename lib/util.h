/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#ifndef UTIL_H
#define UTIL_H

void eprint(char *fmt, ...);
void strlcpy(char *to, const char *from, int len);
char *skip(char *s, char c);
void trim(char *s);

#endif