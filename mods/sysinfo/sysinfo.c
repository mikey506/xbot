#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For the sysinfo function
#include <sys/sysinfo.h> // For the sysinfo function

#define CONFIG_FILE "xbot.cfg" // Update the configuration file name

char owner_nick[256] = ""; // Variable to store the owner's nickname

// Function to read the owner's nickname from the config file
void read_owner_nick() {
    // Construct the full path to the configuration file
    char config_path[256];
    snprintf(config_path, sizeof(config_path), "./%s", CONFIG_FILE);

    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }

    printf("Opened config file successfully.\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[256], value[256];
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            printf("Read line: %s\n", line);
            if (strcmp(key, "admin") == 0) {
                strcpy(owner_nick, value);
                printf("Owner nickname set: %s\n", owner_nick);
                break;
            }
        }
    }

    fclose(file);
}

MY_API void show_sysinfo(struct irc_conn *bot, char *user, char *host, char *chan, const char *text)
{
    printf("show_sysinfo called.\n");

    // Check if the user issuing the command is the owner
    printf("User: %s, Owner Nick: %s\n", user, owner_nick);
    if (strcmp(user, owner_nick) != 0) {
        printf("User is not the owner.\n");
        return;
    }

    // Check if the command is "!sysinfo"
    printf("Text: %s\n", text);
    if (strcmp(text, "!sysinfo") != 0) {
        printf("Not the !sysinfo command.\n");
        return;
    }

    struct sysinfo si;
    if (sysinfo(&si) != 0)
    {
        irc_privmsg(bot, chan, "Failed to retrieve system information.");
        return;
    }

    // Convert system uptime to days, hours, and minutes
    int days = si.uptime / (24 * 3600);
    int hours = (si.uptime % (24 * 3600)) / 3600;
    int minutes = (si.uptime % 3600) / 60;

    char message[512];
    snprintf(message, sizeof(message), "System Uptime: %d days, %d hours, %d minutes | Total RAM: %lu bytes | Free RAM: %lu bytes | Number of processes: %d",
             days, hours, minutes, si.totalram, si.freeram, si.procs);

    irc_privmsg(bot, chan, message);
}

MY_API void mod_init()
{
    register_module("sysinfo", "Your Name", "v1.0", "Show system information with !sysinfo command");
    read_owner_nick(); // Read owner's nickname from config file
    add_handler(PRIVMSG_CHAN, show_sysinfo);
}

MY_API void mod_unload()
{
    unregister_module("sysinfo");
    del_handler(PRIVMSG_CHAN, show_sysinfo);
}
