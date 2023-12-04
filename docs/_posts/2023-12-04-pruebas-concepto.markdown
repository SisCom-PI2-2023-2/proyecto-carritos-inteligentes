En este post se presentan las diferentes pruebas de concepto realizadas a lo largo del proyecto con el fin de probar cada componente importante por separado. Asegurando su correcta funcionalidad para posteriormente adaptarlos todos para su uso conjunto.

#### Pruebas realizadas para pantalla LCD y checkpoints wifi
```c++
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C LCD(0x27, 16, 2);
int brillo = 150; // Ajusta este valor según tus necesidades (0-255)
bool connected = false;

#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 1200;


void setup() {
   analogWrite(14, brillo); // Ajusta el brillo de la retroiluminación
   LCD.init();
   LCD.backlight();
   
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
  // Maintain WiFi connection
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    String red = WiFi.SSID();
    if(!connected){
      LCD.clear();
      connected = true;
    }
    LCD.setCursor(1, 0);
    LCD.print(red);
    WiFi.disconnect();
  } else {
    if(connected){
      LCD.clear();
      connected = false;
    }
      LCD.setCursor(1, 1);
      LCD.print("No conectado");
  }
  delay(500);
}
```

#### Pruebas realizadas para encoder contador de vueltas
```c++
/*  Codificador rotativo Keyes KY-040
 *  ( KY-007 para algunos vendedores )
 *  Prueba de conteo, direccion y  pulsador
 *  No se filtra los ruidos de contacto
 */

#define D1 5
#define D2 4
#define D3 0

int contador = 0;
int estadoA;
int estadoPrevioA;
void setup() {
  pinMode(D1, INPUT);  // pin D1
  pinMode(D2, INPUT);  // pin D2
  // el pulsador debe ser polarizado a valor ALTO
  pinMode(D3, INPUT_PULLUP);

  Serial.begin(9600);
  // Lee el estado inicial de la salida A (D1)
  estadoPrevioA = digitalRead(D1);
}
void loop() {
  // Lee el estado de la salida A (D1)
  estadoA = digitalRead(D1);
  // Si el estado previo de la salida A (D1) era otro
  // significa que se ha producido un pulso
  if (estadoA != estadoPrevioA) {
    // Si el estado de salida B (D2) es diferente del estado
    // de salida A (D1) el codificador esta girando a la derecha
    if (digitalRead(D2) != estadoA) {
      contador++;
    } else {
      contador--; 
    }
    Serial.print("Posición: ");
    Serial.println(contador*0.5105);
  }
  // actualiza el estado guardado
  estadoPrevioA = estadoA;

  //Serial.print(B);
  if (!digitalRead(D3))  // si se pulsa el D3 su valor va a BAJO
  {
    Serial.println("D3 pulsado: Contador a 0");
    contador = 0;
    delay(100);
  }
}
```
