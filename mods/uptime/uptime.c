#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>

void up(struct irc_conn *bot, char *user, char *chan, char *text)
{
	char buf[100];
	FILE* file;

	if (!strcmp(text, "!uptime"))
	{
		 file = popen("uptime", "r");
		 fgets(buf, 100, file);
		 pclose(file);

		 irc_privmsg(bot, chan, "%s", buf);
	}

}

void mod_init()
{
	add_handler(PRIVMSG_CHAN, up);
}