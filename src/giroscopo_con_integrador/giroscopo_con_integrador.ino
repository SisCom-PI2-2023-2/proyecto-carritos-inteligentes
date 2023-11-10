/***************************************************************************
* Example sketch for the MPU9250_WE library
*
* This sketch shows how to obtain raw accleration data and g values from 
* the MPU9250. 
* 
* For further information visit my blog:
*
* https://wolles-elektronikkiste.de/mpu9250-9-achsen-sensormodul-teil-1  (German)
* https://wolles-elektronikkiste.de/en/mpu9250-9-axis-sensor-module-part-1  (English)
* 
***************************************************************************/

#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68

/* There are several ways to create your MPU9250 object:
 * MPU9250_WE myMPU9250 = MPU9250_WE()              -> uses Wire / I2C Address = 0x68
 * MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR)  -> uses Wire / MPU9250_ADDR
 * MPU9250_WE myMPU9250 = MPU9250_WE(&wire2)        -> uses the TwoWire object wire2 / MPU9250_ADDR
 * MPU9250_WE myMPU9250 = MPU9250_WE(&wire2, MPU9250_ADDR) -> all together
 * Successfully tested with two I2C busses on an ESP32
 */
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
  
  //myMPU9250.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

  myMPU9250.setSampleRateDivider(5);
  
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);

  myMPU9250.enableAccDLPF(true);

  myMPU9250.setAccDLPF(MPU9250_DLPF_6);

  //myMPU9250.setLowPowerAccDataRate(MPU9250_LP_ACC_ODR_500);

  //myMPU9250.sleep(true);

  //myMPU9250.enableCycle(true);

  //myMPU9250.enableAccAxes(MPU9250_ENABLE_XYZ);

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

    //Serial.print("accY:");
  //Serial.print(accY);
  //Serial.print("velY:");
  //Serial.print(velY);
  //Serial.print(",posY:");
  //Serial.print(posY*200);
  Serial.print("thetaZ:");
  Serial.print(thetaZ);
  Serial.println();
  
  }
  
  
}
