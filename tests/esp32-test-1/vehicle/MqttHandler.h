#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ArduinoJson.h>
#include <PubSubClient.h>

void setupMqtt();
void handleMqtt();

extern PubSubClient client;

void onMqttMessage(char* topic, byte* payload, unsigned int length);

#endif
