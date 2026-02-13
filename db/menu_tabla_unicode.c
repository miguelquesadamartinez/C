#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "sql_connection_unicode.h"
#include <time.h>
#include <miguel.h>

#define MAX_CAMPOS 50
#define MAX_NOMBRE 100
#define MAX_QUERY 20000

// Conexión SQL global
SQLConnection conexion;

typedef struct {
    wchar_t nombre[MAX_NOMBRE];
    wchar_t tipo[MAX_NOMBRE];
} Campo;

typedef struct {
    wchar_t nombre[MAX_NOMBRE];
    wchar_t tipo[MAX_NOMBRE];
    int longitud;
    int esNulable;
} ColumnaInfo;

void mostrarTiposDatos() {
    wprintf(L"\n=== TIPOS DE DATOS DISPONIBLES ===\n");
    wprintf(L"1. INT           - Número entero\n");
    wprintf(L"2. VARCHAR(n)    - Texto variable (especifica n)\n");
    wprintf(L"3. FLOAT         - Número decimal\n");
    wprintf(L"4. DATE          - Fecha\n");
    wprintf(L"5. BOOLEAN       - Verdadero/Falso\n");
    wprintf(L"6. TEXT          - Texto largo\n");
    wprintf(L"7. DECIMAL(p,s)  - Decimal con precisión\n");
    wprintf(L"==================================\n");
}

void listarTablas(wchar_t tablas[][MAX_NOMBRE], int *numTablas) {
    *numTablas = 0;
    // Consulta para obtener tablas del usuario (excluyendo tablas del sistema)
    const wchar_t *query = L"SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES "
                          L"WHERE TABLE_TYPE = 'BASE TABLE' AND TABLE_CATALOG = DB_NAME() "
                          L"ORDER BY TABLE_NAME";
    SQLResult *resultado = ejecutarConsulta(&conexion, query);
    if (resultado == NULL || resultado->numRows == 0) {
        wprintf(L"\nNo hay tablas disponibles en la base de datos.\n");
        if (resultado) liberarResultado(resultado);
        return;
    }
    wprintf(L"\n=== TABLAS DISPONIBLES ===\n");
    for (int i = 0; i < resultado->numRows && i < MAX_CAMPOS; i++) {
        wcscpy(tablas[i], resultado->data[i][0]);
        wprintf(L"%d. %ls\n", i + 1, tablas[i]);
        (*numTablas)++;
    }
    wprintf(L"==========================\n");
    liberarResultado(resultado);
}

int obtenerColumnasTabla(const wchar_t *nombreTabla, ColumnaInfo columnas[], int *numColumnas) {
    *numColumnas = 0;
    // Consulta para obtener información de columnas
    wchar_t query[500];
    swprintf(query, 500,
        L"SELECT COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, IS_NULLABLE "
        L"FROM INFORMATION_SCHEMA.COLUMNS "
        L"WHERE TABLE_NAME = '%ls' "
        L"ORDER BY ORDINAL_POSITION",
        nombreTabla);
    SQLResult *resultado = ejecutarConsulta(&conexion, query);
    if (resultado == NULL || resultado->numRows == 0) {
        wprintf(L"\n✗ No se pudieron obtener las columnas de la tabla.\n");
        if (resultado) liberarResultado(resultado);
        return 0;
    }
    for (int i = 0; i < resultado->numRows && i < MAX_CAMPOS; i++) {
        wcscpy(columnas[i].nombre, resultado->data[i][0]);
        wcscpy(columnas[i].tipo, resultado->data[i][1]);
        // Longitud (puede ser NULL)
        if (resultado->data[i][2] && wcslen(resultado->data[i][2]) > 0) {
            columnas[i].longitud = _wtoi(resultado->data[i][2]);
        } else {
            columnas[i].longitud = 0;
        }
        // Es nullable
        columnas[i].esNulable = (wcscmp(resultado->data[i][3], L"YES") == 0) ? 1 : 0;
        
        (*numColumnas)++;
    }
    
    liberarResultado(resultado);
    return 1;
}

