#ifndef EVENTS_H
#define EVENTS_H

#include "irc.h"

void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text);
void handle_self_privmsg(struct irc_conn *bot, char *user, char *text);

#endif