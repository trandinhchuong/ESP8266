#include <Wire.h>
#include <Kalman.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#define ToRad PI/180
#define TODeg 180/PI

Kalman kalman;
int TX_PIN= 1;
int RX_PIN= 0;
SoftwareSerial Serial1(RX_PIN, TX_PIN);
#define factortheta PI/20
#define factorphi PI/180

int inChar;
uint32_t timerloop, timerold;

//motor control pin//
int leftpwm= 9;
int leftdir= 5;
int righpwm= 10;
int righdir= 12;

volatile long leftencoder;
volatile long righencoder;
int leftencoder_a= 2;
int leftencoder_b= 6;
int righencoder_a= 3;
int righencoder_b= 7;

//MPU6050 Data//
float mpudata;
float AcX, AcZ;
float Gyro;

uint32_t timer;
uint8_t i2cData[14];

//LQR data//
long PWML, PWMR;
float k1, k2, k3, k4, k5, k6;
bool falldown;

float theta, psi, phi;
float thetadot, psidot, phidot;
float thetaold, psiold, phiold;

float leftvolt;
float righvolt;

float addtheta;
float addphi;

int ForwardBack;
int LeftRigh;
int left_isr;
int righ_isr;


////////////////////////////////////////////
///////////   SERIAL BEGIN   ///////////////
///////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  k1=18;
  k2=11;
  k3=355;
  k4=17;
  k5=37;
  k6=2;
//Set state control motor begin
  ForwardBack= 0;
  LeftRigh= 0;
//Set zero setpoint value
  addphi= 0;
  addtheta= 0;

//Set PWM FREQUENCY 31KHZ
TCCR2B = TCCR2B & B11111000 | B00000001;

//OUTPUT PIN CONTROL MOTOR LEFT AND righ
pinMode(leftpwm, OUTPUT);
pinMode(righpwm, OUTPUT);
pinMode(leftdir, OUTPUT);
pinMode(righdir, OUTPUT);

// INPUT PIN READ ENCODER
pinMode(leftencoder_a, INPUT_PULLUP);
pinMode(leftencoder_b, INPUT_PULLUP);
pinMode(righencoder_a, INPUT_PULLUP);
pinMode(righencoder_b, INPUT_PULLUP);

//INTERRUPT ENCODER
attachInterrupt(0, left_isr, RISING);
attachInterrupt(1, righ_isr, RISING);

//DATA MPU6050
Wire.begin();

TWBR = ((F_CPU/400000L) - 16)/2;
i2cData[0]= 7;
i2cData[1]= 0x00;
i2cData[2]= 0x00;
i2cData[3]= 0x00;
while (i2cWrite(0x19, i2cData, 4, false));
while (i2cWrite(0x6b, 0x01, true));
while (i2cRead(0x75, i2cData, 1));
if (i2cData[0] != 0x68) {
  Serial.print(F("Error reading sensor"));
  while (1);
  
}
delay(100);

while (i2cRead(0x3B, i2cData, 6));
AcX = (i2cData[0] <<8) | i2cData[1];
AcZ = (i2cData[4] <<8) | i2cData[5];

double pitch = atan2(-AcX, AcZ)*RAD_TO_DEG;
kalman.setAngle(pitch);
timer= micros();
}
////////////////////////////////////////////////////////////
/////////////   MAIN PROGRAMMING  /////////////////////////
//////////////////////////////////////////////////////////
void loop() {
  readmpu();

  if(micros() - timerloop) > 6000) {
    theta = gettheta(leftencoder, righencoder)*ToRad;
    psi = (mpudata + 2.1)*ToRad;
    phi = getphi(leftencoder, righencoder)*ToRad;

    //UPDATE TIME COMPARE WITH TIMELOOP
    float dt = (float)(micros() - timerloop)/1000000.0;
    timerloop = micros();
    //UPDATE INPUT ANGLE VALUE
    thetadot = (theta - thetaold)/dt;
    psidot = (psi)/dt;
    phidot = (phi - phiold)/dt;
    //UPDATE OLD ANGLE VALUE
    thetaold = theta;
    psiold = psi;
    phiold = phi;
    //
    addtheta = alltheta + ForwardBack*factortheta;
    addphi = addphi + LeftRigh*factorphi;

    getlqr(theta + addtheta, thetadot, psi, psidot, phi + addphi, phidot);
    motorcontrol(PWML, PWMR,(mpudata + 2.1), falldown);



    String S = "";
    S = (String)(psi*ToDeg) + "," + (String)(theta*ToDeg) + "," + (String)(phi*ToDeg) + "," + (String)(-addtheta*ToDeg) + "," + (String)(-addphi*ToDeg);
    Serial1.println(S);
  }
 }

