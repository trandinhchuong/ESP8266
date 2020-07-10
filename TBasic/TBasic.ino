/***************/


#include <Ticker.h>
#define LED_BUILTIN 16
Ticker flipper;

int count = 0;

void flip()
{
  int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state
  
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  flipper.attach(0.3, flip);
}

void loop() {
}
