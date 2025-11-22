#include "MqttHandler.h"
#include <WiFi.h>
#include "config.h"
#include "VehicleControl.h"

WiFiClient espClient;
PubSubClient client(espClient);

void setupMqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(onMqttMessage);
}

void handleMqtt() {
  if (WiFi.status() != WL_CONNECTED) {
    client.disconnect();
    return;
  }

  if (!client.connected()) {
    static unsigned long last = 0;
    if (millis() - last > 5000) {
      last = millis();
      if (client.connect(MQTT_CLIENT_ID)) {
        Serial.println("MQTT connected");
        client.subscribe(MQTT_CONTROL_TOPIC);
      }
    }
  } else {
    client.loop();
  }
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  // RAW DUMP — EXACTLY WHAT YOU WANTED
  Serial.println("\n=== RAW MQTT MESSAGE RECEIVED ===");
  Serial.printf("Topic: %s\n", topic);
  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.write(payload[i]);   // prints even non-printable chars correctly
  }
  Serial.println("\n================================\n");

  // Now continue with normal JSON processing...
  char buf[length + 1];
  memcpy(buf, payload, length);
  buf[length] = '\0';

  JsonDocument doc;
  if (deserializeJson(doc, buf) != DeserializationError::Ok) {
    Serial.println("Warning: [JSON parse failed – but raw was printed above]");
    return;
  }

  vehicleHandleJson(doc);
}
