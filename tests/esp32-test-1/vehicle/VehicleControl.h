#ifndef VEHICLE_CONTROL_H
#define VEHICLE_CONTROL_H

#include <ArduinoJson.h>
#include <PubSubClient.h>

void vehicleSetup();
void vehicleLoop();
void vehicleHandleJson(JsonDocument& doc);
void vehiclePublishStatus(PubSubClient& client);

#endif
