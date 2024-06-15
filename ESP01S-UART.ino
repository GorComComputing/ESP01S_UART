// ESP01S Wi-Fi

// Wi-Fi for ESP8266
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// for AP  
extern const char* ssid_AP;  
extern const char* password_AP;
IPAddress local_ip_AP(192,168,1,1);        // Настройки IP адреса
IPAddress gateway_AP(192,168,1,1);
IPAddress subnet_AP(255,255,255,0);

// for STA
extern const char* ssid;  
extern const char* password;
byte tries = 10;                         // Попыткок подключения к точке доступа 
bool is_CONNECTED = false;

AsyncWebServer server(80);

const char* PARAM_CMD = "cmd";
String postCmd = "";


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  

  Serial.begin(9600);
  while (!Serial);                // Ждем пока Serial будет готов

  //  Wi-Fi AP and STA
  WiFi.mode(WIFI_AP_STA);

  //  Wi-Fi AP
  WiFi.softAP(ssid_AP, password_AP);
  WiFi.softAPConfig(local_ip_AP, gateway_AP, subnet_AP);
  delay(100);

  // Connect to Wi-Fi network with SSID and password
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  
  //  Wi-Fi STA
  WiFi.begin(ssid, password);
  while (--tries && WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    is_CONNECTED = false;
    //Serial.println("Non Connecting to WiFi..");
  } else {
    is_CONNECTED = true;
    // Print local IP address and start web server
    //Serial.println("");
    //Serial.println("WiFi connected OK");
    //Serial.print("IP address: ");
    //Serial.println(WiFi.localIP());
  }

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request) {
    String respond = "";
    if(request->hasParam(PARAM_CMD, true)) {
      postCmd = request->getParam(PARAM_CMD, true)->value();
      Serial.println(postCmd);
      while (Serial.available() == 0);   //Ожидаем ввода данных
      while (Serial.available() > 0) {
        char inChar = Serial.read();
        Serial.flush();
        if (inChar == '\n') {
          break;
        }
        respond += inChar;
      }
    }
    request->send(200, "text/plain", respond);
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String respond = "";
    if(request->hasParam(PARAM_CMD)) {
      postCmd = request->getParam(PARAM_CMD)->value();
      Serial.println(postCmd); 
      while (Serial.available() == 0);   //Ожидаем ввода данных
      while (Serial.available() > 0) {
        char inChar = Serial.read();
        Serial.flush();
        if (inChar == '\n') {
          break;
        }
        respond += inChar;
      }
    }
    request->send(200, "text/plain", respond);
  });

  server.begin();
}


void loop() {
  if(is_CONNECTED){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }      
}
