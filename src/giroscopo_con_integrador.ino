/***************************************************************************
* Example sketch for the MPU9250_WE library
*
* This sketch shows how to obtain gyroscope data from the MPU9250. 
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

  /* The slope of the curve of acceleration vs measured values fits quite well to the theoretical 
   * values, e.g. 16384 units/g in the +/- 2g range. But the starting point, if you position the 
   * MPU9250 flat, is not necessarily 0g/0g/1g for x/y/z. The autoOffset function measures offset 
   * values. It assumes your MPU9250 is positioned flat with its x,y-plane. The more you deviate 
   * from this, the less accurate will be your results.
   * The function also measures the offset of the gyroscope data. The gyroscope offset does not   
   * depend on the positioning.
   * This function needs to be called at the beginning since it can overwrite your settings!
   */
  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  Serial.println("Done!");
  
  /*  The gyroscope data is not zero, even if you don't move the MPU9250. 
   *  To start at zero, you can apply offset values. These are the gyroscope raw values you obtain
   *  using the +/- 250 degrees/s range. 
   *  Use either autoOffset or setGyrOffsets, not both.
   */
  //myMPU9250.setGyrOffsets(-1180, 503, 165);
  
 /*  You can enable or disable the digital low pass filter (DLPF). If you disable the DLPF, you 
   *  need to select the bandwidth, which can be either 8800 or 3600 Hz. 8800 Hz has a shorter delay,
   *  but higher noise level. If DLPF is disabled, the output rate is 32 kHz.
   *  MPU9250_BW_WO_DLPF_3600 
   *  MPU9250_BW_WO_DLPF_8800
   */
  myMPU9250.enableGyrDLPF();
  //myMPU9250.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800); // bandwidth without DLPF
  
  /*  Digital Low Pass Filter for the gyroscope must be enabled to choose the level. 
   *  MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7 
   *  
   *  DLPF    Bandwidth [Hz]   Delay [ms]   Output Rate [kHz]
   *    0         250            0.97             8
   *    1         184            2.9              1
   *    2          92            3.9              1
   *    3          41            5.9              1
   *    4          20            9.9              1
   *    5          10           17.85             1
   *    6           5           33.48             1
   *    7        3600            0.17             8
   *    
   *    You achieve lowest noise using level 6  
   */
  myMPU9250.setGyrDLPF(MPU9250_DLPF_6);

  /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
   *  Sample rate = Internal sample rate / (1 + divider) 
   *  It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
   *  Divider is a number 0...255
   */
  myMPU9250.setSampleRateDivider(99);


  /*  MPU9250_GYRO_RANGE_250       250 degrees per second (default)
   *  MPU9250_GYRO_RANGE_500       500 degrees per second
   *  MPU9250_GYRO_RANGE_1000     1000 degrees per second
   *  MPU9250_GYRO_RANGE_2000     2000 degrees per second
   */
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);
  
  /* sleep() sends the MPU9250 to sleep or wakes it up. 
   * Please note that the gyroscope needs 35 milliseconds to wake up.
   */
  //myMPU9250.sleep(true);

  /* This is a low power standby mode for the gyro function, which allows quick enabling. 
   * (see data sheet for further information)
   */
  //myMPU9250.enableGyrStandby(true);

  
  /* You can enable or disable the axes for gyrometer and/or accelerometer measurements.
   * By default all axes are enabled. Parameters are:  
   * MPU9250_ENABLE_XYZ  //all axes are enabled (default)
   * MPU9250_ENABLE_XY0  // X, Y enabled, Z disabled
   * MPU9250_ENABLE_X0Z   
   * MPU9250_ENABLE_X00
   * MPU9250_ENABLE_0YZ
   * MPU9250_ENABLE_0Y0
   * MPU9250_ENABLE_00Z
   * MPU9250_ENABLE_000  // all axes disabled
   */
  //myMPU9250.enableGyrAxes(MPU9250_ENABLE_000);  
}

float thetaZ = 0;
unsigned long previousMillis = 0; 
const long interval = 100; 

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    xyzFloat gyrRaw = myMPU9250.getGyrRawValues();
    xyzFloat corrGyrRaw = myMPU9250.getCorrectedGyrRawValues();
    xyzFloat gyr = myMPU9250.getGyrValues();
    
    thetaZ = thetaZ + (currentMillis - previousMillis)*gyr.z*1000;
    
    previousMillis = currentMillis;

    Serial.print("thetaZ:");
    Serial.print(thetaZ/100000);
    Serial.print(",omegaZ:");
    Serial.println(gyr.z);
  }
}
