#ifndef MODULE_H
#define MODULE_H

#include "irc.h"
#include "events.h"

#ifdef _WIN32
#include <windows.h>
#endif

struct module {
    char *name;
    char *author;
    char *version;
    char *description;

    char *fname;

#ifdef _WIN32
    HMODULE handle;
    FARPROC init;
    FARPROC unload;
#else
    void *handle;
    void (*init)();
    void (*unload)();
#endif
};

struct mods {
    int count;
    struct module *modules;
};

void init_mods();
void load_module(struct irc_conn *bot, char *where, char *stype, char *file);
void unload_module(struct irc_conn *bot, char *where, char *file);

#endif
