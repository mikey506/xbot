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

struct ev_handler
{
    int id;
    void *handler;
};

struct handler
{
    char *type;
    int count;
    struct ev_handler *evhands;
};


void init_events();
MY_API int add_handler(char *type, void *handler);
MY_API void del_handler(char *type, void *handler);
void fire_handler(struct irc_conn *bot, char *type, ...);

#endif
