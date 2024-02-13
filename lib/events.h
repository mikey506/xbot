#ifndef EVENTS_H
#define EVENTS_H

#include "irc.h"

#define PRIVMSG_SELF    "CMSG"
#define PRIVMSG_CHAN    "PMSG"
#define PRIVMSG_USER    "UMSG"
#define JOIN            "JOIN"
#define PART            "PART"
#define QUIT            "QUIT"
#define IRC_CONNECTED   "001"
#define IRC_MOTD        "372"
#define IRC_END_MOTD    "376"

struct handler
{
    char *type;
    int count;
    void **handlers;
};

struct event
{
    char *type;
    char *user;
    char *chan;
    char *text;
};

void init_events();
MY_API int add_handler(char *type, void *handler);
void del_handler(int num, char *type);
void handle_connected(struct irc_conn *bot, char *text);
void handle_chan_privmsg(struct irc_conn *bot, char *user, char *chan, char *text);
void handle_self_privmsg(struct irc_conn *bot, char *user, char *text);
void handle_join(struct irc_conn *bot, char *user, char *chan);

#endif
