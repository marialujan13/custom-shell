#ifndef COMMANDS_H
#define COMMANDS_H

#include "internal_commands.h"
#include "monitor.h"
#include <fcntl.h>
#include <microhttpd.h>
#include <prom.h>
#include <promhttp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * @brief This function executes the command.
 * @param cmd The command to execute
 *
 */

void execute_command(const char* cmd);

#endif // COMMANDS_H
