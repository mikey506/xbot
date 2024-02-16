/*
 * irc.c: IRC connection and parser
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#include "irc.h"
#include "util.h"
#include "events.h"
#include "channel.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define FDOPEN _fdopen
#define SETBUF setbuf
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define FDOPEN fdopen
#define SETBUF setbuf
#endif

void irc_connect(struct irc_conn *bot)
{
#ifdef _WIN32
    WSADATA wsaData;
	struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        eprint("WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }

	bot->srv_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bot->srv_fd == INVALID_SOCKET)
	{
		eprint("Error creating socket: %d\n", WSAGetLastError());
		WSACleanup();

		return;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(bot->host);
	server.sin_port = htons(atoi(bot->port));

	if (connect(bot->srv_fd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		eprint("Failed to connect to IRC server: %d\n", WSAGetLastError());

		closesocket(bot->srv_fd);
		WSACleanup();

		return;
	}
#else
    int srv_fd;
    struct addrinfo hints;
    struct addrinfo *res, *r;

    memset(&hints, 0, sizeof hints);
    hints.ai_family		= AF_UNSPEC;
    hints.ai_socktype	= SOCK_STREAM;

    if (getaddrinfo(bot->host, bot->port, &hints, &res) != 0)
    {
        eprint("Error: Cannot resolve hostname '%s':", bot->host);
    }

    for (r = res; r; r->ai_next)
    {
        if ((srv_fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1)
        {
            continue;
        }

        if (connect(srv_fd, r->ai_addr, r->ai_addrlen) == 0)
        {
            break;
        }

        close(srv_fd);
    }

    freeaddrinfo(res);
    if (!r)
    {
        eprint("Error: Cannot connect to host '%s'\n", bot->host);
    }

    printf("Connected!\n");
    bot->srv_fd = FDOPEN(srv_fd, "r+");
#endif
}

void irc_auth(struct irc_conn *bot)
{
    irc_raw(bot, "NICK %s", bot->nick);
    irc_raw(bot, "USER %s \" %s :xbot (v0.5) - developed by ab3800", bot->nick, bot->host);

#ifndef _WIN32
    fflush(bot->srv_fd);
    SETBUF(bot->srv_fd, NULL);
#endif
}

void irc_notice(struct irc_conn *bot, char *to, char *fmt, ...)
{
    char msg_[4096];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(msg_, sizeof msg_, fmt, ap);
    va_end(ap);

    irc_raw(bot, "NOTICE %s :%s", to, msg_);
}

void irc_privmsg(struct irc_conn *bot, char *to, char *fmt, ...)
{
    char msg_[4096];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(msg_, sizeof msg_, fmt, ap);
    va_end(ap);

    irc_raw(bot, "PRIVMSG %s :%s", to, msg_);
}

void irc_raw(struct irc_conn *bot, char *fmt, ...)
{
    va_list ap;
	char outbuf[4096];

    va_start(ap, fmt);
    vsnprintf(bot->out, sizeof bot->out, fmt, ap);
    va_end(ap);

#ifdef _WIN32
	sprintf(outbuf, "%s\r\n", bot->out);
	send(bot->srv_fd, outbuf, strlen(outbuf), 0);
#else
    fprintf(bot->srv_fd, "%s\r\n", bot->out);
#endif
}


void irc_join(struct irc_conn *bot, char *chan)
{
    irc_raw(bot, "JOIN %s", chan);

    add_channel(chan);
}

void irc_part(struct irc_conn *bot, char *chan, char *reason)
{
    if (!reason)
    {
        reason = "";
    }

    irc_raw(bot, "PART %s :%s", chan, reason);
}

void irc_ctcp(struct irc_conn *bot, char *to, char *fmt, ...)
{
    char msg_[4096];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(msg_, sizeof msg_, fmt, ap);
    va_end(ap);

    irc_privmsg(bot, to, "\001%s\001", msg_);
}

void irc_parse_raw(struct irc_conn *bot, char *raw)
{
    char *user, *host, *par, *text, *chan, *nick, *nicks;
    user = bot->host;

    text = calloc(1, strlen(raw) + 1);
	
	printf("raw: %s", raw);

    if (!raw || !*raw)
    {
        return;
    }

    if (raw[0] == ':')
    {
        user = raw + 1;
        raw  = skip(user, ' ');

        if (raw[0] == '\0')
        {
            return;
        }

        host = skip(user, '!');
    }

    skip(raw, '\r');
    par  = skip(raw, ' ');
    text = skip(par, ':');
    
    trim(par);

	printf("dbug raw: %s\n", raw);

    if (!strcmp("PONG", raw))
    {
        return;
    }

    if (!strcmp("PRIVMSG", raw))
    {
        // check for CTCP
        if (text[0] == '\001')
        {
            char *ctcp = text + 1;
            char *end  = strchr(ctcp, '\001');

            if (end)
            {
                *end = '\0';

                // reply to version request
                if (!strcmp("VERSION", ctcp))
                {
#ifdef _WIN32
                    irc_notice(bot, user, "VERSION xbot: v0.5 (Windows) - Developed by ab3800");
#else
                    irc_notice(bot, user, "VERSION xbot: v0.5 (Linux) - Developed by ab3800");
#endif
                }
                else
                {
                    fire_handler(bot, CTCP, user, host, par, ctcp);
                }
            }
        }
        else
        {
            if (!strcmp(par, bot->nick))
            {
                fire_handler(bot, PRIVMSG_SELF, user, host, text);
            }
            else
            {
                fire_handler(bot, PRIVMSG_CHAN, user, host, par, text);
            }
        }
    }
    else if (!strcmp("JOIN", raw))
    {
        if (!strcmp(user, bot->nick))
        {
            add_channel(text);
            add_user_to_channel(user, host, text);
            fire_handler(bot, JOIN_MYSELF, user, host, text);
        }
        else
        {
            add_channel(text);
            add_user_to_channel(user, host, text);
            fire_handler(bot, JOIN, user, host, par);
        }
    }
    else if (!strcmp("PART", raw))
    {
        if (!strcmp(user, bot->nick))
        {
            remove_channel(text);
            fire_handler(bot, PART_MYSELF, user, host, text);
        }
        else
        {
            remove_user_from_channel(user, text);
            fire_handler(bot, PART, user, host, text);
        }

    }
    else if (!strcmp("QUIT", raw))
    {
        user_quit(user);
        fire_handler(bot, QUIT, user, host, text);
    }
    else if (!strcmp("PING", raw))
    {
        irc_raw(bot, "PONG %s", text);
    }
    else if (!strcmp("001", raw))
    {
        fire_handler(bot, IRC_CONNECTED, text);
    }
    else if (!strcmp("433", raw))
    {
        eprint("Error: Nickname '%s' is already in use\n", bot->nick);

        fire_handler(bot, NICK_INUSE, text);

#ifdef _WIN32
        _snprintf(bot->nick, sizeof bot->nick, "%s_", bot->nick);
#else
        snprintf(bot->nick, sizeof bot->nick, "%s_", bot->nick);
#endif
        irc_raw(bot, "NICK %s", bot->nick);
    }
    else if (strstr("353", raw) != NULL)
    {
		printf("debug raw: %s\n", raw);
        printf("debug par: %s, text: %s\n", par, text);
        // par: BotName = #channel
        // extract channel name

        chan = skip(par, ' ');
        chan = skip(chan, '=');
        chan = skip(chan, ' ');

        // text is a list of nicks separated by spaces
#ifdef _WIN32
        nicks = _strdup(text);
#else
        nicks = strdup(text);
#endif
        nick  = strtok(nicks, " ");


        printf("debug: chan: %s, nicks: %s\n", chan, nicks);

        while (nick)
        {
            add_user_to_channel(nick, "", chan);
            nick = strtok(NULL, " ");
        }

        fire_handler(bot, IRC_NAMREPLY, chan, text);
    }
    else
    {
        if (!strcmp("NICK", raw) && !strcmp(user, bot->nick))
        {
            strlcpy(bot->nick, text, sizeof bot->nick);

            fire_handler(bot, NICK_MYSELF, user, text);
        }
    }
}
