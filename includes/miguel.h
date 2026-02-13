#ifndef MIGUEL_H
#define MIGUEL_H
#include <wchar.h>

void escribirLog(const wchar_t *texto);
void escribirLogFmt(const wchar_t *formato, ...);

void limpiarBuffer();
void leerLineaUnicode(wchar_t *buffer, int maxLen);
void quitarSaltoLinea(wchar_t *str);

#endif // MIGUEL_H
