#include "irc.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text)
{

}

void handle_self_privmsg(struct irc_conn *bot, char *user, char *text)
{
	char *cmd, *arg;
	cmd = text;
	arg = skip(cmd, ' ');

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
}