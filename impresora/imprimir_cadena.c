// Programa para imprimir una cadena de texto en una impresora en Windows
// Lista las impresoras disponibles y permite seleccionar una

#include <windows.h>
#include <wchar.h>
#include <stdio.h>

int main() {

    #ifdef UNICODE
        printf("Compilado en Unicode\n");
    #else
        printf("Compilado en ANSI\n");
    #endif

    const wchar_t *texto = L"Hola, esta es una prueba de impresión desde C.";
    DOCINFO di = { sizeof(DOCINFO), L"MiDocumento", NULL };
    HDC hdc = NULL;
    int result = 1;
    DWORD needed, returned;
    PRINTER_INFO_4 *pi4 = NULL;
    int i, seleccion = -1;

    // Obtener el tamaño necesario para el buffer
    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, NULL, 0, &needed, &returned);
    if (needed == 0) {
        printf("No se encontraron impresoras.\n");
        return 1;
    }

    pi4 = (PRINTER_INFO_4*) malloc(needed);
    if (!pi4) {
        printf("No se pudo asignar memoria.\n");
        return 1;
    }

    if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, (LPBYTE)pi4, needed, &needed, &returned)) {
        printf("Error al enumerar impresoras.\n");
        free(pi4);
        return 1;
    }

    wprintf(L"Impresoras disponibles:\n");
    for (i = 0; i < (int)returned; i++) {
        wprintf(L"%d. %s\n", i + 1, pi4[i].pPrinterName);
    }

    wprintf(L"Seleccione el número de la impresora: ");
    if (wscanf(L"%d", &seleccion) != 1 || seleccion < 1 || seleccion > (int)returned) {
        wprintf(L"Selección inválida.\n");
        free(pi4);
        return 1;
    }

    // Abre la impresora seleccionada (Unicode)
    hdc = CreateDC(L"WINSPOOL", pi4[seleccion - 1].pPrinterName, NULL, NULL);
    if (!hdc) {
        wprintf(L"No se pudo abrir la impresora seleccionada.\n");
        free(pi4);
        return 1;
    }

    // Inicia el documento
    if (StartDoc(hdc, &di) <= 0) {
        wprintf(L"No se pudo iniciar el documento.\n");
        DeleteDC(hdc);
        free(pi4);
        return 1;
    }

    // Inicia la página
    if (StartPage(hdc) <= 0) {
        wprintf(L"No se pudo iniciar la página.\n");
        EndDoc(hdc);
        DeleteDC(hdc);
        free(pi4);
        return 1;
    }

    // Imprime el texto en la posición (100, 100)
    TextOutW(hdc, 100, 100, texto, lstrlenW(texto));

    // Finaliza la página y el documento
    EndPage(hdc);
    EndDoc(hdc);
    DeleteDC(hdc);
    free(pi4);

    wprintf(L"Impresión enviada correctamente.\n");
    return 0;
}
