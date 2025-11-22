#include "VehicleControl.h"
#include "config.h"
#include <AccelStepper.h>
#include <Arduino.h>

// === Your motors ===
AccelStepper leftMotor(AccelStepper::FULL4WIRE, LEFT_IN1, LEFT_IN3, LEFT_IN2, LEFT_IN4);
AccelStepper rightMotor(AccelStepper::FULL4WIRE, RIGHT_IN1, RIGHT_IN3, RIGHT_IN2, RIGHT_IN4);

float leftSpeed = 0.0f;
float rightSpeed = 0.0f;

// === Accessory states (remembered) ===
bool highbeam = false;
bool fog = false;
bool mainLights = false;
bool hazard = false;
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
  const float MAX = 450.0f;
  leftMotor.setSpeed(leftSpeed * MAX);
  rightMotor.setSpeed(rightSpeed * MAX);

  bool moving = fabs(leftSpeed) > 0.05 || fabs(rightSpeed) > 0.05;
  digitalWrite(PIN_MAIN_LIGHTS, (mainLights || moving) ? HIGH : LOW);
}

void updateBlinkers() {
  if (millis() - blinkerTimer < 500) return;
  blinkerTimer = millis();
  blinkerState = !blinkerState;

  bool leftTurn  = (leftSpeed < -0.3 && rightSpeed > 0.3);
  bool rightTurn = (rightSpeed < -0.3 && leftSpeed > 0.3);

  digitalWrite(PIN_LEFT_BLINKER,  (leftTurn || hazard) ? blinkerState : LOW);
  digitalWrite(PIN_RIGHT_BLINKER, (rightTurn || hazard) ? blinkerState : LOW);
}

// THIS IS THE ONLY FUNCTION THAT KNOWS YOUR COMMANDS
void vehicleHandleJson(JsonDocument& doc) {
  if (doc.containsKey("left"))  leftSpeed  = constrain(doc["left"]  | 0.0f, -1.0f, 1.0f);
  if (doc.containsKey("right")) rightSpeed = constrain(doc["right"] | 0.0f, -1.0f, 1.0f);

  if (doc.containsKey("main_lights")) mainLights = doc["main_lights"];
  if (doc.containsKey("highbeam"))    highbeam    = doc["highbeam"];
  if (doc.containsKey("fog"))         fog         = doc["fog"];
  if (doc.containsKey("hazard"))      hazard      = doc["hazard"];

  digitalWrite(PIN_HIGH_BEAMS,  highbeam ? HIGH : LOW);
  digitalWrite(PIN_FOG_LIGHTS,  fog      ? HIGH : LOW);

  if (doc.containsKey("horn") && doc["horn"] == true) {
    digitalWrite(PIN_HORN, HIGH);
    delay(150);
    digitalWrite(PIN_HORN, LOW);
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
  DynamicJsonDocument doc(256);
  doc["left"]   = leftSpeed;
  doc["right"]  = rightSpeed;
  doc["batt"]   = roundf(batt * 100) / 100;
  doc["highbeam"] = highbeam;
  doc["fog"]      = fog;
  doc["hazard"]   = hazard;

  String json;
  serializeJson(doc, json);
  client.publish(MQTT_STATUS_TOPIC, json.c_str());
}