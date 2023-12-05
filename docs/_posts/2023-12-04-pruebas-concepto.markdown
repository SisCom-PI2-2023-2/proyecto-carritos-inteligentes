<style>
   .primerPruebaEncoder{
      width: 25%
      height: 25%
   }
   
</style>

En la fase inicial de nuestro proyecto, nos sumergimos en el desarrollo individualizado de cada componente, asegurándonos de que funcionara de manera eficiente por separado. Luego de avanzar, decidimos llevar a cabo pruebas de concepto, uniendo estas partes preexistentes para evaluar su rendimiento en conjunto. Esta estrategia nos ha permitido no solo validar el éxito individual de cada elemento, sino también comprender su impacto colectivo. Las pruebas de concepto han revelado valiosas sinergias y oportunidades para mejoras adicionales.

#### Pruebas realizadas para pantalla LCD y checkpoints wifi
En esta prueba se pretendió probar el cambio de red a la cual se conectaría la placa principal realizando movimientos en sectores con diversas redes wifi, para esto se utilizó la pantalla LCD como ayuda para poder visualizar la red a la cual esta conectada la placa.
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

<video width="320" height="240" controls>
  <source src="https://github.com/SisCom-PI2-2023-2/proyecto-carritos-inteligentes/raw/main/docs/_posts/assetsPosts/PruebaWifiLCD.mp4" type="video/mp4">
  Tu navegador no soporta el elemento de video.
</video>


