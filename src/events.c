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
struct handler irc_connected;

// TODO:
// redo this module, unified api

void init_events()
{
	privmsg_self.type  = PRIVMSG_SELF;
	privmsg_chan.type  = PRIVMSG_CHAN;
	chan_join.type     = JOIN;
	irc_connected.type = IRC_CONNECTED;

	privmsg_self.count  = 0;
	privmsg_chan.count  = 0;
	chan_join.count     = 0;
	irc_connected.count = 0;

	privmsg_self.handlers  = malloc(sizeof(void *) * HANDLERARRY_LEN);
	privmsg_chan.handlers  = malloc(sizeof(void *) * HANDLERARRY_LEN);
	chan_join.handlers     = malloc(sizeof(void *) * HANDLERARRY_LEN);		
	irc_connected.handlers = malloc(sizeof(void *) * HANDLERARRY_LEN);

}

int add_handler(char *type, void *handler)
{
	printf("Installing handler @ %p [type: %s]\n", handler, type);
	int handler_count;

	if (!strcmp(PRIVMSG_SELF, type))
	{
		privmsg_self.handlers[privmsg_self.count] = handler;
		privmsg_self.count++;

		return privmsg_self.count - 1;
	}
	else if (!strcmp(PRIVMSG_CHAN, type))
	{
		privmsg_chan.handlers[privmsg_chan.count] = handler;
		privmsg_chan.count++;

		return privmsg_chan.count - 1;
	}
	else if (!strcmp(JOIN, type))
	{
		chan_join.handlers[chan_join.count] = handler;
		chan_join.count++;

		return chan_join.count - 1;
	}
	else if (!strcmp(IRC_CONNECTED, type))
	{
		irc_connected.handlers[irc_connected.count] = handler;
		irc_connected.count++;

		return irc_connected.count - 1;
	}
}

void del_handler(int num, char *type)
{
	if (type == PRIVMSG_SELF)
		privmsg_self.handlers[num] = NULL;
	else if (type == PRIVMSG_CHAN)
		privmsg_chan.handlers[num] = NULL;
}

void handle_connected(struct irc_conn *bot, char *text)
{
	void (*handler)();

	for (int i = 0; i < irc_connected.count; i++)
	{
		if ((handler = irc_connected.handlers[i]) != NULL)
			(*handler)(bot, text);
	}
}

void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text)
{
	void (*handler)();

	for (int i = 0; i < privmsg_chan.count; i++)
	{
		if ((handler = privmsg_chan.handlers[i]) != NULL)
			(*handler)(bot, user, chan, text);
	}
}

void handle_self_privmsg(struct irc_conn *bot, char *user, char *text)
{
	void (*handler)();
	int i;
	char *cmd, *arg, *modpath;
	cmd = text;
	arg = skip(cmd, ' ');

	modpath = (char *)malloc(sizeof(char)*500);

	for (i = 0; i < privmsg_self.count; i++)
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

	if (!strcmp("PRINT_HANDLERS", cmd))
	{
		if (strcmp(bot->admin, user))
		{
			for (i = 0; i < privmsg_chan.count; i++)
			{
				irc_notice(bot, user, "handler[%i:%i]: %p", i, privmsg_chan.type, privmsg_chan.handlers[i]);
			}

			for (i = 0; i < privmsg_self.count; i++)
			{
				irc_notice(bot, user, "handler[%i:%i]: %p", i, privmsg_self.type, privmsg_self.handlers[i]);
			}
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