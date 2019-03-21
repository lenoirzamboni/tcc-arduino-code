#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "network_name";
const char* password =  "network_passwd";
const byte sensorPin = 22;
byte switcher = 0;
SemaphoreHandle_t syncSemaphore;

void IRAM_ATTR handleInterrupt() {
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting..");
  }
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP());
  syncSemaphore = xSemaphoreCreateBinary();
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, CHANGE);
}

void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  if (digitalRead(sensorPin)) {
    Serial.println("Motion detected - sending POST request");
    if (switcher) {
      sendVagaLivre();
      switcher = 1;
    } else {
      sendVagaOcupada();
      switcher = 0;
    }
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
