//Headers

#include <TimeInterval.h>
#include <Cytron_SmartDriveDuo.h> // Motor Controller library 
#include "QuadEncoder.h"

//Input Pins Defenition

#define IN1 4 // Teensy pin 4 is connected to (MDDS30) Motor Controller pin IN1.
#define AN1 5 // Teensy pin 5 is connected to (MDDS30) Motor Controller pin AN1.
#define AN2 6 // Teensy pin 6 is connected to (MDDS30) Motor Controller pin AN2.
#define IN2 7 // Teensy pin 7 is connected to (MDDS30) Motor Controller pin IN2.
#define LimitApply_Input 8 // Teensy pin 8 is connected to Switch Apply through Input Voltage Divider
#define LimitRetract_Input 9 // Teensy pin 9 is connected to Switch Retract through Input Voltage Divider
#define SwitchApply_Input 10 // Teensy pin 10 is connected to SwitchApply_Input through Input_pullup resistor.
#define SwitchRetract_Input 11 // Teensy pin 11 is connected tp SwitchRetract_Input through Input_pullup resistor.
#define EmergencyPressure_Input 14 // Teensy pin 14 is connected to EmergencyPressure_Input through Input Voltage Divider
#define ServicePressure_Input 15 // Teensy pin 15 is connected to ServicePressure_Input through Input Voltage Divider
QuadEncoder myEnc1(1, 2, 3, 0, 4); // Teensy Pins 2&3 are coneected to the A and B of the Encoder.
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);
//Variable Declarations

int STEPS_PER_MM = 1409;
int MAX_SPEED_APPLY=40; // Positive value to Left
int MAX_SPEED_RETRACT=-40; // Negative to Right
int Displacement; // Displacement
int LimitApply_Value; // Limit Switch Left
int LimitRetract_Value; // Limit Switch Right
int EmergencyPressure_bytes; // Raw bytes
int ServicePressure_bytes; // Raw bytes
int EmergencyPressure_value; // Mapped value
int ServicePressure_value; // Mapped value
int SwitchApply_Value; // Manual Switch control Left
int SwitchRetract_Value;// Manual Switch control Right
uint32_t mCurPosValue; // Currect position of the Encoder
uint32_t old_position = 0; // Old position of the Encoder
uint32_t mCurPosValue1;
uint32_t old_position1 = 0;
String ServiceEmergency_string;// String variables to send Serial data
String ServicePressure_string;
String Displacement_string;
String SerialSendData;
int previous_state = 0;// To keep track of the motors previous state.
char inChar;
signed int speedApply, speedRetract;

elapsedMillis serial_send_timer;
int SERIAL_UPDATE_INTERVAL1= 10; // Delay for Rpi Serial communication
int SERIAL_UPDATE_INTERVAL2= 1; // Delay for Motor Controller Serial Communication
#define BAUDRATE  9600

//Serial read : '0' = Halt 1' = Apply '2'= Retract

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(LimitApply_Input,INPUT);
  pinMode(LimitRetract_Input,INPUT);
  pinMode(SwitchApply_Input,INPUT);
  pinMode(SwitchRetract_Input,INPUT);
  pinMode(EmergencyPressure_Input,INPUT);
  pinMode(ServicePressure_Input,INPUT);
  myEnc1.setInitConfig();
  myEnc1.EncConfig.INDEXTriggerMode = RISING_EDGE;
  myEnc1.init();
  
}

