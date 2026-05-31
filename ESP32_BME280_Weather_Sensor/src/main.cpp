#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_I2C_ADDRESS 0x76 //the default I2C address of the BME280 is 0x76 as verified in the I2C scan test

Adafruit_BME280 bme; //create an instance of the BME280 class

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("BME280 Environmental Sensor");

  //Initialize the  BME sensor at it's address of 0x76
  if (!bme.begin(BME280_I2C_ADDRESS)) //this both initializes the sensor and checks if it is present at the address. This returns a boolean value, so we can check if the sensor is present or not and print an error message if it is not found
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1); //halt the program if the sensor is not found, there is no point in continuing without the sensor
  }

  Serial.println("BME280 sensor found and initialized successfully!");
  Serial.println("======================================================");
}

void loop() 
{
  //now that the sensor has been initialized, we can read the temperature, humidity, and pressure values from the sensor and print them to the serial monitor
  //Since the BME280 has libraries that make it easy to read the sensor values, we can simply call the appropriate functions to get the temperature, humidity, and pressure values

  float temperature = bme.readTemperature(); //read the temperature in degrees Celsius
  float humidity = bme.readHumidity(); //read the humidity in percentage
  float pressure = bme.readPressure() / 100.0F; //read the pressure in hPa (hectopascals)

  //Since I can't read celcius values in my head I will convert them to farenheit as well as showing celcius for easier understanding
  float tempF = (temperature * 9.0 / 5.0) + 32.0; //convert the temperature to degrees Fahrenheit
  Serial.printf("Temperature: %.2f °C / %.2f °F\n", temperature, tempF);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Pressure: %.2f hPa\n", pressure);
  Serial.printf("Current Estimated Altitude: %.2f m\n", bme.readAltitude(1013.25)); //since we have pressure readings we can estimate the altitude we are at as well using the standard sea level pressure
  Serial.println("======================================================");

  delay(2000); //wait for 2 seconds before taking another reading, this is to avoid flooding the serial monitor with too many readings
}