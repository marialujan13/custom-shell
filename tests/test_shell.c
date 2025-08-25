
#include "commands.h"
#include "monitor.h"
#include "shell.h"
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity/unity.h>

/**
 * @brief Variable global para indicar si el shell está en ejecución
 */
extern pid_t monitor_pid;

#define MONITOR_OFF -1

// Prototipos de funciones de prueba
void test_echo_command(void);
void test_change_directory(void);

// Funciones de configuración y limpieza
void setUp(void)
{
    // Configuración antes de cada prueba
}

void tearDown(void)
{
    // Limpieza después de cada prueba
}

/**
 * @brief Función principal de las pruebas
 *
 * Esta función llama a todas las pruebas unitarias.
 */
int main(void)
{
    UNITY_BEGIN(); // Inicia Unity

    // Llama a tus funciones de prueba
    RUN_TEST(test_echo_command);
    RUN_TEST(test_change_directory);

    return UNITY_END(); // Finaliza las pruebas y devuelve el resultado
}

// Prueba de la función echo_command
void test_echo_command(void)
{
    // Configurar un entorno de prueba
    setenv("TEST_VAR", "test_value", 1); // Establece una variable de entorno

    // Preparar entradas y salida esperada
    const char* input1[] = {"echo", "Hello,", "$TEST_VAR", "World!", NULL};
    char expected_output1[] = "Hello, test_value World! \n";

    const char* input2[] = {"echo", "No", "env", "$NOT_SET", NULL};
    char expected_output2[] = "No env Error con la variable de entorno \n";

    // Redirigir stdout para capturar la salida de la función
    char buffer[256];
    FILE* fp = fmemopen(buffer, sizeof(buffer), "w");
    if (fp == NULL)
    {
        TEST_FAIL_MESSAGE("Error al abrir flujo de memoria");
    }

    // Redirige stdout a nuestro flujo de memoria
    FILE* original_stdout = stdout;
    stdout = fp;

    // Ejecutar la función con el primer caso
    echo_command(input1);

    // Restaurar y cerrar stdout
    fflush(stdout);
    stdout = original_stdout;
    fclose(fp);

    // Verificar la salida capturada
    TEST_ASSERT_EQUAL_STRING(expected_output1, buffer);

    // Repetir para el segundo caso
    fp = fmemopen(buffer, sizeof(buffer), "w");
    if (fp == NULL)
    {
        TEST_FAIL_MESSAGE("Error al abrir flujo de memoria");
    }

    stdout = fp;
    echo_command(input2);

    fflush(stdout);
    stdout = original_stdout;
    fclose(fp);

    TEST_ASSERT_EQUAL_STRING(expected_output2, buffer);
}

// Prueba de la función change_directory
void test_change_directory(void)
{
    // Guardar el directorio actual
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));

    // Cambiar al directorio raíz
    change_directory("/");
    char new_dir[1024];
    getcwd(new_dir, sizeof(new_dir));
    TEST_ASSERT_EQUAL_STRING("/", new_dir);

    // Cambiar de vuelta al directorio original
    change_directory(original_dir);
    getcwd(new_dir, sizeof(new_dir));
    TEST_ASSERT_EQUAL_STRING(original_dir, new_dir);
}
