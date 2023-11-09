#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone ของ Bank";
const char* password = "11111111";
const char* serverAddress = "http://172.20.10.3:3000/sensor";

WiFiClient client;
HTTPClient http;
DHT dht(D4, DHT11);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  dht.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long timerDelay = 15000;
  if ((millis() - lastTime) > timerDelay) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor");
    } else {
      Serial.print("Humidity: ");
      Serial.println(humidity);
      Serial.print("Temperature: ");
      Serial.println(temperature);

      DynamicJsonDocument jsonDocument(200);
      jsonDocument["hum"] = humidity;
      jsonDocument["temp"] = temperature;
      jsonDocument["timestamp"] = millis();

      String jsonData;
      serializeJson(jsonDocument, jsonData);

      http.begin(client, serverAddress);
      http.addHeader("Content-Type", "application/json");
      http.setTimeout(10000);

      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        Serial.println("HTTP Response code: " + String(httpResponseCode));
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }
      http.end();
    }
    lastTime = millis();
  }
}
