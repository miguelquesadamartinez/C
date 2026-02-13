
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
    wchar_t ***data;      // Array 2D de strings [fila][columna]
    int numRows;       // Número de filas
    int numCols;       // Número de columnas
    wchar_t **colNames;   // Nombres de las columnas
} SQLResult;

// Funciones de conexión
int conectarSQL(SQLConnection *conn, const wchar_t *server, const wchar_t *database, 
                const wchar_t *username, const wchar_t *password);
int conectarSQLTrusted(SQLConnection *conn, const wchar_t *server, const wchar_t *database);
void desconectarSQL(SQLConnection *conn);

// Funciones de consulta
SQLResult* ejecutarConsulta(SQLConnection *conn, const wchar_t *query);
int ejecutarComando(SQLConnection *conn, const wchar_t *comando);

// Funciones de utilidad
void liberarResultado(SQLResult *result);
void mostrarResultado(SQLResult *result);
void mostrarError(SQLSMALLINT handleType, SQLHANDLE handle);

#endif // SQL_CONNECTION_H
