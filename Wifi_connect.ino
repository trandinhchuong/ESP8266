#include <ESP8266WiFi.h>

const char* ssid = "TranYen";
const char* password = "46512345";


void setup(void)
{
  Serial.begin(115200);
  Serial.printf("\nConnecting to %s \n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
