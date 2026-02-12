# Comunicación Serial/USB desde C en Windows

Este documento explica cómo usar los programas de comunicación serial para enviar datos a dispositivos Arduino, ESP32, adaptadores USB-Serial y otros dispositivos por puerto COM.

---

## 📁 Archivos del proyecto

### Carpeta `serial/`

- **serial.c** - Programa completo con menú interactivo

### Carpeta `arduino/`

- **arduino.c** - Programa simple y directo (un solo comando)

---

## 🚀 Compilación

### Opción 1: Scripts de compilación (Recomendado)

**Compilar todos los programas:**

```powershell
build_all.bat
```

**Compilar y ejecutar en un paso:**

```powershell
run.bat serial\serial.c
run.bat arduino\arduino.c
```

### Opción 2: Comandos manuales

```powershell
# Programa serial completo
gcc serial\serial.c -o serial\serial.exe

# Programa Arduino simple
gcc arduino\arduino.c -o arduino\arduino.exe
```

---

## 📡 Programas disponibles

### 1. serial.c - Programa Completo

**Características:**

- ✅ Menú interactivo
- ✅ Selección de puerto COM
- ✅ Entrada de texto personalizado
- ✅ Opciones para Serial tradicional y USB
- ✅ Configuración de velocidad (baud rate)
- ✅ Ayuda integrada
- ✅ Validación de errores

**Ejecutar:**

```powershell
cd serial
.\serial.exe
```

**Menú de opciones:**

1. Enviar a puerto Serial (COM tradicional)
2. Enviar a dispositivo USB (Arduino, ESP32, etc.)
3. Configuración avanzada
4. Ayuda - ¿Cómo encontrar mi puerto?
5. Salir

### 2. arduino.c - Programa Simple

**Características:**

- ✅ Código minimalista
- ✅ Sin menús, ejecución directa
- ✅ Ideal para aprender
- ✅ Hardcodeado para COM3 a 9600 baud
- ✅ Envía un solo carácter ('1')

**Ejecutar:**

```powershell
cd arduino
.\arduino.exe
```

**Personalizar:**
Para cambiar el puerto o el dato, edita estas líneas en `arduino.c`:

```c
// Cambiar el puerto
hSerial = CreateFile("COM3", ...);  // Cambiar "COM3" por tu puerto

// Cambiar el dato a enviar
char data = '1';  // Cambiar '1' por tu comando
```

---

## 🔍 ¿Cómo encontrar mi puerto COM?

### Opción 1: Administrador de dispositivos (GUI)

1. Presiona `Win + X` → "Administrador de dispositivos"
2. Expande "Puertos (COM y LPT)"
3. Busca tu dispositivo (ejemplo: "Arduino Uno (COM3)")
4. El número entre paréntesis es tu puerto

### Opción 2: PowerShell

```powershell
[System.IO.Ports.SerialPort]::getportnames()
```

### Opción 3: Arduino IDE

Si usas Arduino:

- Abre Arduino IDE
- Menú: **Herramientas → Puerto**
- El puerto con ✓ es el activo

### Opción 4: Device Manager (CLI)

```powershell
Get-WmiObject Win32_SerialPort | Select-Object Name,DeviceID
```

---

## 🔧 API de Windows - Funciones principales

### 1. Abrir puerto

```c
HANDLE hSerial = CreateFile(
    "\\\\.\\COM3",                  // Nombre del puerto (formato Windows)
    GENERIC_READ | GENERIC_WRITE,  // Acceso lectura/escritura
    0,                             // Sin compartir
    NULL,                          // Sin seguridad
    OPEN_EXISTING,                 // Abrir puerto existente
    FILE_ATTRIBUTE_NORMAL,         // Atributos normales
    NULL                           // Sin template
);

if (hSerial == INVALID_HANDLE_VALUE) {
    printf("Error abriendo puerto\n");
}
```

### 2. Configurar puerto

