#include "VehicleControl.h"
#include "config.h"
#include <AccelStepper.h>

AccelStepper leftMotor(AccelStepper::FULL4WIRE, LEFT_IN1, LEFT_IN3, LEFT_IN2, LEFT_IN4);
AccelStepper rightMotor(AccelStepper::FULL4WIRE, RIGHT_IN1, RIGHT_IN3, RIGHT_IN2, RIGHT_IN4);

float leftSpeed = 0.0f, rightSpeed = 0.0f;
bool highbeam = false, fog = false, mainLights = false, hazard = false;
unsigned long blinkerTimer = 0;
bool blinkerState = false;

void vehicleSetup() {
  pinMode(PIN_HORN, OUTPUT);
  pinMode(PIN_HIGH_BEAMS, OUTPUT);
  pinMode(PIN_FOG_LIGHTS, OUTPUT);
  pinMode(PIN_MAIN_LIGHTS, OUTPUT);
  pinMode(PIN_LEFT_BLINKER, OUTPUT);
  pinMode(PIN_RIGHT_BLINKER, OUTPUT);
  leftMotor.setMaxSpeed(500);
  rightMotor.setMaxSpeed(500);
}

void setMotorSpeeds() {
  leftMotor.setSpeed(leftSpeed * 450);
  rightMotor.setSpeed(rightSpeed * 450);
  bool moving = fabs(leftSpeed) > 0.05f || fabs(rightSpeed) > 0.05f;
  digitalWrite(PIN_MAIN_LIGHTS, (mainLights || moving) ? HIGH : LOW);
}

void updateBlinkers() {
  if (millis() - blinkerTimer >= 500) {
    blinkerTimer = millis();
    blinkerState = !blinkerState;
  }
  bool l = (leftSpeed < -0.3f && rightSpeed > 0.3f) || hazard;
  bool r = (rightSpeed < -0.3f && leftSpeed > 0.3f) || hazard;
  digitalWrite(PIN_LEFT_BLINKER,  l ? blinkerState : LOW);
  digitalWrite(PIN_RIGHT_BLINKER, r ? blinkerState : LOW);
}

void vehicleHandleJson(JsonDocument& doc) {
  if (doc["left"].is<float>())  leftSpeed  = constrain(doc["left"].as<float>(), -1.0f, 1.0f);
  if (doc["right"].is<float>()) rightSpeed = constrain(doc["right"].as<float>(), -1.0f, 1.0f);
  if (doc["main_lights"].is<bool>()) mainLights = doc["main_lights"];
  if (doc["highbeam"].is<bool>())    highbeam    = doc["highbeam"];
  if (doc["fog"].is<bool>())         fog         = doc["fog"];
  if (doc["hazard"].is<bool>())      hazard      = doc["hazard"];
  digitalWrite(PIN_HIGH_BEAMS, highbeam ? HIGH : LOW);
  digitalWrite(PIN_FOG_LIGHTS, fog ? HIGH : LOW);
  if (doc["horn"].is<bool>() && doc["horn"]) {
    digitalWrite(PIN_HORN, HIGH); delay(150); digitalWrite(PIN_HORN, LOW);
  }
}

void vehicleLoop() {
  leftMotor.runSpeed();
  rightMotor.runSpeed();
  setMotorSpeeds();
  updateBlinkers();
}

void vehiclePublishStatus(PubSubClient& client) {
  float batt = (analogRead(PIN_BATTERY) / 4095.0f) * VREF * DIVIDER_RATIO;
  JsonDocument doc;
  doc["left"] = leftSpeed;
  doc["right"] = rightSpeed;
  doc["batt"] = roundf(batt * 100) / 100;
  doc["highbeam"] = highbeam;
  doc["fog"] = fog;
  doc["hazard"] = hazard;
  String out; serializeJson(doc, out);
  client.publish(MQTT_STATUS_TOPIC, out.c_str());
}
