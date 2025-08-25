/**
 * @file monitor.c
 * @brief This file contains the implementation of the functions that manage the monitor.
 */
#include "monitor.h"

/**
 * @brief The PID of the monitor.
 */
pid_t monitor_pid = -1; // PID del monitor

/**
 * @brief This function starts the Prometheus monitor.
 */
void start_monitor()
{
    if (monitor_pid > 0)
    {
        printf("El monitor ya está en ejecución con PID %d\n", monitor_pid);
        return;
    }

    // Crear un proceso hijo para el monitor
    monitor_pid = fork();
    if (monitor_pid == 0)
    {
        // Este es el proceso hijo
        execl("bin/metrics", "bin/metrics", NULL); // Ejecutar el programa de monitoreo
        perror("Error al iniciar el monitor");     // Imprimir un mensaje de error si execl() falla
        exit(EXIT_FAILURE);
    }
    else if (monitor_pid < 0)
    {
        perror("Error al crear el proceso de monitor");
    }
    else
    {
        printf("Monitor iniciado con PID %d\n", monitor_pid);
    }
}

/**
 * @brief This function stops the Prometheus monitor.
 */
void stop_monitor()
{
    if (monitor_pid <= 0)
    {
        printf("El monitor no está en ejecución\n");
        return;
    }

    if (kill(monitor_pid, SIGTERM) == 0) // Envia la señal SIGTERM al monitor
    {
        printf("Monitor detenido con éxito\n");
        monitor_pid = -1;
    }
    else
    {
        perror("Error al detener el monitor");
    }
}

/**
 * @brief This function some metrics from the Prometheus monitor.
 */
void status_monitor(void)
{
    // Cargar "settings.json"
    cJSON* settings = cargar_settings("../settings.json");

    // Procesar la FIFO con las configuraciones cargadas
    printf("Cargando estadisticas...\n");
    procesar_fifo("/tmp/monitor_pipe", settings);

    // Liberar memoria usada por cJSON
    cJSON_Delete(settings);
}

/**
 * @brief This function loads the settings from a JSON file.
 */
cJSON* cargar_settings(const char* settings_path)
{
    FILE* settings_file = fopen(settings_path, "r");

    if (!settings_file)
    {
        perror("Error al abrir el path brindado");
        exit(EXIT_FAILURE);
    }

    fseek(settings_file, 0, SEEK_END);
    long unsigned int length = (long unsigned)ftell(settings_file);
    fseek(settings_file, 0, SEEK_SET);

    length = length + 1;
    char* content = (char*)malloc(length);
    if (!content)
    {
        perror("Error al asignar memoria para settings.json");
        fclose(settings_file);
        exit(EXIT_FAILURE);
    }

    fread(content, 1, length, settings_file);
    content[length] = '\0'; // Asegurarse de que la cadena termine en NULL
    fclose(settings_file);

    // Parsear el JSON
    cJSON* settings_json = cJSON_Parse(content);
    free(content);

    if (!settings_json)
    {
        fprintf(stderr, "Error al parsear settings.json: %s\n", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }

    return settings_json;
}

/**
 * @brief This function processes the FIFO and prints the metrics.
 */
void procesar_fifo(const char* fifo_path, cJSON* settings)
{
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1)
    {
        perror("Error al abrir la FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];

    while (1)
    {
        ssize_t bytes_read = read(fifo_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';

            cJSON* metricas = cJSON_Parse(buffer);
            if (!metricas)
            {
                fprintf(stderr, "Error al parsear JSON de la FIFO: %s\n", cJSON_GetErrorPtr());
                continue;
            }

            cJSON* filtrado = filtrar_metricas(metricas, settings);
            imprimir_metricas(filtrado);

            cJSON_Delete(metricas);
            cJSON_Delete(filtrado);
        }
        else if (bytes_read == 0)
        {
            break;
        }
        else
        {
            perror("Error al leer de la FIFO");
            break;
        }
    }

    close(fifo_fd);
}

/**
 * @brief This function filters the metrics according to the settings.
 */
cJSON* filtrar_metricas(cJSON* metricas, cJSON* settings)
{
    cJSON* filtrado = cJSON_CreateObject();

    // Verificar qué métricas se deben incluir según los flags en settings.json
    if (cJSON_IsTrue(cJSON_GetObjectItem(settings, "collect_cpu")))
    {
        cJSON_AddItemToObject(filtrado, "cpu_usage_percentage",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "cpu_usage_percentage"), 1));
    }

    if (cJSON_IsTrue(cJSON_GetObjectItem(settings, "collect_memory")))
    {
        cJSON_AddItemToObject(filtrado, "memory_usage_percentage",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "memory_usage_percentage"), 1));
    }

    if (cJSON_IsTrue(cJSON_GetObjectItem(settings, "collect_disk")))
    {
        cJSON_AddItemToObject(filtrado, "disk_reads", cJSON_Duplicate(cJSON_GetObjectItem(metricas, "disk_reads"), 1));
        cJSON_AddItemToObject(filtrado, "disk_writes",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "disk_writes"), 1));
        cJSON_AddItemToObject(filtrado, "disk_read_time_seconds",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "disk_read_time_seconds"), 1));
        cJSON_AddItemToObject(filtrado, "disk_write_time_seconds",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "disk_write_time_seconds"), 1));
    }

    if (cJSON_IsTrue(cJSON_GetObjectItem(settings, "collect_network")))
    {
        cJSON_AddItemToObject(filtrado, "network_bandwidth_rx",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "network_bandwidth_rx"), 1));
        cJSON_AddItemToObject(filtrado, "network_bandwidth_tx",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "network_bandwidth_tx"), 1));
        cJSON_AddItemToObject(filtrado, "network_packet_ratio",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "network_packet_ratio"), 1));
    }

    if (cJSON_IsTrue(cJSON_GetObjectItem(settings, "collect_process")))
    {
        cJSON_AddItemToObject(filtrado, "running_processes_count",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "running_processes_count"), 1));
        cJSON_AddItemToObject(filtrado, "context_switches_total",
                              cJSON_Duplicate(cJSON_GetObjectItem(metricas, "context_switches_total"), 1));
    }

    return filtrado;
}

/**
 * @brief This function prints the metrics in a pretty way.
 */
void imprimir_metricas(cJSON* filtrado)
{
    printf(COLOR_TITLE "=== Métricas del Sistema ===\n" COLOR_RESET);

    cJSON* item;
    cJSON_ArrayForEach(item, filtrado)
    {
        const char* key = item->string;
        const char* value = cJSON_PrintUnformatted(item);
        printf(COLOR_KEY "%-25s: " COLOR_VALUE "%s\n" COLOR_RESET, key, value);
        free((void*)value);
    }
    printf("\n");
}
