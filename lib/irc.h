/*
 * xbot: Just another IRC bot
 *
 * Written by Aaron Blakely <aaron@ephasic.org>
**/

#ifndef IRC_H
#define IRC_H

#include <stdio.h>

struct irc_conn
{
	FILE *srv_fd;

	

	char nick[32];
	char *admin;
	char host[256];
	char port[5];
	char real_name[512];

	// I/O Buffers
	char out[4096];
	char in[4096];

};

void irc_connect(struct irc_conn *bot);
void irc_auth(struct irc_conn *bot);
void irc_notice(struct irc_conn *bot, char *to, char *fmt, ...);
void irc_privmsg(struct irc_conn *bot, char *to, char *fmt, ...);
void irc_raw(struct irc_conn *bot, char *fmt, ...);
void irc_parse_raw(struct irc_conn *bot, char *raw);

#endif