# Comunicación con Arduino desde C en Windows

Guía completa para enviar comandos desde C a placas Arduino a través del puerto serial en Windows.

---

## 📁 Archivos del proyecto

### Carpeta `arduino/`

- **arduino.c** - Programa minimalista para envío rápido de comandos

---

## 🚀 Inicio rápido

### 1. Compilar

```powershell
# Desde la raíz del proyecto
gcc arduino\arduino.c -o arduino\arduino.exe

# O usar el script
build_all.bat
```

### 2. Identificar puerto Arduino

**Método rápido (PowerShell):**

```powershell
[System.IO.Ports.SerialPort]::getportnames()
```

**Método visual:**

1. Abre **Administrador de dispositivos** (Win + X)
2. Expande **Puertos (COM y LPT)**
3. Busca "Arduino Uno" o similar → Verás el puerto (ej: COM3)

### 3. Configurar el puerto en el código

Edita `arduino\arduino.c` línea 11:

```c
hSerial = CreateFile(
    "COM3",  // ← Cambia esto por tu puerto
    ...
);
```

### 4. Ejecutar

```powershell
cd arduino
.\arduino.exe
```

> ⚠️ **¿Problemas al ejecutar?** Ver [TROUBLESHOOTING_ARDUINO.md](../TROUBLESHOOTING_ARDUINO.md)

---

## 🎯 Funcionalidad

Este programa:

1. Abre el puerto COM3
2. Lo configura a 9600 baud (8N1)
3. Envía el carácter `'1'`
4. Cierra el puerto

**Ideal para:**

- ✅ Aprender comunicación serial básica
- ✅ Enviar un comando rápido sin configuración
- ✅ Entender el código mínimo necesario
- ✅ Integrar en scripts automatizados

---

## ⚙️ Configuración predeterminada

```c
Puerto:       COM3
Velocidad:    9600 baud
Bits datos:   8
Paridad:      None (sin paridad)
Bits stop:    1
Comando:      '1' (un solo carácter)
```

---

## 🔧 Personalización

### Cambiar el puerto

```c
// Línea 11
hSerial = CreateFile(
    "COM4",  // Cambiar COM3 → COM4
    GENERIC_WRITE,
    ...
);
```

### Cambiar el comando

```c
// Línea 47
char data = 'H';  // Enviar 'H' en lugar de '1'
```

### Enviar string completo

```c
// Reemplazar las líneas 47-54 con:
char mensaje[] = "HOLA\n";
DWORD bytesWritten;

if (!WriteFile(hSerial, mensaje, strlen(mensaje), &bytesWritten, NULL)) {
    printf("Error escribiendo en el puerto\n");
    CloseHandle(hSerial);
    return 1;
}

printf("Enviados %lu bytes: %s", bytesWritten, mensaje);
```

### Cambiar velocidad (baud rate)

```c
// Línea 36
dcbSerialParams.BaudRate = CBR_115200;  // Cambiar de 9600 a 115200
```

**Velocidades disponibles:**

- `CBR_9600` - Estándar Arduino
- `CBR_19200`
- `CBR_38400`
- `CBR_57600`
- `CBR_115200` - ESP32, dispositivos rápidos

---

## 🎓 Código Arduino complementario

### Ejemplo 1: Encender LED al recibir '1'

```cpp
const int LED = 13;

void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char comando = Serial.read();

        if (comando == '1') {
            digitalWrite(LED, HIGH);
            Serial.println("LED encendido");
        }
        else if (comando == '0') {
            digitalWrite(LED, LOW);
            Serial.println("LED apagado");
        }
    }
}
```

### Ejemplo 2: Control de múltiples LEDs

```cpp
const int LED1 = 13;
const int LED2 = 12;

void setup() {
    Serial.begin(9600);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        switch(cmd) {
            case '1': digitalWrite(LED1, HIGH); break;
            case '2': digitalWrite(LED1, LOW); break;
            case '3': digitalWrite(LED2, HIGH); break;
            case '4': digitalWrite(LED2, LOW); break;
        }
    }
}
```

