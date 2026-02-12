# Solución: Arduino No Responde

## Cambios realizados en arduino.c

1. ✅ **Aumentado tiempo de espera inicial** de 2000ms a 2500ms
   - Arduino se resetea al abrir el puerto serial
   - Necesita tiempo para que el bootloader termine

2. ✅ **Agregado PurgeComm()** para limpiar buffers
   - Elimina datos residuales del buffer de lectura/escritura

3. ✅ **Agregado Sleep(1500ms)** después de enviar
   - Arduino necesita tiempo para:
     - Recibir el comando
     - Encender LED
     - Esperar 1000ms (tu delay)
     - Apagar LED
     - Enviar respuesta

4. ✅ **Mejorados mensajes de debug**
   - Muestra claramente si hay respuesta o no
   - Indica posibles causas de fallo

## Pasos para probar

### 1. Verificar el puerto COM

```powershell
# En PowerShell
[System.IO.Ports.SerialPort]::getportnames()
```

Si tu Arduino NO está en COM3, edita `arduino.c` línea 12:

```c
hSerial = CreateFile(
    "COM4",  // Cambiar a tu puerto
    ...
);
```

### 2. Cargar código Arduino

Carga el archivo `arduinoCodes/led_control.ino` en tu Arduino usando Arduino IDE.

**IMPORTANTE:** Después de cargar el código:

1. Cierra Arduino IDE (liberar el puerto)
2. Desconecta y reconecta el Arduino
3. Espera 3 segundos
4. Ejecuta arduino.exe

### 3. Compilar y ejecutar

```powershell
cd arduino
gcc arduino.c -o arduino.exe
.\arduino.exe
```

## Salida esperada

```
Puerto abierto. Esperando que Arduino inicie...
Arduino listo. Enviando comando...
✓ Comando '1' enviado (1 bytes)
Esperando respuesta del Arduino...

=== RESPUESTA DEL ARDUINO ===
LED ejecutado
=============================
✓ Recibidos 15 bytes

✓ Puerto cerrado

--- Información de debug ---
Puerto: COM3
Velocidad: 9600 baud
Comando enviado: '1'
---------------------------
```

## Si aún no funciona

### Problema 1: "No se recibió respuesta"

**Causas:**

- Arduino IDE abierto (usa el puerto)
- Código Arduino no cargado correctamente
- Puerto incorrecto

**Solución:**

1. Cierra Arduino IDE
2. Verifica que el LED del Arduino parpadea al cargar el código
3. Usa el Monitor Serial de Arduino IDE para verificar:
   ```
   Serial.begin(9600);
   ```
   Envía '1' manualmente y verifica que responde

### Problema 2: "Error abriendo COM3"

**Causas:**

- Puerto ocupado
- Puerto incorrecto
- Arduino desconectado

**Solución:**

```powershell
# Ver qué proceso usa el puerto
Get-Process | Where-Object {$_.Name -like "*arduino*"}

# Matar Arduino IDE si está corriendo
Stop-Process -Name "Arduino" -Force
```

### Problema 3: LED no enciende pero hay respuesta

**Verificar:**

- PIN correcto (línea 2 del código Arduino): `#define LED_PIN 8`
- LED conectado correctamente:
  - Ánodo (+) → Pin 8
  - Cátodo (-) → Resistencia → GND
- O usa LED integrado: `#define LED_PIN LED_BUILTIN` (pin 13)

### Problema 4: Datos corruptos

**Síntomas:** Caracteres extraños en la respuesta

**Causa:** Baud rate diferente

**Solución:** Verificar que coincidan:

- Arduino: `Serial.begin(9600);`
- C: `dcbSerialParams.BaudRate = CBR_9600;`

## Verificación paso a paso

### Con Monitor Serial de Arduino IDE

1. Abre Arduino IDE
2. Carga el código `led_control.ino`
3. Abre Monitor Serial (Ctrl + Shift + M)
4. Selecciona "9600 baud" en la parte inferior
5. Escribe '1' y presiona Enter
6. Deberías ver: **"LED ejecutado"**
7. El LED debería encender 1 segundo

Si esto funciona, cierra Arduino IDE y ejecuta `arduino.exe`

### Con arduino.exe

Ejecuta y observa la salida:

```
Puerto abierto. Esperando que Arduino inicie...  ← Espera 2.5 seg
Arduino listo. Enviando comando...                ← PurgeComm ejecutado
✓ Comando '1' enviado (1 bytes)                   ← WriteFile exitoso
Esperando respuesta del Arduino...                ← Espera 1.5 seg
```

Si después de "Esperando respuesta" no aparece nada:

- Arduino no recibió el comando
- Arduino no está ejecutando el código
- Problema de conexión

## Test rápido de conexión

Crea un código Arduino simple:

```cpp
void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    Serial.print("Recibido: ");
    Serial.println(c);
  }
}
```

Carga este código y ejecuta `arduino.exe`. Deberías ver: "Recibido: 1"

## Checklist final

- [ ] Arduino conectado por USB
- [ ] Driver instalado (aparece en Administrador de dispositivos)
- [ ] Puerto correcto en el código C
- [ ] Código Arduino cargado correctamente
- [ ] Arduino IDE cerrado
- [ ] Baud rate 9600 en ambos lados
- [ ] LED conectado al pin correcto
- [ ] Esperado 3 segundos después de conectar Arduino

Si sigues todos estos pasos, debería funcionar. ¡Suerte! 🚀
