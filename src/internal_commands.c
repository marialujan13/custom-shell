#include "internal_commands.h"

/**
 * @brief Implementación del comando `echo`.
 */
void echo_command(const char* message[])
{
    int i = 1;
    while (message[i] != NULL)
    {
        if (strchr(message[i], '$') != NULL)
        {
            char* var = strchr(message[i], '$') + 1;
            if (getenv(var) != NULL)
            {
                fprintf(stdout, "%s ", getenv(var));
            }
            else
            {
                fprintf(stdout, "Error con la variable de entorno ");
            }
        }
        else
        {
            fprintf(stdout, "%s ", message[i]);
        }
        i++;
    }
    fprintf(stdout, "\n");
}

/**
 * @brief Implementación del comando `cd`.
 */
void change_directory(char* path)
{
    char* old_pwd = getenv("PWD");
    char* new_pwd;

    if (path == NULL || strcmp(path, "~") == 0)
    {
        // Si no se pasa un directorio o es "~", vamos al directorio home
        new_pwd = getenv("HOME");
    }
    else if (strcmp(path, "-") == 0)
    {
        // Si el comando es "cd -", volvemos al último directorio
        new_pwd = getenv("OLDPWD");
    }
    else
    {
        // Intentamos cambiar al directorio indicado
        new_pwd = path;
    }

    if (chdir(new_pwd) == 0)
    {
        // Actualizamos las variables de entorno
        setenv("OLDPWD", old_pwd, 1);
        setenv("PWD", new_pwd, 1);
    }
    else
    {
        perror("cd");
    }
}

/**
 * @brief Implementación del comando `clear`.
 */
void clear_screen()
{
    if (system("clear") == -1)
    {
        perror("clear");
    }
}

/**
 * @brief Implementación del comando `exit`.
 */
void quit_shell()
{
    printf("Saliendo de la shell...\n");
    exit(0);
}
