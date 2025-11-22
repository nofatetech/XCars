#include "config.h"
#include <WiFi.h>                   // ← THIS WAS MISSING!
#include "MqttHandler.h"
#include "VehicleControl.h"

unsigned long lastStatus = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("RC Car starting...");

  // === Connect to WiFi FIRST ===
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  vehicleSetup();
  setupMqtt();
}

void loop() {
  handleMqtt();          // now safe – WiFi is up
  vehicleLoop();

  if (millis() - lastStatus > 2000) {
    lastStatus = millis();
    if (client.connected()) {
      vehiclePublishStatus(client);
    }
  }
}
