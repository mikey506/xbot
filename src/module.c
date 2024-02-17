#include "irc.h"
#include "util.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

struct mods *mods;

void init_mods()
{
    mods = calloc(1, sizeof(struct mods));
    mods->count = 0;
    mods->modules = calloc(512, sizeof(struct module));
}

void load_module(struct irc_conn *bot, char *where, char *stype, char *file)
{
	char *error = (char *)malloc(sizeof(char *)*1024);
    mods->modules[mods->count].fname = file;
#ifdef _WIN32

    mods->modules[mods->count].handle = LoadLibrary(file);
	if (mods->modules[mods->count].handle == NULL)
	{
		sprintf(error, "Error loading %s\n", file);

		if (strcmp("runtime", stype))
		{
			eprint("%s\n", error);
			return;
		}
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }

        return;
    }

	mods->modules[mods->count].init = GetProcAddress(mods->modules[mods->count].handle, "mod_init");
	if (mods->modules[mods->count].init  == NULL)
	{
		DWORD err = GetLastError();

		sprintf(error, "Error loading mod_init() pointer for %s: %lu", file, err);
		eprint("Error: %s\n", error);

        if (strcmp("runtime", stype))
        {
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }

		return;
    }

	((void(*)(void))mods->modules[mods->count].init)();

    mods->modules[mods->count].unload = GetProcAddress(mods->modules[mods->count].handle, "mod_unload");
    if (mods->modules[mods->count].unload  == NULL)
    {
        DWORD err = GetLastError();

        sprintf(error, "Error loading mod_unload() pointer for %s: %lu", file, err);
        eprint("Error: %s\n", error);

        if (strcmp("runtime", stype))
        {
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }

        return;
    }

	if (strcmp("runtime", stype))
    {
        irc_privmsg(bot, where, "Module '%s' loaded.", file);
    }
    else
    {
        printf("Module '%s' loaded.\n", file);
    }
    free(error);
#else
    void (*mod_init)();

    mods->modules[mods->count].handle = dlopen(file, RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    if (!mods->modules[mods->count].handle)
    {
        sprintf(error, "Error: %s", dlerror());

        if (strcmp("runtime", stype))
        {
            eprint("%s\n", error);
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }

        return;
    }

    dlerror();

    *(void **)(&mods->modules[mods->count].init) = dlsym(mods->modules[mods->count].handle , "mod_init");

    if ((error = dlerror()) != NULL)
    {
        //sprintf(error, "Error: %s", error);
        eprint("Error: %s\n", error);

        if (strcmp("runtime", stype))
        {
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }
    }

    (*mods->modules[mods->count].init)();

    *(void **)(&mods->modules[mods->count].unload) = dlsym(mods->modules[mods->count].handle , "mod_unload");
    if ((error = dlerror()) != NULL)
    {
        //sprintf(error, "Error: %s", error);
        eprint("Error: %s\n", error);

        if (strcmp("runtime", stype))
        {
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }
    }

    if ((error = dlerror()) != NULL)
    {
        //sprintf(error, "Error: %s", error);
        eprint("Error: %s\n", error);

        if (strcmp("runtime", stype))
        {
            return;
        }
        else if (strcmp(PRIVMSG_CHAN, stype))
        {
            irc_privmsg(bot, where, error);
        }
        else
        {
            irc_notice(bot, where, error);
        }
    }


    if (strcmp("runtime", stype))
    {
        irc_privmsg(bot, where, "Module '%s' loaded.", file);
    }
    else
    {
        printf("Module '%s' loaded.\n", file);
    }
    free(error);
#endif

    mods->count++;
}

void unload_module(struct irc_conn *bot, char *where, char *file)
{
    int i;
    for (i = 0; i < mods->count; i++)
    {
        (*mods->modules[i].unload)();

        if (strcmp(mods->modules[i].fname, file) == 0)
        {
#ifdef _WIN32
            FreeLibrary(mods->modules[i].handle);
#else
            dlclose(mods->modules[i].handle);
#endif

            if (strcmp(PRIVMSG_CHAN, where))
            {
                irc_privmsg(bot, where, "Module '%s' unloaded.", file);
            }
            else
            {
                printf("Module '%s' unloaded.\n", file);
            }

            return;
        }
    }
}
