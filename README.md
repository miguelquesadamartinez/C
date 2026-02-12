# Proyecto de vuelta al C

Este repositorio contiene diversos programas en C para aprender conceptos fundamentales y funcionalidades avanzadas.

---

## 📁 Estructura del Proyecto

```
C/
├── db/                     # Programas de base de datos
│   ├── test_sql.c
│   ├── menu_tabla.c
│   ├── sql_connection.c
│   └── sql_connection.h
├── serial/                # Comunicación serial completa
│   └── serial.c
├── arduino/               # Comunicación Arduino simple
│   └── arduino.c
├── arduinoCodes/           # Códigos Arduino IDE
├── multithreading.c        # Ejemplo de multithreading con números aleatorios
├── README.md               # Este archivo
├── README_SQL.md           # Documentación SQL detallada
├── README_SERIAL.md        # Documentación Serial detallada
└── README_ARDUINO.md       # Documentación Arduino detallada
```

---

## 🚀 Programas Disponibles

### 1. **serial.c** - Comunicación Serial/USB

Programa para enviar datos a dispositivos por puerto serial tradicional o dispositivos USB (Arduino, ESP32, módulos USB, etc.).

📖 **[Ver documentación completa → README_SERIAL.md](README_SERIAL.md)**

**Características:**

- 📡 Envío de texto por puerto COM tradicional
- 🔌 Envío a dispositivos USB (Arduino, ESP32, adaptadores USB-Serial)
- 🔧 Configuración de puerto serial
- ⚙️ Velocidad estándar: 9600 baud (8N1)
- 📋 Menú separado para Serial y USB
- 🔌 Soporte para FTDI, CH340, CP2102
- ℹ️ Ayuda para encontrar puertos disponibles
- ✓ Validación de conexión

**Compilar:**

```powershell
gcc serial\serial.c -o serial\serial.exe
```

**Ejecutar:**

```powershell
.\serial\serial.exe
```

**Opciones del menú:**

1. **Enviar a puerto Serial** - Para puertos COM tradicionales (COM1, COM2)
2. **Enviar a dispositivo USB** - Para Arduino, ESP32, ESP8266, adaptadores USB-Serial
3. **Configuración avanzada** - Ajustes de velocidad (baud rate)
4. **Ayuda** - Cómo encontrar tu puerto
5. **Salir**

**Dispositivos USB soportados:**

- Arduino (Uno, Mega, Nano, etc.)
- ESP32 / ESP8266
- Adaptadores USB-TTL (FTDI, CH340, CP2102, PL2303)
- Cualquier dispositivo USB a Serial

💡 **Nota:** Los dispositivos USB aparecen como puertos COM virtuales en Windows (COM3, COM4, COM5, etc.)

**¿Cómo encontrar mi puerto COM?**

**Opción 1 - Administrador de dispositivos:**

1. Abre "Administrador de dispositivos"
2. Expande "Puertos (COM y LPT)"
3. Busca tu dispositivo (Arduino, USB Serial, etc.)

**Opción 2 - PowerShell:**

```powershell
[System.IO.Ports.SerialPort]::getportnames()
```

**Opción 3 - Arduino IDE:**

- Menú: Herramientas → Puerto

---

### 2. **arduino.c** - Comunicación Simple con Arduino

Programa minimalista y directo para enviar comandos a Arduino sin menús.

📖 **[Ver documentación completa → README_ARDUINO.md](README_ARDUINO.md)**

**Características:**

- 🎯 Código simple y directo
- ⚡ Envío rápido de un carácter ('1')
- 🔧 Hardcodeado para COM3 a 9600 baud
- 📝 Ideal para aprender lo básico de comunicación serial
- 🚀 Sin menús, simplemente ejecutar y enviar
- 🔄 Lectura continua durante 2 minutos
- ⏱️ Timestamps en cada lectura recibida

**¿Cuándo usar este programa?**

- Quieres un código simple para entender comunicación serial
- Necesitas enviar un comando rápido sin configuración
- Estás aprendiendo cómo funciona la API de Windows para serial
- Quieres monitorear respuestas del Arduino en tiempo real

**Compilar:**

```powershell
gcc arduino\arduino.c -o arduino\arduino.exe
```

**Ejecutar:**

```powershell
.\arduino\arduino.exe
```

**⚙️ Personalizar:**

Para cambiar el puerto o el dato a enviar, edita `arduino/arduino.c`:

```c
hSerial = CreateFile("COM3", ...);          // Cambiar "COM3" por tu puerto
char data = '1';                             // Cambiar '1' por tu comando
```

**Diferencia con serial.c:**

- `arduino.c`: Simple, directo, sin menús. Un solo comando hardcodeado. Lectura continua por 2 minutos.
- `serial.c`: Menú completo, múltiples opciones, configuración interactiva.

---

