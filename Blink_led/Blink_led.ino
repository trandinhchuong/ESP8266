
void setup() {
  pinMode(16, OUTPUT);
}

void loop() {
  digitalWrite(16, 0);
  delay(1000);
  digitalWrite(16, 1);
  delay(1000);
}
