#ifndef BATCH_FILE_H
#define BATCH_FILE_H

#include "commands.h"
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_BUFFER_SIZE 1024
/**
 * @brief Ejecuta los comandos desde un archivo.
 * @param filename El nombre del archivo.
 * @details Esta función lee el archivo línea por línea y ejecuta cada línea como un comando.
 * Los comandos son ejecutados por la función execute_command.
 */
void execute_batch_file(const char* filename);

#endif // BATCH_FILE_H