### 3. **multithreading.c** - Multithreading con Números Aleatorios

Programa educativo que demuestra el uso de múltiples threads (hilos) en C usando la API de Windows.

**Características:**

- 🧵 Creación de múltiples threads simultáneos
- 🎲 Generación de números aleatorios en cada thread
- 🔒 Sincronización con Critical Sections
- ⏱️ Delays aleatorios para simular trabajo
- 📊 Visualización en tiempo real de la ejecución concurrente
- ✓ Limpieza correcta de recursos

**Conceptos demostrados:**

- `CreateThread()` - Creación de threads
- `WaitForMultipleObjects()` - Esperar múltiples threads
- `CRITICAL_SECTION` - Sincronización de recursos compartidos
- `EnterCriticalSection()` / `LeaveCriticalSection()` - Control de acceso
- Paso de parámetros a threads mediante estructuras

**¿Qué hace el programa?**

Crea 4 threads que ejecutan simultáneamente. Cada thread:

1. Genera 5 números aleatorios (1-100)
2. Los muestra con un identificador único
3. Tiene delays aleatorios para ver la concurrencia
4. Avisa cuando termina

**Compilar:**

```powershell
gcc multithreading.c -o multithreading.exe
```

**Ejecutar:**

```powershell
.\multithreading.exe
```

**Ejemplo de salida:**

```
Creando 4 threads...
[Thread 1] Número 1: 58
[Thread 3] Número 1: 65
[Thread 2] Número 2: 93
[Thread 4] Número 1: 68
[Thread 1] Número 2: 97
...
```

💡 **Nota educativa:** Los threads se ejecutan en paralelo, por eso los números no aparecen en orden secuencial. La Critical Section evita que las líneas de salida se mezclen.

---

### 4. **test_sql.c** - Conexión a SQL Server

Programa para conectarse a SQL Server y ejecutar consultas SELECT, INSERT, UPDATE, etc.

📖 **[Ver documentación completa → README_SQL.md](README_SQL.md)**

Programa para conectarse a SQL Server y ejecutar consultas SELECT, INSERT, UPDATE, etc.

**Características:**

- Autenticación Windows (Trusted Connection)
- Ejecución de consultas SELECT
- Comandos INSERT/UPDATE/DELETE
- Visualización de resultados en tabla
- Manejo de errores

**Configuración:**
Edita las líneas 20-21 en `db/test_sql.c`:

```c
const char *servidor = "localhost";      // Tu servidor SQL
const char *baseDatos = "TestDB";        // Tu base de datos
```

**Compilar:**

```powershell
gcc -o db\test_sql.exe db\test_sql.c db\sql_connection.c -lodbc32
```

**Ejecutar:**

```powershell
.\db\test_sql.exe
```

---

### 5. **menu_tabla.c** - Gestor de Tablas SQL

Aplicación interactiva para crear tablas en SQL Server con un menú fácil de usar.

📖 **[Ver documentación completa → README_SQL.md](README_SQL.md)**

**Funcionalidades:**

- 📋 Menú interactivo
- ✏️ Crear tablas personalizadas
- 🔢 7 tipos de datos disponibles (INT, VARCHAR, FLOAT, DATE, BOOLEAN, TEXT, DECIMAL)
- 📊 Agregar múltiples campos a la tabla
- 🚀 Ejecución directa en la base de datos
- ✓ Validación de errores

**Configuración:**
Edita las líneas 175-176 en `db/menu_tabla.c`:

```c
const char *servidor = "localhost";
const char *baseDatos = "TestDB";
```

**Compilar:**

```powershell
gcc -o db\menu_tabla.exe db\menu_tabla.c db\sql_connection.c -lodbc32
```

**Ejecutar:**

```powershell
.\db\menu_tabla.exe
```

**Flujo de uso:**

1. Selecciona "Crear nueva tabla"
2. Ingresa el nombre de la tabla
3. Agrega campos con sus tipos de datos
4. Escribe "salir" o presiona Enter para terminar
5. La tabla se crea automáticamente en SQL Server

---

### 4. **serial.c** - Comunicación Serial/USB

Programa para enviar datos a dispositivos por puerto serial tradicional o dispositivos USB (Arduino, ESP32, módulos USB, etc.).

📖 **[Ver documentación completa → README_SERIAL.md](README_SERIAL.md)**

**Características:**

- 📡 Envío de texto por puerto COM tradicional
- 🔌 Envío a dispositivos USB (Arduino, ESP32, adaptadores USB-Serial)
- 🔧 Configuración de puerto serial
- ⚙️ Velocidad estándar: 9600 baud (8N1)
- 📋 Menú separado para Serial y USB
- 🔌 Soporte para FTDI, CH340, CP2102
- ℹ️ Ayuda para encontrar puertos disponibles
- ✓ Validación de conexión

