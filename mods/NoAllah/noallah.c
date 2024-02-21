#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
// Windows-specific code
#else
#include <unistd.h>
#endif

MY_API void up(struct irc_conn *bot, char *user, char *host, char *chan, char *text)
{
#ifdef _WIN32
    // Windows-specific code
#else
    printf("dbug up called: %s!%s %s\n", user, host, text);

    if (strstr(text, "Allah") != NULL)
    {
        // Kick and ban the user
        irc_kick(bot, chan, user, "Spamming forbidden content");
        irc_ban(bot, chan, host);
    }
#endif
}

MY_API void mod_init()
{
#ifdef _WIN32
    // Windows-specific initialization
#else
    register_module("uptime", "Aaron Blakely", "v0.1", "Uptime module");
    printf("installing up handler\n");
    add_handler(PRIVMSG_CHAN, up);
#endif
}

MY_API void mod_unload()
{
#ifdef _WIN32
    // Windows-specific cleanup
#else
    unregister_module("uptime");
    printf("unloading up handler\n");
    del_handler(PRIVMSG_CHAN, up);
#endif
}
