#include "miguel.h"
#include "sql_connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// Muestra errores detallados de SQL
void mostrarError(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLWCHAR sqlState[6];
    SQLWCHAR message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT messageLength;
    
    wprintf(L"\n[ERROR SQL]\n");
    
    int i = 1;
    while (SQLGetDiagRecW(handleType, handle, i, sqlState, &nativeError, 
                         message, SQL_MAX_MESSAGE_LENGTH, &messageLength) == SQL_SUCCESS) {
        wprintf(L"  Estado SQL: %ls\n", sqlState);
        wprintf(L"  Error nativo: %d\n", nativeError);
        wprintf(L"  Mensaje: %ls\n", message);
        i++;
    }
}

// Conecta a SQL Server con usuario y contraseña
// Removed duplicate char* version, only wchar_t* version below
int conectarSQL(SQLConnection *conn, const wchar_t *server, const wchar_t *database, const wchar_t *username, const wchar_t *password) {
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
    wchar_t connString[512];
    swprintf(connString, 512,
             L"DRIVER={SQL Server};SERVER=%ls;DATABASE=%ls;UID=%ls;PWD=%ls;",
             server, database, username, password);
    // Conectar
    ret = SQLDriverConnectW(conn->hDbc, NULL, connString, SQL_NTS,
                          NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        wprintf(L"✓ Conectado exitosamente a la base de datos '%ls'\n", database);
        conn->isConnected = 1;
        return 1;
    } else {
        wprintf(L"✗ Error al conectar a la base de datos\n");
        mostrarError(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, conn->hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, conn->hEnv);
        return 0;
    }
}

// Conecta a SQL Server con autenticación de Windows (Trusted)
int conectarSQLTrusted(SQLConnection *conn, const wchar_t *server, const wchar_t *database) {
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
    wchar_t connString[512];
    swprintf(connString, 512,
             L"DRIVER={SQL Server};SERVER=%ls;DATABASE=%ls;Trusted_Connection=yes;",
             server, database);
    // Conectar
    ret = SQLDriverConnectW(conn->hDbc, NULL, connString, SQL_NTS,
                          NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        wprintf(L"✓ Conectado exitosamente a la base de datos '%ls'\n", database);
        conn->isConnected = 1;
        return 1;
    } else {
        wprintf(L"✗ Error al conectar a la base de datos\n");
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
    wprintf(L"✓ Desconectado de la base de datos\n");
}

// Ejecuta una consulta SELECT y retorna los resultados
SQLResult* ejecutarConsulta(SQLConnection *conn, const wchar_t *query) {
    if (!conn->isConnected) {
        wprintf(L"Error: No hay conexión activa\n");
        return NULL;
    }
    
    SQLRETURN ret;
    SQLHSTMT hStmt;
    
    // Asignar handle de statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, conn->hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        wprintf(L"Error al asignar handle de statement\n");
        return NULL;
    }
    
    // Ejecutar consulta
    ret = SQLExecDirectW(hStmt, (SQLWCHAR*)query, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        wprintf(L"Error al ejecutar la consulta\n");
        mostrarError(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return NULL;
    }
    
    // Obtener número de columnas
    SQLSMALLINT numCols;
    SQLNumResultCols(hStmt, &numCols);
    
    if (numCols == 0) {
        wprintf(L"La consulta no retornó columnas\n");
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return NULL;
    }
    
    // Crear estructura de resultado
    SQLResult *result = (SQLResult*)malloc(sizeof(SQLResult));
    result->numCols = numCols;
    result->numRows = 0;
    
    // Obtener nombres de columnas
    result->colNames = (wchar_t**)malloc(numCols * sizeof(wchar_t*));
    for (int i = 0; i < numCols; i++) {
        SQLWCHAR colName[256];
        SQLSMALLINT nameLen;
        SQLDescribeColW(hStmt, i + 1, colName, sizeof(colName)/sizeof(SQLWCHAR), &nameLen, NULL, NULL, NULL, NULL);
        result->colNames[i] = _wcsdup((wchar_t*)colName);
    }
    
    // Contar filas primero
    int capacity = 10;
    result->data = (wchar_t***)malloc(capacity * sizeof(wchar_t**));
    
    while ((ret = SQLFetch(hStmt)) == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        // Redimensionar si es necesario
        if (result->numRows >= capacity) {
            capacity *= 2;
            result->data = (wchar_t***)realloc(result->data, capacity * sizeof(wchar_t**));
        }
        
        // Asignar memoria para la fila
        result->data[result->numRows] = (wchar_t**)malloc(numCols * sizeof(wchar_t*));
        
        // Leer cada columna
        for (int i = 0; i < numCols; i++) {
            SQLWCHAR buffer[1024];
            SQLLEN indicator;
            
            ret = SQLGetData(hStmt, i + 1, SQL_C_WCHAR, buffer, sizeof(buffer), &indicator);
            
            if (indicator == SQL_NULL_DATA) {
                result->data[result->numRows][i] = _wcsdup(L"NULL");
            } else {
                result->data[result->numRows][i] = _wcsdup((wchar_t*)buffer);
            }
        }
        
        result->numRows++;
    }
    
    // Ajustar tamaño al número real de filas
    if (result->numRows > 0) {
        result->data = (wchar_t***)realloc(result->data, result->numRows * sizeof(wchar_t**));
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    
    wprintf(L"✓ Consulta ejecutada: %d filas obtenidas\n - SQL: %ls\n", result->numRows, query);
    return result;
}

// Ejecuta un comando (INSERT, UPDATE, DELETE, etc.)
int ejecutarComando(SQLConnection *conn, const wchar_t *comando) {
    if (!conn->isConnected) {
        wprintf(L"Error: No hay conexión activa\n");
        return 0;
    }
    
    SQLRETURN ret;
    SQLHSTMT hStmt;
    
    // Asignar handle de statement
    ret = SQLAllocHandle(SQL_HANDLE_STMT, conn->hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        wprintf(L"Error al asignar handle de statement\n");
        return 0;
    }
    
    // Ejecutar comando
    ret = SQLExecDirectW(hStmt, (SQLWCHAR*)comando, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        wprintf(L"Error al ejecutar el comando\n");
        mostrarError(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return 0;
    }
    
    // Obtener número de filas afectadas
    SQLLEN rowCount;
    SQLRowCount(hStmt, &rowCount);
    
    wprintf(L"✓ Comando ejecutado: %lld  fila(s) afectada(s)\n - SQL: %ls\n", rowCount, comando);
    
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
        wprintf(L"No hay datos para mostrar\n");
        return;
    }
    
    // Mostrar encabezados
    wprintf(L"\n");
    for (int i = 0; i < result->numCols; i++) {
        wprintf(L"%-20ls", result->colNames[i]);
    }
    wprintf(L"\n");
    
    // Línea separadora
    for (int i = 0; i < result->numCols; i++) {
        wprintf(L"%-20ls", L"--------------------");
    }
    wprintf(L"\n");
    
    // Mostrar datos
    for (int i = 0; i < result->numRows; i++) {
        for (int j = 0; j < result->numCols; j++) {
            wprintf(L"%-20ls", result->data[i][j]);
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}
