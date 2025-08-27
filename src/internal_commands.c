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

/**
 * @brief Función auxiliar para verificar si un archivo tiene la extensión deseada
 */
static int has_extension(const char* filename, const char* extension)
{
    if (filename == NULL || extension == NULL)
        return 0;
    
    size_t filename_len = strlen(filename);
    size_t ext_len = strlen(extension);
    
    if (filename_len < ext_len)
        return 0;
        
    return strcmp(filename + filename_len - ext_len, extension) == 0;
}

/**
 * @brief Función auxiliar para buscar archivos recursivamente
 */
static void search_files_recursive(const char* directory, const char* extension)
{
    DIR* dir;
    struct dirent* entry;
    struct stat file_stat;
    char full_path[1024];
    
    dir = opendir(directory);
    if (dir == NULL)
    {
        printf("Error: No se puede abrir el directorio %s\n", directory);
        return;
    }
    
    while ((entry = readdir(dir)) != NULL)
    {
        // Saltar . y ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
            
        snprintf(full_path, sizeof(full_path), "%s/%s", directory, entry->d_name);
        
        if (stat(full_path, &file_stat) == 0)
        {
            if (S_ISDIR(file_stat.st_mode))
            {
                // Es un directorio, buscar recursivamente
                search_files_recursive(full_path, extension);
            }
            else if (S_ISREG(file_stat.st_mode))
            {
                // Es un archivo regular, verificar extensión
                if (has_extension(entry->d_name, extension))
                {
                    printf("Archivo de configuración encontrado: %s\n", full_path);
                    
                    // Mostrar contenido del archivo
                    FILE* file = fopen(full_path, "r");
                    if (file != NULL)
                    {
                        printf("Contenido de %s:\n", full_path);
                        char line[256];
                        while (fgets(line, sizeof(line), file) != NULL)
                        {
                            printf("%s", line);
                        }
                        fclose(file);
                        printf("\n");
                    }
                    else
                    {
                        printf("Error: No se puede leer el archivo %s\n", full_path);
                    }
                }
            }
        }
    }
    
    closedir(dir);
}

/**
 * @brief Implementación del comando `find_config`.
 */
void find_config_command(const char* args[])
{
    const char* directory = ".";  // Directorio actual por defecto
    const char* extension = ".config";  // Extensión por defecto
    
    // Si se proporciona un directorio
    if (args[1] != NULL)
    {
        directory = args[1];
    }
    
    // Si se proporciona una extensión
    if (args[2] != NULL)
    {
        extension = args[2];
    }
    
    printf("Explorando el directorio: %s en busca de archivos '%s'\n", directory, extension);
    
    search_files_recursive(directory, extension);
}
