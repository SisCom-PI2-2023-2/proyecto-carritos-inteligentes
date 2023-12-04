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
#### Pruebas realizadas para MPU-9250 giroscopio con método integrador incluido
```c++
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if(!myMPU9250.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected");
  }
  
  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  Serial.println("Done!");

  myMPU9250.setSampleRateDivider(5);

  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);

  myMPU9250.enableGyrDLPF();
  myMPU9250.setGyrDLPF(MPU9250_DLPF_6);
  myMPU9250.setSampleRateDivider(99);
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);
  
}

unsigned long previousMillis; 
unsigned long currentMillis;
const long interval = 100;
float alpha = 0.9;
float accY = 0;
float accY1;
float velY = 0;
float velY1;
float posY1 = 0;
float posY = 0;

//GYRO
float thetaZ = 0;

void loop() {
  
  currentMillis = millis();
  long dt = currentMillis - previousMillis ;
  
  if (dt >= interval) {
    previousMillis = currentMillis;

    xyzFloat gyr = myMPU9250.getGyrValues();
     if(gyr.z<0.05 && gyr.z>-0.05){
      gyr.z=0;
    }
    thetaZ = thetaZ + ((dt*gyr.z)/1000)*0.0174533;
    if(thetaZ > 6.283 || thetaZ < -6.283){
      thetaZ = 0;
    }
    
    xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues();
    accY = accCorrRaw.y*9.8/16384;
    if(accY<30*9.8/16384 && accY>-30*9.8/16384){
      accY=0;
    }

    velY1 = velY;
    
    velY = alpha*velY1 + (accY*dt)/1000;

    posY1 = posY;

    if(velY < 0){
      velY = 0;
    }

    posY = posY1 + (velY*dt)/1000;

    x_m = posY*cos(thetaZ);
    y_m = posY*sin(thetaZ);

  Serial.print("thetaZ:");
  Serial.print(thetaZ);
  Serial.println();
  
  }
  
  
}
```
