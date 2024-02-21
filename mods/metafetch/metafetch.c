#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fetch_metadata(struct irc_conn *bot, const char *url, const char *chan);

void handle_privmsg(struct irc_conn *bot, char *user, char *host, char *chan, const char *text);

void mod_init();

void mod_unload();

void fetch_metadata(struct irc_conn *bot, const char *url, const char *chan)
{
    FILE *fp;
    char buffer[1024];
    char title[512] = "";
    char description[512] = "";

    // Execute curl command and read output
    fp = popen("curl -sL \"$1\" | grep -oP '(?<=<title>)(.*)(?=</title>)'", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run curl command\n");
        return;
    }

    // Read the title from the command output
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strncpy(title, buffer, sizeof(title));
        title[strcspn(title, "\r\n")] = '\0'; // Remove trailing newline
    }

    pclose(fp);

    // Construct the message
    char msg[1024];
    snprintf(msg, sizeof(msg), "%s - %s - ( %s )", title, description, url);

    // Send the message to the channel
    irc_privmsg(bot, (char *)chan, msg);
}

void handle_privmsg(struct irc_conn *bot, char *user, char *host, char *chan, const char *text)
{
    // Check if the text contains a URL
    char *url_start = strstr(text, "http://");
    if (url_start || (url_start = strstr(text, "https://"))) {
        char *url_end = strpbrk(url_start, " \t\r\n");
        if (url_end) {
            // Extract the URL
            char url[512];
            strncpy(url, url_start, url_end - url_start);
            url[url_end - url_start] = '\0';

            // Fetch metadata and message the channel
            fetch_metadata(bot, url, chan);
        }
    }
}

void mod_init()
{
    // Register the module
    register_module("metafetch", "Your Name", "v1.0", "Fetches metadata from URL within channel that is msged to channel");
    // Add handler for PRIVMSG events
    add_handler(PRIVMSG_CHAN, handle_privmsg);
}

void mod_unload()
{
    // Unregister the module
    unregister_module("metafetch");
    // Remove handler for PRIVMSG events
    del_handler(PRIVMSG_CHAN, handle_privmsg);
}
