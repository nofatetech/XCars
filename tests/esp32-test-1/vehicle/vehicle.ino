#include "config.h"
#include "MqttHandler.h"
#include "VehicleControl.h"

unsigned long lastStatus = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Vehicle starting...");

  vehicleSetup();
  setupMqtt();
}

void loop() {
  handleMqtt();
  vehicleLoop();

  if (millis() - lastStatus > 2000) {
    lastStatus = millis();
    if (client.connected()) {
      vehiclePublishStatus(client);
    }
  }
}
