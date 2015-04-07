#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>

void hello(struct irc_conn *bot, char *user, char *chan, char *text)
{
	char *buf = (char *)malloc(sizeof(char *) * 500);
	sprintf(buf, "hi %s", bot->nick);

	if (!strcmp(text, buf))
	{
		irc_privmsg(bot, chan, "hi %s", user);
	}

	free(buf);
}

void mod_init()
{
	add_handler(PRIVMSG_CHAN, hello);
}