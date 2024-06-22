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

bool is_CONNECTED = false;
AsyncWebServer server(80);

const char* PARAM_CMD = "cmd";
String postCmd = "";


// Функция настройки режима точки доступа
void setupWiFiAP() {
    WiFi.softAP(ssid_AP, password_AP);
    WiFi.softAPConfig(local_ip_AP, gateway_AP, subnet_AP);
    delay(100);
}


// Функция подключения к Wi-Fi как клиент (STA)
bool connectToWiFiSTA() {
    WiFi.begin(ssid, password);
    for (int i = 0; i < 10; i++) {
        if (WiFi.status() == WL_CONNECTED) return true;
        delay(500);
    }
    return false;
}


// Обработка POST-запросов
void handlePostRequest(AsyncWebServerRequest *request) {
    String response = "";
    if(request->hasParam(PARAM_CMD, true)) {
        postCmd = request->getParam(PARAM_CMD, true)->value();
        Serial.println(postCmd);
        while (Serial.available() == 0); // Ждем данные
        while (Serial.available() > 0) {
            char inChar = Serial.read();
            if (inChar == '\n') break;
            response += inChar;
        }
    }
    request->send(200, "text/plain", response);
}

// Обработка GET-запросов
void handleGetRequest(AsyncWebServerRequest *request) {
    String response = "";
    if(request->hasParam(PARAM_CMD)) {
        postCmd = request->getParam(PARAM_CMD)->value();
        Serial.println(postCmd);
        while (Serial.available() == 0); // Ждем данные
        while (Serial.available() > 0) {
            char inChar = Serial.read();
            if (inChar == '\n') break;
            response += inChar;
        }
    }
    request->send(200, "text/plain", response);
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  

  Serial.begin(9600);
  while (!Serial);  // Ждем готовности Serial

  // Установка режима Wi-Fi как AP и STA
  WiFi.mode(WIFI_AP_STA);

  // Настройка точки доступа
  setupWiFiAP();

  // Попытка подключения к Wi-Fi как клиент
  is_CONNECTED = connectToWiFiSTA();
  
  // Настройка обработчиков запросов
  server.on("/post", HTTP_POST, handlePostRequest);
  server.on("/get", HTTP_GET, handleGetRequest);
  server.begin();
}


void loop() {
  // Индикация состояния подключения
  if(is_CONNECTED){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
