#include <Arduino.h>

#define LED_PIN 2 //LED pin number
#define SAMPLE_COUNT 10 //this is the number of samples we are going to take an average on to perform oversampling.
#define HALL_THRESHOLD 50 //this will be the magnitude threshhold for when detacting a magnet to filter out any noise from any surrounding fields

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);

  Serial.println("On-board ESP32 Hall Effect Sensor with LED Indication");
  Serial.println("Wave a magnet near the ESP32 chip to see the LED light up");
}

void loop() 
{
  int32_t sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) 
  {
    sum += hallRead(); //read the hall sensor values from board. GPIX pins 36 and 39 (VP and VN respectively) are used for the hall sensor and are identified as input only on the datasheet. function does all the work
    delay(10);
  }
  int32_t average = sum / SAMPLE_COUNT; //take the average of the samples to perform oversampling and get a more accurate reading

  Serial.println("Hall Sensor Value: " + String(average));

  if (average < HALL_THRESHOLD)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.println(" << Magnet Detected >>");
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println();
  delay(10);
}