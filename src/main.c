/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include "config.h"
#include "irc.h"
#include "util.h"
#include "events.h"
#include "module.h"


int main()
{
	fd_set rd;
	struct irc_conn bot;

	init_events();

	// Read the config
	bot = read_config(bot, "xbot.cfg");

	// Connect to the server
	printf("Connecting to %s...\n", bot.host);
	irc_connect(&bot);
	irc_auth(&bot);


	for (;;)
	{
		FD_ZERO(&rd);
		FD_SET(fileno(bot.srv_fd), &rd);

		if (FD_ISSET(fileno(bot.srv_fd), &rd))
		{
			if (fgets(bot.in, sizeof bot.in, bot.srv_fd) == NULL)
			{
				eprint("xbot: remote host closed connection\n");
			}

			irc_parse_raw(&bot, bot.in);
		}
	}

	return 0;
}