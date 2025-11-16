#include <WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>

// WiFi credentials
const char* ssid = "?";
const char* password = "?";

// MQTT settings
const char* mqtt_server = "192.168.0.13"; // e.g., "192.168.1.100"
const int mqtt_port = 1883;
const char* mqtt_client_id = "vehicle_1";
const char* mqtt_control_topic = "vehicle/1/control";
const char* mqtt_status_topic = "vehicle/1/status";

// Stepper motor settings
#define STEPS_PER_REV 200 // Adjust for your motor (e.g., NEMA 17)
#define MOTOR1_IN1 13     // GPIO pins for motor 1 (left wheel)
#define MOTOR1_IN2 12
#define MOTOR1_IN3 14
#define MOTOR1_IN4 27
#define MOTOR2_IN1 26     // GPIO pins for motor 2 (right wheel)
#define MOTOR2_IN2 25
#define MOTOR2_IN3 33
#define MOTOR2_IN4 32
Stepper motor1(STEPS_PER_REV, MOTOR1_IN1, MOTOR1_IN2, MOTOR1_IN3, MOTOR1_IN4);
Stepper motor2(STEPS_PER_REV, MOTOR2_IN1, MOTOR2_IN2, MOTOR2_IN3, MOTOR2_IN4);

// Battery monitoring
#define BATTERY_PIN 34    // ADC pin for battery voltage
const float vref = 3.3;   // ESP32 reference voltage
const float divider_ratio = 2.0; // Voltage divider (e.g., 10k/10k resistors)

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long last_status = 0;

void setup() {
  Serial.begin(115200);

  // Initialize motors
  motor1.setSpeed(60); // RPM; adjust for your motor
  motor2.setSpeed(60);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish status every 10 x1k ms
  if (millis() - last_status > 0.5 x 1000) {
    publish_status();
    last_status = millis();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("MQTT connected");
      client.subscribe(mqtt_control_topic);
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Parse JSON payload
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Received: " + message);

  // Example: {"action":"forward","speed":0.5} or {"action":"stop"}
  if (message.indexOf("forward") > -1) {
    float speed = message.substring(message.indexOf("speed\":") + 7, message.indexOf("}")).toFloat();
    int steps_per_sec = speed * STEPS_PER_REV; // Scale speed (0.0 to 1.0)
    motor1.setSpeed(steps_per_sec);
    motor2.setSpeed(steps_per_sec);
    motor1.step(STEPS_PER_REV); // Move one revolution
    motor2.step(STEPS_PER_REV);
  } else if (message.indexOf("stop") > -1) {
    motor1.setSpeed(0);
    motor2.setSpeed(0);
  } else if (message.indexOf("turn_left") > -1) {
    motor1.setSpeed(0); // Stop left wheel
    motor2.setSpeed(60); // Right wheel moves
    motor2.step(STEPS_PER_REV);
  } else if (message.indexOf("turn_right") > -1) {
    motor1.setSpeed(60); // Left wheel moves
    motor2.setSpeed(0); // Stop right wheel
    motor1.step(STEPS_PER_REV);
  }
}

void publish_status() {
  // Read battery voltage
  int analog_value = analogRead(BATTERY_PIN);
  float voltage = (analog_value / 4095.0) * vref * divider_ratio;

  // Publish status
  char status[100];
  snprintf(status, sizeof(status), "{\"battery\":%.2f,\"status\":\"%s\"}", 
           voltage, client.connected() ? "online" : "offline");
  client.publish(mqtt_status_topic, status);
  Serial.println("Status: " + String(status));
}