**Compilar:**

```powershell
gcc serial\serial.c -o serial\serial.exe
```

**Ejecutar:**

```powershell
.\serial\serial.exe
```

**Opciones del menú:**

1. **Enviar a puerto Serial** - Para puertos COM tradicionales (COM1, COM2)
2. **Enviar a dispositivo USB** - Para Arduino, ESP32, ESP8266, adaptadores USB-Serial
3. **Configuración avanzada** - Ajustes de velocidad (baud rate)
4. **Ayuda** - Cómo encontrar tu puerto
5. **Salir**

**Dispositivos USB soportados:**

- Arduino (Uno, Mega, Nano, etc.)
- ESP32 / ESP8266
- Adaptadores USB-TTL (FTDI, CH340, CP2102, PL2303)
- Cualquier dispositivo USB a Serial

💡 **Nota:** Los dispositivos USB aparecen como puertos COM virtuales en Windows (COM3, COM4, COM5, etc.)

**¿Cómo encontrar mi puerto COM?**

**Opción 1 - Administrador de dispositivos:**

1. Abre "Administrador de dispositivos"
2. Expande "Puertos (COM y LPT)"
3. Busca tu dispositivo (Arduino, USB Serial, etc.)

**Opción 2 - PowerShell:**

```powershell
[System.IO.Ports.SerialPort]::getportnames()
```

**Opción 3 - Arduino IDE:**

- Menú: Herramientas → Puerto

---

### 5. **arduino.c** - Comunicación Simple con Arduino

Programa minimalista y directo para enviar comandos a Arduino sin menús.

📖 **[Ver documentación completa → README_ARDUINO.md](README_ARDUINO.md)**

**Características:**

- 🎯 Código simple y directo
- ⚡ Envío rápido de un carácter ('1')
- 🔧 Hardcodeado para COM3 a 9600 baud
- 📝 Ideal para aprender lo básico de comunicación serial
- 🚀 Sin menús, simplemente ejecutar y enviar

**¿Cuándo usar este programa?**

- Quieres un código simple para entender comunicación serial
- Necesitas enviar un comando rápido sin configuración
- Estás aprendiendo cómo funciona la API de Windows para serial

**Compilar:**

```powershell
gcc arduino\arduino.c -o arduino\arduino.exe
```

**Ejecutar:**

```powershell
.\arduino\arduino.exe
```

**⚙️ Personalizar:**

Para cambiar el puerto o el dato a enviar, edita `arduino/arduino.c`:

```c
hSerial = CreateFile("COM3", ...);          // Cambiar "COM3" por tu puerto
char data = '1';                             // Cambiar '1' por tu comando
```

**Diferencia con serial.c:**

- `arduino.c`: Simple, directo, sin menús. Un solo comando hardcodeado.
- `serial.c`: Menú completo, múltiples opciones, configuración interactiva.

---

## 📚 Biblioteca SQL Connection

### **sql_connection.h / sql_connection.c**

Biblioteca reutilizable para conectarse a SQL Server desde C usando ODBC.

**Funciones principales:**

```c
// Conectar con autenticación Windows
int conectarSQLTrusted(SQLConnection *conn, const char *server, const char *database);

// Ejecutar consulta SELECT (retorna resultados)
SQLResult* ejecutarConsulta(SQLConnection *conn, const char *query);

// Ejecutar comando INSERT/UPDATE/DELETE (no retorna datos)
int ejecutarComando(SQLConnection *conn, const char *comando);

// Desconectar
void desconectarSQL(SQLConnection *conn);

// Liberar memoria de resultados
void liberarResultado(SQLResult *result);

// Mostrar resultados en formato tabla
void mostrarResultado(SQLResult *result);
```

**Uso en tus propios programas:**

```c
#include "sql_connection.h"

SQLConnection conn;
conectarSQLTrusted(&conn, "localhost", "MiBaseDeDatos");
ejecutarComando(&conn, "INSERT INTO Tabla VALUES (1, 'Valor')");
desconectarSQL(&conn);
```

---

## ⚙️ Requisitos

### Software necesario:

1. **Compilador GCC** (MinGW para Windows)
   - Descargar: https://www.mingw-w64.org/

2. **SQL Server** (para programas SQL)
   - SQL Server Express: https://www.microsoft.com/sql-server

3. **ODBC Driver** (incluido en Windows)
   - Ya instalado en Windows por defecto

4. **Arduino / Dispositivos USB** (opcional, para serial.c y arduino.c)
   - Arduino IDE: https://www.arduino.cc/en/software
   - Drivers USB-Serial (CH340, FTDI) si es necesario

---

## 🛠️ Compilación Rápida

### Todos los programas a la vez:

