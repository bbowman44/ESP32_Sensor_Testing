#include <Arduino.h>

#define LED_PIN 2 //GPIO pin number for the built-in LED
#define BLINK_DELAY 250 //delay in milliseconds for blinking the LED

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200); //initialize serial communication at 115200 to match platform.ini settings for monitor_speed.
  delay(1000); //wait for serial monitor to initialize.
  pinMode(LED_PIN, OUTPUT); //set the LED pin as output.
  Serial.println("========================================");
  Serial.println("   ESP32 LED BLINK PROGRAM INITIATED");
  Serial.println("========================================");
  Serial.println("LED_PIN: GPIO " + String(LED_PIN));
  Serial.println("BLINK_DELAY: " + String(BLINK_DELAY) + " ms");
  Serial.println("========================================");
}

void loop() {
  digitalWrite(LED_PIN, HIGH); //HIGH indicated the voltage is on, so the led light will turn on.
  Serial.println("LED ON");

  delay(BLINK_DELAY); // wait 1 second

  digitalWrite(LED_PIN, LOW); //LOW indicates the voltage is off, so the led light will turn off.
  Serial.println("LED OFF");

  delay(BLINK_DELAY); //wait another second, then loop back to the HIGH state.
}