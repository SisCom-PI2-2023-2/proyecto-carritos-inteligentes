#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 2000;
int iniciado = 0; //0 si no se ha iniciado, 1 si ya se inició
int posX;
int posY;

void setup() {
  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  Serial.begin(115200);

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("esp-wifi-inicio", "pass-esp-wifi-inicio");
  wifiMulti.addAP("esp-wifi-a", "pass-esp-wifi-a");
  wifiMulti.addAP("esp-wifi-b", "pass-esp-wifi-b");
}

void loop() {
  // Maintain WiFi connection
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {

    
    Serial.print("Conectado a: ");
    String miSSID = WiFi.SSID();
    Serial.print(miSSID);
    Serial.print(" con RSSI: ");
    float miRSSI = WiFi.RSSI();
    Serial.println(miRSSI);

    //Si el RSSI < 40
    if(miRSSI < 40){
      
      //Si estoy en el inicio y no se ha iniciado el sistema, lo inicio
      if(miSSID == "esp-wifi-inicio" && iniciado == 0){
          inicio_pos();
      }
      
      //sino confirmo si estoy cercano a un beacon
      else if(iniciado == 1){
        confirmar_cercano(); 
      }
    }
    WiFi.disconnect();
    delay(500);
  } else {
    Serial.println("WiFi not connected!");
  }
}

void confirmar_cercano(){
    WiFi.disconnect();
    delay(500);
    if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
      
    Serial.print("ETAPA DE CONFIRMACIÓN, conectado a: ");
    String miSSID = WiFi.SSID();
    Serial.print(miSSID);
    Serial.print(" con RSSI: ");
    
    float miRSSI = WiFi.RSSI();
    if(miRSSI < 40){
      confirmado_cercano(miRSSI,miSSID);
    }
    
  } else {
    Serial.println("WiFi not connected!");
  }
}

void confirmado_cercano(float miRSSI, String miSSID){
    
}

void inicio_pos(){
  iniciado = 1;
  posX = 0;
  posY = 0;
}
