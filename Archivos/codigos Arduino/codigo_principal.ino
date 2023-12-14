//========= BIBLIOTECAS =========/

 

#include <ESP8266WiFi.h>  // Biblioteca para generar la conexión a internet a través de WiFi
#include <PubSubClient.h> // Biblioteca para generar la conexión MQTT con un servidor (Ej.: ThingsBoard)
#include <ArduinoJson.h>  // Biblioteca para manejar Json en Arduino
#include <LiquidCrystal_I2C.h> // Biblioteca para manejar el Display
//MPU
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68
const int pinBuzzer = 14;


//========= CONSTANTES =========/
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

// Credenciales de la red WiFi
const char* ssid = "Lucas";
const char* password = "lucas109";

// Host de ThingsBoard
const char* mqtt_server = "mqtt.thingsboard.cloud"; 
const int mqtt_port = 1883;
// Token del dispositivo en ThingsBoard
const char* token = "QD9y5dc6DIJxBUq5MJX0";


// Tipo de sensor
#define DHTTYPE DHT11 // DHT 11
#define DHT_PIN 3     // Conexión en PIN D3

 
// Pines para el encoder
#define D7 13
#define D8 15


//========= VARIABLES =========/

// Guardar el tick actual 
unsigned long now;
int posicion = 0; 

// Objetos de conexión 
WiFiClient espClient;             // Objeto de conexión WiFi
PubSubClient client(espClient);   // Objeto de conexión MQTT

 // Declaración de variables para los datos a manipular
unsigned long lastMsg = 0;  // Control de tiempo de reporte
unsigned long lastScroll = 0;  // Control de tiempo de reporte


boolean TurnoPedidoUsuario1 = false;
boolean TurnoPedidoUsuario2 = false;
int numeroCarniceria = 0;
int numeroUsuario1 = 0;
int diferencia = 0;
int numeroUsuario2 = 0;
int diferencia2 = 0;
int turnoUsuario1 = 0;
int turnoUsuario2 = 0;
int turnoLibre = 1;

int totalColumns = 16;
int totalRows = 2;
LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);  


// Declaración de variables para los datos a manipular
int msgPeriod = 1000;       // Actualizar los datos cada 2 segundos
float posX = 0;
float posY = 0;
boolean led_state = false;

 //Variables para comunicacion serial
int incomingByte = 0;
bool debug = true;

// Mensajes y buffers
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];

 

// Objeto Json para recibir mensajes desde el servidor
DynamicJsonDocument incoming_message(256);

//Variables para el encoder
bool adelante;
int estadoA;
int estadoPrevioA;

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
    //Serial.print(".");
  }
  randomSeed(micros());
  
  Serial.println("");
  Serial.println("¡Conectado!");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
  
}

 
//LCD
String texto1;
String texto2;