### Ejemplo 3: Respuesta al PC

```cpp
void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        if (cmd == '1') {
            digitalWrite(13, HIGH);
            Serial.println("OK:LED_ON");  // Responder al PC
        }
    }
}
```

---

## 📊 Flujo de comunicación

```
┌──────────────┐                    ┌──────────────┐
│   PC (C)     │                    │   Arduino    │
│              │                    │              │
│ arduino.exe  │ ─── '1' ───────>   │ Serial.read()│
│              │    (COM3)          │              │
│ CreateFile() │                    │ digitalWrite │
│ WriteFile()  │                    │   (LED ON)   │
│ CloseHandle()│                    │              │
└──────────────┘                    └──────────────┘
```

---

## 🔍 Diagnóstico de problemas

> 📖 **[Ver guía completa de solución de problemas → TROUBLESHOOTING_ARDUINO.md](../TROUBLESHOOTING_ARDUINO.md)**
>
> Incluye soluciones paso a paso, verificación de conexión, código Arduino de prueba y checklist completo.

---

### Síntoma: "Error abriendo COM3"

**Diagnóstico:**

```powershell
# Ver puertos disponibles
[System.IO.Ports.SerialPort]::getportnames()

# Ver dispositivos Arduino
Get-PnpDevice -FriendlyName "*Arduino*"
```

**Soluciones:**

1. ✅ Verifica que Arduino esté conectado
2. ✅ Cambia COM3 por el puerto correcto
3. ✅ Cierra Arduino IDE (usa el puerto)
4. ✅ Desconecta y reconecta el Arduino
5. ✅ Reinstala drivers CH340/FTDI si es necesario

### Síntoma: "Error obteniendo estado"

**Causa:** Puerto abierto pero no accesible

**Solución:**

```powershell
# Ejecutar como administrador
# O verificar permisos del usuario
```

### Síntoma: Arduino no responde

**Diagnóstico:**

1. LED TX del Arduino parpadea? → Puerto correcto, datos enviados
2. Arduino se resetea al conectar? → Normal, espera 2 segundos
3. Código Arduino cargado? → Verifica con Arduino IDE

**Solución para delay en reset:**

```c
// Agregar después de CreateFile():
Sleep(2000);  // Esperar 2 segundos después de abrir
```

### Síntoma: "Comando enviado" pero nada pasa

**Diagnóstico:**

- Baud rate diferente en Arduino y C
- Arduino ejecutando otro sketch
- Pin incorrecto en Arduino

**Solución:**

```c
// En C: 9600 baud (línea 36)
dcbSerialParams.BaudRate = CBR_9600;

// En Arduino: mismo baud rate
Serial.begin(9600);
```

---

## 📦 Modelos Arduino compatibles

### ✅ Totalmente compatibles

- Arduino Uno (R3, R4)
- Arduino Mega 2560
- Arduino Nano
- Arduino Pro Mini (con adaptador USB)
- Arduino Leonardo
- Arduino Micro

### ✅ Compatibles con ajustes

- ESP32 (usar 115200 baud)
- ESP8266 NodeMCU (usar 115200 baud)
- Arduino Due
- Teensy (2.0, 3.x, 4.x)

### ⚠️ Requieren drivers adicionales

**Placas con chip CH340:**

- Arduino Nano clones
- Arduino Uno clones chinos

