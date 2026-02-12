#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD bytesWritten, bytesRead;
    char buffer[256];

    // Abrir puerto
    hSerial = CreateFile(
        "COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error abriendo COM3\n");
        return 1;
    }

    // Configuración básica
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

    // Configurar timeouts (muy importante)
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 2000;   // Espera hasta 2 segundos
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 2000;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    SetCommTimeouts(hSerial, &timeouts);

    printf("Puerto abierto. Esperando que Arduino inicie...\n");
    
    // IMPORTANTE: Arduino se resetea al abrir el puerto serial
    // Esperar a que termine el bootloader (2.5 segundos)
    Sleep(2500);

    // Limpiar cualquier dato residual en el buffer
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
    
    printf("Arduino listo. Enviando comando...\n");

    // Enviar comando
    char comando = '1';

    if (!WriteFile(hSerial, &comando, 1, &bytesWritten, NULL)) {
        printf("Error enviando comando\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("✓ Comando '%c' enviado (%lu bytes)\n", comando, bytesWritten);
    
    // Esperar a que Arduino ejecute el comando
    // (enciende LED + delay 1000ms + apaga LED + envía respuesta)
    printf("Esperando respuesta del Arduino...\n");
    Sleep(1500);  // Dar tiempo al Arduino para procesar

    // Leer respuesta
    bytesRead = 0;
    if (ReadFile(hSerial, buffer, sizeof(buffer)-1, &bytesRead, NULL)) {
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Terminar string
            printf("\n=== RESPUESTA DEL ARDUINO ===\n");
            printf("%s", buffer);
            printf("=============================\n");
            printf("✓ Recibidos %lu bytes\n", bytesRead);
        } else {
            printf("\n✗ No se recibió respuesta del Arduino\n");
            printf("Posibles causas:\n");
            printf("  1. Arduino no está enviando datos\n");
            printf("  2. Código Arduino no cargado correctamente\n");
            printf("  3. Velocidad (baud rate) diferente\n");
            printf("  4. Puerto incorrecto\n");
        }
    } else {
        printf("\n✗ Error leyendo del puerto\n");
    }

    printf("\n✓ Puerto cerrado\n");
    CloseHandle(hSerial);
    
    printf("\n--- Información de debug ---\n");
    printf("Puerto: COM3\n");
    printf("Velocidad: 9600 baud\n");
    printf("Comando enviado: '1'\n");
    printf("---------------------------\n");
    
    return 0;
}
