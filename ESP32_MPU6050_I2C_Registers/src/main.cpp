#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>

// MPU-6050 Register-Level Driver Test (no sensor library)
// Trying to understand how to write, read, and manupulate the MPU-6050's registers using the Wire library, 
// without using any sensor libraries that abstract away the details.
// reassembling 16-bit values, and waking the device.

#define MPU6050_ADDR 0x68 // I2C address of the MPU-6050 (AD0 pin low)

//define the OLED display parameters and the BME280 sensor parameters
#define TEXT_SIZE 1 //Text size for the OLED display, steps in multiples of 6 pixels, so 1 is 6x8 pixels, 2 is 12x16 pixels, etc.
#define SCREEN_WIDTH 128 //OLED display width, in pixels
#define SCREEN_HEIGHT 64 //OLED display height, in pixels
#define OLED_RESET     -1 //No reset pin for the OLED display but listed anyways just to take into account since the Adafruit_SSD1306 library requires it
#define BME280_ADDRESS 0X76 //I2C address of the BME280 sensor
#define OLED_ADDRESS 0X3C //I2C address of the OLED display

Adafruit_BME280 bme; //Use I2C with the default pins of D21 (SDA) and D22 (SCL) and the default address of 0x7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Use I2C with the default pins of D21 (SDA) and D22 (SCL)

//Register addresses from the mpu6050 register map
//https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf
#define REG_PWR_MGMT_1      0x6B  // Power management
#define REG_WHO_AM_I        0x75  // Identity (should read 0x68)
#define REG_ACCEL_CONFIG    0x1C  // Accel full-scale range
#define REG_GYRO_CONFIG     0x1B  // Gyro full-scale range
#define REG_ACCEL_XOUT_H    0x3B  // First data register

//Full-scale range scaling factors
//At default ±2g, sensitivity = 16384 LSB/g
//At default ±250°/s, sensitivity = 131 LSB/(°/s)
#define ACCEL_SCALE         16384.0f
#define GYRO_SCALE          131.0f

void writeRegister(uint8_t reg, uint8_t value) 
{
  Wire.beginTransmission(MPU6050_ADDR); //start I2C transaction with the MPU-6050
  Wire.write(reg); //telling the MPU-6050 which register we want to write to
  Wire.write(value); //the value we want to write to that register
  Wire.endTransmission(); //actually send the data to the MPU-6050, this is when the I2C transaction happens
}

uint8_t readRegister(uint8_t reg) 
{
  Wire.beginTransmission(MPU6050_ADDR); //start I2C transaction with the MPU-6050
  Wire.write(reg); //telling the MPU-6050 which register we want to read from
  Wire.endTransmission(false); // Send repeated start (don't release the bus)
  Wire.requestFrom(MPU6050_ADDR, (uint8_t)1); //request 1 byte of data from the MPU-6050, this is when the I2C transaction happens
  return Wire.read();
}

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  Wire.begin();

  Serial.println("MPU-6050 Register-Level Test");

  //perform a basic check to see if the MPU-6050 is connected and responding by reading the WHO_AM_I register
  uint8_t whoAmI = readRegister(REG_WHO_AM_I);
  Serial.printf("WHO_AM_I register: 0x%02X\n", whoAmI);
  if (whoAmI != MPU6050_ADDR) 
  {
    Serial.println("MPU-6050 not found, check your connections and try again");
    while (1); //halt execution
  }
  Serial.println("MPU-6050 Identity Verified");

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
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MPU-6050 Starting Up...");
  display.println("BME280 Starting Up...");
  display.display();

  //Wake the device up by writing to it
  //pwr_mgmt_1 register defaults to 0x40 (sleep mode enabled), we need to set it to 0 to wake the device up
  //the device will be in sleep mode and not respond to register reads/writes until we wake it up, so this is an important step
  writeRegister(REG_PWR_MGMT_1, 0x00); //wake the device up by writing 0 to the power management register
  Serial.println("MPU-6050 Woken Up");
  display.println("MPU-6050 Woken Up");
  display.println("BME280 Woken Up");
  display.display();

  //Set the accelerometer full-scale ranges
  writeRegister(REG_ACCEL_CONFIG, 0x00); //set the accelerometer full-scale range to ±2g (00), other options are ±4g (01), ±8g (10), and ±16g (11)
  writeRegister(REG_GYRO_CONFIG, 0x00); //set the gyroscope full-scale range to ±250°/s (00), other options are ±500°/s (01), ±1000°/s (10), and ±2000°/s (11)
  Serial.println("Ranges set: accel +/-2g, gyro +/-250 deg/s");
  Serial.println("------------------------------------------------");

  delay(1000); //wait a second before starting the main loop to give us time to read the serial output before it starts flooding with data
}