//LEFT MOTOR ENCODER INTERRUPT
void left_isr(); {
  if(digitalRead(leftencoder_b)) {
    leftencoder++;
  }
  else {
    leftencoder--;
  }
}

//RIGH MOTOR ENCODER INTERRUPT
 void righ_isr(); {
   if(digitalRead(righencoder_b)) {
    righencoder++;
  }
  else {
    righencoder--;
  }
 }
// READ PSI
void readmpu() {
  while (i2cRead(0x3B, i2cData, 14));
  AcX = ((i2cData[0] <<8) | i2cData[1]);
  AcZ = ((i2cData[4] <<8) | i2cData[5]); 
  Gyro = ((i2cData[10] <<8) | i2cData[11]);

  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  double pitch = atan2(-AcX, AcZ)*RAD_TO_DEG;
  double Gyrorate = Gyro/131.0;

  mpudata = kalman.getAngle(pitch, Gyrorate, dt);
  //
  if(abs(mpudata) > 30)
  {
    falldowm = true;
  }
  else
  { 
    falldown = false;
  }
  //
  }
  //READ THETA ANGLE FUNCTION
  float gettheta(long lencoder, long rencoder) {
    float angle = 0.9*(lencoder + rencoder);
    return angle;
}
//READ PHI ANGLE FUNCTION
float getphi(long lencoder, long rencoder) {
  float angle = (7.2/22.5)*(lencoder - rencoder);
    return angle;
}
//LQR FUNCTION
void getlqr(float theta_, float thetadot_, float psi_, float psidot_, float phi_, float phidot_) {
  leftvolt = k1*theta_ + k2*thetadot_ + k3*psi_ + k4*psidot_ + k5*phi_ + k6*psidot_;
  righvolt = k1*theta_ + k2*thetadot_ + k3*psi_ + k4*psidot_ + k5*phi_ + k6*psidot_;

  PWML = map(leftvolt, -(k3*PI)/15, -(k3*PI)/15, -250, 250);
  PWMR = map(righvolt, -(k3*PI)/15, -(k3*PI)/15, -250, 250);

  PWML = constrain(PWML, -240, 240);
  PWMR = constrain(PWMR, -240, 240);
}
//MOTOR CONTROL FUNCTION
void motorcontrol(long lpwm, long rpwm, float angle, bool stopstate) {
  if(stopstate == true) {
    stopandreset();
    }
    else {
      if(abs(angle) > 30)
      {
        stopandreset();
       
      }
      else
      {
        if(leftvolt > 0);
        {
          leftmotor(abs(lpwm), 1);
        }
        else if(leftvolt < 0)
        {
          leftmotor(abs(lpwm), 0);
        }
        else 
        {
          stopandreset();
        }
        if(righvolt > 0)
        {
          righmotor(abs(rpwm), 1);
        }
        else  if(righvolt < 0)
        {
          righmotor(abs(rpwm), 0);
        }
      }
    }
 
}
// STOP MOTOR AND RESET DATA
void stopandreset()
{
  digitalWrite(leftpwm, LOW);
  digitalWrite(righpwm, LOW);

  leftencoder = 0;
  righencoder = 0;

  addtheta = 0;
  addphi = 0;
  
}
//CONTROL LEFT MOTOR
void leftmotor(uint8_t lpwm, int direct) {
  if(direct == 1) {
    digitalWrite(leftdir, HIGH);
    analogWrite(leftpwm, lpwm);
    
  }
  else {
     digitalWrite(leftdir, LOW);
     analogWrite(leftpwm, lpwm);
  }
}
//CONTROL RIGH MOTOR
void righmotor(uint8_t rpwm, int direct) {
    if(direct == 1) {
       digitalWrite(righdir, HIGH);
       analogWrite(righpwm, rpwm);
    }
    else {
     digitalWrite(righdir, LOW);
     analogWrite(righpwm, rpwm);
    }
}
// SERIAL INTERRUPT RECEICE DATA CONTROL
void serialEvent1() {
  while (Serial1.available() > 0)
  {
    inChar = Serial1.read();
    
  }
  //CONTROL MOTOR
  if(inChar == 70)
  {
    ForwardBack = 1;
    
  }
  if(inChar == 84)
  {
    ForwardBack = 0;
  }
  if(inChar == 66)
  {
   ForwardBack = -1;
  }
  if(inChar == 86)
  {
   ForwardBack = 0; 
  }
  if(inChar == 76)
  {
   LeftRigh = 1; 
  }
  if(inChar == 85)
  {
   LeftRigh = 0;  
  }
  if(inChar == 82)
  {
   LeftRigh = -1;  
  }
  if(inChar == 79)
  {
   LeftRigh = 0;   
  }
}
