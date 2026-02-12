#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql_connection.h"

#define MAX_CAMPOS 50
#define MAX_NOMBRE 100
#define MAX_QUERY 2000

// Conexión SQL global
SQLConnection conexion;

typedef struct {
    char nombre[MAX_NOMBRE];
    char tipo[MAX_NOMBRE];
} Campo;

typedef struct {
    char nombre[MAX_NOMBRE];
    char tipo[MAX_NOMBRE];
    int longitud;
    int esNulable;
} ColumnaInfo;

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void mostrarTiposDatos() {
    printf("\n=== TIPOS DE DATOS DISPONIBLES ===\n");
    printf("1. INT           - Número entero\n");
    printf("2. VARCHAR(n)    - Texto variable (especifica n)\n");
    printf("3. FLOAT         - Número decimal\n");
    printf("4. DATE          - Fecha\n");
    printf("5. BOOLEAN       - Verdadero/Falso\n");
    printf("6. TEXT          - Texto largo\n");
    printf("7. DECIMAL(p,s)  - Decimal con precisión\n");
    printf("==================================\n");
}

void listarTablas(char tablas[][MAX_NOMBRE], int *numTablas) {
    *numTablas = 0;
    
    // Consulta para obtener tablas del usuario (excluyendo tablas del sistema)
    const char *query = "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES "
                        "WHERE TABLE_TYPE = 'BASE TABLE' AND TABLE_CATALOG = DB_NAME() "
                        "ORDER BY TABLE_NAME";
    
    SQLResult *resultado = ejecutarConsulta(&conexion, query);
    
    if (resultado == NULL || resultado->numRows == 0) {
        printf("\nNo hay tablas disponibles en la base de datos.\n");
        if (resultado) liberarResultado(resultado);
        return;
    }
    
    printf("\n=== TABLAS DISPONIBLES ===\n");
    for (int i = 0; i < resultado->numRows && i < MAX_CAMPOS; i++) {
        strcpy(tablas[i], resultado->data[i][0]);
        printf("%d. %s\n", i + 1, tablas[i]);
        (*numTablas)++;
    }
    printf("==========================\n");
    
    liberarResultado(resultado);
}

int obtenerColumnasTabla(const char *nombreTabla, ColumnaInfo columnas[], int *numColumnas) {
    *numColumnas = 0;
    
    // Consulta para obtener información de columnas
    char query[500];
    sprintf(query, 
        "SELECT COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, IS_NULLABLE "
        "FROM INFORMATION_SCHEMA.COLUMNS "
        "WHERE TABLE_NAME = '%s' "
        "ORDER BY ORDINAL_POSITION", 
        nombreTabla);
    
    SQLResult *resultado = ejecutarConsulta(&conexion, query);
    
    if (resultado == NULL || resultado->numRows == 0) {
        printf("\n✗ No se pudieron obtener las columnas de la tabla.\n");
        if (resultado) liberarResultado(resultado);
        return 0;
    }
    
    for (int i = 0; i < resultado->numRows && i < MAX_CAMPOS; i++) {
        strcpy(columnas[i].nombre, resultado->data[i][0]);
        strcpy(columnas[i].tipo, resultado->data[i][1]);
        
        // Longitud (puede ser NULL)
        if (resultado->data[i][2] && strlen(resultado->data[i][2]) > 0) {
            columnas[i].longitud = atoi(resultado->data[i][2]);
        } else {
            columnas[i].longitud = 0;
        }
        
        // Es nullable
        columnas[i].esNulable = (strcmp(resultado->data[i][3], "YES") == 0) ? 1 : 0;
        
        (*numColumnas)++;
    }
    
    liberarResultado(resultado);
    return 1;
}