void loop() 
{
  //clear the display and set the cursor to the top left corner for new data
  display.clearDisplay();
  display.setCursor(0, 0);

  //clear the display for the new frame of data coming through and set up size and color again just in case
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);

  //burst read the 14 data registers starting from ACCEL_XOUT_H
  //layout is as follows in the burst read: AccelX(2) AccelY(2) AccelZ(2) Temp(2) GyroX(2) GyroY(2) GyroZ(2)
  Wire.beginTransmission(MPU6050_ADDR); //start I2C transaction with the MPU-6050
  Wire.write(REG_ACCEL_XOUT_H); //telling the MPU-6050 which register we want to start reading from
  Wire.endTransmission(false); // Send repeated start (don't release the bus)
  Wire.requestFrom(MPU6050_ADDR, (uint8_t)14); //request 14 bytes of data from the MPU-6050, this is when the I2C transaction happens

  //reassemble the 16-bit values from the burst read (same layout as above)
  int16_t rawAccelX = (Wire.read() << 8) | Wire.read();
  int16_t rawAccelY = (Wire.read() << 8) | Wire.read();
  int16_t rawAccelZ = (Wire.read() << 8) | Wire.read();
  int16_t rawTemp   = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroX  = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroY  = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroZ  = (Wire.read() << 8) | Wire.read();
  
  //read the BME280 sensor data SECOND, if not then I will interrupt the burst read of the MPU-6050 and mess up the data coming through since they are on the same bus.
  float tempF = bme.readTemperature() * 9 / 5 + 32; //convert temperature to Fahrenheit
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; //convert pressure to hPa
  float altitude = bme.readAltitude(1013.25); //estimate altitude based on standard sea level pressure

  // Convert raw values to real units

  // in g's
    float accelX = rawAccelX / ACCEL_SCALE;
    float accelY = rawAccelY / ACCEL_SCALE;
    float accelZ = rawAccelZ / ACCEL_SCALE;

    // in degrees/sec
    float gyroX = rawGyroX / GYRO_SCALE;
    float gyroY = rawGyroY / GYRO_SCALE;
    float gyroZ = rawGyroZ / GYRO_SCALE;

    // Temperature formula from the datasheet
    float tempC = (rawTemp / 340.0f) + 36.53f;

    Serial.printf("X=%4.1fY=%4.1fZ=%4.1f\n", accelX, accelY, accelZ);
    Serial.printf("X=%4.1fY=%4.1fZ=%4.1f\n", gyroX, gyroY, gyroZ);
    Serial.printf("MPU-TEMP(C): %.2f\n", tempC);
    Serial.println("------------------------------------------------");

    Serial.printf("Temperature: %.2f °F\n",  tempF);
    Serial.printf("Humidity: %.2f %%\n", humidity);
    Serial.printf("Pressure: %.2f hPa\n", pressure);
    Serial.printf("Current Estimated Altitude: %.2f m\n", bme.readAltitude(1013.25));
    Serial.println("======================================================");

    //display the accelerometer data on the top 2 yellow rows of 16x128 pixels
    display.printf("X=%4.1fY=%4.1fZ=%4.1f\n", accelX, accelY, accelZ);
    display.printf("X=%4.1fY=%4.1fZ=%4.1f\n", gyroX, gyroY, gyroZ);
    display.printf("MPU-TEMP(C): %.2f\n", tempC);

    //display the temperature
    display.printf("Temp: %.2f °F\n", tempF);

    //display the humidity
    display.printf("Humidity: %.2f %%\n", humidity);
    
    //display the pressure
    display.printf("Press: %.2f hPa\n", pressure);

    //display the altitude
    display.printf("Alt: %.2f m\n", altitude);

    //push everything to the screen
    display.display();

    delay(50);
}