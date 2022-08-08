//Brake Force Tester
//Teensy 4.0 Code
//Teensy<->Raspberry pi(Serial)
//Teensy<->Motor Controller(Serial)

//Headers
#include <TimeInterval.h>
#include <SoftwareSerial.h>
#include <Cytron_SmartDriveDuo.h>
#include "QuadEncoder.h"
TimeInterval myDelay; 

//variable defenition
int condition = 0;
int newdisplacement=0;
int previous_state=0;
unsigned long lastButtonPress = 0;
long previousMillis = 0; 
long interval = 50; 
uint32_t mCurPosValue;
uint32_t old_position = 0;
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
char inChar;

QuadEncoder myEnc1(1, 2, 3, 0, 4);

//pin Defenition
#define IN1 5 
#define AN1 7 
#define AN2 4 
#define IN2 6
#define limitapply 8 
#define limitretract 9
#define limitapplysafety 10 
#define limitretractsafety 11
#define dispreset 13
#define supplypressure 14
#define servicepressure 15
#define limitstop 17
#define pressure1in 15
#define motorapply 18
#define motorretract 19
#define BAUDRATE  9600

Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);


signed int speedLeft, speedRight;


void setup()
{
  pinMode(dispreset, INPUT);
  Serial.begin(9600);
  delay(1000); // Delay for 1 second.
  pinMode(supplypressure,INPUT);
  pinMode(servicepressure,INPUT);
  pinMode(limitapply,INPUT);
  pinMode(limitretract,INPUT);
  pinMode(motorapply,INPUT);
  pinMode(motorretract,INPUT);
  pinMode(limitstop,INPUT_PULLUP);
  pinMode(dispreset,INPUT_PULLUP);
  
  myEnc1.setInitConfig();
  myEnc1.EncConfig.INDEXTriggerMode = RISING_EDGE;
  myEnc1.init();
  
}

void loop()
{
  unsigned long currentMillis = millis();
  if (Serial.available()<0)
  {
    condition = 0;
  }
  int y, pressure1, pressure2;
  int limitapplyvalue=digitalRead(limitapply); 
  int limitretractvalue=digitalRead(limitretract);
  int limitapplysafetyvalue=digitalRead(limitapplysafety); 
  int limitretractsafetyvalue=digitalRead(limitretractsafety);
  int limitstopvalue=digitalRead(limitstop);
  int dispresetvalue=digitalRead(dispreset);
  int supplypressurevalue=analogRead(supplypressure);
  int servicepressurevalue=analogRead(servicepressure);
  int motorapplyvalue=digitalRead(motorapply); 
  int motorretractvalue=digitalRead(motorretract);
     
  supplypressurevalue = map(supplypressurevalue, 88 , 485, 0, 100); //Calibrated values 

  servicepressurevalue = map(servicepressurevalue, 100, 636, 0, 100);// Calibrated values
  
   mCurPosValue = myEnc1.read();

  if(mCurPosValue != old_position){
    old_position = mCurPosValue;
  }

  old_position = mCurPosValue;
  displacement=old_position;
  displacement=displacement/2035; // calibration with steps to cm
  if(dispresetvalue==0)
  {
    newdisplacement=displacement;
  }
  displacement=displacement-newdisplacement;
  String supplypressurestr;
  String servicepressurestr;
  String displacementstr;
  String Serialstr;
  supplypressurestr = String(supplypressurevalue);
  servicepressurestr = String(servicepressurevalue);
  displacementstr = String(displacement);

  Serialstr = myStr+"+"+myStr1+"+"+myStr2;

  Serial.print(myStr3);
  Serial.print("\n");

  delay(1); 

  if(limitstop==0)
  {
    
  if(limitapplyvalue==0||limitapplysafetyvalue==0)
  {
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| motorapplyvalue == 0) {
      if(limitapplyvalue==0||limitapplysafetyvalue==0)
      {
      halt();
      }
      if(limitapplyvalue==1||limitapplysafetyvalue==1)
      {
      left();
      }
    }
    else if (inChar == '2'||motorretractvalue == 0) {
      right();
      
    }
    
  }

  if (motorapplyvalue == 1&&motorretractvalue == 1)
    {
      halt();
    }
    else if (motorapplyvalue == 0) {
      if(limitapplyvalue==0||limitapplysafetyvalue==0)
      {
      halt();
      }
      if(limitapplyvalue==1||limitapplysafetyvalue==1)
      {
      left();
      }
    }
    else if (motorretractvalue == 0) {
      right();
    }

  
  }

  if(limitretractvalue==0||limitretractsafetyvalue==0)
  {

    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| motorapplyvalue == 0) {
      left();
    }
    else if (inChar == '2'||motorretractvalue == 0) {
     if(limitretractvalue==0||limitretractsafetyvalue==0)
      {
      halt();
      }
      if(limitretractvalue==1||limitretractsafetyvalue==1)
      {
      right();
      }

     
    }
      
    
  }

  if (motorapplyvalue == 1 && motorretractvalue == 1)
    {
      halt();
    }
    else if (motorapplyvalue == 0) {
      left();
    }
    else if (motorretractvalue == 0) {
     if(limitretractvalue==0||limitretractsafetyvalue==0)
      {
      halt();
      }
      if(limitretractvalue==1||limitretractsafetyvalue==1)
      {
      right();
      }
  }
  }

  
  if(limitapplyvalue==1&&limitretractvalue==1)
  {

    
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();


      if (inChar == '0')
    {
      halt();
      previous_state=0;
    }
    else if (inChar == '1' || motorapplyvalue == 0 ) {
      left();
      previous_state=1;    
    }
    else if (inChar == '2'|| motorretractvalue == 0 ) {
      right();
      previous_state=2;      
    }     
    } 
  }

  if ( motorapplyvalue == 1&& motorapplyretract == 1 && previous_state==0)
    {
      halt();
    }
    else if ( motorapplyvalue== 0) {
      
       left();
    }
    else if ( motorretractvalue== 0) {
      right();
    }

}

}
if(limitstop==1)
{
  halt();
}
smartDriveDuo30.control(speedLeft, speedRight);
}
  

void halt()
{
  speedLeft = 0;
  speedRight = 0;
}

void right()
{
  if(speedLeft==0)
  {
    for (int i = 0; i <= 400; i++) {
    speedLeft = i/10;
    speedRight = i/10;
    delay(1);    
  }
  }
  else
  {
  speedLeft = 40;
  speedRight = 40;
  
}
}


void left()
{
  
  if(speedLeft==0)
  {
    for (int i = 0; i <= 400; i++) { 
    speedLeft = -i/10;
    speedRight = -i/10;
    delay(1)      
  }
  }
  
  else
  {
  speedLeft = -40;
  speedRight = -40;
  }
 
}
