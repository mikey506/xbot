/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "config.h"
#include "irc.h"
#include "util.h"
#include "events.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif


static time_t trespond;

int main()
{
    int n;
    fd_set rd;
    struct irc_conn bot;
    struct timeval tv;

	int bytesRecv;

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

#ifdef _WIN32
		FD_SET(bot.srv_fd, &rd);
#else
		FD_SET(0, &rd);
        FD_SET(fileno(bot.srv_fd), &rd);
#endif
        tv.tv_sec  = 120;
        tv.tv_usec = 0;

#ifdef _WIN32
		n = select(bot.srv_fd, &rd, NULL, NULL, &tv);
		if (n == SOCKET_ERROR)
		{
			eprint("xbot: error on select(): %d\n", WSAGetLastError());
			closesocket(bot.srv_fd);
			WSACleanup();

			return -1;
		}
#else
        n = select(fileno(bot.srv_fd) + 1, &rd, 0, 0, &tv);
		if (n < 0)
		{
            if (errno == EINTR)
                continue;

            eprint("xbot: error on select()\n");
            return -1;
        }
#endif


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
#ifdef _WIN32
        if (FD_ISSET(bot.srv_fd, &rd))
		{
			bytesRecv = recv(bot.srv_fd, bot.in, sizeof(bot.in), 0);
			if (bytesRecv == SOCKET_ERROR)
			{
				eprint("Error receiving data: %d\n", WSAGetLastError());
				closesocket(bot.srv_fd);
				WSACleanup();

				return -1;
			}

			bot.in[bytesRecv] = '\0';
#else
		if (FD_ISSET(fileno(bot.srv_fd), &rd))
		{
            if (fgets(bot.in, sizeof bot.in, bot.srv_fd) == NULL)
            {
                eprint("xbot: remote host closed connection\n");
                return 0;
            }
#endif
			printf("recv: %s\r\n", bot.in);
            irc_parse_raw(&bot, bot.in);
            trespond = time(NULL);
        }
    }

    return 0;
}
