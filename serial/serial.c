#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_TEXTO 500
#define MAX_PUERTO 20

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void listarPuertosComunes() {
    printf("\n=== PUERTOS DISPONIBLES ===\n");
    printf("SERIAL TRADICIONAL:\n");
    printf("  COM1, COM2          - Puerto serial DB9 (tradicional)\n");
    printf("\nDISPOSITIVOS USB:\n");
    printf("  COM3, COM4, COM5+   - Dispositivos USB a Serial\n");
    printf("                        Arduino, ESP32, ESP8266\n");
    printf("                        Adaptadores USB-TTL\n");
    printf("                        Módulos FTDI, CH340, CP2102\n");
    printf("============================\n\n");
    printf("💡 Tip: Los dispositivos USB se muestran como puertos COM virtuales\n\n");
}

int enviarDatosSerial(const char *puerto, const char *datos) {
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD bytesWritten;
    char nombrePuerto[MAX_PUERTO + 10];
    
    // Formato del nombre del puerto para Windows
    sprintf(nombrePuerto, "\\\\.\\%s", puerto);
    
    printf("\nAbriendo puerto %s...\n", puerto);
    
    // Abrir el puerto serial
    hSerial = CreateFile(
        nombrePuerto,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("✗ Error: No se pudo abrir el puerto %s\n", puerto);
        printf("  Verifica que:\n");
        printf("  1. El dispositivo está conectado\n");
        printf("  2. El puerto está disponible (no usado por otra app)\n");
        printf("  3. El nombre del puerto es correcto\n");
        return 0;
    }
    
    printf("✓ Puerto abierto exitosamente\n");
    
    // Configurar parámetros del puerto serial
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("✗ Error al obtener configuración del puerto\n");
        CloseHandle(hSerial);
        return 0;
    }
    
    // Configuración típica: 9600 baud, 8 bits, sin paridad, 1 bit de stop
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("✗ Error al configurar el puerto\n");
        CloseHandle(hSerial);
        return 0;
    }
    
    // Configurar timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("✗ Error al configurar timeouts\n");
        CloseHandle(hSerial);
        return 0;
    }
    
    printf("✓ Puerto configurado correctamente (9600 baud, 8N1)\n");
    
    // Enviar datos
    printf("\nEnviando datos...\n");
    
    if (!WriteFile(hSerial, datos, strlen(datos), &bytesWritten, NULL)) {
        printf("✗ Error al enviar datos\n");
        CloseHandle(hSerial);
        return 0;
    }
    
    printf("✓ Enviados %lu bytes correctamente\n", bytesWritten);
    printf("  Datos: \"%s\"\n", datos);
    
    // Cerrar puerto
    CloseHandle(hSerial);
    printf("\n✓ Puerto cerrado\n");
    
    return 1;
}

