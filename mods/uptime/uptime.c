#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MY_API void up(struct irc_conn *bot, char *user, char *host, char *chan, char *text)
{
	char buf[100];
	FILE* file;

	printf("dbug up called: %s!%s %s\n", user, host, text);

	if (!strcmp(text, "!uptime"))
	{
		 file = popen("uptime", "r");
		 fgets(buf, 100, file);
		 pclose(file);

		 irc_privmsg(bot, chan, "%s", buf);
	}

}

MY_API void mod_init()
{
    printf("installing up handler\n");
	add_handler(PRIVMSG_CHAN, up);
}

MY_API void mod_unload()
{
    printf("unloading up handler\n");
    del_handler(PRIVMSG_CHAN, up);
}
