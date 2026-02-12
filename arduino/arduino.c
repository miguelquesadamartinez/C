#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD bytesWritten;

    // Abrir puerto COM3
    hSerial = CreateFile(
        "COM3",
        GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error abriendo COM3\n");
        return 1;
    }

    // Configuración del puerto
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error obteniendo estado\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error configurando puerto\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Enviar carácter '1'
    char data = '1';

    if (!WriteFile(hSerial, &data, 1, &bytesWritten, NULL)) {
        printf("Error escribiendo en el puerto\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Comando enviado correctamente\n");

    CloseHandle(hSerial);
    return 0;
}