```c
DCB dcbSerialParams = {0};
dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

// Obtener configuración actual
GetCommState(hSerial, &dcbSerialParams);

// Configurar parámetros (9600 baud, 8N1)
dcbSerialParams.BaudRate = CBR_9600;    // Velocidad
dcbSerialParams.ByteSize = 8;           // 8 bits de datos
dcbSerialParams.StopBits = ONESTOPBIT;  // 1 bit de stop
dcbSerialParams.Parity = NOPARITY;      // Sin paridad

// Aplicar configuración
SetCommState(hSerial, &dcbSerialParams);
```

### 3. Configurar timeouts

```c
COMMTIMEOUTS timeouts = {0};
timeouts.ReadIntervalTimeout = 50;
timeouts.ReadTotalTimeoutConstant = 50;
timeouts.ReadTotalTimeoutMultiplier = 10;
timeouts.WriteTotalTimeoutConstant = 50;
timeouts.WriteTotalTimeoutMultiplier = 10;

SetCommTimeouts(hSerial, &timeouts);
```

### 4. Enviar datos

```c
char mensaje[] = "Hola Arduino\n";
DWORD bytesWritten;

WriteFile(hSerial, mensaje, strlen(mensaje), &bytesWritten, NULL);
printf("Enviados %lu bytes\n", bytesWritten);
```

### 5. Recibir datos

```c
char buffer[256];
DWORD bytesRead;

ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL);
buffer[bytesRead] = '\0';  // Terminar string
printf("Recibidos: %s\n", buffer);
```

### 6. Cerrar puerto

```c
CloseHandle(hSerial);
```

---

## 📝 Ejemplo completo de uso

```c
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD bytesWritten;

    // 1. Abrir puerto
    hSerial = CreateFile(
        "\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error: No se pudo abrir COM3\n");
        return 1;
    }

    printf("✓ Puerto COM3 abierto\n");

    // 2. Configurar puerto (9600 baud, 8N1)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    SetCommState(hSerial, &dcbSerialParams);

    // 3. Configurar timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    printf("✓ Puerto configurado (9600 baud, 8N1)\n");

    // 4. Enviar datos
    char mensaje[] = "Hola desde C!\n";
    WriteFile(hSerial, mensaje, strlen(mensaje), &bytesWritten, NULL);
    printf("✓ Enviados %lu bytes: %s", bytesWritten, mensaje);

    // 5. Cerrar puerto
    CloseHandle(hSerial);
    printf("✓ Puerto cerrado\n");

    return 0;
}
```

---

## ⚙️ Velocidades (Baud Rate) comunes

| Velocidad | Constante Windows | Uso común                             |
| --------- | ----------------- | ------------------------------------- |
| 9600      | `CBR_9600`        | Arduino estándar, dispositivos lentos |
| 19200     | `CBR_19200`       | Comunicación media                    |
| 38400     | `CBR_38400`       | Comunicación media-alta               |
| 57600     | `CBR_57600`       | Comunicación rápida                   |
| 115200    | `CBR_115200`      | ESP32, dispositivos rápidos           |

**⚠️ Importante:** El baud rate debe coincidir en ambos lados (PC y dispositivo).

---

## 🔌 Dispositivos USB soportados

### Arduino

- Arduino Uno (CH340, ATmega16U2)
- Arduino Mega
- Arduino Nano
- Arduino Leonardo

### ESP

- ESP32
- ESP8266 (NodeMCU)

### Adaptadores USB-Serial

- FTDI (FT232RL, FT231X)
- CH340 / CH341
- CP2102 / CP2104
- PL2303

### Otros

- Módulos Bluetooth HC-05/HC-06
- GPS USB
- Cualquier dispositivo USB-Serial

💡 **Nota:** Todos aparecen como puertos COM virtuales en Windows.

---

## 🐛 Solución de problemas

### Error: "No se pudo abrir el puerto"

**Causas comunes:**

