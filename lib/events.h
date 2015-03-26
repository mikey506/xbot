#ifndef EVENTS_H
#define EVENTS_H

#include "irc.h"

#define PRIVMSG_SELF    1
#define PRIVMSG_CHAN    2
#define JOIN            3

struct handler
{
	int type;
	int count;
	void **handlers;
};

void init_events();
void add_handler(int type, void *handler);
void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text);
void handle_self_privmsg(struct irc_conn *bot, char *user, char *text);
void handle_join(struct irc_conn *bot, char *user, char *chan);

#endif