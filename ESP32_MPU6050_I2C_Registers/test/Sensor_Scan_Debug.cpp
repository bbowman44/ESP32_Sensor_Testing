#include <Arduino.h>
#include <Wire.h>

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  //Initialize I2C on the default pins of D21 (SDA) and D22 (SCL)
  Wire.begin();

  Serial.println("I2C Scanner");
  Serial.println("Scanning for I2C devices");
}

void loop() 
{
  byte count = 0;

  Serial.println("Begin scanning all possible I2C addresses");

  //loop over all possible I2C addresses and check for the device
  //I2C uses 7 bit addresses, so we loop from 1 to 127 (0 is reserved for general call and the 8th bit is used for read/write)
  for (byte address; address < 127; address++) 
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission(); //this variable is used to determine is the address is in use or not, it returns 0 if a device is present at the address

    //check for error 0 and acknowledge the device's address
    if (error == 0)
    {
      Serial.printf("Device found at address 0x%02X\n", address); //address is always 2 digits in this context
      count++;
    }
  }

  if (count == 0) //not found case
  {
    Serial.println("No I2C devices found, please check your connection and try again");
  }
  else //found addresses case
  {
    Serial.printf("Found %d devices", count);
  }

  Serial.println("===============================");
  delay(5000); //wait 5 seconds before scanning again to not flood the serial output
}