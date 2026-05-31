#include <Arduino.h>
#include <WiFi.h>

void setup() 
{
  Serial.begin(115200);
  delay(1000); // Wait for serial to initialize

  Serial.println("ESP32 WiFi Scanner");

  // Set WiFi to Station mode - scan only and do not connect to an networks
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Disconnect from any previously connected networks

  delay(1000); // Wait for WiFi to initialize

  Serial.println("WiFi setup complete. Starting scan process");
}

void loop() 
{
  Serial.println("==== Scanning for Wifi networks ====");

  int networkCount = WiFi.scanNetworks(); //returns a count of the number of networks found

  if (networkCount == 0) 
  {
    Serial.println("No networks found");
  }
  else
  {
    Serial.println(String(networkCount) + " networks found");
    Serial.println("==================================");

    for (int i = 0; i <networkCount; i++)
    {
      int channel = WiFi.channel(i);

      Serial.println(String(i + 1) + ": " + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + "dBm) " + "Channel: " + String(channel));
      Serial.println("MAC Address: " + WiFi.BSSIDstr(i));

      switch (WiFi.encryptionType(i)) 
      {
        case WIFI_AUTH_OPEN:
          Serial.println("Encryption Type: Open");
          break;
        case WIFI_AUTH_WEP:
          Serial.println("Encryption Type: WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.println("Encryption Type: WPA/PSK");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.println("Encryption Type: WPA2/PSK");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.println("Encryption Type: WPA/WPA2/PSK");
          break;
        default:
          Serial.println("Encryption Type: Unknown");
          break;
      }
      Serial.println();
    }
  }

  WiFi.scanDelete(); // Delete the results of the scan to free memory

  Serial.println("=============================================");
  Serial.println("Waiting 10 seconds and then scanning again");
  delay(10000);
}