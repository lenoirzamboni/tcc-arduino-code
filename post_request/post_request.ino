#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "network_name";
const char* password =  "network_passwd";
const byte sensorPin = A0;
bool switcher = true;
const int bufferSize = 10;
int readingBuffer[bufferSize];
int bufferPointer = 0;
const String host = "http://192.168.1.3:5544";

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

void initializeReadingBuffer() {
  Serial.println("Initialize reading buffer");
  for (int i; i < sizeof(readingBuffer); i++) {
    readingBuffer[i] = -1;
  }
}

bool detectedMotion() {
  int counter = 0;
  for (int i; i < bufferSize; i++) {
    if (readingBuffer[i] > 100) {
      counter++;
    }
  }
  return counter == bufferSize;
}

void loop() {
  int val = analogRead(sensorPin);
  Serial.print("Motion value: ");
  Serial.println(val);

  readingBuffer[bufferPointer] = val;

  if (detectedMotion()) { // Input voltage range of bare ESP8266 is 0 — 1.0V
    Serial.println();
    Serial.println("Motion detected - sending POST request");
    Serial.println();
    if (switcher) {
      sendVagaLivre();
      switcher = false;
    } else {
      sendVagaOcupada();
      switcher = true;
    }
  } else {
    Serial.println("No motion detected");
  }
  
  bufferPointer++;
  if (bufferPointer == bufferSize) {
    initializeReadingBuffer();
    bufferPointer = 0;
  }
  
  delay(100);
}

void sendVagaLivre() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(5000);
    http.begin(host + "/vagaLivre");
    http.addHeader("Content-Type", "application/json");
    int respondeCode = http.GET();
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
    http.setTimeout(5000);
    http.begin(host + "/vagaOcupada");
    http.addHeader("Content-Type", "application/json");
    int respondeCode = http.POST("{\"token\":\"1234\"}");
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
