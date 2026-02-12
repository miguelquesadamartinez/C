# Conexión a SQL Server desde C

## Archivos del proyecto

- **sql_connection.h** - Header con las declaraciones de funciones
- **sql_connection.c** - Implementación de las funciones
- **ejemplo_sql.c** - Ejemplos de uso

## Compilación

### Opción 1: Scripts de compilación (Recomendado)

**Compilar y ejecutar en un paso:**

```bash
run.bat test_sql.c
```

**Compilar un archivo específico:**

```bash
build.bat test_sql.c
```

**Compilar todos los programas SQL:**

```bash
build_all.bat
```

**Limpiar ejecutables:**

```bash
clean.bat
```

### Opción 2: Comandos manuales

```bash
gcc -o ejemplo_sql ejemplo_sql.c sql_connection.c -lodbc32
gcc -g test_sql.c sql_connection.c -o test_sql.exe -lodbc32
```

## Funciones disponibles

### Conexión

1. **conectarSQLTrusted()** - Autenticación de Windows

```c
SQLConnection conn;
conectarSQLTrusted(&conn, "localhost", "NombreBaseDatos");
```

2. **conectarSQL()** - Autenticación con usuario y contraseña

```c
SQLConnection conn;
conectarSQL(&conn, "localhost", "NombreBaseDatos", "usuario", "contraseña");
```

3. **desconectarSQL()** - Cerrar conexión

```c
desconectarSQL(&conn);
```

### Consultas

4. **ejecutarConsulta()** - Ejecutar SELECT y obtener resultados

```c
SQLResult *result = ejecutarConsulta(&conn, "SELECT * FROM Tabla");
```

5. **ejecutarComando()** - Ejecutar INSERT, UPDATE, DELETE

```c
ejecutarComando(&conn, "INSERT INTO Tabla (Col1) VALUES ('valor')");
```

### Resultados

6. **mostrarResultado()** - Mostrar resultados en formato tabla

```c
mostrarResultado(result);
```

7. **liberarResultado()** - Liberar memoria de resultados

```c
liberarResultado(result);
```

## Estructura SQLResult

Los resultados se almacenan en un `SQLResult*` con:

- `data` - Array 2D con los datos [fila][columna]
- `numRows` - Número de filas
- `numCols` - Número de columnas
- `colNames` - Nombres de las columnas

### Acceso a datos específicos

```c
SQLResult *result = ejecutarConsulta(&conn, "SELECT Nombre, Edad FROM Usuarios");

// Acceder a la primera fila, primera columna
char *nombre = result->data[0][0];

// Acceder a la segunda fila, segunda columna
char *edad = result->data[1][1];

// Iterar todos los datos
for (int i = 0; i < result->numRows; i++) {
    for (int j = 0; j < result->numCols; j++) {
        printf("%s ", result->data[i][j]);
    }
    printf("\n");
}
```

## Ejemplo completo

```c
#include <stdio.h>
#include "sql_connection.h"

int main() {
    SQLConnection conn;

    // Conectar
    if (conectarSQLTrusted(&conn, "localhost", "MiBaseDatos")) {

        // Ejecutar consulta
        SQLResult *result = ejecutarConsulta(&conn, "SELECT * FROM Clientes");

        if (result != NULL) {
            // Mostrar resultados
            mostrarResultado(result);

            // O acceder manualmente
            printf("Total de clientes: %d\n", result->numRows);
            printf("Primer cliente: %s\n", result->data[0][0]);

            // Liberar memoria
            liberarResultado(result);
        }

        // Insertar datos
        ejecutarComando(&conn,
            "INSERT INTO Clientes (Nombre) VALUES ('Pedro')");

        // Desconectar
        desconectarSQL(&conn);
    }

    return 0;
}
```

## Requisitos

- Windows con SQL Server instalado
- Driver ODBC de SQL Server (incluido en Windows)
- MinGW o compilador de C compatible
- Librería `odbc32.lib` (se enlaza con `-lodbc32`)

## Configuración de SQL Server

Asegúrate de que:

1. SQL Server esté corriendo
2. El servicio SQL Server Browser esté activo (si usas instancia nombrada)
3. Autenticación de Windows esté habilitada (para `conectarSQLTrusted`)
4. O autenticación mixta esté habilitada (para `conectarSQL`)

## Cadena de conexión

El código usa estas cadenas de conexión:

**Windows Authentication:**

```
DRIVER={SQL Server};SERVER=localhost;DATABASE=TestDB;Trusted_Connection=yes;
```

**SQL Authentication:**

```
DRIVER={SQL Server};SERVER=localhost;DATABASE=TestDB;UID=sa;PWD=1804;
```

## Manejo de errores

Todas las funciones incluyen manejo de errores:

- Retornan 0/NULL en caso de fallo
- Muestran mensajes de error detallados
- La función `mostrarError()` muestra información de diagnóstico SQL
