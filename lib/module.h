#ifndef MODULE_H
#define MODULE_H

#include "irc.h"

struct module {
    char *name;
    //event_handler handlers[50];
};

typedef struct module module;

MY_API void load_module(struct irc_conn *bot, char *where, char *stype, char *file);


#endif
