#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ONE_WIRE_BUS 2

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, old;

int oldDistance = 0;
int ledState = LOW;
unsigned long lastChangeTime = 0;
unsigned long blinkInterval = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(10, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  unsigned long currentMillis = millis();

  // Ultrasonic sensor measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration / 2) / 29.1;

  if (cm < 25){
    digitalWrite(10, HIGH);
    delay(10000);
    oldDistance = cm;
  } else if (cm < 50) {
    blinkInterval = 500;
  } else if (cm < 100) {
    blinkInterval = 1000;
  } else if (cm < 150) {
    blinkInterval = 1500;
  } else {
    blinkInterval = 2000;
  }

  if (currentMillis - lastChangeTime >= blinkInterval) {
    lastChangeTime = currentMillis; // Update last change time
    ledState = !ledState; // Toggle LED state
    digitalWrite(10, ledState); // Update LED state
  }

  // Display distance on LCD
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Distance:"));
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.print(cm);
  display.display();
}
