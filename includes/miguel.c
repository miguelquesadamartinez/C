#include "miguel.h"
#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include <stdarg.h>

// Escribe en log.txt usando formato variable (como wprintf)
void escribirLogFmt(const wchar_t *formato, ...) {
    FILE *f = _wfopen(L"..\\log.txt", L"a+, ccs=UNICODE");
    if (!f) {
        wprintf(L"✗ No se pudo abrir/crear log.txt\n");
        return;
    }

    // Fecha y hora actual
    time_t t = time(NULL);
    struct tm tm_info;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_info, &t);
#else
    tm_info = *localtime(&t);
#endif
    wchar_t fechaHora[100];
    wcsftime(fechaHora, 100, L"%Y-%m-%d %H:%M:%S", &tm_info);

    // Construir el texto formateado
    wchar_t texto[2048];
    va_list args;
    va_start(args, formato);
    vswprintf(texto, 2048, formato, args);
    va_end(args);

    // Escribir en el log
    fwprintf(f, L"%ls | %ls", fechaHora, texto);
    fclose(f);
}

// Escribe una línea en log.txt con fecha, hora y el texto recibido (Unicode)
void escribirLog(const wchar_t *texto) {
    FILE *f = _wfopen(L"..\\log.txt", L"a+, ccs=UNICODE");
    if (!f) {
        wprintf(L"✗ No se pudo abrir/crear log.txt\n");
        return;
    }

    time_t t = time(NULL);
    struct tm tm_info;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_info, &t);
#else
    tm_info = *localtime(&t);
#endif
    wchar_t fechaHora[100];
    wcsftime(fechaHora, 100, L"%Y-%m-%d %H:%M:%S", &tm_info);

    fwprintf(f, L"%ls | %ls\n", fechaHora, texto);
    fclose(f);
}

// Limpia el buffer de entrada estándar (wide)
void limpiarBuffer() {
    wint_t c;
    while ((c = getwchar()) != L'\n' && c != WEOF);
}

// Lee una línea Unicode desde consola Windows (soporta acentos)
#include <windows.h>
void leerLineaUnicode(wchar_t *buffer, int maxLen) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD charsRead = 0;
    if (hStdin == INVALID_HANDLE_VALUE) {
        buffer[0] = L'\0';
        return;
    }
    // Leer caracteres Unicode desde consola
    if (!ReadConsoleW(hStdin, buffer, maxLen - 1, &charsRead, NULL)) {
        buffer[0] = L'\0';
        return;
    }
    // Eliminar salto de línea final si existe
    if (charsRead > 0 && buffer[charsRead - 1] == L'\n') {
        buffer[charsRead - 1] = L'\0';
    } else {
        buffer[charsRead] = L'\0';
    }

    wprintf(L"Debug: Leídos %d caracteres Unicode\n", charsRead); // Debug: Verificar número de caracteres leídos

}

void quitarSaltoLinea(wchar_t *str) {
    size_t len = wcslen(str);
    while (len > 0 && (str[len - 1] == L'\n' || str[len - 1] == L'\r')) {
        str[len - 1] = L'\0';
        len--;
    }
}