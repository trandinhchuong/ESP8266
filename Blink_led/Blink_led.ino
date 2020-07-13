
#define LED1 2

void setup() {
  pinMode(LED1, OUTPUT);
}

void loop() {
  digitalWrite(LED1, 0);
  delay(1000);
  digitalWrite(LED1, 1);
  delay(1000);
}
