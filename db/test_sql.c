#include <stdio.h>
#include "sql_connection.h"

/*
 * USO RÁPIDO:
 * 1. Cambia "localhost" por tu servidor si es diferente
 * 2. Cambia "TestDB" por el nombre de tu base de datos
 * 3. Cambia la consulta SQL por la que necesites
 * 4. Compila: gcc -o test_sql test_sql.c sql_connection.c -lodbc32
 * 5. Ejecuta: ./test_sql.exe
 */

int main() {
    SQLConnection conn;
    SQLResult *result;
    
    // ============================================
    // CONFIGURA AQUÍ TU CONEXIÓN
    // ============================================
    const char *servidor = "localhost";          // O "localhost\\SQLEXPRESS"
    const char *baseDatos = "TestDB";            // Tu base de datos
    
    // ============================================
    // CONECTAR (Autenticación Windows)
    // ============================================
    printf("Conectando a %s...\n", baseDatos);
    
    if (!conectarSQLTrusted(&conn, servidor, baseDatos)) {
        printf("\nNo se pudo conectar. Verifica:\n");
        printf("  1. SQL Server está corriendo\n");
        printf("  2. El nombre de la base de datos es correcto\n");
        printf("  3. Tienes permisos de acceso\n");
        return 1;
    } else {
        printf("Conexión exitosa\n");
        //return 0;
    }
    
    // ============================================
    // EJECUTAR CONSULTA
    // ============================================
    printf("\nEjecutando consulta...\n");
    
    // CAMBIA ESTA CONSULTA POR LA QUE NECESITES
    result = ejecutarConsulta(&conn, "SELECT TOP 10 * FROM sys.tables");
    
    if (result != NULL) {
        printf("\n=== RESULTADOS ===\n");
        mostrarResultado(result);
        
        // Ejemplo: Acceder a datos específicos
        printf("\nAcceso manual a datos:\n");
        printf("  Total de filas: %d\n", result->numRows);
        printf("  Total de columnas: %d\n", result->numCols);
        
        if (result->numRows > 0) {
            printf("  Primera celda [0][0]: %s\n", result->data[0][0]);
        }
        
        // Ejemplo: Recorrer todas las filas
        printf("\nRecorriendo datos:\n");
        for (int i = 0; i < result->numRows; i++) {
            printf("  Fila %d: ", i);
            for (int j = 0; j < result->numCols; j++) {
                printf("%s ", result->data[i][j]);
            }
            printf("\n");
        }
        
        liberarResultado(result);
    }
    
    // ============================================
    // EJEMPLO DE INSERT (Descomentalo si necesitas)
    // ============================================
    /*
    printf("\nEjecutando INSERT...\n");
    if (ejecutarComando(&conn, "INSERT INTO MiTabla (Columna1) VALUES ('Valor')")) {
        printf("INSERT exitoso\n");
    }
    */
    
    // ============================================
    // EJEMPLO DE UPDATE (Descomentalo si necesitas)
    // ============================================
    /*
    printf("\nEjecutando UPDATE...\n");
    if (ejecutarComando(&conn, "UPDATE MiTabla SET Columna1 = 'NuevoValor' WHERE Id = 1")) {
        printf("UPDATE exitoso\n");
    }
    */
    
    // ============================================
    // DESCONECTAR
    // ============================================
    desconectarSQL(&conn);
    
    printf("\n¡Listo!\n");
    return 0;
}
