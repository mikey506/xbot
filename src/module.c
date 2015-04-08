#include "irc.h"
#include "util.h"
#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

void load_module(struct irc_conn *bot, char *where, char *stype, char *file)
{
	void *handle;
	void (*mod_init)();
	char *error = (char *)malloc(sizeof(char *)*1024);


	handle = dlopen(file, RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
	if (!handle)
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

	*(void **)(&mod_init) = dlsym(handle, "mod_init");

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

	(*mod_init)();

	dlclose(handle);

	if (strcmp("runtime", stype))
	{
		irc_privmsg(bot, where, "Module '%s' loaded.", file);
	}
	else
	{
		printf("Module '%s' loaded.\n", file);
	}
	free(error);
}