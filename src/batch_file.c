#include "batch_file.h"

/*
 * @brief This function executes the commands from a file.
 * @param filename the name of the file.
 */

void execute_batch_file(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[LINE_BUFFER_SIZE];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;
        execute_command(line);
    }

    fclose(file);
}