String staticMessage;
String scrollingMessage; 
String mensaje1 = "Oferta de Hamburguesas, compra 3 y paga 1.       | ";
String mensaje2 = "Oferta de Alfajores, con la compra de 2 alfajores te llevas un poster de regalo       | ";
String mensaje3 = "El proyecto no sirve con wifi       | ";

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
      if (metodo=="Usuario1") {  
        boolean estado = incoming_message["params"]; 
        //if (estado && !TurnoPedidoUsuario1) { esta linea se va a utilizar cuando querramos que el usuario solo pueda pedir un turno, para realizar las pruebas de concepto conviene tenerla desactivada
        if (estado) {
          numeroUsuario1 = turnoLibre;
          turnoLibre=turnoLibre+1;
           //Serial.println(numeroUsuario1);
           diferencia=numeroUsuario1-numeroCarniceria;
           //TurnoPedidoUsuario1=true;
        } 
      } 
      else {
          if (metodo=="carniceriaAvanza"){
            boolean estado = incoming_message["params"];
            if (estado) {
              //Serial.println("SumarCarniceria");
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
                //Serial.println(numeroCarniceria);
              }
               
            else {
              //Serial.println("SumarCarniceria");
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
            if (estado ) {
              //Serial.println("SumarCarniceria");
              numeroUsuario2 = turnoLibre;
              turnoLibre=turnoLibre+1;
              diferencia2=numeroUsuario2-numeroCarniceria;
               //Serial.println(numeroUsuario2);
            } else {
              //Serial.println("SumarCarniceria");
              numeroUsuario2 = turnoLibre;
              turnoLibre=turnoLibre+1;
              diferencia2=numeroUsuario2-numeroCarniceria;
            }
            }
               }
            }
            if (metodo=="CancelarUsuario1"){
                numeroUsuario1=0;
                diferencia=diferencia-1;
                turnoLibre=turnoLibre-1;
                //TurnoPedidoUsuario1=false;
              }
      }
    }
 
    lcd.setCursor(0, 0);
  lcd.print(staticMessage);

  now = millis();
  if (now - lastScroll > 300) {
    lastScroll = now;
    posicion = scrollMessage(1, scrollingMessage, posicion, totalColumns);
  }

  texto1= String(numeroUsuario1);
  texto2= String(numeroCarniceria);
  staticMessage= "num:"+ texto1+" - Act:"+texto2;


      // Actualizar el atributo relacionado
      DynamicJsonDocument resp(256);
      resp["estado"] = !digitalRead(LED_BUILTIN);
      char buffer[256];
      serializeJson(resp, buffer);
      client.publish("v1/devices/me/attributes", buffer);  //Topico para actualizar atributos
      //Serial.print("Publish message [attribute]: ");
      //Serial.println(buffer);

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

 int scrollMessage(int row, String message, int position, int totalColumns) {
  if(position < (message.length() - totalColumns)) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    position++;
    //delay(delayTime);
  }
  return position %(message.length() - totalColumns);
}
 

//========= SETUP =========/

void setup() {
  pinMode(pinBuzzer, OUTPUT);
  
  lcd.init();                    
  lcd.backlight();
  
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
  //Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  //Serial.println("Done!");
  myMPU9250.setSampleRateDivider(5);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);
  
  myMPU9250.enableGyrDLPF();
  myMPU9250.setGyrDLPF(MPU9250_DLPF_6);
  myMPU9250.setSampleRateDivider(99);
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);


  //ENCODER
  pinMode(D7, INPUT);  // pin D7
  pinMode(D8, INPUT);  // pin D8
  // Lee el estado inicial de la salida A (D1)
  estadoPrevioA = digitalRead(D7);
}



//MPU
unsigned long previousMillis; 
unsigned long currentMillis;
const long interval = 100;
float alpha = 0.9;
float accY = 0;
float accY2;
float med1 = 0;
float med2;
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

String str_pos_x;
String str_pos_y;

unsigned long cuentaAnterior; 
unsigned long cuentaActual;

//========= BUCLE PRINCIPAL =========/

