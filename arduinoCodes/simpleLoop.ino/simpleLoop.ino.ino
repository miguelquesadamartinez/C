void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));  
}

void loop() {
  Serial.println(random(0, 100));
  delay(2000);
}
