#ifndef SQL_CONNECTION_H
#define SQL_CONNECTION_H

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

// Estructura para almacenar la conexión
typedef struct {
    SQLHENV hEnv;      // Handle del entorno
    SQLHDBC hDbc;      // Handle de la conexión
    SQLHSTMT hStmt;    // Handle del statement
    int isConnected;
} SQLConnection;

// Estructura para almacenar resultados
typedef struct {
    char ***data;      // Array 2D de strings [fila][columna]
    int numRows;       // Número de filas
    int numCols;       // Número de columnas
    char **colNames;   // Nombres de las columnas
} SQLResult;

// Funciones de conexión
int conectarSQL(SQLConnection *conn, const char *server, const char *database, 
                const char *username, const char *password);
int conectarSQLTrusted(SQLConnection *conn, const char *server, const char *database);
void desconectarSQL(SQLConnection *conn);

// Funciones de consulta
SQLResult* ejecutarConsulta(SQLConnection *conn, const char *query);
int ejecutarComando(SQLConnection *conn, const char *comando);

// Funciones de utilidad
void liberarResultado(SQLResult *result);
void mostrarResultado(SQLResult *result);
void mostrarError(SQLSMALLINT handleType, SQLHANDLE handle);

#endif // SQL_CONNECTION_H
