#ifndef MODULE_H
#define MODULE_H

#include "irc.h"
#include "events.h"

struct module {
    char *name;
    char file[256];
    struct ev_handler *handlers;
};

MY_API void load_module(struct irc_conn *bot, char *where, char *stype, char *file);


#endif
