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
#define PI 3.1415926535897932384626433832795

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
  
  /*  This is a more accurate method for calibration. You have to determine the minimum and maximum 
   *  raw acceleration values of the axes determined in the range +/- 2 g. 
   *  You call the function as follows: setAccOffsets(xMin,xMax,yMin,yMax,zMin,zMax);
   *  Use either autoOffset or setAccOffsets, not both.
   */
  //myMPU9250.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

  /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
   *  Sample rate = Internal sample rate / (1 + divider) 
   *  It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
   *  Divider is a number 0...255
   */
  myMPU9250.setSampleRateDivider(5);
  
  /*  MPU9250_ACC_RANGE_2G      2 g   
   *  MPU9250_ACC_RANGE_4G      4 g
   *  MPU9250_ACC_RANGE_8G      8 g   
   *  MPU9250_ACC_RANGE_16G    16 g
   */
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);

  /*  Enable/disable the digital low pass filter for the accelerometer 
   *  If disabled the bandwidth is 1.13 kHz, delay is 0.75 ms, output rate is 4 kHz
   */
  myMPU9250.enableAccDLPF(true);

  /*  Digital low pass filter (DLPF) for the accelerometer, if enabled 
   *  MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7 
   *   DLPF     Bandwidth [Hz]      Delay [ms]    Output rate [kHz]
   *     0           460               1.94           1
   *     1           184               5.80           1
   *     2            92               7.80           1
   *     3            41              11.80           1
   *     4            20              19.80           1
   *     5            10              35.70           1
   *     6             5              66.96           1
   *     7           460               1.94           1
   */
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);

  /*  Set accelerometer output data rate in low power mode (cycle enabled)
   *   MPU9250_LP_ACC_ODR_0_24          0.24 Hz
   *   MPU9250_LP_ACC_ODR_0_49          0.49 Hz
   *   MPU9250_LP_ACC_ODR_0_98          0.98 Hz
   *   MPU9250_LP_ACC_ODR_1_95          1.95 Hz
   *   MPU9250_LP_ACC_ODR_3_91          3.91 Hz
   *   MPU9250_LP_ACC_ODR_7_81          7.81 Hz
   *   MPU9250_LP_ACC_ODR_15_63        15.63 Hz
   *   MPU9250_LP_ACC_ODR_31_25        31.25 Hz
   *   MPU9250_LP_ACC_ODR_62_5         62.5 Hz
   *   MPU9250_LP_ACC_ODR_125         125 Hz
   *   MPU9250_LP_ACC_ODR_250         250 Hz
   *   MPU9250_LP_ACC_ODR_500         500 Hz
   */
  //myMPU9250.setLowPowerAccDataRate(MPU9250_LP_ACC_ODR_500);

  /* sleep() sends the MPU9250 to sleep or wakes it up. 
   * Please note that the gyroscope needs 35 milliseconds to wake up.
   */
  //myMPU9250.sleep(true);

 /* If cycle is set, and standby or sleep are not set, the module will cycle between
   *  sleep and taking a sample at a rate determined by setLowPowerAccDataRate().
   */
  //myMPU9250.enableCycle(true);

  /* You can enable or disable the axes for gyroscope and/or accelerometer measurements.
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
  //myMPU9250.enableAccAxes(MPU9250_ENABLE_XYZ);
  
}
float res0 = 0;
float res1 = 0;
float res2;
float res3;
float med0 = 0;
float med1 = 0;
float med2;
float med3;

float phi = 1/4;

//float G = 1/10;
void loop() {
  xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues();
  med3 = med2;
  med2 = med1;
  med1 = med0;
  med0 = accCorrRaw.y;
  
  res3 = res2;
  res3 = res2;
  res2 = res1;
  res1 = res0;
  
  res0 = med0 - (2*cos(2*PI*phi) + 1)*med1 + (2*cos(2*PI*phi) + 1)*med2 - med3 - 0.9*res1;
  
  Serial.print("accCorrRawy:");
  Serial.print(accCorrRaw.y);
  Serial.print(",yfiltrado:");
  Serial.println(res0);

}
