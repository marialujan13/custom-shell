/**
 * @file monitor.h
 * @brief This file contains the declaration of the functions that handle the Prometheus monitor.
 */

#include <cjson/cJSON.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief The buffer size for reading the FIFO.
 */
#define BUFFER_SIZE 4096

/**
 * @brief Color codes for the terminal.
 * @details These color codes are used to print the metrics in a pretty way.
 */
#define COLOR_RESET "\033[0m"

/**
 * @brief Color codes for the terminal.
 * @details These color codes are used to print the metrics in a pretty way.
 */
#define COLOR_TITLE "\033[38;5;197m" // Rosa

/**
 * @brief Color codes for the terminal.
 * @details These color codes are used to print the metrics in a pretty way.
 */
#define COLOR_KEY "\033[1;32m" // Verde

/**
 * @brief Color codes for the terminal.
 * @details These color codes are used to print the metrics in a pretty way.
 */
#define COLOR_VALUE "\033[1;33m" // Amarillo

/**
 * @brief This function starts the monitor.
 */
void start_monitor(void);

/**
 * @brief This function stops the monitor.
 */
void stop_monitor(void);

/**
 * @brief This function shows the status of the monitor.
 */
void status_monitor(void);

/**
 * @brief This function loads the settings from a JSON file.
 * @param settings_path The path to the settings file.
 * @return The settings loaded from the file.
 */
cJSON* cargar_settings(const char* settings_path);

/**
 * @brief This function filters the metrics according to the settings.
 * @param metricas The metrics to filter.
 * @param settings The settings to filter the metrics.
 * @return The filtered metrics.
 */
cJSON* filtrar_metricas(cJSON* metricas, cJSON* settings);

/**
 * @brief This function processes the FIFO and prints the metrics.
 * @param fifo_path The path to the FIFO.
 * @param settings The settings to filter the metrics.
 */
void procesar_fifo(const char* fifo_path, cJSON* settings);

/**
 * @brief This function prints the metrics in a pretty way.
 * @param filtrado The filtered metrics to print.
 */
void imprimir_metricas(cJSON* filtrado);
