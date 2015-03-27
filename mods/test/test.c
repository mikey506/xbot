#include "irc.h"
#include "events.h"
#include <stdio.h>
#include <stdlib.h>

void hello(struct irc_conn *bot, char *user, char *chan, char *text)
{
	printf("hi\n");

	char *buf = (char *)malloc(sizeof(char *) * 500);
	sprintf(buf, "hi %s", bot->nick);

	printf("trigger: %s\n", buf);

	if (!strcmp(text, buf))
	{
		irc_privmsg(bot, chan, "hi %s", user);
	}

	free(buf);
}

void mod_init(struct irc_conn *b, char *where)
{
	add_handler(PRIVMSG_CHAN, hello);
}