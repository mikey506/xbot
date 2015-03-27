/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <libconfig.h>
#include "irc.h"
#include "util.h"
#include "events.h"


void hello_cmd(struct irc_conn *bot, char *user, char *text)
{
	printf("cmd exec\n");
	irc_notice(bot, user, "Hello");
}

int main()
{
	fd_set rd;
	config_t cfg, *cf;
	const config_setting_t *retries;
	const char *base = NULL;
	struct irc_conn bot;

	init_events();

	// Init the config parser
	cf = &cfg;
	config_init(cf);

	if (!config_read_file(cf, "xbot.cfg"))
	{
		printf("xbot.cfg:%d - %s\n",
			config_error_line(cf),
			config_error_text(cf));

		config_destroy(cf);
		return -1;
	}

	// Fill our bot struct with values from the config
	if (config_lookup_string(cf, "bot.nick", &base))
		strlcpy(bot.nick, base, sizeof bot.nick);

	if (config_lookup_string(cf, "server.host", &base))
		bot.host = (char *)base;

	if (config_lookup_string(cf, "server.port", &base))
		bot.port = (char *)base;

	if (config_lookup_string(cf, "bot.admin", &base))
		bot.admin = (char *)base;

	// Connect to the server
	printf("Connecting to %s...\n", bot.host);
	irc_connect(&bot);
	irc_auth(&bot);

	// Free the config before entering the main loop
	config_destroy(cf);


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