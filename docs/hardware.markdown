---
layout: page
title: Hardware
permalink: /Hardware/
---

## Hardware

En esta sección exploraremos los componentes físicos que forman la columna vertebral de nuestra solución. Estos dispositivos son los cimientos sobre los cuales construimos nuestra innovadora creación.

Aquí te guiaremos a través de cada componente, desde las placas de circuito hasta los sensores especializados, revelando cómo cada uno desempeña un papel crucial en el funcionamiento de nuestro proyecto. Estos no son simplemente objetos físicos; son las herramientas que permiten llevar a cabo nuestra solución.

## ESP-8266
 <style>

img {
    width: 345.6px;
    height: 230.4px;
}
</style>

![ESP-8266](/assets/ESP8266.jpg)

La ESP-8266 es la protagonista del proyecto. Con su capacidad de procesamiento es la responsable de recibir todas las entradas, procesarlas y enviar información a la nube. Además toma entradas para desplegar mensajes y emitir sonidos con ayuda de otros módulos.

## ESP-01
![ESP-01](/assets/ESP-01.jpg)

La placa ESP-01 se conecta directamente a la principal mediante una consola serial, para proporcionarle información sobre el Wifi a la que está conectada, y por lo tanto, la zona en la que se encuentra. La misma cuenta con un código MultiWifi que permite cambiar la conexión a lo largo del tiempo.

## MPU-9250
![MPU-9250](/assets/MPU-9250.jpg)

El MPU-9250 es el módulo estrella. Cuenta con acelerómetro y un giróscopo, quienes miden la aceleración del movimiento y del giro que realiza el sistema, permitiendonos a traves del procesamiento de esos datos, determinar la posición del carrito.


## Buzzer Activo
![Buzzer Activo](/assets/BuzzerActivo.jpg)
Es el responsable de las diferentes alarmas. Éste módulo emite diferentes pitidos cuando la placa principal se lo indica. Se usa para advertir sobre nuestro turno en los lugares como la carnicería o fiambrería.

## Encoder Rotativo
![Encoder-Rotativo](/assets/Encoder.jpg)
Proporciona información sobre el sentido del movimiento del sistema. Gracias a esa información, tratamos los datos provistos por el IMU de diferentes maneras, para la correcta obtención de la posición.

## Pantalla LCD
![Pantalla-LCD](/assets/LCD.jpg)
Es nuestra forma de comunicar al cliente. La misma despliega información sobre el turno actual y el turno propio de algun servicio, así como diferentes publicidades u ofertas que varían dependiendo de la zona en la que se encuentre el sistema.




