#ifndef CONFIG_H
#define CONFIG_H

#include "irc.h"

struct irc_conn read_config(struct irc_conn bot, char *file);

#endif