```powershell
# Programa básico
gcc main.c -o main.exe

# Programas SQL (requieren sql_connection.c)
gcc -o db\test_sql.exe db\test_sql.c db\sql_connection.c -lodbc32
gcc -o db\menu_tabla.exe db\menu_tabla.c db\sql_connection.c -lodbc32

# Comunicación serial/USB
gcc serial\serial.c -o serial\serial.exe
gcc arduino\arduino.c -o arduino\arduino.exe
```

---

## 📖 Conceptos de C Cubiertos

- ✅ Variables y tipos de datos
- ✅ Funciones y prototipos
- ✅ Punteros (`*`, `&`, `malloc`, `free`)
- ✅ Estructuras (`struct`)
- ✅ Arrays y strings
- ✅ Entrada/salida estándar (`printf`, `scanf`, `fgets`)
- ✅ Manejo de archivos (`fopen`, `fprintf`, `fgets`, `fclose`)
- ✅ Control de flujo (`if`, `while`, `do-while`, `switch`)
- ✅ Bibliotecas estándar (`stdio.h`, `stdlib.h`, `string.h`)
- ✅ API de Windows (`windows.h` para serial)
- ✅ ODBC para bases de datos

---

## 🐛 Solución de Problemas

### Error: "gcc no se reconoce como comando"

- Instala MinGW y agrega `C:\mingw\bin` al PATH de Windows

### Error: "No se pudo conectar a SQL Server"

- Verifica que SQL Server esté corriendo
- Verifica el nombre del servidor y base de datos
- Comprueba que tienes permisos de acceso

### Error: "No se pudo abrir el puerto COM"

- Verifica que el dispositivo esté conectado
- Cierra otras aplicaciones que usen el puerto (Arduino IDE, PuTTY)
- Verifica el número de puerto correcto

### Error al compilar programas SQL

- Asegúrate de incluir `-lodbc32` al final del comando
- Incluye ambos archivos: `program.c db\sql_connection.c`

---

## 📚 Documentación Detallada

Para información completa sobre cada módulo, consulta estos documentos:

### 📦 Base de Datos (SQL Server)

**[README_SQL.md](README_SQL.md)** - Documentación completa de SQL

- Guía de funciones SQL (conectar, consultas, comandos)
- Estructura SQLResult y acceso a datos
- Ejemplos de SELECT, INSERT, UPDATE, DELETE
- Configuración de cadenas de conexión
- Manejo de errores ODBC
- Requisitos y configuración de SQL Server

### 📡 Comunicación Serial/USB

**[README_SERIAL.md](README_SERIAL.md)** - Documentación completa de Serial

- API de Windows para comunicación serial
- Funciones CreateFile, WriteFile, ReadFile
- Configuración de puerto (baud rate, paridad)
- Cómo encontrar puertos COM
- Ejemplos de lectura/escritura
- Dispositivos soportados (Arduino, ESP32, FTDI, CH340)
- Solución de problemas de conexión

### 🤖 Arduino

**[README_ARDUINO.md](README_ARDUINO.md)** - Documentación completa de Arduino

- Guía de inicio rápido
- Personalización de puerto y comandos
- Código Arduino complementario (LED, sensores)
- Flujo de comunicación PC ↔ Arduino
- Diagnóstico de problemas
- Modelos Arduino compatibles
- Drivers necesarios (CH340, FTDI)

---

## 🔧 Scripts de Automatización

El proyecto incluye scripts `.bat` para facilitar la compilación:

- **build.bat** - Compila un archivo específico
- **build_all.bat** - Compila todos los programas
- **run.bat** - Compila y ejecuta en un paso
- **clean.bat** - Elimina todos los ejecutables

**Uso:**

```powershell
build_all.bat          # Compilar todo
run.bat db\test_sql.c   # Compilar y ejecutar
clean.bat              # Limpiar
```

---

## 🗂️ Proyectos por Categoría

### Aprendizaje Básico

- [main.c](main.c) - Fundamentos de C

### Base de Datos

- [db/test_sql.c](db/test_sql.c) - Ejemplos de consultas
- [db/menu_tabla.c](db/menu_tabla.c) - Crear tablas interactivamente
- [db/sql_connection.h](db/sql_connection.h) - API SQL

### Comunicaciones

- [serial/serial.c](serial/serial.c) - Menú completo serial/USB
- [arduino/arduino.c](arduino/arduino.c) - Ejemplo mínimo Arduino

---

## 🎓 Próximos Pasos

Conceptos avanzados por aprender:

- [ ] Multi-threading
- [ ] Sockets y comunicación de red
- [ ] Estructuras de datos (listas enlazadas, árboles)
- [ ] Manejo avanzado de errores
- [ ] Preprocesador y macros
- [ ] Compilación condicional

---

## 📄 Licencia

Este es un proyecto educativo personal. Siéntete libre de usar y modificar el código para aprender.

---

**¡Feliz aprendizaje de C!** 🚀
