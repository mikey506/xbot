#include "irc.h"
#include "events.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data);

void fetch_metadata(struct irc_conn *bot, const char *url, const char *chan);

void handle_privmsg(struct irc_conn *bot, char *user, char *host, char *chan, const char *text);

void mod_init();

void mod_unload();

void fetch_metadata(struct irc_conn *bot, const char *url, const char *chan)
{
    CURL *curl;
    CURLcode res;
    char title[512] = "";
    char description[512] = "";

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the URL to fetch metadata from
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set up callbacks to receive title and description
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, title);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, description);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            // Format the message with retrieved metadata
            char msg[1024];
            snprintf(msg, sizeof(msg), "%s - %s - ( %s )", title, description, url);

            // Send the message to the channel
            irc_privmsg(bot, (char *)chan, msg);
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data)
{
    // Concatenate the received data
    strcat(data, (char *)ptr);
    return size * nmemb;
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
