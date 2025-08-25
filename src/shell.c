
#include "shell.h"

#define HOSTNAME_BUFFER_SIZE 1024
#define CWD_BUFFER_SIZE 1024
/*
 * @brief This function displays the prompt.
 * @details This function displays the prompt with the user, hostname, and current working directory.
 */
void display_prompt(void)
{
    char hostname[HOSTNAME_BUFFER_SIZE];
    char cwd[CWD_BUFFER_SIZE];
    struct passwd* pw = getpwuid(getuid());

    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    printf("She'll: %s@%s:%s$ ", pw->pw_name, hostname, cwd);
}

/*
 * @brief This function manages the signals.
 * @details This function manages the signals to avoid the shell to be terminated by the signals.
 */
int main(int argc, char* argv[])
{

    if (argc == 2)
    {
        // Leer y ejecutar comandos desde el archivo de comandos
        execute_batch_file(argv[1]);
    }
    else if (argc == 1)
    {
        // Mostrar el prompt y esperar comandos del usuario
        while (1)
        {
            // manage_signals();
            display_prompt();
            manage_signals();
            char input[1024];
            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            input[strcspn(input, "\n")] = 0;

            execute_command(input);
        }
    }
    else
    {
        fprintf(stderr, "Uso: %s [archivo_de_comandos]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
