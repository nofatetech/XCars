#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ArduinoJson.h>

void setupMqtt();                   // call from setup()
void handleMqtt();                  // call from loop()
void onMqttMessage(char* topic, byte* payload, unsigned int length);

#endif