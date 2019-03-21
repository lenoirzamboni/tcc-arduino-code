#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "network_name";
const char* password =  "network_passwd";
const byte sensorPin = A0;
byte switcher = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting..");
  }
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (analogRead(sensorPin) > 0) { // Input voltage range of bare ESP8266 is 0 — 1.0V
    Serial.println("Motion detected - sending POST request");
    if (switcher) {
      sendVagaLivre();
      switcher = 1;
    } else {
      sendVagaOcupada();
      switcher = 0;
    }
    delay(5000);
  }
}

void sendVagaLivre() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.1.88:8090/vagaLivre");
    http.addHeader("Content-Type", "text/plain");
    int respondeCode = http.POST("passwd");
    if (respondeCode > 0) {
      Serial.print("Got response code - vagaLivre: ");
      Serial.println(respondeCode);
    } else {
      Serial.print("Error on sending request - vagaLivre: ");
      Serial.println(respondeCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void sendVagaOcupada() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.1.88:8090/vagaOcupada");
    http.addHeader("Content-Type", "text/plain");
    int respondeCode = http.POST("passwd");
    if (respondeCode > 0) {
      Serial.print("Got response code - vagaOcupada: ");
      Serial.println(respondeCode);
    } else {
      Serial.print("Error on sending request - vagaOcupada: ");
      Serial.println(respondeCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}
