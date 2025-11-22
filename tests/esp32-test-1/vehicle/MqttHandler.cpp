#include "MqttHandler.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "VehicleControl.h"   // ← only this include knows your car

WiFiClient espClient;
PubSubClient client(espClient);

void setupMqtt() {
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(onMqttMessage);
}

void handleMqtt() {
  if (!client.connected()) {
    if (client.connect(MQTT_CLIENT_ID)) {
      client.subscribe(MQTT_CONTROL_TOPIC);
    }
  }
  client.loop();
}

// THIS IS THE ONLY PLACE THAT TOUCHES JSON
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  char buffer[length + 1];
  memcpy(buffer, payload, length);
  buffer[length] = '\0';

  DynamicJsonDocument doc(768);
  DeserializationError err = deserializeJson(doc, buffer);
  if (err) {
    Serial.print("JSON error: ");
    Serial.println(err.c_str());
    return;
  }

  // Forward everything to your car code
  vehicleHandleJson(doc);
}