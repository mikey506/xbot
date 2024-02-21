#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For the sysinfo function
#include <sys/sysinfo.h> // For the sysinfo function

// Configuration file path
#define CONFIG_FILE_PATH "xbox.cfg"

// Maximum length for configuration values
#define MAX_VALUE_LENGTH 256

// Structure to hold configuration settings
struct Config {
    char admin[MAX_VALUE_LENGTH];
};

// Global configuration instance
struct Config config;

// Function to load configuration from file
int load_config() {
    FILE *file = fopen(CONFIG_FILE_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open configuration file.\n");
        return 0;
    }

    // Read lines from the file
    char line[MAX_VALUE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Find the 'admin' setting
        if (strstr(line, "admin = ") == line) {
            // Extract the admin value
            sscanf(line, "admin = \"%[^\"]\"", config.admin);
            fclose(file);
            return 1; // Configuration loaded successfully
        }
    }

    fclose(file);
    fprintf(stderr, "Error: 'admin' setting not found in configuration file.\n");
    return 0;
}

MY_API void show_sysinfo(struct irc_conn *bot, char *user, char *host, char *chan, const char *text)
{
    // Check if the user issuing the command is the owner
    if (strcmp(user, config.admin) != 0)
        return;

    // Check if the command is "!sysinfo"
    if (strcmp(text, "!sysinfo") != 0)
        return;

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
    // Load configuration
    if (!load_config()) {
        fprintf(stderr, "Error: Failed to load configuration.\n");
        return;
    }

    register_module("sysinfo", "Your Name", "v1.0", "Show system information with !sysinfo command");
    add_handler(PRIVMSG_CHAN, show_sysinfo);
}

MY_API void mod_unload()
{
    unregister_module("sysinfo");
    del_handler(PRIVMSG_CHAN, show_sysinfo);
}
