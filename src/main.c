/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include "config.h"
#include "irc.h"
#include "util.h"
#include "events.h"


static time_t trespond;

int main()
{
	int n;
	fd_set rd;
	struct irc_conn bot;
	struct timeval tv;

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
		FD_SET(0, &rd);
		FD_SET(fileno(bot.srv_fd), &rd);

		tv.tv_sec  = 120;
		tv.tv_usec = 0;
		n = select(fileno(bot.srv_fd) + 1, &rd, 0, 0, &tv);

		if (n < 0)
		{
			if (errno == EINTR)
				continue;

			eprint("xbot: error on select()\n");
			return 0;
		}

		else if (n == 0)
		{
			if (time(NULL) - trespond >= 300)
			{
				eprint("xbot shutting down: parse timeout\n");
				return -1;
			}

			irc_raw(&bot, "PING %s", bot.host);
			continue;
		}

		if (FD_ISSET(fileno(bot.srv_fd), &rd))
		{
			if (fgets(bot.in, sizeof bot.in, bot.srv_fd) == NULL)
			{
				eprint("xbot: remote host closed connection\n");
				return 0;
			}

			irc_parse_raw(&bot, bot.in);
			trespond = time(NULL);
		}
	}

	return 0;
}
