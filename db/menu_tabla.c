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
        printf("\n-----------------------------------\n");
        printf("|    MENÚ GESTIÓN DE TABLAS     |\n");
        printf("-----------------------------------\n");
        printf("\n1. Crear nueva tabla\n");
        printf("2. Salir\n");
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
                printf("\n¡Hasta luego!\n");
                break;
            default:
                printf("\nOpción inválida. Intenta de nuevo.\n");
        }
        
    } while (opcion != 2);
    
    // Desconectar
    desconectarSQL(&conexion);
    
    return 0;
}
