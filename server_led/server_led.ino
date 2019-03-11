
#include <ESP8266WiFi.h>


#if DBG == 1
	#define DBG( x )	 Serial.print(x)
	#define DBG_LN( x )  Serial.println(x)
  #define CONSOL(x) Serial.println(x)

#else
	#define DBG( x )
	#define DBG_LN( x )
	#define CONSOL(x) Serial.println(x)

#endif

#define CONECTING	'D'
#define CONECTED	'C'

#define DONGDAU		'A'
#define DUAGIAYRA 'R'
#define HOME		'H'
#define DICHTRAI	'T'
#define DICHPHAI	'P'
#define UP        'U'
#define DOWN      'W'

#define DUYET     'I'


#define DCN  0
#define CN	1

int wifi=DCN;
char state=0;

// Thong so WiFi 
const char* ssid = "STKHKT_01";
const char* password = "123456789";


// Tao server
WiFiServer server(80);
WiFiClient client;
// ThingSpeak Settings
const int channelID = 618957; //
String writeAPIKey = "OLFUU9N95Q4J9EZU"; // write API key for your ThingSpeak Channel
const char* servers = "api.thingspeak.com";


// Pin
int output_pin = 2;

void setup() {

  // Khoi dong Serial
  Serial.begin(9600);
  delay(10);

  // Khai bao GPIO5
  pinMode(output_pin, OUTPUT);
  digitalWrite(output_pin, 0);

  // Ket noi toi mang WiFi 
  DBG_LN();
  DBG_LN();
  DBG("Dang ket noi toi mang ");
  DBG_LN(ssid);
  delay(5000);
  delay(5000);
  delay(5000);
  delay(5000);
  delay(5000);
  delay(5000);
  delay(5000);
  delay(5000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
	DBG(".");
	CONSOL(CONECTING);
  }
	CONSOL(CONECTED);
	wifi=CN;
   digitalWrite(output_pin, 1);
  DBG_LN("");
  DBG_LN("Da ket noi WiFi");

  // Khoi dong server

  server.begin();
  DBG_LN("Khoi dong Server");

  // In dia chi IP
  Serial.println(WiFi.localIP());
}

void loop() {


	if (Serial.available() > 0){
		state=Serial.read();
		DBG("state:");
		DBG_LN(state);
		if(state=='C')
			 {
			 CONSOL(CONECTED);
			  while(Serial.available() > 0)Serial.read();
			}
	}
  // Kiem tra neu co client ket noi
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Doi client gui data
  DBG_LN("Co client moi");
  while(!client.available()){
    delay(1);
  }

  // Doc yeu cau tu client
  String req = client.readStringUntil('\r');
  DBG_LN(req);
  client.flush();

  // Kiem tra yeu cau la gi
  if (req.indexOf("/dongdau") != -1){
    digitalWrite(output_pin, 1);
	CONSOL(DONGDAU);
//  thingspeak();
  }
  else if (req.indexOf("/duagiayra") != -1) {
	digitalWrite(output_pin, 0);
	CONSOL(DUAGIAYRA);
  }
   else if (req.indexOf("/home") != -1) {
	digitalWrite(output_pin, 1);
	CONSOL(HOME);
  }
	else if (req.indexOf("/dichphai") != -1) {
	 digitalWrite(output_pin,0);
	 CONSOL(DICHPHAI);
	}
  else if (req.indexOf("/dichtrai") != -1) {
   digitalWrite(output_pin,0);
	CONSOL(DICHTRAI);
   }
  else if (req.indexOf("/up") != -1) {
   digitalWrite(output_pin,0);
 CONSOL(UP);
   }
  else if (req.indexOf("/down") != -1) {
   digitalWrite(output_pin,0);
 CONSOL(DOWN);
   }
   else if (req.indexOf("/duyet") != -1) {
   digitalWrite(output_pin,0);
 CONSOL(DUYET);
   }

  client.flush();

  // Chuan bi thong tin phan hoi
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<head>";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "</head>";
  s += "<div class=\"container\">";
  s += "<h1>SANG TAO KHOA HOC KI THUAT </h1>";
  s += "<h1>dieu khien may dong dau 4.0 </h1>";
  s += "</div></div>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"DUYET HS\" onclick=\"duyet()\"></div>";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-primary\" type=\"button\" value=\"DONG DAU\" onclick=\"dongdau()\"></div>";
  s += "</div></div>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-primary\" type=\"button\" value=\"DUA GIAY RA\" onclick=\"duagiayra()\"></div>";
  s += "</div></div>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"HOME\" onclick=\"home()\"></div>";
  s += "</div></div>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"DICH TRAI\" onclick=\"dichtrai()\"></div>";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"DICH PHAI\" onclick=\"dichphai()\"></div>";
  s += "</div></div>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"DAM\" onclick=\"up()\"></div>";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"NHAT\" onclick=\"down()\"></div>";
  s += "</div></div>";

  s += "<div class=\"row\">";

  s += "</div></div>";

  s += "<script>function dongdau() {$.get(\"/dongdau\");}</script>";
  s += "<script>function duyet() {$.get(\"/duyet\");}</script>";
  
  s += "<script>function duagiayra() {$.get(\"/duagiayra\");}</script>";
  s += "<script>function home() {$.get(\"/home\");}</script>";
  s += "<script>function dichphai() {$.get(\"/dichphai\");}</script>";
  s += "<script>function dichtrai() {$.get(\"/dichtrai\");}</script>";
  
  s += "<script>function up() {$.get(\"/up\");}</script>";
  s += "<script>function down() {$.get(\"/down\");}</script>";

	s += "<h1>giam sat camera </h1>";

	s += "<img src='http://192.168.1.10/jpg_stream'>";

	s += "<form action='msg'><p>ghi chu van ban... <input type='text' name='msg' size=50 autofocus> <input type='luu van ban' value='save'></form>";

  s += "<p>";


  // Gui thong tin cho client
  client.print(s);

 // delay(100);
  DBG_LN("Ngat ket noi Client");
}


void thingspeak(){


	if (client.connect(servers, 80)) {
			// Construct API request body
			String body = "field1= + da dong dau  + &field2= + noi dung van ban";

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
			//Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp, 1).c_str(), String(humi, 1).c_str());
	}
	client.stop();
}

