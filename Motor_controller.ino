/* 
 * This example shows how to control MDDS30 in Serial Simplified mode with Arduino.
 * Set MDDS30 input mode to 0b11001100
 * Open Serial Monitor, set baudrate to 9600bps and "No line ending".
 * Send 0: Left motor stops
 *      1: Left motor rotates CW with half speed
 *      2: Left motor rotates CW with full speed
 *      3: Left motor rotates CCW with half speed
 *      4: Left motor rotates CCW with full speed
 *      5: Right motor stops
 *      6: Right motor rotates CW with half speed
 *      7: Right motor rotates CW with full speed
 *      8: Right motor rotates CCW with half speed
 *      9: Right motor rotates CCW with full speed
 * 
 * Note: This example also compatible with MDDS10 and MDDS60
 *
 * Hardware Connection:
 *   Arduino Uno    MDDS30
 *   GND ---------- GND
 *   4 ------------ IN1
 *
 * Related Products:
 * - SmartDriveDuo-30: http://www.cytron.com.my/P-MDDS60
 * - CT UNO: http://www.cytron.com.my/p-ct-uno
 * - DC Brush Motors: http://www.cytron.com.my/c-84-dc-motor
 * - LiPo Battery: http://www.cytron.com.my/c-87-power/c-97-lipo-rechargeable-battery-and-charger
 * - Male to Male Jumper: https://www.cytron.com.my/p-wr-jw-mm65
 *
 * URL: http://www.cytron.com.my
 */

#include <TimeInterval.h>
#include "QuadEncoder.h"
TimeInterval myDelay; 
int input=18;
int input1=19;

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
#define IN1 4 // Arduino pin 4 is connected to MDDS30 pin IN1.
#define AN1 5 // Arduino pin 5 is connected to MDDS30 pin AN1.
#define AN2 6 // Arduino pin 6 is connected to MDDS30 pin AN2.
#define IN2 7
#define limitswitch1 14 
#define limitswitch2 15
#define pressurein 17
#define pressure1in 16
#define test 9

int displacement;

#define BAUDRATE  9600
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);

char inChar;
signed int speedLeft, speedRight;


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);

  digitalWrite(13, HIGH);
  delay(1000); // Delay for 5 seconds.
  digitalWrite(13, LOW);
  pinMode(input,INPUT_PULLUP);
  pinMode(input1,INPUT_PULLUP);
  pinMode(limitswitch1,INPUT);
  pinMode(test,INPUT);
  pinMode(limitswitch2,INPUT);
  pinMode(pressurein,INPUT);
  pinMode(pressure1in,INPUT);
  //pinMode(CLK,INPUT);
  //pinMode(DT,INPUT);
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
  int a=40;
  //=analogRead(limitswitch1); 
  int b=40;
  //=analogRead(limitswitch2);
  int c=analogRead(pressurein);
  
  int d=analogRead(pressure1in);
  //int e=digitalRead(test);
  int sw1=digitalRead(input); 
  int sw2=digitalRead(input1);
  

  pressure1 = c; 
   
  pressure1 = map(c, 132 , 726, 0, 100);
  if(pressure1<0)
  {
    pressure1=0;
  }
  pressure2 = map(d, 150, 954, 0, 100);
   if(pressure2<0)
  {
    pressure2=0;
  }
 // Serial.print(b);
  //Serial.println(d);
  //Serial.println("      ");
    mCurPosValue = myEnc1.read();

  if(mCurPosValue != old_position){
    /* Read the position values. */
    //Serial.printf("Current position value1: %ld\r\n", mCurPosValue);
    //Serial.printf("Position differential value1: %d\r\n", (int16_t)myEnc1.getHoldDifference());
    //Serial.printf("Position HOLD revolution value1: %d\r\n", myEnc1.getHoldRevolution());
    //Serial.printf("Index Counter: %d\r\n", myEnc1.indexCounter);
    //Serial.println();
  }

  old_position = mCurPosValue;
  displacement=old_position;
 //Serial.print(b);.
  
  displacement=displacement/1409;
  String myStr;
  String myStr1;
  String myStr2;
  String myStr3;
  myStr = String(pressure1);
  myStr1 = String(pressure2);
  myStr2 = String(displacement);
  //Serial.println(counter);  
  myStr3 = myStr+"+"+myStr1+"+"+myStr2;
  Serial.println(myStr3);
  // Serial.println(sw1);
  //Serial.println(sw2);
  //Serial.println(myStr1);
  //Serial.println(myStr2);
  //delay(10);
  
 /* if(a>50)
  {
    
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   
    if (inChar == "0")
    {
      halt();
    }
    else if (inChar == "1"|| sw2 == 0) {
      if(a>50)
      {
      halt();
      }
      if(a<50)
      {
      left();
      }
    }
    else if (inChar == "2"||sw1 == 0) {
      right();
      
    }
    
  }

  if (sw2 == 1&&sw1 == 1)
    {
      halt();
    }
    else if (sw2 == 0) {
      if(a>50)
      {
      halt();
      }
      if(a<50)
      {
      left();
      }
    }
    else if (sw1 == 0) {
      right();
    }

  
  }

  if(b>50)
  {
    Serial.print("CM");
    if (Serial.available()) {
    condition=1;
    delay(1);
    inChar = Serial.read();
   
    if (inChar == "0")
    {
      halt();
    }
    else if (inChar == "1"|| sw2 == 0) {
      left();
    }
    else if (inChar == "2"||sw1 == 0) {
     if(b>50)
      {
      halt();
      }
      if(b<50)
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
     if(b>50)
      {
      halt();
      }
      if(b<50)
      {
      right();
      }
  }
  }*/

  
  if(a<50&&b<50)
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
    else if (inChar == '1' || sw2 == 0) {
      left();
      previous_state=1;
      //Serial.print("Left2");
      //delay(1000);
     
      
    }
    else if (inChar == '2'|| sw1 == 0) {
      right();
      previous_state=2;
      //Serial.print("rit");
      //delay(1000);
      
    }   
  }

}
//Serial.println(speedLeft);
//Serial.println(speedRight);
smartDriveDuo30.control(speedLeft, speedRight);
}
  

void halt()
{
  digitalWrite(13, LOW);
  speedLeft = 0;
  speedRight = 0;
  //Serial.print("stop");
  //delay(1000);
}

void left()
{
  if(speedLeft==0)
  {
    for (int i = 0; i <= 40; i++) {
    speedLeft = i;
    speedRight = i;
    //Serial.println(speedLeft);
    smartDriveDuo30.control(speedLeft, speedRight);
    delay(20);
    digitalWrite(13, HIGH);
    
  }
  }
  else
  {
  digitalWrite(13, HIGH);
  speedLeft = 40;
  speedRight = 40;
  
}
}


void right()
{
  unsigned long currentMillis = millis(); 
  if(speedLeft==0)
  {
    
    for (int i = 0; i <= 40; i++) {
    
    speedLeft = -i;
    speedRight = -i;
    delay(20);
    digitalWrite(13, HIGH);
    
       
  }
  }
  
  else
  {
  digitalWrite(13, HIGH);
  speedLeft = -40;
  speedRight = -40;
  }
 
}
