#ifndef SHELL_H
#define SHELL_H

#include "batch_file.h"
#include "commands.h"
#include "internal_commands.h"
#include "signals.h"
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * @brief This function displays the prompt.
 * @details This function displays the prompt with the user, hostname, and current working directory.
 */
void display_prompt(void);

#endif // SHELL_H
