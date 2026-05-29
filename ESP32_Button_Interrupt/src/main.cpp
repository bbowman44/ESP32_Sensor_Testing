#include <Arduino.h>

#define LED_PIN 2 //LED ON ESP32 IS 2
#define BUTTON_PIN 0 //BOOT BUTTON ON ESP32 IS 0

volatile bool buttonPressed = false;
volatile uint16_t pressCount = 0;

//placing this in the IRAM for the interrupt to be able to run without issues
//also have to declare the variables as volatile to prevent optimization issues
//flash can also apparently be disabled during certain operations, so placing the function in IRAM ensures it can run even when flash is unavailable
//the code normally lives in the flash memory as compared to the ram.
void IRAM_ATTR buttonInterrupt() 
{
  buttonPressed = true;
  pressCount++;
}

void setup() 
{
  Serial.begin(115200);
  delay(1000); // Give some time for the serial monitor to initialize
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor to filter out the noise

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING); // Attach interrupt to the button pin

  Serial.println("Press the BOOT button to toggle the LED and begin count!");
}

void loop() 
{
  //so if the button is pressed then the interrupt will trigger since it has been attached to the button pin and the button interrupt function
  //since the button is set to falling when that signal is detected it will fall into the if statement and then immediately set the button pressed to false again so that if pressed again it will trigger
  //we set the led pin to the opposite of what is currently the read state of the led so that it will toggle the led and feel responsive then print the button press count.
  if(buttonPressed) { 
    buttonPressed = false;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle the LED state
    Serial.println("Button pressed! Count: " + String(pressCount));
  }
}