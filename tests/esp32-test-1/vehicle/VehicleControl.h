#ifndef VEHICLE_CONTROL_H
#define VEHICLE_CONTROL_H

#include <ArduinoJson.h>

void vehicleSetup();          // call from setup()
void vehicleLoop();           // call from loop()
void vehicleHandleJson(JsonDocument& doc);  // called by MQTT handler

// Optional: expose current state for status publishing
void vehiclePublishStatus(PubSubClient& client);

#endif