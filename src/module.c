#include "irc.h"
#include "util.h"
#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void load_module(struct irc_conn *bot, char *where, int stype, char *file)
{
	void *handle;
	void (*mod_init)();
	char *error = (char *)malloc(sizeof(char *)*1024);


	handle = dlopen(file, RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
	if (!handle)
	{
		sprintf(error, "Error: %s", dlerror());

		if (stype == 3)
		{
			eprint("%s\n", error);
			return;
		}
		else if (stype == PRIVMSG_CHAN)
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

		if (stype == 3)
		{
			return;
		}
		else if (stype == PRIVMSG_CHAN)
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

	if (stype != 3)
	{
		irc_privmsg(bot, where, "Module '%s' loaded.", file);
	}
	else
	{
		printf("Module '%s' loaded.\n", file);
	}
	free(error);
}