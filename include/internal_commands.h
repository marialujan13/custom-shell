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
#include <dirent.h>

/**
 * @brief This function changes the current directory.
 * @param path The path to change to.
 */
void change_directory(char* path);

/**
 * @brief This function clears the screen.
 * @details This function uses the `clear` command to clear the screen.
 */
void clear_screen(void);

/**
 * @brief This function echoes a message.
 * @param message The message to echo.
 */
void echo_command(const char* message[]);

/**
 * @brief This function quits the shell.
 * @details This function sends a signal to the shell to quit.
 */
void quit_shell(void);

/**
 * @brief This function searches for configuration files in a directory.
 * @param args Array containing the directory path and optionally the file extension
 * @details Searches recursively for configuration files with specified extension
 */
void find_config_command(const char* args[]);