void loop()
{
  // Read inputs
  LimitApply_Value = digitalRead(LimitApply_Input); 
  LimitRetract_Value = digitalRead(LimitRetract_Input);
  EmergencyPressure_bytes = analogRead(EmergencyPressure_Input);
  ServicePressure_bytes=analogRead(ServicePressure_Input);
  SwitchApply_Value=digitalRead(SwitchApply_Input); 
  SwitchRetract_Value=digitalRead(SwitchRetract_Input);
  // Map pressure readings
  
  EmergencyPressure_value = map(EmergencyPressure_bytes, 132 , 726, 0, 100); //
  ServicePressure_value = map(ServicePressure_bytes, 150, 954, 0, 100);
  // Read encoder value
  
  mCurPosValue = myEnc1.read();
  Displacement=mCurPosValue;;
  Displacement=Displacement/STEPS_PER_MM;
  
  // Prepare data for serial communication
  ServiceEmergency_string = String(EmergencyPressure_value);
  ServicePressure_string = String(ServicePressure_value);
  Displacement_string = String(Displacement);
  SerialSendData = ServiceEmergency_string+"+"+ServicePressure_string+"+"+Displacement_string;

  // Send data serially at specified intervals
  if (serial_send_timer>= SERIAL_UPDATE_INTERVAL1)
  {
  serial_send_timer = 0;
  Serial.print(SerialSendData); // Sending the string Serially.
  Serial.print("\n");
  }
 
  if(LimitApply_Value==1)
  { 
    if (Serial.available()) {
    if (serial_send_timer>= SERIAL_UPDATE_INTERVAL2){
    serial_send_timer = 0;
    inChar = Serial.read();
    }
    inChar = Serial.read();
   
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| SwitchApply_Value == 0) {
      if(LimitApply_Value==1)
      {
      halt();
      }
      if(LimitApply_Value==0)
      {
      Apply();
      }
    }
    else if (inChar == '2'||SwitchRetract_Value == 0) 
    {
      Retract(); 
    }
  }

  if ( SwitchApply_Value == 1 && SwitchRetract_Value == 1)
    {
      halt();
    }
    else if (SwitchApply_Value == 0) {
      if(LimitApply_Value == 1)
      {
      halt();
      }
      if(LimitApply_Value == 0)
      {
      Apply();
      }
    }
    else if (SwitchRetract_Value == 0) {
      Retract();
    }

  
  }

  if(LimitRetract_Value==1)
  {
    if (Serial.available()) {
    if (serial_send_timer>= SERIAL_UPDATE_INTERVAL2){
    serial_send_timer = 0;
    inChar = Serial.read();
    }
   
    if (inChar == '0')
    {
      halt();
    }
    else if (inChar == '1'|| SwitchApply_Value == 0) {
      Apply();
    }
    else if (inChar == '2'||SwitchRetract_Value == 0) {
     if(LimitRetract_Value == 1)
      {
      halt();
      }
      if(LimitRetract_Value == 0)
      {
      Retract();
      }
    }
      
    
  }

  if (SwitchApply_Value == 1 && SwitchRetract_Value == 1)
    {
      halt();
    }
    else if (SwitchApply_Value == 0) {
      Apply();
    }
    else if (SwitchRetract_Value == 0) {
     if(LimitRetract_Value == 1)
      {
      halt();
      }
      if(LimitRetract_Value == 0)
      {
      Retract();
      }
  }
  }

  if(LimitApply_Value == 0 && LimitRetract_Value ==0)
  {
    if (Serial.available()) {
    if (serial_send_timer>= SERIAL_UPDATE_INTERVAL2){
    serial_send_timer = 0;
    inChar = Serial.read();
    }
        if (inChar == '0')
    {
      halt();
      previous_state=0;
    }
    else if (inChar == '1' || SwitchApply_Value == 0) {
      Apply();
      previous_state=1;
    }
    else if (inChar == '2'|| SwitchRetract_Value == 0) {
      Retract();
      previous_state=2;     
    }   
  }
    if (SwitchApply_Value == 1 && SwitchRetract_Value == 1 && previous_state == 0)
    {
      halt();
    }
    else if (SwitchApply_Value == 0) {
      Apply();
    }
    else if (SwitchRetract_Value == 0) {
      Retract();
    }
  
}
Motorcontrol();
}
  

void halt()
{
  speedApply = 0;
  speedRetract = 0;
}

void Apply()
{
  if(speedApply==0)
  {
    for (int i = 0; i <= 400; i++) {
    speedApply = i/10;
    speedRetract = i/10;
    
    if (serial_send_timer>= SERIAL_UPDATE_INTERVAL2){
    serial_send_timer = 0;
    Motorcontrol();
    }   
  }
  }
  else
  {
  speedApply = 40;
  speedRetract = 40;
}
}


void Retract()
{
  
  if(speedApply==0)
  {
    for (int i = 0; i <= 400; i++) { 
    speedApply = -i/10;
    speedRetract = -i/10;
    if (serial_send_timer>= SERIAL_UPDATE_INTERVAL2){
    serial_send_timer = 0;
    Serial.print("came here");
    Motorcontrol();
    }     
  }
  }
  
  else
  {
  speedApply = -40;
  speedRetract = -40;
  }
 
}

void Motorcontrol()
{
  smartDriveDuo30.control(speedApply, speedRetract);
}
