#include "irc.h"
#include "util.h"
#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void load_module(struct irc_conn *bot, char *where, int stype, char *file)
{
	void *handle;
	void (*mod_init)(struct irc_conn *bot, char *where, void (*ah)());
	char *error = (char *)malloc(sizeof(char *)*1024);


	handle = dlopen(file, RTLD_NOW | RTLD_GLOBAL);
	if (!handle)
	{
		sprintf(error, "Error: %s", dlerror());

		if (stype == PRIVMSG_CHAN)
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

	*(void **)(&mod_init) = dlsym(handle, "mod_init");

	if ((error = dlerror()) != NULL)
	{
		//sprintf(error, "Error: %s", error);
		eprint("Error: %s\n", error);
		if (stype == PRIVMSG_CHAN)
		{
			irc_privmsg(bot, where, error);
		}
		else
		{
			irc_notice(bot, where, error);
		}
	}

	(*mod_init)(bot, where);

	dlclose(handle);

	irc_privmsg(bot, where, "Module '%s' loaded.", file);

	free(error);
}