

#include <TimeInterval.h>
#include "QuadEncoder.h"
TimeInterval myDelay; 
int input=18;
int input1=19;
int limitstop=17;
int dispreset=13;
int newdisplacement=0;

int condition = 0;
unsigned long lastButtonPress = 0;
int previous_state=0;
long previousMillis = 0; 
long interval = 50; 
uint32_t mCurPosValue;
uint32_t old_position = 0;
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
QuadEncoder myEnc1(1, 2, 3, 0, 4);

#include <SoftwareSerial.h>
#include <Cytron_SmartDriveDuo.h>
#define IN1 5 
#define AN1 7 
#define AN2 4 
#define IN2 6
#define limitapply 8 
#define limitretract 9
#define limitapplysafety 10 
#define limitretractsafety 11
#define pressurein 14
#define pressure1in 15
//#define test 9 

int displacement;

#define BAUDRATE  9600
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);

char inChar;
signed int speedLeft, speedRight;


void setup()
{
  pinMode(dispreset, INPUT);
  Serial.begin(9600);

  //digitalWrite(13, HIGH);
  delay(1000); // Delay for 5 seconds.
  //digitalWrite(13, LOW);
  pinMode(input,INPUT_PULLUP);
  pinMode(input1,INPUT_PULLUP);
  pinMode(limitapply,INPUT);
//  pinMode(test,INPUT);
  pinMode(limitretract,INPUT);
  pinMode(pressurein,INPUT);
  pinMode(pressure1in,INPUT);
  pinMode(limitstop,INPUT_PULLUP);
  pinMode(dispreset,INPUT_PULLUP);
  //pinMode(CLK,INPUT);
  //pinMode(DT,INPUT);
  myEnc1.setInitConfig();
  myEnc1.EncConfig.INDEXTriggerMode = RISING_EDGE;

  myEnc1.init();
  
}

void loop()
{
  
  if (Serial.available()<0)
  {
    condition = 0;
  }
  int y, pressure1, pressure2;
  int a=digitalRead(limitapply); 
  int b=digitalRead(limitretract);
  int g=digitalRead(limitapplysafety); 
  int h=digitalRead(limitretractsafety);
  int i=digitalRead(limitstop);
  int j=digitalRead(dispreset);
  int c=analogRead(pressurein);
  
  int d=analogRead(pressure1in);
  //int e=digitalRead(test);
  int sw1=digitalRead(input); 
  int sw2=digitalRead(input1);

  //Serial.print();
  

  pressure1 = c; 
  
   
  pressure1 = map(d, 92 , 502, 0 , 100);
  if(pressure1<0)
  {
    pressure1=0;
  }
  pressure2 = map(c, 103, 660, 0, 100);
   if(pressure2<0)
  {
    pressure2=0;
  }
 // Serial.print(b);
  //Serial.println(d);
  //Serial.println("      ");
    mCurPosValue = myEnc1.read();

  

  old_position = mCurPosValue;
  displacement=old_position;
 //Serial.print(b);.
  
  displacement=displacement/2035;
  if(j==0)
  {
    myEnc1.init();
  }
  
  String myStr;
  String myStr1;
  String myStr2;
  String myStr3;
  myStr = String(pressure1);
  myStr1 = String(pressure2);
  myStr2 = String(displacement);
  //Serial.println(counter);  
  myStr3 = myStr+"+"+myStr1+"+"+myStr2;
  //Serial.print(c);
  //Serial.print(sw1);
  Serial.print(myStr3);
  Serial.print("\n");
  // Serial.println(sw1);
  //Serial.println(sw2);
  //Serial.println(myStr1);
  //Serial.println(myStr2);
  delay(1);

  if(i==1)
  {
     
  if(a==0||g==0)
  {
    
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| sw2 == 0) {
      if(a==0||g==0)
      {
      halt();
      }
      if(a==1||g==1)
      {
      left();
      }
    }
    else if (inChar == '2'||sw1 == 0) {
      right();
      
    }
    
  }

  if (sw2 == 1&&sw1 == 1)
    {
      halt();
    }
    else if (sw2 == 0) {
      if(a==0||g==0)
      {
      halt();
      }
      if(a==1||g==1)
      {
      left();
      }
    }
    else if (sw1 == 0) {
      right();
    }

  
  }

  if(b==1||h==0)
  {
    //Serial.print("CM");
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| sw2 == 0) {
      left();
    }
    else if (inChar == '2'||sw1 == 0) {
     if(b==1||h==0)
      {
      halt();
      }
      if(b==0||h==1)
      {
      right();
      }

     
    }
      
    
  }

  if (sw2 == 1&&sw1 == 1)
    {
      halt();
    }
    else if (sw2 == 0) {
      left();
    }
    else if (sw1 == 0) {
     if(b==1||h==0)
      {
      halt();
      }
      if(b==0||h==1)
      {
      right();
      }
  }
  }

  
  if(a==1&&b==0)
  {
    
  //delay(500);
    
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   // Serial.print(inChar);

      if (inChar == '0')
    {
      halt();
      previous_state=0;
      //Serial.print("Stoooooppppp");
      //delay(1000);
    }
    else if (inChar == '1' || sw2 == 0 ) {
      left();
      previous_state=1;
      //Serial.print("Left2");
      //delay(1000);
     
      
    }
    else if (inChar == '2'|| sw1 == 0 ) {
      right();
      previous_state=2;
      //Serial.print("rit");
      //delay(1000);
      
    }  
     else if (inChar == '3'|| sw1 == 0 ) {
      slowleft();
      previous_state=2;
      //Serial.print("rit");
      //delay(1000);
      
    } 
  }

  if (sw2 == 1&&sw1 == 1 && previous_state==0)
    {
      halt();
    }
    else if (sw2 == 0) {
      
       left();
    }
    else if (sw1 == 0) {
      right();
    }

}
//Serial.println(speedLeft);
//Serial.println(speedRight);
smartDriveDuo30.control(speedLeft, speedRight);
}

}
  

void halt()
{
  //digitalWrite(13, LOW);
  speedLeft = 0;
  speedRight = 0;
  //Serial.print("stop");
  //delay(1000);
}

void right()
{
  if(speedLeft==0)
  {
    for (int i = 0; i <= 400; i++) {
    speedLeft = i/10;
    speedRight = i/10;
    //Serial.println(speedLeft);
    smartDriveDuo30.control(speedLeft, speedRight);
    delay(1);
    //digitalWrite(13, HIGH);
    
  }
  }
  else
  {
  //digitalWrite(13, HIGH);
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
    smartDriveDuo30.control(speedLeft, speedRight);
    delay(1);
    
    
       
  }
  }
  
  else
  {
  //digitalWrite(13, HIGH);
  speedLeft = -40;
  speedRight = -40;
  }
 
}

void slowleft()
{
 speedLeft = 20;
 speedRight = 20; 
 //Serial.print("stop");
}
