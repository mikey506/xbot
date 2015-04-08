#ifndef MODULE_H
#define MODULE_H

#include "irc.h"

void load_module(struct irc_conn *bot, char *where, char *stype, char *file);

#endif