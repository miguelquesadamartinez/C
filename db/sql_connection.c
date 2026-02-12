#include "sql_connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Muestra errores detallados de SQL
void mostrarError(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLCHAR sqlState[6];
    SQLCHAR message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT messageLength;
    
    printf("\n[ERROR SQL]\n");
    
    int i = 1;
    while (SQLGetDiagRec(handleType, handle, i, sqlState, &nativeError, 
                         message, sizeof(message), &messageLength) == SQL_SUCCESS) {
        printf("  Estado SQL: %s\n", sqlState);
        printf("  Error nativo: %d\n", nativeError);
        printf("  Mensaje: %s\n", message);
        i++;
    }
}

// Conecta a SQL Server con usuario y contraseña
int conectarSQL(SQLConnection *conn, const char *server, const char *database, 
                const char *username, const char *password) {
    SQLRETURN ret;
    
    // Inicializar estructura
    conn->isConnected = 0;
    
    // Asignar handle de entorno
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &conn->hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de entorno\n");
        return 0;
    }
    
    // Establecer versión ODBC
    ret = SQLSetEnvAttr(conn->hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al establecer versión ODBC\n");
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
    
    // Asignar handle de conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, conn->hEnv, &conn->hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de conexión\n");
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
    
    // Crear cadena de conexión
    char connString[512];
    snprintf(connString, sizeof(connString),
             "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;",
             server, database, username, password);
    
    // Conectar
    ret = SQLDriverConnect(conn->hDbc, NULL, (SQLCHAR*)connString, SQL_NTS,
                          NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        printf("✓ Conectado exitosamente a la base de datos '%s'\n", database);
        conn->isConnected = 1;
        return 1;
    } else {
        printf("✗ Error al conectar a la base de datos\n");
        mostrarError(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
}

// Conecta a SQL Server con autenticación de Windows (Trusted)
int conectarSQLTrusted(SQLConnection *conn, const char *server, const char *database) {
    SQLRETURN ret;
    
    // Inicializar estructura
    conn->isConnected = 0;
    
    // Asignar handle de entorno
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &conn->hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de entorno\n");
        return 0;
    }
    
    // Establecer versión ODBC
    ret = SQLSetEnvAttr(conn->hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al establecer versión ODBC\n");
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
    
    // Asignar handle de conexión
    ret = SQLAllocHandle(SQL_HANDLE_DBC, conn->hEnv, &conn->hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de conexión\n");
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
    
    // Crear cadena de conexión con Trusted Connection
    char connString[512];
    snprintf(connString, sizeof(connString),
             "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;Trusted_Connection=yes;",
             server, database);
    
    // Conectar
    ret = SQLDriverConnect(conn->hDbc, NULL, (SQLCHAR*)connString, SQL_NTS,
                          NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        printf("✓ Conectado exitosamente a la base de datos '%s'\n", database);
        conn->isConnected = 1;
        return 1;
    } else {
        printf("✗ Error al conectar a la base de datos\n");
        mostrarError(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
}

// Desconecta y libera recursos
void desconectarSQL(SQLConnection *conn) {
    if (!conn->isConnected) return;
    
    SQLDisconnect(conn->hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, conn->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
    
    conn->isConnected = 0;
    printf("✓ Desconectado de la base de datos\n");
}

// Ejecuta una consulta SELECT y retorna los resultados
SQLResult* ejecutarConsulta(SQLConnection *conn, const char *query) {
    if (!conn->isConnected) {
        printf("Error: No hay conexión activa\n");
        return NULL;
    }
    
    SQLRETURN ret;
    SQLHSTMT hStmt;
    
    // Asignar handle de statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, conn->hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de statement\n");
        return NULL;
    }
    
    // Ejecutar consulta
    ret = SQLExecDirect(hStmt, (SQLCHAR*)query, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al ejecutar la consulta\n");
        mostrarError(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return NULL;
    }
    
    // Obtener número de columnas
    SQLSMALLINT numCols;
    SQLNumResultCols(hStmt, &numCols);
    
    if (numCols == 0) {
        printf("La consulta no retornó columnas\n");
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return NULL;
    }
    
    // Crear estructura de resultado
    SQLResult *result = (SQLResult*)malloc(sizeof(SQLResult));
    result->numCols = numCols;
    result->numRows = 0;
    
    // Obtener nombres de columnas
    result->colNames = (char**)malloc(numCols * sizeof(char*));
    for (int i = 0; i < numCols; i++) {
        SQLCHAR colName[256];
        SQLSMALLINT nameLen;
        SQLDescribeCol(hStmt, i + 1, colName, sizeof(colName), &nameLen, NULL, NULL, NULL, NULL);
        result->colNames[i] = _strdup((char*)colName);
    }
    
    // Contar filas primero
    int capacity = 10;
    result->data = (char***)malloc(capacity * sizeof(char**));
    
    while ((ret = SQLFetch(hStmt)) == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        // Redimensionar si es necesario
        if (result->numRows >= capacity) {
            capacity *= 2;
            result->data = (char***)realloc(result->data, capacity * sizeof(char**));
        }
        
        // Asignar memoria para la fila
        result->data[result->numRows] = (char**)malloc(numCols * sizeof(char*));
        
        // Leer cada columna
        for (int i = 0; i < numCols; i++) {
            SQLCHAR buffer[1024];
            SQLLEN indicator;
            
            ret = SQLGetData(hStmt, i + 1, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);
            
            if (indicator == SQL_NULL_DATA) {
                result->data[result->numRows][i] = _strdup("NULL");
            } else {
                result->data[result->numRows][i] = _strdup((char*)buffer);
            }
        }
        
        result->numRows++;
    }
    
    // Ajustar tamaño al número real de filas
    if (result->numRows > 0) {
        result->data = (char***)realloc(result->data, result->numRows * sizeof(char**));
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    
    printf("✓ Consulta ejecutada: %d filas obtenidas\n", result->numRows);
    return result;
}

// Ejecuta un comando (INSERT, UPDATE, DELETE, etc.)
int ejecutarComando(SQLConnection *conn, const char *comando) {
    if (!conn->isConnected) {
        printf("Error: No hay conexión activa\n");
        return 0;
    }
    
    SQLRETURN ret;
    SQLHSTMT hStmt;
    
    // Asignar handle de statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, conn->hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al asignar handle de statement\n");
        return 0;
    }
    
    // Ejecutar comando
    ret = SQLExecDirect(hStmt, (SQLCHAR*)comando, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        printf("Error al ejecutar el comando\n");
        mostrarError(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return 0;
    }
    
    // Obtener número de filas afectadas
    SQLLEN rowCount;
    SQLRowCount(hStmt, &rowCount);
    
    printf("✓ Comando ejecutado: %lld fila(s) afectada(s)\n", rowCount);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 1;
}

// Libera la memoria de un resultado
void liberarResultado(SQLResult *result) {
    if (result == NULL) return;
    
    // Liberar datos
    for (int i = 0; i < result->numRows; i++) {
        for (int j = 0; j < result->numCols; j++) {
            free(result->data[i][j]);
        }
        free(result->data[i]);
    }
    free(result->data);
    
    // Liberar nombres de columnas
    for (int i = 0; i < result->numCols; i++) {
        free(result->colNames[i]);
    }
    free(result->colNames);
    
    free(result);
}

// Muestra el resultado en formato tabla
void mostrarResultado(SQLResult *result) {
    if (result == NULL || result->numRows == 0) {
        printf("No hay datos para mostrar\n");
        return;
    }
    
    // Mostrar encabezados
    printf("\n");
    for (int i = 0; i < result->numCols; i++) {
        printf("%-20s", result->colNames[i]);
    }
    printf("\n");
    
    // Línea separadora
    for (int i = 0; i < result->numCols; i++) {
        printf("%-20s", "--------------------");
    }
    printf("\n");
    
    // Mostrar datos
    for (int i = 0; i < result->numRows; i++) {
        for (int j = 0; j < result->numCols; j++) {
            printf("%-20s", result->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
