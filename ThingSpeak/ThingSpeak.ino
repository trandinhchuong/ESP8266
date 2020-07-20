
#include <ESP8266WiFi.h>
// Wi-Fi Settings
const char* ssid = "BanhSuMinhHang58"; // your wireless network name (SSID)
const char* password = "123456789"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 618957; //
String writeAPIKey = "OLFUU9N95Q4J9EZU"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 2 * 1000; // post data every 2 seconds

void setup() {
		Serial.begin(115200);
		Serial.print("Connecting");
		WiFi.begin(ssid, password);
		while (WiFi.status() != WL_CONNECTED) {
				Serial.print(".");
				delay(100);
		}
		Serial.println("\r\nWiFi connected");
}

void loop() {
		// wait and then post again
		delay(postingInterval);

		float vol =analogRead(A0);
		float	temp=( vol * 5.0 / 1024.0 )* 100.0;
		float humi =analogRead(A0);

		if (client.connect(server, 80)) {
				// Construct API request body
				String body = "field1= +da dong dau  + &field2=" + String(humi, 1);

				client.print("POST /update HTTP/1.1\n");
				client.print("Host: api.thingspeak.com\n");
				client.print("Connection: close\n");
				client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
				client.print("Content-Type: application/x-www-form-urlencoded\n");
				client.print("Content-Length: ");
				client.print(body.length());
				client.print("\n\n");
				client.print(body);
				client.print("\n\n");
				Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp, 1).c_str(), String(humi, 1).c_str());
		}
		client.stop();
}
