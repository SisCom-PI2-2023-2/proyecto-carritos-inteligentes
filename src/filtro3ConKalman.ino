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

void loop() {
  // eventually update your evolution matrix inside the loop
  K.F = {1.0,  0.2,
         0.0,  1.0};
  
  // GRAB MEASUREMENT and WRITE IT INTO 'obs'
  get_sensor_data();
  
  // APPLY KALMAN FILTER
  K.update(obs);

  // PRINT RESULTS: measures and estimated state
  Serial.print("medicionesY:");
  Serial.print(obs(1));
  //Serial.print(",mediciones y:");
  //Serial.print(obs(1));
  Serial.print(",aproxY:");
  Serial.print(K.x(1));
  //Serial.print(",aprox y:");
  //Serial.println(K.x(1));
  Serial.println();

}

//-----------------------------------
/****     GET SENSOR DATA       ****/
//-----------------------------------

float res0 = 0;
float res1 = 0;
float res2;
float res3;
float med0 = 0;
float med1 = 0;
float med2;
float med3;

float phi = 0;

void get_sensor_data(){
  
  xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues();
  
   med2 = med1;
  med1 = med0;
  med0 = accCorrRaw.y;
  
  res3 = res2;
  res3 = res2;
  res2 = res1;
  res1 = res0;
  
  res0 = med0 - (2*cos(2*PI*phi) + 1)*med1 + (2*cos(2*PI*phi) + 1)*med2 - med3 - 0.9*res1;
  
  // It is your job to fill in this method
  // grab data from your accelerometer, GPS, etc...
  obs(0) = accCorrRaw.x; // some dummy measurement
  obs(1) = res1; // some dummy measurement
  
}
