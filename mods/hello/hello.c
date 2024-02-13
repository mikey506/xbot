#define MY_DLL_EXPORTS 1

#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HANDLER = 0;

MY_API void hello(struct irc_conn *bot, char *user, char *chan, const char *text)
{
	char *buf = (char *)malloc(sizeof(char *) * 500);
	sprintf(buf, "hi %s", bot->nick);

	if (!strcmp(text, buf))
	{
		irc_privmsg(bot, chan, "hi %s", user);

        printf("%s said hi to me\n", user);
	}

	free(buf);
}

MY_API void mod_init()
{
	HANDLER = add_handler(PRIVMSG_CHAN, hello);
}
