#ifndef MODULE_H
#define MODULE_H

#include "irc.h"

void load_module(struct irc_conn *bot, char *where, int stype, char *file);
void export_handler(int type, char *hname);

#endif