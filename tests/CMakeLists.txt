cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

include_directories(/include)

# Establece las banderas para la cobertura
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Crear el ejecutable de pruebas
add_executable(test_shell
    test_shell.c
    ../src/commands.c
    ../src/monitor.c
    ../src/signals.c
    ../src/batch_file.c
    ../src/internal_commands.c)


target_link_libraries(test_shell PRIVATE unity::unity cjson::cjson)

# Definir la macro TESTING solo para las pruebas
target_compile_definitions(test_shell PRIVATE TESTING) # Agrega esta línea para definir la macro TESTING
    
# Registrar la prueba con CTest
add_test(NAME test_shell COMMAND test_shell)