void enviarADispositivoUSB() {
    char puerto[MAX_PUERTO];
    char texto[MAX_TEXTO];
    
    printf("\n=== ENVIAR DATOS A DISPOSITIVO USB ===\n");
    printf("(Arduino, ESP32, Adaptadores USB-Serial)\n");
    
    // Mostrar puertos comunes
    listarPuertosComunes();
    
    // Pedir puerto
    printf("Ingresa el puerto USB (ej: COM3, COM4, COM5): ");
    fgets(puerto, sizeof(puerto), stdin);
    puerto[strcspn(puerto, "\n")] = 0;
    
    // Convertir a mayúsculas
    for (int i = 0; puerto[i]; i++) {
        puerto[i] = toupper(puerto[i]);
    }
    
    // Pedir texto a enviar
    printf("\nIngresa el texto a enviar al dispositivo: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = 0;
    
    // Confirmar
    printf("\n--- RESUMEN ---");
    printf("\nDispositivo: USB en puerto %s", puerto);
    printf("\nTexto: \"%s\"", texto);
    printf("\nLongitud: %zu bytes", strlen(texto));
    printf("\n---------------\n");
    
    printf("\n¿Continuar? (S/N): ");
    char confirmar;
    scanf(" %c", &confirmar);
    limpiarBuffer();
    
    if (confirmar == 'S' || confirmar == 's') {
        enviarDatosSerial(puerto, texto);
    } else {
        printf("\nOperación cancelada.\n");
    }
}

void configurarYEnviar() {
    char puerto[MAX_PUERTO];
    char texto[MAX_TEXTO];
    
    printf("\n=== ENVIAR DATOS AL PUERTO SERIAL ===\n");
    printf("(Puertos COM tradicionales)\n");
    
    // Mostrar puertos comunes
    listarPuertosComunes();
    
    // Pedir puerto
    printf("Ingresa el puerto (ej: COM3, COM4): ");
    fgets(puerto, sizeof(puerto), stdin);
    puerto[strcspn(puerto, "\n")] = 0;
    
    // Convertir a mayúsculas
    for (int i = 0; puerto[i]; i++) {
        puerto[i] = toupper(puerto[i]);
    }
    
    // Pedir texto a enviar
    printf("\nIngresa el texto a enviar: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = 0;
    
    // Confirmar
    printf("\n--- RESUMEN ---");
    printf("\nPuerto: %s", puerto);
    printf("\nTexto: \"%s\"", texto);
    printf("\nLongitud: %zu bytes", strlen(texto));
    printf("\n---------------\n");
    
    printf("\n¿Continuar? (S/N): ");
    char confirmar;
    scanf(" %c", &confirmar);
    limpiarBuffer();
    
    if (confirmar == 'S' || confirmar == 's') {
        enviarDatosSerial(puerto, texto);
    } else {
        printf("\nOperación cancelada.\n");
    }
}

void configurarAvanzada() {
    char puerto[MAX_PUERTO];
    int baudRate;
    char texto[MAX_TEXTO];
    
    printf("\n=== CONFIGURACIÓN AVANZADA ===\n");
    
    listarPuertosComunes();
    
    printf("Ingresa el puerto: ");
    fgets(puerto, sizeof(puerto), stdin);
    puerto[strcspn(puerto, "\n")] = 0;
    
    printf("\n=== VELOCIDAD (BAUD RATE) ===\n");
    printf("1. 9600   (Estándar - Recomendado)\n");
    printf("2. 19200\n");
    printf("3. 38400\n");
    printf("4. 57600\n");
    printf("5. 115200 (Alta velocidad)\n");
    printf("\nSelecciona: ");
    
    int opcion;
    scanf("%d", &opcion);
    limpiarBuffer();
    
    switch (opcion) {
        case 1: baudRate = 9600; break;
        case 2: baudRate = 19200; break;
        case 3: baudRate = 38400; break;
        case 4: baudRate = 57600; break;
        case 5: baudRate = 115200; break;
        default:
            printf("Opción inválida, usando 9600\n");
            baudRate = 9600;
    }
    
    printf("\nIngresa el texto a enviar: ");
    fgets(texto, sizeof(texto), stdin);
    texto[strcspn(texto, "\n")] = 0;
    
    printf("\nNOTA: Esta versión usa configuración estándar 9600 baud.\n");
    printf("Para cambiar baud rate, modifica la línea 'dcbSerialParams.BaudRate' en el código.\n");
    
    enviarDatosSerial(puerto, texto);
}

int main() {
    system("chcp 65001 > nul");

    int opcion;
    
    printf("╔═══════════════════════════════════════╗\n");
    printf("║   COMUNICACIÓN PUERTO SERIAL/USB     ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    
    do {
        printf("\n--- MENÚ PRINCIPAL ---\n");
        printf("1. Enviar a puerto Serial (COM tradicional)\n");
        printf("2. Enviar a dispositivo USB (Arduino, ESP32, etc.)\n");
        printf("3. Configuración avanzada\n");
        printf("4. Ayuda - ¿Cómo encontrar mi puerto?\n");
        printf("5. Salir\n");
        printf("\nSelecciona una opción: ");
        
        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = 0;
        } else {
            limpiarBuffer();
        }
        
        switch (opcion) {
            case 1:
                configurarYEnviar();
                break;
            case 2:
                enviarADispositivoUSB();
                break;
            case 3:
                configurarAvanzada();
                break;
            case 4:
                printf("\n╔═══════════════════════════════════════╗\n");
                printf("║    ¿CÓMO ENCONTRAR MI PUERTO?        ║\n");
                printf("╚═══════════════════════════════════════╝\n");
                printf("\nWindows:\n");
                printf("  1. Abre 'Administrador de dispositivos'\n");
                printf("  2. Expande 'Puertos (COM y LPT)'\n");
                printf("  3. Busca tu dispositivo (Arduino, USB Serial, etc.)\n");
                printf("  4. El nombre será algo como 'COM3' o 'COM4'\n");
                printf("\nPowerShell:\n");
                printf("  Ejecuta: [System.IO.Ports.SerialPort]::getportnames()\n");
                printf("\nArduino IDE:\n");
                printf("  Menú: Herramientas > Puerto\n");
                printf("\n📌 NOTA IMPORTANTE:\n");
                printf("  Los dispositivos USB (Arduino, ESP32) aparecen como\n");
                printf("  puertos COM en Windows. Elige la opción 2 del menú.\n\n");
                break;
            case 5:
                printf("\n¡Hasta luego!\n");
                break;
            default:
                printf("\nOpción inválida. Intenta de nuevo.\n");
        }
        
    } while (opcion != 5);
    
    return 0;
}
