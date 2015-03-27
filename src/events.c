#include "irc.h"
#include "util.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define HANDLERARRY_LEN 500

struct handler privmsg_self;
struct handler privmsg_chan;
struct handler chan_join;

void init_events()
{
	privmsg_self.type = PRIVMSG_SELF;
	privmsg_chan.type = PRIVMSG_CHAN;
	chan_join.type    = JOIN;

	privmsg_self.count = 0;
	privmsg_chan.count = 0;
	chan_join.count    = 0;

	privmsg_self.handlers = malloc(sizeof(void *) * HANDLERARRY_LEN);
	privmsg_chan.handlers = malloc(sizeof(void *) * HANDLERARRY_LEN);
	chan_join.handlers    = malloc(sizeof(void *) * HANDLERARRY_LEN);		
}

void add_handler(int type, void *handler)
{
	printf("Installing handler @ %p [type: %i]\n", handler, type);
	int handler_count;

	if (type == PRIVMSG_SELF)
	{
		privmsg_self.handlers[privmsg_self.count] = handler;
		privmsg_self.count++;
	}
	else if (type == PRIVMSG_CHAN)
	{
		privmsg_chan.handlers[privmsg_chan.count] = handler;
		privmsg_chan.count++;
	}
	else if (type == JOIN)
	{
		chan_join.handlers[chan_join.count] = handler;
		chan_join.count++;
	}

}

void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text)
{
	void (*handler)();

	for (int i = 0; i < privmsg_chan.count; i++)
	{
		handler = privmsg_chan.handlers[i];
		((void(*)())handler)(bot, user, chan, text);
	}
}

void handle_self_privmsg(struct irc_conn *bot, char *user, char *text)
{
	void (*handler)();
	char *cmd, *arg, *modpath;
	cmd = text;
	arg = skip(cmd, ' ');

	modpath = (char *)malloc(sizeof(char)*500);

	for (int i = 0; i < privmsg_self.count; i++)
	{
		handler = privmsg_self.handlers[i];
		((void(*)())handler)(bot, user, text);
	}	

	if (!strcmp("JOIN", cmd))
	{
		if (strcmp(bot->admin, user))
		{
			irc_raw(bot, "JOIN %s", arg);
		}
		else
		{
			irc_notice(bot, user, "You are unauthorized to use this command.");
		}
	}

	if (!strcmp("LOADMOD", cmd))
	{
		if (strcmp(bot->admin, user))
		{
			irc_notice(bot, user, "Loading module: mods/%s.so", arg);
			sprintf(modpath, "./mods/%s.so", arg);
			load_module(bot, user, PRIVMSG_SELF, modpath);
		}
		else
		{
			irc_notice(bot, user, "You are unauthorized to use this command.");
		}
	}

	free(modpath);
}

void handle_join(struct irc_conn *bot, char *user, char *chan)
{
	void (*handler)();

	for (int i = 0; i < chan_join.count; i++)
	{
		handler = chan_join.handlers[i];
		((void(*)())handler)(bot, user, chan);
	}
}

void free_events()
{
	free(privmsg_self.handlers);
	free(privmsg_chan.handlers);
	free(chan_join.handlers);
}