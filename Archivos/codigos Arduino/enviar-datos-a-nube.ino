//========= BIBLIOTECAS =========/

 

#include <ESP8266WiFi.h>  // Biblioteca para generar la conexión a internet a través de WiFi
#include <PubSubClient.h> // Biblioteca para generar la conexión MQTT con un servidor (Ej.: ThingsBoard)
#include <ArduinoJson.h>  // Biblioteca para manejar Json en Arduino
//MPU
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68



//========= CONSTANTES =========/
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

// Credenciales de la red WiFi
const char* ssid = "HUAWEI-IoT";
const char* password = "ORTWiFiIoT";

// Host de ThingsBoard
const char* mqtt_server = "mqtt.thingsboard.cloud"; 
const int mqtt_port = 1883;
// Token del dispositivo en ThingsBoard
const char* token = "QD9y5dc6DIJxBUq5MJX0";
// Tipo de sensor
#define DHTTYPE DHT11 // DHT 11
#define DHT_PIN 3     // Conexión en PIN D3

 


//========= VARIABLES =========/

 

// Objetos de conexión 
WiFiClient espClient;             // Objeto de conexión WiFi
PubSubClient client(espClient);   // Objeto de conexión MQTT

 

// Declaración de variables para los datos a manipular
unsigned long lastMsg = 0;  // Control de tiempo de reporte
int msgPeriod = 1000;       // Actualizar los datos cada 2 segundos
float posX = 0;
float posY = 0;
boolean led_state = false;

 

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
      
    } else if (metodo=="setLedStatus") {  // Establecer el estado del led y reflejar en el atributo relacionado
      
      boolean estado = incoming_message["params"]; // Leer los parámetros del método

 

      if (estado) {
        digitalWrite(LED_BUILTIN, LOW); // Encender LED 
        Serial.println("Encender LED");
      } else {
        digitalWrite(LED_BUILTIN, HIGH); // Apagar LED
        Serial.println("Apagar LED");
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
  }
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


//MPU
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

//MPU
unsigned long previousMillis; 
unsigned long currentMillis;
const long interval = 100;
float alpha = 0.9;
float accY = 0;
float accY1;
float velY = 0;
float velY1;
float posY1 = 0;
float hposY = 0;
 //GYRO
float thetaZ = 0;
float x_m = 0;
float y_m = 0;
float x_m1 = 0;
float y_m1 = 0;

//========= BUCLE PRINCIPAL =========/

void loop() {
   //MPU
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

    x_m1 = x_m;
    y_m1 = y_m;

    if(velY < 0){
      velY = 0;
    }

    x_m = x_m1 + ((velY*dt)/1000)*cos(thetaZ);
    y_m = y_m1 + ((velY*dt)/1000)*sin(thetaZ);


    //Serial.print("accY:");
  //Serial.print(accY);
  /*
  Serial.print("thetaZ:");
  Serial.print(thetaZ);

  
  Serial.print(",x_m:");
  Serial.print(x_m*10);
  
  Serial.print(",y_m:");
  Serial.print(y_m*10);
  Serial.println();
  */
  }


  // === Conexión e intercambio de mensajes MQTT ===
  if (!client.connected()) {  // Controlar en cada ciclo la conexión con el servidor
    reconnect();              // Y recuperarla en caso de desconexión
  }
  
  client.loop();              // Controlar si hay mensajes entrantes o para enviar al servidor


  // === Realizar las tareas asignadas al dispositivo ===
  // En este caso se medirá temperatura y humedad para reportar periódicamente
  // El control de tiempo se hace con millis para que no sea bloqueante y en "paralelo" completar
  // ciclos del bucle principal
  
  unsigned long now = millis();
  if (now - lastMsg > msgPeriod) {
    lastMsg = now;
 

    // Publicar los datos en el tópio de telemetría para que el servidor los reciba
    DynamicJsonDocument resp(256);
    resp["posX"] = x_m;
    resp["posY"] = y_m;
    char buffer[256];
    serializeJson(resp, buffer);
    client.publish("v1/devices/me/telemetry", buffer);  // Publica el mensaje de telemetría
    
    //Serial.print("Publicar mensaje [telemetry]: ");
    //Serial.println(buffer);
    
  }
}