void insertarDatosEnTabla() {
    char tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    
    printf("\n=== INSERTAR DATOS EN TABLA ===\n");
    
    // Listar tablas disponibles
    listarTablas(tablas, &numTablas);
    
    if (numTablas == 0) {
        printf("\nNo hay tablas disponibles. Crea una tabla primero.\n");
        return;
    }
    
    // Seleccionar tabla
    printf("\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    scanf("%d", &seleccion);
    limpiarBuffer();
    
    if (seleccion == 0) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    if (seleccion < 1 || seleccion > numTablas) {
        printf("\n✗ Selección inválida.\n");
        return;
    }
    
    char *tablaSeleccionada = tablas[seleccion - 1];
    printf("\n✓ Tabla seleccionada: %s\n", tablaSeleccionada);
    
    // Confirmación antes de continuar
    printf("\n¿Desea insertar datos en esta tabla? (1=Sí, 0=Cancelar): ");
    int confirmar;
    scanf("%d", &confirmar);
    limpiarBuffer();
    
    if (confirmar != 1) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    // Obtener columnas de la tabla
    ColumnaInfo columnas[MAX_CAMPOS];
    int numColumnas = 0;
    
    if (!obtenerColumnasTabla(tablaSeleccionada, columnas, &numColumnas)) {
        return;
    }
    
    printf("\n--- FORMULARIO DE DATOS ---\n");
    printf("(Presiona Enter para dejar NULL en campos opcionales)\n\n");
    
    // Arrays para almacenar los valores ingresados
    char valores[MAX_CAMPOS][500];
    int esNulo[MAX_CAMPOS];
    
    // Recopilar datos de cada columna
    for (int i = 0; i < numColumnas; i++) {
        printf("[%s] (%s", columnas[i].nombre, columnas[i].tipo);
        if (columnas[i].longitud > 0) {
            printf("(%d)", columnas[i].longitud);
        }
        if (columnas[i].esNulable) {
            printf(" - opcional");
        }
        printf(")\n");
        
        printf("Valor: ");
        fgets(valores[i], sizeof(valores[i]), stdin);
        valores[i][strcspn(valores[i], "\n")] = 0;
        
        // Verificar si dejó el campo vacío
        if (strlen(valores[i]) == 0) {
            if (columnas[i].esNulable) {
                esNulo[i] = 1;
                printf("  → NULL\n");
            } else {
                printf("  ✗ Este campo es obligatorio. Ingresa un valor: ");
                fgets(valores[i], sizeof(valores[i]), stdin);
                valores[i][strcspn(valores[i], "\n")] = 0;
                esNulo[i] = 0;
            }
        } else {
            esNulo[i] = 0;
        }
        
        printf("\n");
    }
    
    // Construir la sentencia INSERT
    char query[MAX_QUERY];
    char nombresColumnas[1000] = "";
    char valoresSQL[1000] = "";
    
    // Construir lista de nombres de columnas
    for (int i = 0; i < numColumnas; i++) {
        strcat(nombresColumnas, columnas[i].nombre);
        if (i < numColumnas - 1) {
            strcat(nombresColumnas, ", ");
        }
    }
    
    // Construir lista de valores
    for (int i = 0; i < numColumnas; i++) {
        if (esNulo[i]) {
            strcat(valoresSQL, "NULL");
        } else {
            // Determinar si necesita comillas (tipos de texto)
            if (strstr(columnas[i].tipo, "char") != NULL || 
                strstr(columnas[i].tipo, "text") != NULL ||
                strstr(columnas[i].tipo, "date") != NULL) {
                
                strcat(valoresSQL, "'");
                strcat(valoresSQL, valores[i]);
                strcat(valoresSQL, "'");
            } else {
                strcat(valoresSQL, valores[i]);
            }
        }
        
        if (i < numColumnas - 1) {
            strcat(valoresSQL, ", ");
        }
    }
    
    // Construir INSERT completo
    sprintf(query, "INSERT INTO %s (%s) VALUES (%s)", 
            tablaSeleccionada, nombresColumnas, valoresSQL);
    
    printf("=== SENTENCIA SQL ===\n");
    printf("%s\n", query);
    printf("=====================\n\n");
    
    // Ejecutar la inserción
    printf("Ejecutando inserción...\n");
    if (ejecutarComando(&conexion, query)) {
        printf("\n✓ ¡Datos insertados exitosamente en '%s'!\n\n", tablaSeleccionada);
    } else {
        printf("\n✗ Error al insertar datos. Verifica los valores ingresados.\n\n");
    }
}

void agregarCampoATabla() {
    char tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    
    printf("\n=== AGREGAR CAMPO A TABLA ===\n");
    
    // Listar tablas disponibles
    listarTablas(tablas, &numTablas);
    
    if (numTablas == 0) {
        printf("\nNo hay tablas disponibles. Crea una tabla primero.\n");
        return;
    }
    
    // Seleccionar tabla
    printf("\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    scanf("%d", &seleccion);
    limpiarBuffer();
    
    if (seleccion == 0) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    if (seleccion < 1 || seleccion > numTablas) {
        printf("\n✗ Selección inválida.\n");
        return;
    }
    
    char *tablaSeleccionada = tablas[seleccion - 1];
    printf("\n✓ Tabla seleccionada: %s\n", tablaSeleccionada);
    
    // Confirmación antes de continuar
    printf("\n¿Desea agregar un campo a esta tabla? (1=Sí, 0=Cancelar): ");
    int confirmar;
    scanf("%d", &confirmar);
    limpiarBuffer();
    
    if (confirmar != 1) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    // Pedir nombre del nuevo campo
    char nombreCampo[MAX_NOMBRE];
    printf("\n--- NUEVO CAMPO ---\n");
    printf("Nombre del campo: ");
    fgets(nombreCampo, sizeof(nombreCampo), stdin);
    nombreCampo[strcspn(nombreCampo, "\n")] = 0;
    
    if (strlen(nombreCampo) == 0) {
        printf("\n✗ Nombre de campo inválido.\n");
        return;
    }
    
    // Mostrar tipos de datos y seleccionar
    mostrarTiposDatos();
    printf("\nSelecciona el tipo de dato (1-7, 0 para cancelar): ");
    int opcion;
    scanf("%d", &opcion);
    limpiarBuffer();
    
    if (opcion == 0) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    char tipoDato[MAX_NOMBRE];
    
    // Asignar tipo según la opción
    switch (opcion) {
        case 1:
            strcpy(tipoDato, "INT");
            break;
        case 2:
            printf("Ingresa el tamaño (ej: 50): ");
            int tam;
            scanf("%d", &tam);
            limpiarBuffer();
            sprintf(tipoDato, "VARCHAR(%d)", tam);
            break;
        case 3:
            strcpy(tipoDato, "FLOAT");
            break;
        case 4:
            strcpy(tipoDato, "DATE");
            break;
        case 5:
            strcpy(tipoDato, "BIT");
            break;
        case 6:
            strcpy(tipoDato, "TEXT");
            break;
        case 7:
            printf("Ingresa precisión y escala (ej: 10,2): ");
            int p, s;
            scanf("%d,%d", &p, &s);
            limpiarBuffer();
            sprintf(tipoDato, "DECIMAL(%d,%d)", p, s);
            break;
        default:
            printf("Opción inválida, usando INT por defecto\n");
            strcpy(tipoDato, "INT");
    }
    
    // Preguntar si acepta NULL
    printf("\n¿El campo debe aceptar valores NULL? (s/n): ");
    char acepta;
    scanf("%c", &acepta);
    limpiarBuffer();
    
    // Construir sentencia ALTER TABLE
    char query[500];
    sprintf(query, "ALTER TABLE %s ADD %s %s", 
            tablaSeleccionada, nombreCampo, tipoDato);
    
    if (acepta == 'n' || acepta == 'N') {
        strcat(query, " NOT NULL");
    }
    
    printf("\n=== SENTENCIA SQL ===\n");
    printf("%s\n", query);
    printf("=====================\n\n");
    
    // Ejecutar la alteración
    printf("Ejecutando alteración...\n");
    if (ejecutarComando(&conexion, query)) {
        printf("\n✓ ¡Campo '%s' agregado exitosamente a la tabla '%s'!\n\n", 
               nombreCampo, tablaSeleccionada);
    } else {
        printf("\n✗ Error al agregar el campo. Verifica que el nombre no exista ya.\n\n");
    }
}

void borrarCampoDeTabla() {
    char tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    
    printf("\n=== BORRAR CAMPO DE TABLA ===\n");
    
    // Listar tablas disponibles
    listarTablas(tablas, &numTablas);
    
    if (numTablas == 0) {
        printf("\nNo hay tablas disponibles.\n");
        return;
    }
    
    // Seleccionar tabla
    printf("\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    scanf("%d", &seleccion);
    limpiarBuffer();
    
    if (seleccion == 0) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    if (seleccion < 1 || seleccion > numTablas) {
        printf("\n✗ Selección inválida.\n");
        return;
    }
    
    char *tablaSeleccionada = tablas[seleccion - 1];
    printf("\n✓ Tabla seleccionada: %s\n", tablaSeleccionada);
    
    // Obtener columnas de la tabla
    ColumnaInfo columnas[MAX_CAMPOS];
    int numColumnas = 0;
    
    if (!obtenerColumnasTabla(tablaSeleccionada, columnas, &numColumnas)) {
        return;
    }
    
    // Mostrar columnas disponibles
    printf("\n=== CAMPOS DE LA TABLA ===\n");
    for (int i = 0; i < numColumnas; i++) {
        printf("%d. %s (%s", i + 1, columnas[i].nombre, columnas[i].tipo);
        if (columnas[i].longitud > 0) {
            printf("(%d)", columnas[i].longitud);
        }
        printf(")\n");
    }
    printf("==========================\n");
    
    // Seleccionar campo a borrar
    printf("\nSelecciona el número del campo a borrar (0 para cancelar): ");
    int campoSeleccion;
    scanf("%d", &campoSeleccion);
    limpiarBuffer();
    
    if (campoSeleccion == 0) {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    if (campoSeleccion < 1 || campoSeleccion > numColumnas) {
        printf("\n✗ Selección inválida.\n");
        return;
    }
    
    char *nombreCampo = columnas[campoSeleccion - 1].nombre;
    
    // Confirmación
    printf("\n⚠️  ¿Estás seguro de borrar el campo '%s'? (s/n): ", nombreCampo);
    char confirma;
    scanf("%c", &confirma);
    limpiarBuffer();
    
    if (confirma != 's' && confirma != 'S') {
        printf("\n✗ Operación cancelada.\n");
        return;
    }
    
    // Construir sentencia ALTER TABLE
    char query[500];
    sprintf(query, "ALTER TABLE %s DROP COLUMN %s", tablaSeleccionada, nombreCampo);
    
    printf("\n=== SENTENCIA SQL ===\n");
    printf("%s\n", query);
    printf("=====================\n\n");
    
    // Ejecutar la alteración
    printf("Ejecutando alteración...\n");
    if (ejecutarComando(&conexion, query)) {
        printf("\n✓ ¡Campo '%s' eliminado exitosamente de la tabla '%s'!\n\n", 
               nombreCampo, tablaSeleccionada);
    } else {
        printf("\n✗ Error al eliminar el campo. Verifica que no sea una columna clave.\n\n");
    }
}

void crearTabla() {
    char nombreTabla[MAX_NOMBRE];
    Campo campos[MAX_CAMPOS];
    int numCampos = 0;
    char continuar;
    
    printf("\n=== CREAR NUEVA TABLA ===\n");
    
    // Pedir nombre de la tabla
    printf("\nIngresa el nombre de la tabla: ");
    fgets(nombreTabla, sizeof(nombreTabla), stdin);
    nombreTabla[strcspn(nombreTabla, "\n")] = 0;
    
    printf("\nTabla: %s\n", nombreTabla);
    printf("\n--- Agregar Campos ---\n");
    
    // Loop para agregar campos
    while (numCampos < MAX_CAMPOS) {
        printf("\n[Campo %d]\n", numCampos + 1);
        
        // Pedir nombre del campo
        printf("Nombre del campo (o escribe 'salir' para terminar): ");
        fgets(campos[numCampos].nombre, sizeof(campos[numCampos].nombre), stdin);
        campos[numCampos].nombre[strcspn(campos[numCampos].nombre, "\n")] = 0;
        
        // Verificar si quiere salir
        if (strcmp(campos[numCampos].nombre, "salir") == 0 || 
            strcmp(campos[numCampos].nombre, "") == 0) {
            break;
        }
        
        // Mostrar tipos de datos disponibles
        mostrarTiposDatos();
        
        // Pedir tipo de dato
        printf("\nSelecciona el tipo de dato (1-7): ");
        int opcion;
        scanf("%d", &opcion);
        limpiarBuffer();
        
        // Asignar tipo según la opción
        switch (opcion) {
            case 1:
                strcpy(campos[numCampos].tipo, "INT");
                break;
            case 2:
                printf("Ingresa el tamaño (ej: 50): ");
                int tam;
                scanf("%d", &tam);
                limpiarBuffer();
                sprintf(campos[numCampos].tipo, "VARCHAR(%d)", tam);
                break;
            case 3:
                strcpy(campos[numCampos].tipo, "FLOAT");
                break;
            case 4:
                strcpy(campos[numCampos].tipo, "DATE");
                break;
            case 5:
                strcpy(campos[numCampos].tipo, "BOOLEAN");
                break;
            case 6:
                strcpy(campos[numCampos].tipo, "TEXT");
                break;
            case 7:
                printf("Ingresa precisión y escala (ej: 10,2): ");
                int p, s;
                scanf("%d,%d", &p, &s);
                limpiarBuffer();
                sprintf(campos[numCampos].tipo, "DECIMAL(%d,%d)", p, s);
                break;
            default:
                printf("Opción inválida, usando INT por defecto\n");
                strcpy(campos[numCampos].tipo, "INT");
        }
        
        printf("✓ Campo agregado: %s %s\n", campos[numCampos].nombre, campos[numCampos].tipo);
        numCampos++;
    }
    
    // Mostrar resumen y generar SQL
    if (numCampos == 0) {
        printf("\nNo se agregaron campos. Operación cancelada.\n");
        return;
    }
    
    printf("\n\n=== RESUMEN DE LA TABLA ===\n");
    printf("Tabla: %s\n", nombreTabla);
    printf("Campos:\n");
    
    for (int i = 0; i < numCampos; i++) {
        printf("  %d. %s - %s\n", i + 1, campos[i].nombre, campos[i].tipo);
    }
    
    // Generar sentencia SQL
    printf("\n=== SENTENCIA SQL GENERADA ===\n");
    printf("CREATE TABLE %s (\n", nombreTabla);
    
    for (int i = 0; i < numCampos; i++) {
        printf("    %s %s", campos[i].nombre, campos[i].tipo);
        if (i < numCampos - 1) {
            printf(",\n");
        } else {
            printf("\n");
        }
    }
    
    printf(");\n");
    printf("==============================\n\n");
    
    // Construir la consulta SQL
    char query[MAX_QUERY];
    sprintf(query, "CREATE TABLE %s (\n", nombreTabla);
    
    for (int i = 0; i < numCampos; i++) {
        char linea[200];
        sprintf(linea, "    %s %s", campos[i].nombre, campos[i].tipo);
        strcat(query, linea);
        
        if (i < numCampos - 1) {
            strcat(query, ",\n");
        } else {
            strcat(query, "\n");
        }
    }
    strcat(query, ")");
    
    // Ejecutar la consulta
    printf("Ejecutando consulta...\n");
    if (ejecutarComando(&conexion, query)) {
        printf("\n✓ ¡Tabla '%s' creada exitosamente en la base de datos!\n\n", nombreTabla);
    } else {
        printf("\n✗ Error al crear la tabla. Verifica que no exista ya.\n\n");
    }
}

int main() {
    system("chcp 65001 > nul");

    int opcion;
    
    // Configuración de conexión (hardcodeada)
    const char *servidor = "localhost";
    const char *baseDatos = "TestDB";
    
    // Conectar a la base de datos
    printf("Conectando a %s...\n", baseDatos);
    if (!conectarSQLTrusted(&conexion, servidor, baseDatos)) {
        printf("\n✗ No se pudo conectar. Verifica:\n");
        printf("  1. SQL Server está corriendo\n");
        printf("  2. El nombre de la base de datos es correcto\n");
        printf("  3. Tienes permisos de acceso\n");
        return 1;
    }
    printf("✓ Conexión exitosa\n\n");
    
    // Menú principal
    do {
        printf("\n-------------------------------------\n");
        printf("|      MENÚ GESTIÓN DE TABLAS     |\n");
        printf("-------------------------------------\n");
        printf("\n1. Crear nueva tabla\n");
        printf("2. Agregar campo a tabla existente\n");
        printf("3. Borrar campo de tabla existente\n");
        printf("4. Insertar datos en tabla existente\n");
        printf("5. Salir\n");
        printf("\nSelecciona una opción: ");
        
        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;  // Opción inválida
        } else {
            limpiarBuffer();
        }
        
        switch (opcion) {
            case 1:
                crearTabla();
                break;
            case 2:
                agregarCampoATabla();
                break;
            case 3:
                borrarCampoDeTabla();
                break;
            case 4:
                insertarDatosEnTabla();
                break;
            case 5:
                printf("\n¡Hasta luego!\n");
                break;
            default:
                printf("\nOpción inválida. Intenta de nuevo.\n");
        }
        
    } while (opcion != 5);
    
    // Desconectar
    desconectarSQL(&conexion);
    
    return 0;
}
