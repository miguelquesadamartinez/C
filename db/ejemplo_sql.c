#include <stdio.h>
#include "sql_connection.h"

int main() {
    SQLConnection conn;
    SQLResult *result;
    
    printf("=== EJEMPLO DE CONEXIÓN A SQL SERVER ===\n\n");
    
    // ============================================
    // OPCIÓN 1: Autenticación de Windows (Trusted)
    // ============================================
    printf("Conectando con autenticación de Windows...\n");
    if (conectarSQLTrusted(&conn, "localhost", "TestDB")) {
        
        // Ejemplo 1: Consulta SELECT simple
        printf("\n--- Ejemplo 1: SELECT simple ---\n");
        result = ejecutarConsulta(&conn, "SELECT * FROM Usuarios");
        if (result != NULL) {
            mostrarResultado(result);
            liberarResultado(result);
        }
        
        // Ejemplo 2: Consulta con WHERE
        printf("\n--- Ejemplo 2: SELECT con WHERE ---\n");
        result = ejecutarConsulta(&conn, 
            "SELECT Nombre, Email FROM Usuarios WHERE Edad > 25");
        if (result != NULL) {
            mostrarResultado(result);
            
            // Acceder a datos específicos
            printf("Primer nombre: %s\n", result->data[0][0]);
            printf("Primer email: %s\n", result->data[0][1]);
            
            liberarResultado(result);
        }
        
        // Ejemplo 3: INSERT
        printf("\n--- Ejemplo 3: INSERT ---\n");
        ejecutarComando(&conn, 
            "INSERT INTO Usuarios (Nombre, Email, Edad) VALUES ('Juan', 'juan@email.com', 30)");
        
        // Ejemplo 4: UPDATE
        printf("\n--- Ejemplo 4: UPDATE ---\n");
        ejecutarComando(&conn, 
            "UPDATE Usuarios SET Edad = 31 WHERE Nombre = 'Juan'");
        
        // Ejemplo 5: DELETE
        printf("\n--- Ejemplo 5: DELETE ---\n");
        ejecutarComando(&conn, 
            "DELETE FROM Usuarios WHERE Nombre = 'Juan'");
        
        desconectarSQL(&conn);
    }
    
    printf("\n");
    
    // ============================================
    // OPCIÓN 2: Autenticación con usuario y contraseña
    // ============================================
    printf("Conectando con usuario y contraseña...\n");
    if (conectarSQL(&conn, "localhost", "TestDB", "sa", "1804")) {
        
        result = ejecutarConsulta(&conn, "SELECT TOP 5 * FROM Usuarios");
        if (result != NULL) {
            mostrarResultado(result);
            liberarResultado(result);
        }
        
        desconectarSQL(&conn);
    }
    
    return 0;
}