**Descargar:** [CH340 Driver](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

**Placas con chip FTDI:**

- Arduino Pro
- FTDI adapters

**Descargar:** [FTDI Driver](https://ftdichip.com/drivers/vcp-drivers/)

---

## 🛠️ Compilación avanzada

### Compilar con debug

```powershell
gcc -g arduino\arduino.c -o arduino\arduino.exe -Wall
```

### Compilar optimizado

```powershell
gcc -O2 arduino\arduino.c -o arduino\arduino.exe
```

### Compilar con warnings detallados

```powershell
gcc -Wall -Wextra arduino\arduino.c -o arduino\arduino.exe
```

---

## 📝 Código fuente completo explicado

```c
#include <windows.h>  // API Windows para serial
#include <stdio.h>    // printf()

int main() {
    HANDLE hSerial;                    // Handle del puerto
    DCB dcbSerialParams = {0};         // Parámetros del puerto
    COMMTIMEOUTS timeouts = {0};       // Timeouts
    DWORD bytesWritten;                // Bytes escritos

    // 1. ABRIR PUERTO
    //    \\.\COM3 es el formato de Windows
    hSerial = CreateFile(
        "COM3",                        // Nombre del puerto
        GENERIC_WRITE,                 // Solo escritura
        0,                             // Sin compartir
        0,                             // Sin atributos seguridad
        OPEN_EXISTING,                 // Abrir existente
        FILE_ATTRIBUTE_NORMAL,         // Archivo normal
        0                              // Sin template
    );

    // Verificar si abrió correctamente
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error abriendo COM3\n");
        return 1;
    }

    // 2. OBTENER CONFIGURACIÓN ACTUAL
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error obteniendo estado\n");
        CloseHandle(hSerial);
        return 1;
    }

    // 3. CONFIGURAR PUERTO
    //    9600 baud, 8 data bits, 1 stop bit, sin paridad (8N1)
    dcbSerialParams.BaudRate = CBR_9600;      // Velocidad
    dcbSerialParams.ByteSize = 8;             // 8 bits datos
    dcbSerialParams.StopBits = ONESTOPBIT;    // 1 bit stop
    dcbSerialParams.Parity   = NOPARITY;      // Sin paridad

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error configurando puerto\n");
        CloseHandle(hSerial);
        return 1;
    }

    // 4. ENVIAR DATOS
    //    Enviar el carácter '1'
    char data = '1';

    if (!WriteFile(hSerial, &data, 1, &bytesWritten, NULL)) {
        printf("Error escribiendo en el puerto\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Comando enviado correctamente\n");

    // 5. CERRAR PUERTO
    CloseHandle(hSerial);
    return 0;
}
```

---

## 🔗 Recursos adicionales

### Documentación

- [Arduino Serial Reference](https://www.arduino.cc/reference/en/language/functions/communication/serial/)
- [Windows Serial API](https://docs.microsoft.com/en-us/windows/win32/devio/communications-resources)

### Tutoriales Arduino

- [Arduino Getting Started](https://www.arduino.cc/en/Guide)
- [Serial Communication](https://www.arduino.cc/en/Tutorial/BuiltInExamples)

### Para más funcionalidades

Ver [README_SERIAL.md](README_SERIAL.md) para el programa completo con:

- Menú interactivo
- Entrada de texto personalizada
- Múltiples opciones de configuración
- Lectura de datos del Arduino

---

## ✅ Checklist antes de ejecutar

- [ ] Arduino conectado por USB
- [ ] Driver instalado (Administrador de dispositivos lo muestra)
- [ ] Puerto COM identificado (ej: COM3)
- [ ] Sketch Arduino cargado y ejecutándose
- [ ] Puerto correcto en el código C (línea 11)
- [ ] Baud rate coincide (9600 en ambos)
- [ ] Arduino IDE cerrado (liberar puerto)
- [ ] Programa compilado sin errores

---

## �️ ¿Problemas? Solución completa

Si tienes problemas de conexión, Arduino no responde, o recibes errores:

👉 **[TROUBLESHOOTING_ARDUINO.md](../TROUBLESHOOTING_ARDUINO.md)** - Guía completa con:

- Soluciones paso a paso para cada error
- Código Arduino de prueba
- Verificación de conexión
- Test rápidos de diagnóstico
- Checklist completo

---

## �🚀 Siguiente paso

Una vez que domines este programa básico, explora:

1. **[serial.c](../serial/)** - Programa completo con menú
2. **[README_SERIAL.md](README_SERIAL.md)** - Documentación completa
3. Lectura de datos desde Arduino (ReadFile)
4. Comunicación bidireccional
5. Protocolos personalizados

---

**¡Empieza a controlar tu Arduino desde C!** 🎯