1. Puerto incorrecto → Verifica con Administrador de dispositivos
2. Puerto en uso → Cierra Arduino IDE, PuTTY, otros programas
3. Sin permisos → Ejecuta como administrador
4. Dispositivo desconectado → Conecta y verifica

**Solución:**

```powershell
# Ver puertos disponibles
[System.IO.Ports.SerialPort]::getportnames()

# Verificar dispositivos USB
Get-PnpDevice -PresentOnly | Where-Object {$_.Class -eq "Ports"}
```

### Error: "Error al configurar puerto"

**Causa:** Parámetros incorrectos

**Solución:** Verifica que el baud rate coincida con tu dispositivo Arduino/ESP32.

### No recibo datos

**Causas:**

1. Baud rate diferente
2. Dispositivo no envía datos
3. Timeout muy corto

**Solución Arduino:**

```cpp
// En Arduino, asegúrate de usar el mismo baud rate
void setup() {
    Serial.begin(9600);  // Mismo que en C
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        Serial.print("Recibido: ");
        Serial.println(c);
    }
}
```

### Puerto desaparece al abrir

**Causa:** Algunos Arduino se resetean al abrir el puerto serial

**Solución:** Espera 2 segundos después de abrir antes de enviar:

```c
Sleep(2000);  // Esperar 2 segundos
WriteFile(...);
```

---

## 🎓 Ejemplos de uso

### Ejemplo 1: Encender LED en Arduino

**C (PC):**

```c
// Enviar '1' para encender, '0' para apagar
char comando = '1';
WriteFile(hSerial, &comando, 1, &bytesWritten, NULL);
```

**Arduino:**

```cpp
void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == '1') digitalWrite(LED_BUILTIN, HIGH);
        if (cmd == '0') digitalWrite(LED_BUILTIN, LOW);
    }
}
```

### Ejemplo 2: Enviar múltiples comandos

**C (PC):**

```c
char comandos[] = "LED1:ON\nLED2:OFF\n";
WriteFile(hSerial, comandos, strlen(comandos), &bytesWritten, NULL);
```

**Arduino:**

```cpp
void loop() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');

        if (cmd == "LED1:ON") digitalWrite(13, HIGH);
        if (cmd == "LED1:OFF") digitalWrite(13, LOW);
    }
}
```

### Ejemplo 3: Lectura de sensores

**Arduino (envía datos):**

```cpp
void loop() {
    int sensor = analogRead(A0);
    Serial.print("TEMP:");
    Serial.println(sensor);
    delay(1000);
}
```

**C (PC - recibe datos):**

```c
char buffer[256];
DWORD bytesRead;

while (1) {
    ReadFile(hSerial, buffer, sizeof(buffer)-1, &bytesRead, NULL);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Sensor: %s", buffer);
    }
    Sleep(100);
}
```

---

## 📚 Headers necesarios

```c
#include <windows.h>  // Para API de Windows (serial)
#include <stdio.h>    // Para printf, scanf
#include <string.h>   // Para strlen, strcpy
```

---

## 🔐 Permisos

No se necesitan permisos especiales para puertos COM en Windows. Si hay problemas:

```powershell
# Ejecutar PowerShell como Administrador
Set-ExecutionPolicy RemoteSigned
```

---

## 📖 Referencias

### Documentación oficial:

- [MSDN - CreateFile](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea)
- [MSDN - SetCommState](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcommstate)
- [MSDN - WriteFile](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile)

### Arduino:

- [Arduino Serial](https://www.arduino.cc/reference/en/language/functions/communication/serial/)

---

## ✅ Checklist de conexión

Antes de ejecutar tu programa:

- [ ] Dispositivo USB conectado físicamente
- [ ] Driver instalado (verifica en Administrador de dispositivos)
- [ ] Puerto COM identificado (ej: COM3)
- [ ] Baud rate conocido (9600 para Arduino por defecto)
- [ ] Ningún otro programa usando el puerto
- [ ] Código compilado sin errores

---

**¡Listo para comunicarte con tus dispositivos!** 🚀
