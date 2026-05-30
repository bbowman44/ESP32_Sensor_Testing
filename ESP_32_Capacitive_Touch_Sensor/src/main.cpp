#include <Arduino.h>

#define LED_PIN 2 // Define the LED pin
#define TOUCH_PIN T0 // Define the touch pin
#define TOUCH_THRESHOLD 30 // Define the touch threshold

void setup() 
{
  Serial.begin(115200); // Start serial communication
  delay(1000); // Wait for serial to initialize
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output

  Serial.println("Begin Capacitive Touch Test/Demo");
  Serial.println("touch the GPIO4 (T0) with your finger");
  Serial.println("Watch the raw values change to calibrate threshold");
}

void loop() 
{
  uint16_t touchValue = touchRead(TOUCH_PIN); // Read the touch value, the closer to 0 that it reads is the value of it being touched, the higher the value the less capatcitance that has been detected
  
  Serial.print("Touch Value: " + String(touchValue));

  if (touchValue < TOUCH_THRESHOLD) // Check if touch value is below the threshold
  {
    digitalWrite(LED_PIN, HIGH); // Turn on the LED
    Serial.println(" << TOUCHED >>");
  } 
  else 
  {
    digitalWrite(LED_PIN, LOW); //turn off the LED if the touch value is above the threshold
  }

  Serial.println();
  delay(100); // Delay for a short period before the next reading to avoid flooding the serial output
}
