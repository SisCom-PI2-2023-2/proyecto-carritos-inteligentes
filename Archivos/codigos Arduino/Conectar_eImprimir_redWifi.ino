#include "Wire.h"

bool corrigio-b = false;
bool corrigio-c = false;
bool corrigio-d = false;
bool corrigio-e = false;
bool connected = false;

unsigned long now;
unsigned long ultimo_cambio_b;
unsigned long ultimo_cambio_c;
unsigned long ultimo_cambio_d;
unsigned long ultimo_cambio_e;

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
         if(miRSSI < 35 && !corrigio-b){
          Serial.println("2");
          corrigio-b = true;
          ultimo_cambio_b=millis();
         }
         else{
          Serial.println("b");
         }
    }
   if(red=="esp-wifi-c"){
         if(miRSSI < 35 && !corrigio-c){
          Serial.println("3");
          corrigio-c = true;
              ultimo_cambio_c=millis();
         }
         else{
          Serial.println("c");
         }
    }
   if(red=="esp-wifi-d"){
         if(miRSSI < 35 && !corrigio-d){
          Serial.println("4");
          corrigio-d = true;
          ultimo_cambio_d=millis();
         }
         else{
          Serial.println("d");
         }
    }
   if(red=="esp-wifi-e"){
         if(miRSSI < 35 && !corrigio-e){
          Serial.println("5");
          corrigio-e = true;
          ultimo_cambio_e=millis();
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
     
     now = millis();
     if(now - ultimo_cambio_b > 1000 && corrigio-b){
          corrigio-b=false;
     }
     if(now - ultimo_cambio_c > 1000 && corrigio-c){
          corrigio-c=false;
     }
     if(now - ultimo_cambio_d > 1000 && corrigio-d){
          corrigio-d=false;
     }
     if(now - ultimo_cambio_e > 1000 && corrigio-e){
          corrigio-e=false;
     }
     
}


