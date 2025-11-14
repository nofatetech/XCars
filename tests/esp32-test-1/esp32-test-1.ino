
// ESP32-WROOM OLED Test for ER-OLEDM0.95-2C-SPI
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Pin definitions
#define OLED_SCL   18  // SPI Clock (SCLK)
#define OLED_SDA   23  // SPI Data (MOSI)
#define OLED_CS    15  // Chip Select
#define OLED_DC    4   // Data/Command
#define OLED_RES   5   // Reset

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SPI_SPEED 1000000 // 1 MHz SPI speed

// Initialize OLED (SPI)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RES, OLED_CS);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);
  Serial.println("OLED Test for ER-OLEDM0.95-2C-SPI");
  Serial.println("VCC connected to ESP32 3.3V pin");

  // Initialize SPI
  SPI.begin(OLED_SCL, -1, OLED_SDA, OLED_CS); // SCL, MISO (unused), SDA, CS

  // Initialize OLED
  Serial.println("Attempting to initialize OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed - check wiring, power, or controller"));
    for (;;); // Halt if OLED fails
  }
  Serial.println("OLED initialized successfully");

  // Clear display and show test message
  display.clearDisplay();
  display.setTextSize(1);      // 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // White text
  display.setCursor(0, 0);     // Top-left
  display.println("OLED Test");
  display.println("ER-OLEDM0.95");
  display.println("3.3V Power");
  display.println("Hello, World!");
  display.display();           // Update screen
  Serial.println("Test message sent to OLED");
}

void loop() {
  // Do nothing
}
