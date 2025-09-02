#include "commands.h"

/**
 * @brief The maximum length of a command.
 *
 * This value is used to allocate memory for the command string.
 * It is also used to allocate memory for the arguments array.
 *
 */

#define MAX_COMMAND_LENGTH 1024
#define MAX_NUM_PIPES 10

static int job_id = 1;

pid_t foreground_pid;

extern pid_t monitor_pid;
prom_counter_t* my_counter = NULL;

/*
 * @brief This function executes the command.
 *
 * @param cmd the command to execute
 */
void execute_command(const char* cmd){

    if (cmd == NULL)
    {
        fprintf(stderr, "Error: comando nulo\n");
        return;
    }

    char* args[MAX_COMMAND_LENGTH];
    int i = 0;
    int background = 0;
    int input_redirect = 0;
    int output_redirect = 0;
    char* input_file = NULL;
    char* output_file = NULL;

    // Crear una copia del comando porque strtok modifica la cadena original
    char cmd_copy[MAX_COMMAND_LENGTH];
    strncpy(cmd_copy, cmd, MAX_COMMAND_LENGTH);
    cmd_copy[MAX_COMMAND_LENGTH - 1] = '\0'; // Asegurarse de que la cadena esté terminada en NULL

    // Verificar si el comando termina con '&'
    size_t len = strlen(cmd_copy);
    if (len > 0 && cmd_copy[len - 1] == '&')
    {
        background = 1;
        cmd_copy[len - 1] = '\0'; // Eliminar el '&' del comando
    }

    // Dividir el comando en múltiples comandos separados por '|'
    char* commands[MAX_NUM_PIPES + 1];
    int num_commands = 0;
    char* tokenp = strtok(cmd_copy, "|");
    while (tokenp != NULL && num_commands < MAX_NUM_PIPES + 1)
    {
        commands[num_commands++] = tokenp;
        tokenp = strtok(NULL, "|");
    }

    // Crear pipes
    int pipes[MAX_NUM_PIPES][2];
    for (int j = 0; j < num_commands - 1; j++)
    {
        if (pipe(pipes[j]) == -1)
        {
            perror("pipe error");
            return;
        }
    }

    // Tokenizar el comando en base a los espacios
    char* token = strtok(commands[0], " ");
    while (token != NULL && i < MAX_COMMAND_LENGTH)
    {
        if (strcmp(token, "<") == 0)
        {
            input_redirect = 1;
            token = strtok(NULL, " ");
            input_file = token;
        }
        else if (strcmp(token, ">") == 0)
        {
            output_redirect = 1;
            token = strtok(NULL, " ");
            output_file = token;
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Terminar el arreglo con NULL para las funciones que lo requieran

    // Verificar que args[0] no sea NULL antes de acceder a él
    if (args[0] == NULL)
    {
        fprintf(stderr, "Error: comando vacío\n");
        return;
    }

    int saved_stdout = dup(STDOUT_FILENO); // Guardar salida estándar
    int saved_stdin = dup(STDIN_FILENO);   // Guardar entrada estándar

    //-------COMANDOS INTERNOS-------//

    if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "clr") == 0 || strcmp(args[0], "echo") == 0 ||
        strcmp(args[0], "quit") == 0 || strcmp(args[0], "find_config") == 0 || 
        strcmp(args[0], "start_monitor") == 0 || strcmp(args[0], "stop_monitor") == 0 ||
        strcmp(args[0], "status_monitor") == 0){
            
            if (background){
                pid_t pid = fork();
        
                if (pid < 0){
                    perror("fork error");
                }else if (pid == 0){
                    // Proceso hijo
                    // Redirecciones en el hijo
                    if (input_redirect){
                        int fd_in = open(input_file, O_RDONLY);
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                    }
                    if (output_redirect)
                    {
                        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                    }
                    execvp(args[0], args);

                    // Ejecutar el comando interno
                    if (strcmp(args[0], "cd") == 0)
                    {
                        if (args[1] != NULL)
                        {
                            change_directory(args[1]);
                        }
                        else
                        {
                            change_directory(NULL); // Cambiar al directorio home
                        }
                    }
                    else if (strcmp(args[0], "clr") == 0)
                    {
                        clear_screen();
                    }
                    else if (strcmp(args[0], "echo") == 0)
                    {
                        echo_command((const char**)args);
                    }
                    else if (strcmp(args[0], "quit") == 0)
                    {
                        quit_shell();
                    }
                    else if (strcmp(args[0], "find_config") == 0)
                    {
                        find_config_command((const char**)args);
                    }
                    else if (strcmp(args[0], "start_monitor") == 0)
                    {
                        start_monitor();
                        return;
                    }
                    else if (strcmp(args[0], "stop_monitor") == 0)
                    {
                        stop_monitor();
                        return;
                    }
                    else if (strcmp(args[0], "status_monitor") == 0)
                    {
                        status_monitor(); // Llamar a la función adaptada
                        return;
                    }

                    exit(EXIT_SUCCESS);
                }
                else
                {
                    printf("[%d] %d\n", job_id++, pid);
                }
            }else{
                if (input_redirect){
                    int fd_in = open(input_file, O_RDONLY);
                    if (fd_in < 0)
                    {
                        perror("open input file");
                    }
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                }
                if (output_redirect){
                    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out < 0)
                    {
                        perror("open output file");
                    }
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                }

                if (strcmp(args[0], "cd") == 0)
                {
                    if (args[1] != NULL)
                    {
                        change_directory(args[1]);
                    }
                    else
                    {
                        change_directory(NULL); // Cambiar al directorio home
                    }
                }
                else if (strcmp(args[0], "clr") == 0)
                {
                    clear_screen();
                }
                else if (strcmp(args[0], "echo") == 0)
                {
                    echo_command((const char**)args);
                }
                else if (strcmp(args[0], "quit") == 0)
                {
                    quit_shell();
                }
                else if (strcmp(args[0], "find_config") == 0)
                {
                    find_config_command((const char**)args);
                }
                else if (strcmp(args[0], "start_monitor") == 0)
                {
                    start_monitor();
                    return;
                }
                else if (strcmp(args[0], "stop_monitor") == 0)
                {
                    stop_monitor();
                    return;
                }
                else if (strcmp(args[0], "status_monitor") == 0)
                {

                    status_monitor(); // Llamar a la función adaptada
                    return;
                }
                // Restaura la salida estándar
                if (output_redirect)
                {
                    fflush(stdout);
                    dup2(saved_stdout, STDOUT_FILENO);
                    close(saved_stdout);
                }
                if (input_redirect)
                {
                    dup2(saved_stdin, STDIN_FILENO);
                    close(saved_stdin);
                }
                return;
            }

            // Ejecutar cada comando en un proceso hijo
            for (int j = 0; j < num_commands; j++)
            {
                pid_t pid = fork();
                if (pid < 0)
                {
                    perror("fork error");
                    return;
                }
                else if (pid == 0)
                {
                    // Proceso hijo
                    if (j > 0)
                    {
                        // Redirigir la entrada estándar al extremo de lectura del pipe anterior
                        dup2(pipes[j - 1][0], STDIN_FILENO);
                    }
                    if (j < num_commands - 1)
                    {
                        // Redirigir la salida estándar al extremo de escritura del pipe actual
                        dup2(pipes[j][1], STDOUT_FILENO);
                    }

                    // Cerrar todos los pipes en el proceso hijo
                    for (int k = 0; k < num_commands - 1; k++)
                    {
                        close(pipes[k][0]);
                        close(pipes[k][1]);
                    }

                    // Tokenizar el comando en base a los espacios
                    char* cmd_args[MAX_COMMAND_LENGTH];
                    int k = 0;
                    char* cmd_token = strtok(commands[j], " ");
                    while (cmd_token != NULL && k < MAX_COMMAND_LENGTH)
                    {
                        cmd_args[k++] = cmd_token;
                        cmd_token = strtok(NULL, " ");
                    }
                    cmd_args[k] = NULL; // Terminar el arreglo con NULL para las funciones que lo requieran

                    // Redirigir la entrada y salida estándar si es necesario
                    if (input_redirect)
                    {
                        int fd_in = open(input_file, O_RDONLY);
                        if (fd_in < 0)
                        {
                            perror("open input file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                    }
                    if (output_redirect)
                    {
                        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0)
                        {
                            perror("open output file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                    }

                    // Ejecutar el comando
                    execvp(cmd_args[0], cmd_args);
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }
            }

            // Cerrar todos los pipes en el proceso padre
            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Esperar a que todos los procesos hijos terminen
            for (int j = 0; j < num_commands; j++)
            {
                int status;
                wait(&status);
            }

            // Restaurar la salida estándar si se redirigió
            if (output_redirect)
            {
                fflush(stdout);
                dup2(STDOUT_FILENO, fileno(stdout));
            }
        }else{
            //---------------COMANDOS EXTERNOS---------------//
            if (background){
                pid_t pid = fork();
                if (pid < 0)
                {
                    perror("fork error");
                }
                else if (pid == 0)
                {
                    // Proceso hijo para comando en background
                    if (input_redirect)
                    {
                        int fd_in = open(input_file, O_RDONLY);
                        if (fd_in < 0)
                        {
                            perror("open input file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                    }
                    if (output_redirect)
                    {
                        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0)
                        {
                            perror("open output file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                    }
                    
                    execvp(args[0], args);
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("[%d] %d\n", job_id++, pid);
                }
            }else{

                // Ejecutar comando externo en foreground
                pid_t pid = fork();
                if (pid < 0)
                {
                    perror("fork error");
                    return;
                }
                else if (pid == 0)
                {
                    // Proceso hijo
                    if (input_redirect)
                    {
                        int fd_in = open(input_file, O_RDONLY);
                        if (fd_in < 0)
                        {
                            perror("open input file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                    }
                    if (output_redirect)
                    {
                        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0)
                        {
                            perror("open output file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                    }
                    
                    execvp(args[0], args);
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    // Proceso padre espera al hijo
                    int status;
                    foreground_pid = pid;
                    waitpid(pid, &status, 0);
                    foreground_pid = 0;
                }
            }
        }
        // Restaurar descriptores de archivo
        if (output_redirect)
        {
            fflush(stdout);
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        if (input_redirect)
        {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
}
