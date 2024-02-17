#define MY_DLL_EXPORTS 1

#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MY_API void hello(struct irc_conn *bot, char *user, char *host, char *chan, const char *text)
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

MY_API void hello_join(struct irc_conn *bot, char *user, char *host, char *chan)
{
    printf("%s!%s joined %s\n", user, host, chan);

    irc_privmsg(bot, chan, "Hi %s! Welcome to %s", user, chan);
}

MY_API void mod_init()
{
    add_handler(PRIVMSG_CHAN, hello);
    add_handler(JOIN, hello_join);
}

MY_API void mod_unload()
{
    del_handler(PRIVMSG_CHAN, hello);
    del_handler(JOIN, hello_join);
}