void insertarDatosEnTabla() {
    int seleccion = 0;
    wchar_t tablas[MAX_CAMPOS][MAX_NOMBRE];
    listarTablas(tablas, &seleccion);
    if (seleccion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    wchar_t *tablaSeleccionada = tablas[seleccion - 1];
    // Confirmación antes de continuar
    wprintf(L"\n¿Desea insertar datos en esta tabla? (1=Sí, 0=Cancelar): ");
    int confirmar;
    wscanf(L"%d", &confirmar);
    limpiarBuffer();
    if (confirmar != 1) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    // Obtener columnas de la tabla
    ColumnaInfo columnas[MAX_CAMPOS];
    int numColumnas = 0;
    if (!obtenerColumnasTabla(tablaSeleccionada, columnas, &numColumnas)) {
        return;
    }
    wprintf(L"\n--- FORMULARIO DE DATOS ---\n");
    wprintf(L"(Presiona Enter para dejar NULL en campos opcionales)\n\n");
    // Arrays para almacenar los valores ingresados
    wchar_t valores[MAX_CAMPOS][500];
    int esNulo[MAX_CAMPOS];
    // Recopilar datos de cada columna
    for (int i = 0; i < numColumnas; i++) {
        wprintf(L"[%ls] (%ls", columnas[i].nombre, columnas[i].tipo);
        if (columnas[i].longitud > 0) {
            wprintf(L"(%d)", columnas[i].longitud);
        }
        if (columnas[i].esNulable) {
            wprintf(L" - opcional");
        }
        wprintf(L")\n");
        wprintf(L"Valor: ");
        leerLineaUnicode(valores[i], 500);
        // Verificar si dejó el campo vacío
        if (wcslen(valores[i]) == 0) {
            if (columnas[i].esNulable) {
                esNulo[i] = 1;
                wprintf(L"  → NULL\n");
            } else {
                wprintf(L"  ✗ Este campo es obligatorio. Ingresa un valor: ");
                leerLineaUnicode(valores[i], 500);
                esNulo[i] = 0;
            }
        } else {
            esNulo[i] = 0;
        }
    }
    wchar_t nombresColumnas[1000] = L"";
    wchar_t valoresSQL[1000] = L"";
    // Construir lista de nombres de columnas
    for (int i = 0; i < numColumnas; i++) {
        wcscat(nombresColumnas, columnas[i].nombre);
        if (i < numColumnas - 1) {
            wcscat(nombresColumnas, L", ");
        }
    }
    // Construir lista de valores
    for (int i = 0; i < numColumnas; i++) {
        if (esNulo[i]) {
            wcscat(valoresSQL, L"NULL");
        } else {
            // Determinar si necesita comillas (tipos de texto)
            if (wcsstr(columnas[i].tipo, L"char") != NULL || 
                wcsstr(columnas[i].tipo, L"text") != NULL ||
                wcsstr(columnas[i].tipo, L"date") != NULL) {
                wcscat(valoresSQL, L"'");
                // Escapar comillas simples
                for (int k = 0; valores[i][k] != L'\0'; k++) {
                    if (valores[i][k] == L'\'') {
                        wcscat(valoresSQL, L"''");
                    } else {
                        size_t len2 = wcslen(valoresSQL);
                        valoresSQL[len2] = valores[i][k];
                        valoresSQL[len2 + 1] = L'\0';
                    }
                }
                wcscat(valoresSQL, L"'");
            } else {
                wcscat(valoresSQL, valores[i]);
            }
        }
        if (i < numColumnas - 1) {
            wcscat(valoresSQL, L", ");
        }
    }
    wchar_t query[MAX_QUERY];
    swprintf(query, MAX_QUERY, L"INSERT INTO %ls (%ls) VALUES (%ls)", tablaSeleccionada, nombresColumnas, valoresSQL);
    if (ejecutarComando(&conexion, query)) {
        wprintf(L"\n✓ Datos insertados correctamente.\n");
    } else {
        wprintf(L"\n✗ Error al insertar datos.\n");
    }
// (Removed unreachable/duplicate code using narrow string functions)
    
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
    wchar_t tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    wprintf(L"\n=== AGREGAR CAMPO A TABLA ===\n");
    listarTablas(tablas, &numTablas);
    if (numTablas == 0) {
        wprintf(L"\nNo hay tablas disponibles. Crea una tabla primero.\n");
        return;
    }
    wprintf(L"\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    wscanf(L"%d", &seleccion);
    limpiarBuffer();
    if (seleccion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    if (seleccion < 1 || seleccion > numTablas) {
        wprintf(L"\n✗ Selección inválida.\n");
        return;
    }
    wchar_t *tablaSeleccionada = tablas[seleccion - 1];
    wprintf(L"\n✓ Tabla seleccionada: %ls\n", tablaSeleccionada);
    wprintf(L"\n¿Desea agregar un campo a esta tabla? (1=Sí, 0=Cancelar): ");
    int confirmar;
    wscanf(L"%d", &confirmar);
    limpiarBuffer();
    if (confirmar != 1) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    wchar_t nombreCampo[MAX_NOMBRE];
    wprintf(L"\n--- NUEVO CAMPO ---\n");
    wprintf(L"Nombre del campo: ");
    leerLineaUnicode(nombreCampo, MAX_NOMBRE);
    if (wcslen(nombreCampo) == 0) {
        wprintf(L"\n✗ Nombre de campo inválido.\n");
        return;
    }
    mostrarTiposDatos();
    wprintf(L"\nSelecciona el tipo de dato (1-7, 0 para cancelar): ");
    int opcion;
    wscanf(L"%d", &opcion);
    limpiarBuffer();
    if (opcion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    wchar_t tipoDato[MAX_NOMBRE];
    switch (opcion) {
        case 1:
            wcscpy(tipoDato, L"INT");
            break;
        case 2: {
            wprintf(L"Ingresa el tamaño (ej: 50): ");
            int tam;
            wscanf(L"%d", &tam);
            limpiarBuffer();
            swprintf(tipoDato, MAX_NOMBRE, L"VARCHAR(%d)", tam);
            break;
        }
        case 3:
            wcscpy(tipoDato, L"FLOAT");
            break;
        case 4:
            wcscpy(tipoDato, L"DATE");
            break;
        case 5:
            wcscpy(tipoDato, L"BIT");
            break;
        case 6:
            wcscpy(tipoDato, L"TEXT");
            break;
        case 7: {
            wprintf(L"Ingresa precisión y escala (ej: 10,2): ");
            int p, s;
            wscanf(L"%d,%d", &p, &s);
            limpiarBuffer();
            swprintf(tipoDato, MAX_NOMBRE, L"DECIMAL(%d,%d)", p, s);
            break;
        }
        default:
            wprintf(L"Opción inválida, usando INT por defecto\n");
            wcscpy(tipoDato, L"INT");
    }
    wprintf(L"\n¿El campo debe aceptar valores NULL? (s/n): ");
    wchar_t acepta;
    wscanf(L" %lc", &acepta);
    limpiarBuffer();
    wchar_t query[500];
    swprintf(query, 500, L"ALTER TABLE %ls ADD %ls %ls", tablaSeleccionada, nombreCampo, tipoDato);
    if (acepta == L'n' || acepta == L'N') {
        wcscat(query, L" NOT NULL");
    }
    wprintf(L"\n=== SENTENCIA SQL ===\n");
    wprintf(L"%ls\n", query);
    wprintf(L"=====================\n\n");
    wprintf(L"Ejecutando alteración...\n");
    if (ejecutarComando(&conexion, query)) {
        wprintf(L"\n✓ ¡Campo '%ls' agregado exitosamente a la tabla '%ls'!\n\n", nombreCampo, tablaSeleccionada);
    } else {
        wprintf(L"\n✗ Error al agregar el campo. Verifica que el nombre no exista ya.\n\n");
    }
}

void borrarCampoDeTabla() {
    wchar_t tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    wprintf(L"\n=== BORRAR CAMPO DE TABLA ===\n");
    listarTablas(tablas, &numTablas);
    if (numTablas == 0) {
        wprintf(L"\nNo hay tablas disponibles.\n");
        return;
    }
    wprintf(L"\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    wscanf(L"%d", &seleccion);
    limpiarBuffer();
    if (seleccion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    if (seleccion < 1 || seleccion > numTablas) {
        wprintf(L"\n✗ Selección inválida.\n");
        return;
    }
    wchar_t *tablaSeleccionada = tablas[seleccion - 1];
    wprintf(L"\n✓ Tabla seleccionada: %ls\n", tablaSeleccionada);
    ColumnaInfo columnas[MAX_CAMPOS];
    int numColumnas = 0;
    if (!obtenerColumnasTabla(tablaSeleccionada, columnas, &numColumnas)) {
        return;
    }
    wprintf(L"\n=== CAMPOS DE LA TABLA ===\n");
    for (int i = 0; i < numColumnas; i++) {
        wprintf(L"%d. %ls (%ls", i + 1, columnas[i].nombre, columnas[i].tipo);
        if (columnas[i].longitud > 0) {
            wprintf(L"(%d)", columnas[i].longitud);
        }
        wprintf(L")\n");
    }
    wprintf(L"==========================\n");
    wprintf(L"\nSelecciona el número del campo a borrar (0 para cancelar): ");
    int campoSeleccion;
    wscanf(L"%d", &campoSeleccion);
    limpiarBuffer();
    if (campoSeleccion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    if (campoSeleccion < 1 || campoSeleccion > numColumnas) {
        wprintf(L"\n✗ Selección inválida.\n");
        return;
    }
    wchar_t *nombreCampo = columnas[campoSeleccion - 1].nombre;
    wprintf(L"\n⚠️  ¿Estás seguro de borrar el campo '%ls'? (s/n): ", nombreCampo);
    wchar_t confirma;
    wscanf(L" %lc", &confirma);
    limpiarBuffer();
    if (confirma != L's' && confirma != L'S') {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    wchar_t query[500];
    swprintf(query, 500, L"ALTER TABLE %ls DROP COLUMN %ls", tablaSeleccionada, nombreCampo);
    wprintf(L"\n=== SENTENCIA SQL ===\n");
    wprintf(L"%ls\n", query);
    wprintf(L"=====================\n\n");
    wprintf(L"Ejecutando alteración...\n");
    if (ejecutarComando(&conexion, query)) {
        wprintf(L"\n✓ ¡Campo '%ls' eliminado exitosamente de la tabla '%ls'!\n\n", nombreCampo, tablaSeleccionada);
    } else {
        wprintf(L"\n✗ Error al eliminar el campo. Verifica que no sea una columna clave.\n\n");
    }
}

void actualizarDatosEnTabla() {
    wchar_t tablas[MAX_CAMPOS][MAX_NOMBRE];
    int numTablas = 0;
    wprintf(L"\n=== ACTUALIZAR DATOS EN TABLA ===\n");
    listarTablas(tablas, &numTablas);
    if (numTablas == 0) {
        wprintf(L"\nNo hay tablas disponibles.\n");
        return;
    }
    wprintf(L"\nSelecciona el número de tabla (0 para cancelar): ");
    int seleccion;
    wscanf(L"%d", &seleccion);
    limpiarBuffer();
    if (seleccion == 0) {
        wprintf(L"\n✗ Operación cancelada.\n");
        return;
    }
    if (seleccion < 1 || seleccion > numTablas) {
        wprintf(L"\n✗ Selección inválida.\n");
        return;
    }
    wchar_t *tablaSeleccionada = tablas[seleccion - 1];
    wprintf(L"\n✓ Tabla seleccionada: %ls\n", tablaSeleccionada);
    ColumnaInfo columnas[MAX_CAMPOS];
    int numColumnas = 0;
    if (!obtenerColumnasTabla(tablaSeleccionada, columnas, &numColumnas)) {
        return;
    }
    wchar_t campoID[MAX_NOMBRE];
    wcscpy(campoID, columnas[0].nombre);
    wprintf(L"\n=== CAMPOS DE LA TABLA ===\n");
    for (int i = 0; i < numColumnas; i++) {
        wprintf(L"%d. %ls\n", i + 1, columnas[i].nombre);
    }
    wprintf(L"==========================\n");
    wprintf(L"\nSelecciona el id del número del campo de busqueda: ");
    int campoSel;
    wscanf(L"%d", &campoSel);
    limpiarBuffer();
    if (campoSel < 1 || campoSel > numColumnas) {
        wprintf(L"\n✗ Selección inválida.\n");
        return;
    }
    wcscpy(campoID, columnas[campoSel - 1].nombre);
    wprintf(L"\nIngresa el valor de %ls del registro a actualizar: ", campoID);
    wchar_t valorID[500];
    leerLineaUnicode(valorID, 500);
    if (wcslen(valorID) == 0) {
        wprintf(L"\n✗ Valor inválido.\n");
        return;
    }
    wchar_t querySelect[MAX_QUERY];
    swprintf(querySelect, MAX_QUERY, L"SELECT * FROM %ls WHERE %ls = ", tablaSeleccionada, campoID);
    int idEsTexto = 0;
    for (int i = 0; i < numColumnas; i++) {
        if (wcscmp(columnas[i].nombre, campoID) == 0) {
            if (wcsstr(columnas[i].tipo, L"char") != NULL || wcsstr(columnas[i].tipo, L"text") != NULL) {
                idEsTexto = 1;
            }
            break;
        }
    }
    if (idEsTexto) {
        wcscat(querySelect, L"'");
        wcscat(querySelect, valorID);
        wcscat(querySelect, L"'");
    } else {
        wcscat(querySelect, valorID);
    }
    SQLResult *resultado = ejecutarConsulta(&conexion, querySelect);
    if (resultado == NULL || resultado->numRows == 0) {
        wprintf(L"\n✗ No se encontró ningún registro con %ls = %ls\n", campoID, valorID);
        if (resultado) liberarResultado(resultado);
        return;
    }
    wprintf(L"\n=== DATOS ACTUALES DEL REGISTRO ===\n");
    for (int i = 0; i < numColumnas; i++) {
        wprintf(L"[%ls]: %ls\n", columnas[i].nombre, resultado->data[0][i]);
    }
    wprintf(L"===================================\n\n");
    wprintf(L"¿Desea actualizar este registro? (1=Sí, 0=Cancelar): ");
    int confirmar;
    wscanf(L"%d", &confirmar);
    limpiarBuffer();
    if (confirmar != 1) {
        wprintf(L"\n✗ Operación cancelada.\n");
        liberarResultado(resultado);
        return;
    }
    wprintf(L"\n--- FORMULARIO DE ACTUALIZACIÓN ---\n");
    wprintf(L"(Presiona Enter para mantener el valor actual)\n\n");
    wchar_t nuevosValores[MAX_CAMPOS][500];
    int cambiarValor[MAX_CAMPOS];
    for (int i = 0; i < numColumnas; i++) {
        wprintf(L"Columns: %ls\n", columnas[i].nombre);
        if (wcscmp(columnas[i].nombre, campoID) == 0) {
            cambiarValor[i] = 0;
            continue;
        }
        wprintf(L"[%ls] (%ls)\n", columnas[i].nombre, columnas[i].tipo);
        wprintf(L"Valor actual: %ls\n", resultado->data[0][i]);
        wprintf(L"Nuevo valor: ");
        leerLineaUnicode(nuevosValores[i], 500); // eMiKi
        // Si el usuario deja en blanco, NO actualizar ese campo (mantener valor actual)
        int lenValor = wcslen(nuevosValores[i]);
        if (wcslen(nuevosValores[i]) == 1 && (int)nuevosValores[i][0] == 13) {
            cambiarValor[i] = 0;
            wprintf(L"Mantener valor actual\n");

            wcscpy(nuevosValores[i], resultado->data[0][i]); // Copia el valor actual para evitar vacío
        } else {
            cambiarValor[i] = 1;
        }
        wprintf(L"\n");
    }
    wchar_t queryUpdate[MAX_QUERY];
    swprintf(queryUpdate, MAX_QUERY, L"UPDATE %ls SET ", tablaSeleccionada);

    int primerCambio = 1;
    for (int i = 0; i < numColumnas; i++) {
        if (wcscmp(columnas[i].nombre, campoID) == 0) continue;
        if (!cambiarValor[i]) continue;
        if (!primerCambio) {
            wcscat(queryUpdate, L", ");
        }
        wcscat(queryUpdate, columnas[i].nombre);
        wcscat(queryUpdate, L" = ");

        if (wcsstr(columnas[i].tipo, L"char") != NULL || wcsstr(columnas[i].tipo, L"text") != NULL || wcsstr(columnas[i].tipo, L"date") != NULL) {
            wcscat(queryUpdate, L"'");
            quitarSaltoLinea(nuevosValores[i]);
            wcscat(queryUpdate, nuevosValores[i]); // eMiKi
            wcscat(queryUpdate, L"'");
        } else {
            wcscat(queryUpdate, nuevosValores[i]);
        }
        primerCambio = 0;
    }

    wcscat(queryUpdate, L" WHERE ");
    wcscat(queryUpdate, campoID);
    wcscat(queryUpdate, L" = ");

    if (idEsTexto) {
        wcscat(queryUpdate, L"'");
        wcscat(queryUpdate, valorID);
        wcscat(queryUpdate, L"'");
    } else {
        wcscat(queryUpdate, valorID);
    }

    escribirLog(queryUpdate); // eMiKi

    escribirLogFmt(L"Consulta UPDATE generada: %ls", queryUpdate); // Log: Registrar la consulta UPDATE generada

    liberarResultado(resultado);
    if (primerCambio) {
        wprintf(L"\n✗ No se realizaron cambios.\n");
        return;
    }
    // Mostrar la consulta SQL completa y bien formateada
    wprintf(L"=== SENTENCIA SQL ===\n");
    wprintf(L"%ls\n", queryUpdate);
    wprintf(L"=====================\n\n");
    wprintf(L"Ejecutando actualización...\n");
    if (ejecutarComando(&conexion, queryUpdate)) {
        wprintf(L"\n✓ ¡Registro actualizado exitosamente!\n\n");
    } else {
        wprintf(L"\n✗ Error al actualizar el registro.\n\n");
    }
}

void crearTabla() {
    wchar_t nombreTabla[MAX_NOMBRE];
    Campo campos[MAX_CAMPOS];
    int numCampos = 0;
    wprintf(L"\n=== CREAR NUEVA TABLA ===\n");
    wprintf(L"\nIngresa el nombre de la tabla: ");
    leerLineaUnicode(nombreTabla, MAX_NOMBRE);
    wprintf(L"\nTabla: %ls\n", nombreTabla);
    wprintf(L"\n--- Agregar Campos ---\n");
    while (numCampos < MAX_CAMPOS) {
        wprintf(L"\n[Campo %d]\n", numCampos + 1);
        wprintf(L"Nombre del campo (o escribe 'salir' para terminar): ");
        leerLineaUnicode(campos[numCampos].nombre, MAX_NOMBRE);
        if (wcscmp(campos[numCampos].nombre, L"salir") == 0 || wcscmp(campos[numCampos].nombre, L"") == 0) {
            break;
        }
        mostrarTiposDatos();
        wprintf(L"\nSelecciona el tipo de dato (1-7): ");
        int opcion;
        wscanf(L"%d", &opcion);
        limpiarBuffer();
        switch (opcion) {
            case 1:
                wcscpy(campos[numCampos].tipo, L"INT");
                break;
            case 2: {
                wprintf(L"Ingresa el tamaño (ej: 50): ");
                int tam;
                wscanf(L"%d", &tam);
                limpiarBuffer();
                swprintf(campos[numCampos].tipo, MAX_NOMBRE, L"VARCHAR(%d)", tam);
                break;
            }
            case 3:
                wcscpy(campos[numCampos].tipo, L"FLOAT");
                break;
            case 4:
                wcscpy(campos[numCampos].tipo, L"DATE");
                break;
            case 5:
                wcscpy(campos[numCampos].tipo, L"BOOLEAN");
                break;
            case 6:
                wcscpy(campos[numCampos].tipo, L"TEXT");
                break;
            case 7: {
                wprintf(L"Ingresa precisión y escala (ej: 10,2): ");
                int p, s;
                wscanf(L"%d,%d", &p, &s);
                limpiarBuffer();
                swprintf(campos[numCampos].tipo, MAX_NOMBRE, L"DECIMAL(%d,%d)", p, s);
                break;
            }
            default:
                wprintf(L"Opción inválida, usando INT por defecto\n");
                wcscpy(campos[numCampos].tipo, L"INT");
        }
        wprintf(L"✓ Campo agregado: %ls %ls\n", campos[numCampos].nombre, campos[numCampos].tipo);
        numCampos++;
    }
    if (numCampos == 0) {
        wprintf(L"\nNo se agregaron campos. Operación cancelada.\n");
        return;
    }
    wprintf(L"\n\n=== RESUMEN DE LA TABLA ===\n");
    wprintf(L"Tabla: %ls\n", nombreTabla);
    wprintf(L"Campos:\n");
    for (int i = 0; i < numCampos; i++) {
        wprintf(L"  %d. %ls - %ls\n", i + 1, campos[i].nombre, campos[i].tipo);
    }
    wprintf(L"\n=== SENTENCIA SQL GENERADA ===\n");
    wprintf(L"CREATE TABLE %ls (\n", nombreTabla);
    for (int i = 0; i < numCampos; i++) {
        wprintf(L"    %ls %ls", campos[i].nombre, campos[i].tipo);
        if (i < numCampos - 1) {
            wprintf(L",\n");
        } else {
            wprintf(L"\n");
        }
    }
    wprintf(L");\n");
    wprintf(L"==============================\n\n");
    wchar_t query[MAX_QUERY];
    swprintf(query, MAX_QUERY, L"CREATE TABLE %ls (\n", nombreTabla);
    for (int i = 0; i < numCampos; i++) {
        wchar_t linea[200];
        swprintf(linea, 200, L"    %ls %ls", campos[i].nombre, campos[i].tipo);
        wcscat(query, linea);
        if (i < numCampos - 1) {
            wcscat(query, L",\n");
        } else {
            wcscat(query, L"\n");
        }
    }
    wcscat(query, L")");
    wprintf(L"Ejecutando consulta...\n");
    if (ejecutarComando(&conexion, query)) {
        wprintf(L"\n✓ ¡Tabla '%ls' creada exitosamente en la base de datos!\n\n", nombreTabla);
    } else {
        wprintf(L"\n✗ Error al crear la tabla. Verifica que no exista ya.\n\n");
    }
}

int main() {
    //_wsetlocale(LC_ALL, L"Spanish_Spain.65001");

    //setlocale(LC_ALL, "es_ES.UTF-8");
    setlocale(LC_ALL, "");

    #ifdef UNICODE
        printf("Compilado en Unicode\n");
    #else
        printf("Compilado en ANSI\n");
    #endif

    int opcion;
    // Configuración de conexión (hardcodeada)
    const wchar_t *servidor = L"localhost";
    const wchar_t *baseDatos = L"TestDB";
    wprintf(L"Conectando a %ls...\n", baseDatos);
    if (!conectarSQLTrusted(&conexion, servidor, baseDatos)) {
        wprintf(L"\n✗ No se pudo conectar. Verifica:\n");
        wprintf(L"  1. SQL Server está corriendo\n");
        wprintf(L"  2. El nombre de la base de datos es correcto\n");
        wprintf(L"  3. Tienes permisos de acceso\n");
        return 1;
    }
    wprintf(L"✓ Conexión exitosa\n\n");
    do {
        wprintf(L"\n-------------------------------------\n");
        wprintf(L"|      MENÚ GESTIÓN DE TABLAS     |\n");
        wprintf(L"-------------------------------------\n");
        wprintf(L"\n1. Crear nueva tabla\n");
        wprintf(L"2. Agregar campo a tabla existente\n");
        wprintf(L"3. Borrar campo de tabla existente\n");
        wprintf(L"4. Insertar datos en tabla existente\n");
        wprintf(L"5. Actualizar datos en tabla existente\n");
        wprintf(L"6. Salir\n");
        wprintf(L"\nSelecciona una opción: ");
        if (wscanf(L"%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;
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
                actualizarDatosEnTabla();
                break;
            case 6:
                wprintf(L"\n¡Hasta luego!\n");
                break;
            default:
                wprintf(L"\nOpción inválida. Intenta de nuevo.\n");
        }
    } while (opcion != 6);
    desconectarSQL(&conexion);
    return 0;
}
