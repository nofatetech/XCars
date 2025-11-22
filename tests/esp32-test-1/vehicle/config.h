#ifndef CONFIG_H
#define CONFIG_H

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

extern const char* MQTT_SERVER;
extern uint16_t    MQTT_PORT;
extern const char* MQTT_CLIENT_ID;
extern const char* MQTT_CONTROL_TOPIC;
extern const char* MQTT_STATUS_TOPIC;

// Pins
#define LEFT_IN1    13
#define LEFT_IN2    12
#define LEFT_IN3    14
#define LEFT_IN4    27
#define RIGHT_IN1   26
#define RIGHT_IN2   25
#define RIGHT_IN3   33
#define RIGHT_IN4   32

#define PIN_HORN          23
#define PIN_HIGH_BEAMS    22
#define PIN_FOG_LIGHTS    21
#define PIN_MAIN_LIGHTS   19
#define PIN_LEFT_BLINKER  18
#define PIN_RIGHT_BLINKER 5
#define PIN_BATTERY       34

#define VREF           3.3f
#define DIVIDER_RATIO  2.0f

#endif
