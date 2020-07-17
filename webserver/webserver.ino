#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
ESP8266WebServer server(80);
void setup() {
  Serial.begin(115200);
  WiFi.begin("SSID", "PASSWORD");  //Connect to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP
  server.on("/mechasolution", []() 
    server.send(200, "text / plain", "<!DOCTYPE html><html><head><style></style></head><body><h2>Button</h2><p>This is example for button</p><button class=‘button’>Click me</button></body></html>");
  });
  server.on("/", handleRootPath);    //Associate the handler function to the path
  server.begin();                    //Start the server
  Serial.println("Server listening");
}
void loop() {
  server.handleClient();         //Handling of incoming requests
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}
void handleRootPath() {            //Handler for the rooth path
  server.send(200, "text/plain", "Hello world");
}