void loop() {

  //IMPRIMIR PUBLICIDADES DEPENDIENDO DE QUE NUMERO LLEGUE POR EL MONITOR SERIAL
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if(incomingByte == 50){
      lcd.setCursor(0, 1);
      lcd.print("publcidad");
    }
    if(incomingByte == 120){
      lcd.setCursor(0, 1);
      lcd.print("no conectado");
    }
    if(debug) Serial.print("I received: ");
    if(debug) Serial.println(incomingByte);
  }

  //ENCODER
  // Lee el estado de la salida A (D7)
  estadoA = digitalRead(D7);
  // Si el estado previo de la salida A (D7) era otro significa que se ha producido un pulso
  if (estadoA != estadoPrevioA) {
    // Si el estado de salida B (D8) es diferente del estado de salida A (D7) el codificador esta girando a la derecha
    if (digitalRead(D8) != estadoA) {
      adelante = true;
    } else {
      adelante = false; 
    }
  }
  //Actualizo el estado
  estadoPrevioA = estadoA;

  //Serial.println("loop");
  // === Conexión e intercambio de mensajes MQTT ===
  if (!client.connected()) {  // Controlar en cada ciclo la conexión con el servidor
    reconnect();              // Y recuperarla en caso de desconexión
  }
  
  client.loop();  // Controlar si hay mensajes entrantes o para enviar al servidor

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
    med2 = med1;
    med1 = accCorrRaw.y*9.8/16384;
    accY2 = accY;
    accY = med1 - med2 - 0.9*accY2;
    
    if(accY<30*9.8/16384 && accY>-30*9.8/16384){
      accY=0;
    }

    velY1 = velY;
    
    velY = alpha*velY1 + (accY*dt)/1000;

    x_m1 = x_m;
    y_m1 = y_m;

    if(adelante){
      if(velY < 0){
        velY = 0;
      }
    }
    if(!adelante){
      if(velY > 0){
        velY = 0;
      }
    }
    

    x_m = x_m1 + ((velY*dt)/1000)*cos(thetaZ);
    
    y_m = y_m1 + ((velY*dt)/1000)*sin(thetaZ);

    

    Serial.print("posx:");
    Serial.print(x_m);
    Serial.print(",posy:");
    Serial.println(y_m);
    Serial.print(adelante);
    cuentaActual = millis();
    if(cuentaActual - cuentaAnterior >= 1000){
      cuentaAnterior = cuentaActual;
      //hayError(x_m,y_m);
      str_pos_x += x_m+5;
      str_pos_y += y_m+5;
    }
    

    if(str_pos_x.length() > 32){
      enviarData();
      str_pos_x = "";
      str_pos_y = "";
    }
  }
}

void hayError(float coordX,float coordY){
  posX = (coordY)*127 + 590;
  posY = (-coordX)*127 + 280;
  bool hay = false;

  //zona izquierda del todo
  if(posX < 14){
    posX = 17;
  }
  //zona derecha del todo
  if(posX > 625){
    posX = 622;
  }
  //zona arriba del todo
  if(posY < 14){
    posY = 17;
  }
  //zona abajo del todo
  if(posY > 353){
    posY = 350;
  }
  //gondola de arriba
  if(posX > 224 && posX < 593 && posY > 12 && posY < 64){
    posY = 67;
  }
  //gondola de abajo
  if(posX > 224 && posX < 594 && posY > 303 && posY < 355){
    posY = 300;
  }
  //gondola del medio
  if(posX > 129 && posX < 549 && posY > 138 && posY < 227){
    if(posX > 534){
      posX = 552;
    }
    if(posX < 114){
      posX = 126;
    }
    if(posY > 182){
      posY = 135;
    }
    else{
      posY = 230;
    }
  }
  //FUNCION INVERSA PARA PASAR DE PIXELES A METROS
  y_m = (posX - 590)/127;
  x_m = -(posY - 280)/127;
}

void enviarData(){
  
  
  // === Realizar las tareas asignadas al dispositivo ===
  // En este caso se medirá temperatura y humedad para reportar periódicamente
  // El control de tiempo se hace con millis para que no sea bloqueante y en "paralelo" completar
  // ciclos del bucle principal
  
    // Publicar los datos en el tópio de telemetría para que el servidor los reciba
    DynamicJsonDocument resp(20480);
    
    resp["numeroCarniceria"] = numeroCarniceria;
    resp["numeroUsuario1"] = numeroUsuario1;
    resp["diferencia"] = diferencia; 
    resp["numeroUsuario2"] = numeroUsuario2;
    resp["diferencia2"] = diferencia2;  
    resp["turnoUsuario1"] = turnoUsuario1;
    resp["turnoUsuario2"] = turnoUsuario2; 
    resp["posX"] = str_pos_x;
    resp["posY"] = str_pos_y;
    
    char buffer[20480];
    serializeJson(resp, buffer);
    client.publish("v1/devices/me/telemetry", buffer);  // Publica el mensaje de telemetría
    //Serial.println("Se enviaron los datos");
    //Serial.print("Publicar mensaje [telemetry]: ");
    //Serial.println(buffer);
}
