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
   pinMode (D1,INPUT); // pin D1
   pinMode (D2,INPUT); // pin D2
   // el pulsador debe ser polarizado a valor ALTO
   pinMode (D3, INPUT_PULLUP);
   
   Serial.begin (9600);
   // Lee el estado inicial de la salida A (D1)
   estadoPrevioA = digitalRead(D1);   
 } 
 void loop() 
 {
   // Lee el estado de la salida A (D1)
   estadoA = digitalRead(D1);
   // Si el estado previo de la salida A (D1) era otro
   // significa que se ha producido un pulso
   if (estadoA != estadoPrevioA){     
     // Si el estado de salida B (D2) es diferente del estado
     // de salida A (D1) el codificador esta girando a la derecha
     if (digitalRead(D2) != estadoA) { 
       contador ++;
     } else {
       contador --;
     }
     Serial.print("Posición: ");
     Serial.println(contador);
   } 
   // actualiza el estado guardado
   estadoPrevioA = estadoA;
 
  bool Bot = digitalRead(D3);
  //Serial.print(B);
   if (!Bot) // si se pulsa el D3 su valor va a BAJO
    { Serial.println("D3 pulsado: Contador a 0");
      contador = 0 ;
      delay(300);
    }
 }