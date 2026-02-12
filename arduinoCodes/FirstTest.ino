#define LED_PIN 8

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);  // Comunicación a 9600 baudios
}

void loop() {
    if (Serial.available() > 0) {
        char comando = Serial.read();

        if (comando == '1') {
            digitalWrite(LED_PIN, HIGH);
            delay(1000);              // 1 segundo
            digitalWrite(LED_PIN, LOW);
        }
    }
}