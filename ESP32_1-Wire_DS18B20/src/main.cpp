#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4
int countingnumber = 0;

//set up the 1-Wire bus and pass it to the dallas library
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("DS18B20 Temperature Sensor 1-Wire Example");

  sensors.begin(); // Start up the library

  //report how many 1-Wire devices are present
  int deviceCount = sensors.getDeviceCount();
  countingnumber = deviceCount;
  Serial.printf("Found %d devices on the bus.\n", deviceCount);

  if (deviceCount == 0) {
    Serial.println("No devices found. Check your wiring and for pull-up resistors (4.7kΩ).");
  }

  Serial.println("------------------------------------------------");

  //print the first address in the unique 64-bit address of each device
  for (int i = 0; i < deviceCount; i++) {
    uint8_t address[8];
    if (sensors.getAddress(address, i)) {
      Serial.print("Device ");
      Serial.print(i);
      Serial.print(" Address: ");
      for (uint8_t j = 0; j < 8; j++) {
        Serial.printf("%02X", address[j]);
      }
      Serial.println();
    } else {
      Serial.printf("Unable to find address for Device %d\n", i);
    }
  }
  
}

void loop() {
  //requset temperature readings from all sensors on the bus
  //This is a command sent over 1-Wire
  sensors.requestTemperatures();

  float tempC, tempF;

  //read the the results and print them to the Serial Monitor
  if(countingnumber > 0)
  {
    for (int i = 0; i < countingnumber; i++) 
    {
      tempC = sensors.getTempCByIndex(i);
      tempF = sensors.getTempFByIndex(i);

      //DEVICE_DISCONNECTED_C is returned if the device is not connected or if the read failed
      if (tempC == DEVICE_DISCONNECTED_C)  //could make this a function just to decrease code repetition but I am too lazy to do it right now
      {
        Serial.println("Error: Could not read temperature");
      } else 
      {
        Serial.printf("Temperature Device %d: %.2f °C / %.2f °F\n", i, tempC, tempF);
      }
    }
  }
  else
  {
    float tempC = sensors.getTempCByIndex(0); // Get the temperature of the first sensor on the bus
    float tempF = sensors.getTempFByIndex(0);

    //DEVICE_DISCONNECTED_C is returned if the device is not connected or if the read failed
    if (tempC == DEVICE_DISCONNECTED_C) //could make this a function just to decrease code repetition but I am too lazy to do it right now
    {
      Serial.println("Error: Could not read temperature");
    } else 
    {
      Serial.printf("Temperature Device 0: %.2f °C / %.2f °F\n", tempC, tempF);
    }
  }

  delay(2000); // Wait 2 seconds before the next reading
}