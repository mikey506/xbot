/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#ifndef IRC_H
#define IRC_H

#include <stdio.h>

#ifdef _WIN32
#include <winsock2.h>
#endif


struct irc_conn
{
#ifdef _WIN32
	SOCKET srv_fd;
#else
	FILE *srv_fd;
#endif

    char nick[32];
    char admin[64];
    char host[256];
    char port[5];
    char real_name[512];

    // I/O Buffers
    char out[4096];
    char in[4096];
};

typedef struct handler event_handler;

#ifdef _WIN32
#ifdef MY_DLL_EXPORTS
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif
#else
#define MY_API
#endif

void irc_connect(struct irc_conn *bot);
void irc_auth(struct irc_conn *bot);

MY_API void irc_notice(struct irc_conn *bot, char *to, char *fmt, ...);
MY_API void irc_privmsg(struct irc_conn *bot, char *to, char *fmt, ...);
MY_API void irc_raw(struct irc_conn *bot, char *fmt, ...);
MY_API void irc_join(struct irc_conn *bot, char *channel);
MY_API void irc_part(struct irc_conn *bot, char *channel, char *reason);


void irc_parse_raw(struct irc_conn *bot, char *raw);

#endif
