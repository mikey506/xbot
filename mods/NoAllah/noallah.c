#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MY_API void kick_ban_on_allah(struct irc_conn *bot, char *user, char *host, char *chan, const char *text)
{
    // Check if the text contains the word "Allah"
    if (strstr(text, "Allah") != NULL)
    {
        // Construct the raw IRC commands to kick and ban the user
        char kick_command[512];
        char ban_command[512];

        snprintf(kick_command, sizeof(kick_command), "KICK %s %s :Spamming forbidden content", chan, user);
        snprintf(ban_command, sizeof(ban_command), "MODE %s +b %s", chan, host);

        // Send the raw IRC commands
        irc_raw(bot, kick_command);
        irc_raw(bot, ban_command);

        printf("%s was kicked and banned for mentioning Allah\n", user);
    }
}

MY_API void mod_init()
{
    register_module("kick_ban_allah", "Your Name", "v1.0", "Kick and ban users who mention Allah");
    add_handler(PRIVMSG_CHAN, kick_ban_on_allah);
}

MY_API void mod_unload()
{
    unregister_module("kick_ban_allah");
    del_handler(PRIVMSG_CHAN, kick_ban_on_allah);
}

