#include <Ultrasonic.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const int trigPin = 5;
const int echoPin = 18;
const int ledPin = 17;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Ultrasonic ultrasonic(trigPin, echoPin);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}

void loop() {
  int distance;
  distance = ultrasonic.read();

  bool ledStatus= false;
  if(distance <= 15) ledStatus = true;
  else ledStatus = false;

  display.clearDisplay();
  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  if(ledStatus) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(ledPin, HIGH);
    display.println("LED status: On");
    display.print(distance);
    display.print(" cm");
  }else {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(ledPin, LOW);
    display.println("LED status: Off");
    display.print(distance);
    display.print(" cm");
  }

  display.display();
}