#### Pruebas realizadas para encoder contador de vueltas
Con esta prueba se pretendía conseguir una relación de las mediciones del encoder con distancias medidas en metros y centímetros. Para esto se creo un artefato que facilitó el agarre del encoder para realizar las mediciones.
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
![PrimerPruebaEncoder](https://github.com/SisCom-PI2-2023-2/proyecto-carritos-inteligentes/blob/main/docs/_posts/assetsPosts/PrimerPruebaEncoder.jpg?raw=true){:class=primerPruebaEncoder}
#### Pruebas realizadas para MPU-9250 giroscopio con método integrador incluido
Esta prueba fue realizada con el fin de probar la exactitud de medición del giroscopio con el método de integración discreta aplicado, para esto se colocó las conexiones de la placa y el MPU-9250 poseedor del giroscopio sobre un carro y se realizó un giro de 90 grados. 
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
<video width="320" height="240" controls>
  <source src="https://github.com/SisCom-PI2-2023-2/proyecto-carritos-inteligentes/raw/main/docs/_posts/assetsPosts/PruebaGiroscopio.mp4" type="video/mp4">
  Tu navegador no soporta el elemento de video.
</video>
#### Pruebas realizadas para aplicación de Kalman y filtrado
```c++
/* 
 * This code is a minimal empty shell.
 * Fill it and modify it to match your own case.
 * 
 * Author:
 *  R.JL. Fétick
 * 
 * Revision:
 *  24 Aug 2019 - Creation
 */

#include <Kalman.h>
using namespace BLA;

//IMU
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68

//------------------------------------
/****       KALMAN PARAMETERS    ****/
//------------------------------------

// Dimensions of the matrices
#define Nstate 2 // length of the state vector
#define Nobs 2   // length of the measurement vector

// measurement std (to be characterized from your sensors)
#define n1 0.2 // noise on the 1st measurement component
#define n2 0.1 // noise on the 2nd measurement component 

// model std (~1/inertia). Freedom you give to relieve your evolution equation
#define m1 0.01
#define m2 0.02

KALMAN<Nstate,Nobs> K; // your Kalman filter
BLA::Matrix<Nobs> obs; // observation vector

// Note: I made 'obs' a global variable so memory is allocated before the loop.
//       This might provide slightly better speed efficiency in loop.

//Crear el IMU
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

//-----------------------------------
/****           SETUP           ****/
//-----------------------------------

void setup() {

  Serial.begin(57600);
  
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

  // example of evolution matrix. Size is <Nstate,Nstate>
  K.F = {1.0, 0.0,
         0.0, 1.0};
  // example of measurement matrix. Size is <Nobs,Nstate>
  K.H = {1.0, 0.0,
         0.0, 1.0};
  // example of measurement covariance matrix. Size is <Nobs,Nobs>
  K.R = {n1*n1,   0.0,
           0.0, n2*n2};
  // example of model covariance matrix. Size is <Nstate,Nstate>
  K.Q = {m1*m1,   0.0,
           0.0, m2*m2};
  


  myMPU9250.setSampleRateDivider(5);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);
}

//-----------------------------------
/****            LOOP           ****/
//-----------------------------------

float res1 = 0;
float res2;
float med1 = 0;
float med2;

void loop() {
  // eventually update your evolution matrix inside the loop
  K.F = {1.0,  0.2,
         0.0,  1.0};
  
  // GRAB MEASUREMENT and WRITE IT INTO 'obs'
  get_sensor_data();
  
  // APPLY KALMAN FILTER
  K.update(obs);

  med2 = med1;
  med1 = K.x(1);
  res2 = res1;
  res1 = med1 - med2 - 0.9*res2;
    
  // PRINT RESULTS: measures and estimated state
  Serial.print("sinfiltro:");
  Serial.print(K.x(0));
  Serial.print(",confiltro:");
  Serial.println(res1);
  Serial.println();
}

//-----------------------------------
/****     GET SENSOR DATA       ****/
//-----------------------------------


void get_sensor_data(){
  
  xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues();

  // It is your job to fill in this method
  // grab data from your accelerometer, GPS, etc...
  obs(0) = accCorrRaw.y; // some dummy measurement
  obs(1) = accCorrRaw.y; // some dummy measurement
  
}
```
#### Pruebas realizadas para solicitar número en ThingsBoard y buzzer activo
```c++
//========= BIBLIOTECAS =========/

 

#include <ESP8266WiFi.h>  // Biblioteca para generar la conexión a internet a través de WiFi
#include <PubSubClient.h> // Biblioteca para generar la conexión MQTT con un servidor (Ej.: ThingsBoard)
#include <ArduinoJson.h>  // Biblioteca para manejar Json en Arduino
//#include "DHT.h"          // Biblioteca para trabajar con DHT 11 (Sensor de temperatura y humedad)

 


//========= CONSTANTES =========/

 

// Credenciales de la red WiFi
const char* ssid = "HUAWEI-IoT";
const char* password = "ORTWiFiIoT";

 

// Host de ThingsBoard
//const char* mqtt_server = "mqtt.thingsboard.cloud";
const char* mqtt_server = "demo.thingsboard.io"; 
const int mqtt_port = 1883;

 const int pinBuzzer = 4;

// Token del dispositivo en ThingsBoard
const char* token = "TQDD6rCoubscnCk8NByq";

 

// Tipo de sensor
//#define DHTTYPE DHT11 // DHT 11
//#define DHT_PIN 3     // Conexión en PIN D3

 


//========= VARIABLES =========/

 

// Objetos de conexión
WiFiClient espClient;             // Objeto de conexión WiFi
PubSubClient client(espClient);   // Objeto de conexión MQTT

 

// Objetos de Sensores o Actuadores
//DHT dht(DHT_PIN, DHTTYPE);

 

// Declaración de variables para los datos a manipular
unsigned long lastMsg = 0;  // Control de tiempo de reporte
int msgPeriod = 2000;       // Actualizar los datos cada 2 segundos
float humidity = 0;
float temperature = 0;
boolean led_state = false;
float numeroCarniceria = 0;
float numeroUsuario1 = 0;
float diferencia = 0;
float numeroUsuario2 = 0;
float diferencia2 = 0;
float turnoUsuario1 = 0;
float turnoUsuario2 = 0;
float turnoLibre = 1;

 

// Mensajes y buffers
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];

 

// Objeto Json para recibir mensajes desde el servidor
DynamicJsonDocument incoming_message(256);

 

//========= FUNCIONES =========/

 

// Inicializar la conexión WiFi
void setup_wifi() {

 

  delay(10);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

 

  WiFi.mode(WIFI_STA); // Declarar la ESP como STATION
  WiFi.begin(ssid, password);

 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

 

  randomSeed(micros());

 

  Serial.println("");
  Serial.println("¡Conectado!");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

 


// Función de callback para recepción de mensajes MQTT (Tópicos a los que está suscrita la placa)
// Se llama cada vez que arriba un mensaje entrante (En este ejemplo la placa se suscribirá al tópico: v1/devices/me/rpc/request/+)
void callback(char* topic, byte* payload, unsigned int length) {
 
  // Log en Monitor Serie
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

 

  // En el nombre del tópico agrega un identificador del mensaje que queremos extraer para responder solicitudes
  String _topic = String(topic);

 

  // Detectar de qué tópico viene el "mensaje"
  if (_topic.startsWith("v1/devices/me/rpc/request/")) { // El servidor "me pide que haga algo" (RPC)
    // Obtener el número de solicitud (request number)
    String _request_id = _topic.substring(26);

 

    // Leer el objeto JSON (Utilizando ArduinoJson)
    deserializeJson(incoming_message, payload); // Interpretar el cuerpo del mensaje como Json
    String metodo = incoming_message["method"]; // Obtener del objeto Json, el método RPC solicitado

 

    // Ejecutar una acción de acuerdo al método solicitado
    if (metodo=="checkStatus") {  // Chequear el estado del dispositivo. Se debe responder utilizando el mismo request_number
     
      char outTopic[128];
      ("v1/devices/me/rpc/response/"+_request_id).toCharArray(outTopic,128);
     
      DynamicJsonDocument resp(256);
      resp["status"] = true;
      char buffer[256];
      serializeJson(resp, buffer);
      client.publish(outTopic, buffer);
     
    } else {
      if (metodo=="Usuario1") {  // Establecer el estado del led y reflejar en el atributo relacionado
       
        boolean estado = incoming_message["params"]; // Leer los parámetros del método 
  
        if (estado) {
          digitalWrite(LED_BUILTIN, LOW); // Encender LED
          Serial.println("Encender LED");
          numeroUsuario1 = turnoLibre;
          turnoLibre=turnoLibre+1;
           Serial.println(numeroUsuario1);
           diferencia=numeroUsuario1-numeroCarniceria;
        } else {
          digitalWrite(LED_BUILTIN, HIGH); // Apagar LED
          Serial.println("Apagar LED");
          numeroUsuario1 = turnoLibre;
          turnoLibre=turnoLibre+1;
          diferencia=numeroUsuario1-numeroCarniceria;
        }
    } else {
          if (metodo=="carniceriaAvanza"){
            boolean estado = incoming_message["params"];
            if (estado) {
              Serial.println("SumarCarniceria");
              numeroCarniceria = numeroCarniceria+1;
              diferencia=numeroUsuario1-numeroCarniceria;
              diferencia2=numeroUsuario2-numeroCarniceria;
               if(diferencia==3){
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
              }
              if(diferencia==2){
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
              }
              if(diferencia==1){
                digitalWrite(pinBuzzer, HIGH);
                delay(1000);
                digitalWrite(pinBuzzer, LOW);
              }
              if(diferencia==0){
                turnoUsuario1=1;
                }
                if(diferencia2==0){
                turnoUsuario2=1;
                }
                if(diferencia<0){
                numeroUsuario1=0;
                turnoUsuario1=0;
                diferencia=0;
                }
                if(diferencia2<0){
                numeroUsuario2=0;
                turnoUsuario2=0;
                diferencia2=0;
                }
                Serial.println(numeroCarniceria);
              }
               
            else {
              Serial.println("SumarCarniceria");
              numeroCarniceria = numeroCarniceria+1;
              diferencia=numeroUsuario1-numeroCarniceria;
              diferencia2=numeroUsuario2-numeroCarniceria;
              if(diferencia==3){
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
              }
              if(diferencia==2){
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
                digitalWrite(pinBuzzer, HIGH);
                delay(250);
                digitalWrite(pinBuzzer, LOW);
                delay(250);
              }
              if(diferencia==1){
                digitalWrite(pinBuzzer, HIGH);
                delay(1000);
                digitalWrite(pinBuzzer, LOW);
              }
              if(diferencia==0){
                turnoUsuario1=1;
                }
                if(diferencia2==0){
                turnoUsuario2=1;
                }
                if(diferencia<0){
                numeroUsuario1=0;
                }
                if(diferencia2<0){
                numeroUsuario2=0;
                }
            }
            
             }else{
              if (metodo=="Usuario2"){
            boolean estado = incoming_message["params"];
            if (estado) {
              Serial.println("SumarCarniceria");
              numeroUsuario2 = turnoLibre;
              turnoLibre=turnoLibre+1;
              diferencia2=numeroUsuario2-numeroCarniceria;
               Serial.println(numeroUsuario2);
            } else {
              Serial.println("SumarCarniceria");
              numeroUsuario2 = turnoLibre;
              turnoLibre=turnoLibre+1;
              diferencia2=numeroUsuario2-numeroCarniceria;
            }
            }
               }
            }
      }
    }
 

      // Actualizar el atributo relacionado
      DynamicJsonDocument resp(256);
      resp["estado"] = !digitalRead(LED_BUILTIN);
      char buffer[256];
      serializeJson(resp, buffer);
      client.publish("v1/devices/me/attributes", buffer);  //Topico para actualizar atributos
      Serial.print("Publish message [attribute]: ");
      Serial.println(buffer);
    }
 

 

// Establecer y mantener la conexión con el servidor MQTT (En este caso de ThingsBoard)
void reconnect() {
 
  // Bucle hasta lograr la conexión
  while (!client.connected()) {
    Serial.print("Intentando conectar MQTT...");
    if (client.connect("ESP8266", token, token)) {  //Nombre del Device y Token para conectarse
      Serial.println("¡Conectado!");
     
      // Una vez conectado, suscribirse al tópico para recibir solicitudes RPC
      client.subscribe("v1/devices/me/rpc/request/+");
     
    } else {
     
      Serial.print("Error, rc = ");
      Serial.print(client.state());
      Serial.println("Reintenar en 5 segundos...");
      // Esperar 5 segundos antes de reintentar
      delay(5000);
     
    }
  }
}

 

//========= SETUP =========/

 

void setup() {
  // Conectividad
  Serial.begin(115200);                   // Inicializar conexión Serie para utilizar el Monitor
  setup_wifi();                           // Establecer la conexión WiFi
  client.setServer(mqtt_server, mqtt_port);// Establecer los datos para la conexión MQTT
  client.setCallback(callback);           // Establecer la función del callback para la llegada de mensajes en tópicos suscriptos

 if (!client.connected()) {  // Controlar en cada ciclo la conexión con el servidor
    reconnect();              // Y recuperarla en caso de desconexión
  }
  pinMode(pinBuzzer, OUTPUT);
 

  // Sensores y actuadores
  pinMode(LED_BUILTIN, OUTPUT);       // Inicializar el LED como salida
//  pinMode(DHT_PIN, INPUT);            // Inicializar el DHT como entrada
//  dht.begin();                        // Iniciar el sensor DHT
}

 

//========= BUCLE PRINCIPAL =========/

 

void loop() {

 

  // === Conexión e intercambio de mensajes MQTT ===
  if (!client.connected()) {  // Controlar en cada ciclo la conexión con el servidor
    reconnect();              // Y recuperarla en caso de desconexión
  }
 

  // === Realizar las tareas asignadas al dispositivo ===
  // En este caso se medirá temperatura y humedad para reportar periódicamente
  // El control de tiempo se hace con millis para que no sea bloqueante y en "paralelo" completar
  // ciclos del bucle principal
 
  unsigned long now = millis();
  if (now - lastMsg > msgPeriod) {
    lastMsg = now;
   
  //  temperature = dht.readTemperature();  // Leer la temperatura
  //  humidity = dht.readHumidity();        // Leer la humedad

  // Publicar los datos en el tópio de telemetría para que el servidor los reciba
  DynamicJsonDocument resp(256);
  resp["temperature"] = random(-5,30); //temperature;  //Agrega el dato al Json, ej: "temperature": 21.5
  //resp["humidity"] = random(10,90); //humidity;        //Agrega el dato al Json, ej: "humidituy: 75.0
  resp["numeroCarniceria"] = numeroCarniceria;
  resp["numeroUsuario1"] = numeroUsuario1;
  resp["diferencia"] = diferencia; 
  resp["numeroUsuario2"] = numeroUsuario2;
  resp["diferencia2"] = diferencia2;  
  resp["turnoUsuario1"] = turnoUsuario1;
  resp["turnoUsuario2"] = turnoUsuario2; 
  char buffer[256];
  serializeJson(resp, buffer);
  client.publish("v1/devices/me/telemetry", buffer);  // Publica el mensaje de telemetría
 
  Serial.print("Publicar mensaje [telemetry]: ");
  Serial.println(buffer);

  client.loop();              // Controlar si hay mensajes entrantes o para enviar al servidor

}
}
```
