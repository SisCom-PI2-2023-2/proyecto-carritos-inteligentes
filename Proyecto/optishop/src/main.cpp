#include <Arduino.h>
#include <MPU9250_WE.h>
#include <Wire.h>

MPU9250_WE imu;

float dt = 0.01;  // Intervalo de tiempo en segundos (ajusta según sea necesario)
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float roll = 0, pitch = 0;
float velocityX = 0, velocityY = 0, velocityZ = 0;
float positionX = 0, positionY = 0, positionZ = 0;

void setup() {
  Wire.begin();
  imu.setWire(&Wire);
  if (!imu.begin()) {
    Serial.println("No se pudo iniciar el sensor MPU9250. Comprueba las conexiones.");
    while (1);
  }
  Serial.begin(115200);
}

void loop() {
  imu.readSensor();
  
  accX = imu.getAccelX_mss();
  accY = imu.getAccelY_mss();
  accZ = imu.getAccelZ_mss();
  gyroX = imu.getGyroX_rads();
  gyroY = imu.getGyroY_rads();
  gyroZ = imu.getGyroZ_rads();

  // Actualiza la velocidad usando datos del giroscopio
  velocityX += gyroX * dt;
  velocityY += gyroY * dt;
  velocityZ += gyroZ * dt;

  // Actualiza la posición usando datos de aceleración
  positionX += velocityX * dt + 0.5 * accX * dt * dt;
  positionY += velocityY * dt + 0.5 * accY * dt * dt;
  positionZ += velocityZ * dt + 0.5 * accZ * dt * dt;

  // Calcula los ángulos de orientación (pitch y roll) si es necesario
  roll = atan2(accY, accZ) * 180.0 / PI;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * 180.0 / PI;

  // Muestra los resultados
  Serial.print("PositionX: ");
  Serial.print(positionX);
  Serial.print("   PositionY: ");
  Serial.print(positionY);
  Serial.print("   PositionZ: ");
  Serial.println(positionZ);

  delay(dt * 1000);  // Espera según el intervalo de tiempo definido
}