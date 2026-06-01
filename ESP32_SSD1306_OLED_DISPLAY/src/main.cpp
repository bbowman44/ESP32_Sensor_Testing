#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>

#define TEXT_SIZE 1 //Text size for the OLED display, steps in multiples of 6 pixels, so 1 is 6x8 pixels, 2 is 12x16 pixels, etc.
#define SCREEN_WIDTH 128 //OLED display width, in pixels
#define SCREEN_HEIGHT 64 //OLED display height, in pixels
#define OLED_RESET     -1 //No reset pin for the OLED display but listed anyways just to take into account since the Adafruit_SSD1306 library requires it
#define BME280_ADDRESS 0X76 //I2C address of the BME280 sensor
#define OLED_ADDRESS 0X3C //I2C address of the OLED display

Adafruit_BME280 bme; //Use I2C with the default pins of D21 (SDA) and D22 (SCL) and the default address of 0x7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Use I2C with the default pins of D21 (SDA) and D22 (SCL)

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  //Initialize the BME280 sensor and check if it is connected properly
  if (!bme.begin(BME280_ADDRESS)) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BME280 sensor initialized successfully");

  //Initialize the OLED display and check if it is connected properly
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) //using SSD1306_SWITCHCAPVCC for 3.3V power supply since that is what the ESP32 uses, and the OLED display is compatible with 3.3V
  {
    Serial.println("SSD1306 allocation failed, check wiring!");
    while (1);
  }
  Serial.println("OLED display initialized successfully");

  //display a startup message on the oled to confirm the display is working correctly
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE); //check definition at top of file for explanation and size of the pixels
  display.setTextColor(SSD1306_WHITE); //set text color to white. Yellow is also available and may be the top 16 pixels, will have to check to see if this works for all areas of the display
  display.setCursor(0, 0); //set cursor to top left corner of the display
  display.println("BME280 Monitor");
  display.println("Starting up...");
  display.display(); //display the text on the OLED
  delay(2000); //wait 2 seconds before starting the main loop to give time to read the startup message
}

void loop() 
{
  //read the sensor values for the BME280 sensor
  float tempF = bme.readTemperature() * 9 / 5 + 32; //convert temperature to Fahrenheit
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; //convert pressure to hPa
  float altitude = bme.readAltitude(1013.25); //estimate altitude based on standard sea level pressure

  //clear the display for the new frame of data coming through and set up size and color again just in case
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);
  //trying without seting the text size and color to see what happens, may need to include again if errors occur
  
  display.setCursor(0, 0); //set the cursor top left
  display.println("Environmental Data"); //title for the display
  display.drawLine(0, 12, SCREEN_WIDTH-1, 12, SSD1306_WHITE); //draw a line to separate the title from the data

  //display the temperature
  display.setCursor(0, 16);
  display.printf("Temp: %.2f °F\n", tempF);

  //display the humidity
  display.setCursor(0, 24);
  display.printf("Humidity: %.2f %%\n", humidity);
  
  //display the pressure
  display.setCursor(0, 32);
  display.printf("Press: %.2f hPa\n", pressure);

  //display the altitude
  display.setCursor(0, 40);
  display.printf("Alt: %.2f m\n", altitude);

  //push everything to the screen
  display.display();

  //print to Serial for debugging purposes
  Serial.printf("Temperature: %.2f °F\n",  tempF);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Pressure: %.2f hPa\n", pressure);
  Serial.printf("Current Estimated Altitude: %.2f m\n", bme.readAltitude(1013.25));
  Serial.println("======================================================");
  delay(250); //wait 250 milliseconds before the next loop to not flood the serial output and to give time for the OLED display to update
}