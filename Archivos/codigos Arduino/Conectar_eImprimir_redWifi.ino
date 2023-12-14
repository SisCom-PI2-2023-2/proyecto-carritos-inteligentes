#include "Wire.h"


bool connected = false;

#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;
float miRSSI = WiFi.RSSI();

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 1200;


void setup() {
     // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  Serial.begin(115200);

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("esp-wifi-a", "esp_wifi_a");
  wifiMulti.addAP("esp-wifi-b", "esp_wifi_b");
  wifiMulti.addAP("esp-wifi-c", "esp_wifi_c");
  wifiMulti.addAP("esp-wifi-d", "esp_wifi_d");
  wifiMulti.addAP("esp-wifi-e", "esp_wifi_e");
  // More is possible
}
/*
    This sketch shows how to use multiple WiFi networks.

    In this example, ESP8266 works in AP mode.
    It demonstrates:
    - Fast connect to previous WiFi network at startup
    - Registering multiple networks (at least 1)
    - Connect to WiFi with strongest signal (RSSI)
    - Fall back to connect to next WiFi when a connection failed or lost
    - Fall back to connect to hidden SSID's which are not reported by WiFi scan

    To enable debugging output, select in the Arduino iDE:
    - Tools | Debug Port: Serial
    - Tools | Debug Level: WiFi
*/


void loop() {

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    String red = WiFi.SSID();
    if(!connected){
      connected = true;
    }
    if(red=="esp-wifi-b"){
         if(miRSSI < 35);{
          Serial.println("2");
         }
         else{
          Serial.println("b");
         }
    }
   if(red=="esp-wifi-c"){
         if(miRSSI < 35);{
          Serial.println("3");
         }
         else{
          Serial.println("c");
         }
    }
   if(red=="esp-wifi-d"){
         if(miRSSI < 35);{
          Serial.println("4");
         }
         else{
          Serial.println("d");
         }
    }
   if(red=="esp-wifi-e"){
         if(miRSSI < 35);{
          Serial.println("5");
         }
         else{
          Serial.println("d");
         }
    }
    WiFi.disconnect();
  } else {
    if(connected){
      connected = false;
    }
      Serial.println("x");//No conectado
  }
  delay(500);
}
