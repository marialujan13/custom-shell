# Instalación

### Requisitos previos

Sistema operativo: Linux.

### Herramientas

- CMake 3.10 o superior
- GCC
- CUnit
- cJSON
- Prometheus-Client-C
- libmicrohttpd
- lcov

## 1. Instalación de dependencias

Para instalar las dependencias en una distribución basada en Debian (como Ubuntu), ejecuta el siguiente comando:

```bash
sudo apt-get install cmake gcc libcunit1-dev libcjson-dev libmicrohttpd-dev lcov
```

## 2. Clona el repositorio

```bash
git clone https://github.com/ICOMP-UNC/so-i-24-chp2-marialujan13.git

cd so-i-24-chp2-marialujan13
```

### 2.1 Clonar sub-repositorio (como submodulo)

Esto es necesario debido a que el submodulo ya está agregado en el repositorio pero al momento de clonarlo no aparecerá.

**Si solo se necesita el repositorio:**

```bash
git clone https://github.com/MVene/monitor_prometheus.git
```

**Si se desea agregarlo como submodulo:**

```bash
git submodule add https://github.com/MVene/monitor_prometheus.git

git submodule update --init --recursive
```

## 3. Configuración e instalación de Conan para dependencias

### 3.1 Instalación y Activación de Venv

En este paso seteamos un *python environment*.

```bash
python3 -m venv venv
```

```bash
source venv/bin/activate
```

Una vez dentro del *environment*, se verá la palabra `(venv)` antes del prompt de la terminal, asi sabemos que está bien seteado.

```python
pip install conan
```

Y por ultimo instalamos conan, un manejador de dependencias para CMake.

### 3.2 Conan, cJSON y Unity

Asegurarse que previamente se cuenta con un archivo `conanfile.txt` dentro de la carpeta del repositorio.

Para generar un perfil default:

```python
conan profile detect
```

Luego generaremos un perfil **debug**.

#### 3.2.1 Generación de perfil Conan Debug

Localizar la carpeta `.conan2`, por default esta se encuentra en `/home/$(user)/.conan2`, en la carpeta `profiles`, crear un nuevo archivo de perfil llamado **debug**, esto se puede hacer con el siguiente comando:

```bash
touch debug
```

Y utilizando un gestor de archivos, cambiar el contenido del archivo **debug** por el siguiente:

```file
[settings]
arch=x86_64
build_type=Debug
compiler=gcc
compiler.cppstd=gnu17
compiler.libcxx=libstdc++11
compiler.version=14
os=Linux
```

Una vez con el perfil debug y el archivo `conanfile.txt`, ejecutar el siguiente comando:

```python
conan install . --build=missing --profile=debug
```

## 4. Compilar usando CMake

```bash
cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_COVERAGE=1

make
```

### 4.1 Compilar el submodulo para obtener el archivo `.metrics`

```bash
cd monitor_prometheus

make
```

Listo, ya se encuentran todos los archivos configurados y compilados.

## 5. Ejecución de la Shell

En la carpeta `/build/`:

```bash
./my_shell
```

## 6. Testing y Coverage Report

En la carpeta `/build/tests/`

```bash
./test_shell
```

### 6.1 Coverage Report y HTML

```bash
lcov --capture --directory . --output-file coverage.info

genhtml coverage.info --output-directory coverage

firefox coverage/index.html
```

## 7. Usando la SHELL

### 7.1 Comandos Internos

La shell soporta algunos comandos internos como:

#### cd (Change Directory)

Este comando se utiliza para moverse entre directorios, algunas caracteristicas del comando son las siguientes:

- Si no se ingresa ningun directorio, la shell imprimirá el directorio actual.
- Si el comando ingresado es `cd -`, la shell volverá al directorio anterior inmediato.
- Si se ingresa un directorio valido la shell cambiará de directorio.

#### echo

Este comando permite imprimir por pantalla un mensaje, el comando también acepta variables de entorno solo si estas empiezan son el simbolo `$(env)` siendo (env) el nombre de la variable que se desea imprimir.

#### clr (Clear)

Este comando limpia la pantalla para mejorar la visibilidad.

#### quit

Este comando cierra la shell.

#### start_monitor

Este comando inicia el monitoreo de metricas generadas por Prometheus.

#### stop_monitor

Este comando detiene el monitoreo de metricas generadas por Prometheus.

#### status_monitor

Este comando permite observar el estado actual de las metricas, las metricas a mostrar pueden ser editadas en el archivo `settings.json`.

### 7.2 Comandos Externos

Cualquier comando que no pertenezca a la lista de arriba será tomado como comando externo y ejecutado.

### 7.3 Pipes, Segundo Plano y I/O redirection.

La shell tambien soporta el uso de pipes, denotadas con el simbolo `|`, el uso de ejecución en segundo plano, al agregar el simbolo `&` al final del comando, y el uso de redirección de entrada o salida con los simbolos `<`y `>`.

