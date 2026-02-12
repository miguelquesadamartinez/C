// Código Arduino para probar con arduino.c
// Carga este código en tu Arduino antes de ejecutar arduino.exe

#define LED_PIN 8

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);  // Mismo baud rate que arduino.c
  
  // Indicar que Arduino está listo
  digitalWrite(LED_PIN, LOW);
  
  // Esperar a que la conexión serial esté estable
  delay(100);
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '1') {
      // Encender LED
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);

      // IMPORTANTE: Enviar respuesta DESPUÉS de ejecutar la acción
      Serial.println("LED ejecutado");
      
      // Limpiar buffer serial
      Serial.flush();
    }
  }
}
