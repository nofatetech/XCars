#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
#include "config.h"

// Stepper objects
Stepper leftMotor(STEPS_PER_REV, LEFT_IN1, LEFT_IN2, LEFT_IN3, LEFT_IN4);
Stepper rightMotor(STEPS_PER_REV, RIGHT_IN1, RIGHT_IN2, RIGHT_IN3, RIGHT_IN4);

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Current motor speeds (-1.0 to +1.0)
float leftSpeed  = 0.0;
float rightSpeed = 0.0;

unsigned long lastStatus = 0;
unsigned long blinkerTimer = 0;
bool blinkerState = false;

void setMainLights(bool on) {
  digitalWrite(PIN_MAIN_LIGHTS, on ? HIGH : LOW);
}

void updateBlinkers() {
  if (millis() - blinkerTimer >= 500) {
    blinkerTimer = millis();
    blinkerState = !blinkerState;
    bool anyTurning = (leftSpeed < -0.3 && rightSpeed > 0.3) || (rightSpeed < -0.3 && leftSpeed > 0.3);
    digitalWrite(PIN_LEFT_BLINKER,  (leftSpeed < -0.3 && rightSpeed > 0.3) ? blinkerState : LOW);
    digitalWrite(PIN_RIGHT_BLINKER, (rightSpeed < -0.3 && leftSpeed > 0.3) ? blinkerState : LOW);
    if (!anyTurning) {
      digitalWrite(PIN_LEFT_BLINKER, LOW);
      digitalWrite(PIN_RIGHT_BLINKER, LOW);
    }
  }
}

void setMotorSpeeds() {
  int rpm = 15;  // Safe max for 28BYJ-48
  int leftRPM  = (int)(abs(leftSpeed)  * rpm * (leftSpeed  >= 0 ? 1 : -1));
  int rightRPM = (int)(abs(rightSpeed) * rpm * (rightSpeed >= 0 ? 1 : -1));

  leftMotor.setSpeed(abs(leftRPM));
  rightMotor.setSpeed(abs(rightRPM));

  // Non-blocking small steps
  if (leftRPM != 0)  leftMotor.step(leftRPM > 0 ? 4 : -4);
  if (rightRPM != 0) rightMotor.step(rightRPM > 0 ? 4 : -4);

  // Main lights on when any wheel is moving
  setMainLights(abs(leftSpeed) > 0.05 || abs(rightSpeed) > 0.05);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.println("MQTT: " + msg);

  // Expected format: {"left":0.8,"right":-0.5} or {"left":0,"right":0}
  int l = msg.indexOf("\"left\":");
  int r = msg.indexOf("\"right\":");
  if (l != -1 && r != -1) {
    leftSpeed  = msg.substring(l + 7, msg.indexOf(",", l)).toFloat();
    rightSpeed = msg.substring(r + 8, msg.indexOf("}", r)).toFloat();

    leftSpeed  = constrain(leftSpeed,  -1.0, 1.0);
    rightSpeed = constrain(rightSpeed, -1.0, 1.0);
  }

  // Extra simple commands (optional)
  if (msg.indexOf("honk") != -1) {
    digitalWrite(PIN_HORN, HIGH); delay(150); digitalWrite(PIN_HORN, LOW);
  }
  if (msg.indexOf("highbeam_on") != -1)  digitalWrite(PIN_HIGH_BEAMS, HIGH);
  if (msg.indexOf("highbeam_off") != -1) digitalWrite(PIN_HIGH_BEAMS, LOW);
  if (msg.indexOf("fog_on") != -1)       digitalWrite(PIN_FOG_LIGHTS, HIGH);
  if (msg.indexOf("fog_off") != -1)      digitalWrite(PIN_FOG_LIGHTS, LOW);
}

void publishStatus() {
  float v = (analogRead(PIN_BATTERY) / 4095.0) * VREF * DIVIDER_RATIO;
  String json = "{\"left\":" + String(leftSpeed,3) +
                ",\"right\":" + String(rightSpeed,3) +
                ",\"batt\":" + String(v,2) + "}";
  client.publish(MQTT_STATUS_TOPIC, json.c_str());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connecting...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
      client.subscribe(MQTT_CONTROL_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_MAIN_LIGHTS, OUTPUT);
  pinMode(PIN_FOG_LIGHTS, OUTPUT);
  pinMode(PIN_HIGH_BEAMS, OUTPUT);
  pinMode(PIN_LEFT_BLINKER, OUTPUT);
  pinMode(PIN_RIGHT_BLINKER, OUTPUT);
  pinMode(PIN_HORN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi OK");

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  setMotorSpeeds();
  updateBlinkers();

  if (millis() - lastStatus > 1000) {
    publishStatus();
    lastStatus = millis();
  }
}