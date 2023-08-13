//Headers

#include <Cytron_SmartDriveDuo.h> // Motor Controller library from https://github.com/CytronTechnologies/Cytron_SmartDriveDuo
#include "QuadEncoder.h" // Quadurature encoder library from Teensyduino

//Input Pins Defenition
#define EncoderA_pin 2
#define EncoderB_pin 3
#define IN1 4 // Teensy pin 4 is connected to (MDDS30) Motor Controller pin IN1.
#define AN1 5 // Teensy pin 5 is connected to (MDDS30) Motor Controller pin AN1.
#define AN2 6 // Teensy pin 6 is connected to (MDDS30) Motor Controller pin AN2.
#define IN2 7 // Teensy pin 7 is connected to (MDDS30) Motor Controller pin IN2.
#define LimitApply_Input       8 // Teensy pin 8 is connected to Switch Apply through Input Voltage Divider
#define LimitRetract_Input     9 // Teensy pin 9 is connected to Switch Retract through Input Voltage Divider
#define SafetyRetract_Input   10 // Teensy pin 10 is connected to the second safety Switch Retract through Input Voltage Divider
#define SafetyApply_Input     11 // Teensy pin 11 is connected to the second safety Switch Apply through Input Voltage Divider
#define ResetButton_Input     13 // Input Pullup for reseting the Displacement and starting a new serial stream header
#define SupplyPressure_Input  14 // Teensy pin 14 is connected to SupplyPressure_Input through Input Voltage Divider
#define ServicePressure_Input 15 // Teensy pin 15 is connected to ServicePressure_Input through Input Voltage Divider
#define EmergencyStop_Input   17 // Teensy pin 17 is connected to the Read Emergency stop Button on the top.
#define SwitchApply_Input     18 // Teensy pin 10 is connected to SwitchApply_Input through Input_pullup resistor.
#define SwitchRetract_Input   19 // Teensy pin 11 is connected tp SwitchRetract_Input through Input_pullup resistor.

#define BlueLED_pin   16
#define GreenLED_pin  21
#define YellowLED_pin 23
#define OrangeLED_pin 22

#define PRESSED  0
#define RELEASED 1

//Calibrations for the pressure sensor
int SupplyPress_bytes_low = 98; // from AnalogRead
int SupplyPres_value_low = 0; // Tenths of a PSI

int SupplyPress_bytes_high = 507; // from AnalogRead
int SupplyPres_value_high = 800; // Tenths of a PSI

int ServicePress_bytes_low = 88; // from AnalogRead
int ServicePres_value_low = 0; // Tenths of a PSI

int ServicePress_bytes_high = 469; // from AnalogRead
int ServicePres_value_high = 1000; // Tenths of a PSI

//TODO: Kaushik, please change these magic numbers to a define (e.g. #define anImportantNumber 1)  and use the labels below
QuadEncoder positionEncoder(1, EncoderA_pin, EncoderB_pin, 0, 4); // Teensy Pins 2&3 are coneected to the A and B of the Encoder.
Cytron_SmartDriveDuo smartDriveDuo30(PWM_INDEPENDENT, IN1, IN2, AN1, AN2);

//Variable Declarations
const double STEPS_PER_MM = 1409.0;
const int MAX_SPEED =  40; // Positive value to Left in percent
const int MIN_SPEED = -40; // Negative to Right in percent

char Direction[9] = {};

double Displacement; // Displacement in mm
int32_t mCurPosValue; // Currect position of the Encoder

bool LimitApply_Value; // Limit Switch Left
bool LimitRetract_Value; // Limit Switch Right
bool SafetyApply_Value; // Limit Switch Left
bool SafetyRetract_Value; // Limit Switch Right
bool SwitchApply_Value; // Manual Switch control Left
bool SwitchRetract_Value;// Manual Switch control Right
bool SerialApply_Value; // Command from serial connection to apply actuator
bool SerialRetract_Value; // Command from serial connection to retract actuator
bool ResetButton_Value; // Value of the button on the box
bool EmergencyStop_Value; // Value of the Red Emergency stop button on the box

bool BlueLED_state = HIGH;
bool GreenLED_state = HIGH;
bool YellowLED_state = HIGH;
bool OrangeLED_state = HIGH;


int16_t SupplyPressure_bytes; // Raw bytes
int16_t ServicePressure_bytes; // Raw bytes
double SupplyPressure_Value; // Mapped value
double ServicePressure_Value; // Mapped value

int ActuatorVelocity;
int magnitude_increase;

elapsedMillis control_loop_timer;
elapsedMillis serial_send_timer;
elapsedMillis ramp_timer;
elapsedMillis run_timer;
elapsedMillis serial_RX_timer;

const uint32_t control_loop_time = 50; //milliseconds
const uint32_t serial_send_time  = 100; //milliseconds
const uint32_t ramp_time = 50; //milliseconds
const uint32_t serial_RX_time = 1000;
uint32_t run_time = 0; //milliseconds


bool Timeout_Value = 0;

void setup()
{
  delay(200);
  ActuatorVelocity = 0;
  smartDriveDuo30.control(ActuatorVelocity, ActuatorVelocity);
  
  pinMode(BlueLED_pin,OUTPUT);
  pinMode(GreenLED_pin,OUTPUT);
  pinMode(YellowLED_pin,OUTPUT);
  pinMode(OrangeLED_pin,OUTPUT);
  
 
  pinMode(ResetButton_Input, INPUT_PULLUP);
  pinMode(LimitApply_Input,INPUT_PULLUP);
  pinMode(LimitRetract_Input,INPUT_PULLUP);
  pinMode(SafetyApply_Input,INPUT_PULLUP);
  pinMode(SafetyRetract_Input,INPUT_PULLUP);
  pinMode(SwitchApply_Input,INPUT_PULLUP);
  pinMode(SwitchRetract_Input,INPUT_PULLUP);
  pinMode(SupplyPressure_Input,INPUT); // Analog Input
  pinMode(ServicePressure_Input,INPUT); // Analog Input
  pinMode(EmergencyStop_Input, INPUT_PULLUP);

  positionEncoder.setInitConfig();
  positionEncoder.EncConfig.INDEXTriggerMode = RISING_EDGE;
  positionEncoder.init();
  Timeout_Value = 1;
  ResetButton_Value = PRESSED;
}

void loop()
{
  if (control_loop_timer >= control_loop_time){
    control_loop_timer = 0;

    // Do this for reset (this will enter upon the first loop if the ResetButton_Value is PRESSED
    if (ResetButton_Value == PRESSED){
      ActuatorVelocity = 0;
      smartDriveDuo30.control(ActuatorVelocity, ActuatorVelocity);
      magnitude_increase = 0;
      Displacement = 0;
      positionEncoder.init();
      mCurPosValue = 0;
      BlueLED_state = HIGH;
      GreenLED_state = HIGH;
      run_timer = 0;
      while (Serial.available()) Serial.read();
      Serial.println();
      Serial.println("Time, Displacement, Direction, Actuator Velocity, Service Pressure, Supply Pressure, Apply Limit Switch,Retract Limit Switch, Apply Safety Switch, Retract Safety Switch");
      //TODO: Kaushik, please check the engineering units
      Serial.println("[millis], [mm], [label], [%], [kpa], [kpa], [0 = pressed], [0 = pressed], [0 = pressed], [0 = pressed]");
      serial_RX_timer = 0;
    }

    // Read inputs
    readInputs();
    // Get user input from User by Raspberry Pi over Serial
    while (Serial.available()) {
      
      serial_RX_timer = 0;
      Timeout_Value = 1;
      char inChar = Serial.read();
      if (inChar == '0') {
        magnitude_increase = 0;
        ActuatorVelocity =0 ;
        SerialRetract_Value = 0;
        SerialApply_Value = 0; 
      }
      else if (inChar == '1') {
        magnitude_increase = 1;
        SerialRetract_Value = 0;
        SerialApply_Value = 1;
      }
      else if (inChar == '2'){
        magnitude_increase = -1;
        SerialRetract_Value = 1;
        SerialApply_Value = 0;
        
      }
      else if (inChar == 'F'){
        //Read Force Value
//        char buff[100] = {};
//        Serial.readuntil('\n')
//        millipounds = atoi(buff); 
      }
      else {
        magnitude_increase = 0;
        SerialRetract_Value = 0;
        SerialApply_Value = 0;  
      
      }
      Serial.flush();
    }
   

    // Get User input from box mounted switch
    if (SwitchApply_Value == PRESSED) magnitude_increase = 1;
    else if (SwitchRetract_Value == PRESSED) magnitude_increase = -1;
    else magnitude_increase = 0;

    //check limits
    if (LimitApply_Value == PRESSED || SafetyApply_Value == PRESSED){ // Limit Switch Left
      ActuatorVelocity = 0; 
      BlueLED_state = HIGH;
    } 
    else{
      BlueLED_state = LOW;
    }

    //check limits
    if (LimitRetract_Value == PRESSED || SafetyRetract_Value == PRESSED){ // Limit Switch Right
      ActuatorVelocity = 0;
      GreenLED_state = HIGH;
    } 
    else{
      GreenLED_state = LOW;
    }

    //Direction is based on the sign (Direction) of the actuator velocity value
    memset(Direction,0,sizeof(Direction));
    if (ActuatorVelocity > 0) {
      memcpy(&Direction,"Apply",5);
      YellowLED_state = HIGH;
      OrangeLED_state = LOW;
    }
    else if (ActuatorVelocity < 0) {
      memcpy(&Direction,"Retract",7);
      YellowLED_state = LOW;
      OrangeLED_state = HIGH;
    }
    else {
      memcpy(&Direction,"Halt",5);
      YellowLED_state = HIGH;
      OrangeLED_state = HIGH;
    }

    // Change the actuator velocity according to a constant increase with respect to time
    if (ramp_timer >= ramp_time){
      ramp_timer = 0;
      ActuatorVelocity += magnitude_increase;
    }

//    // Make sure a Serial message from the Pi is available
//    if (serial_RX_timer > serial_RX_time) {
//      ActuatorVelocity = 0;
//      Timeout_Value = 0;
//    }
//    
    if (EmergencyStop_Value == PRESSED){
      ActuatorVelocity = 0;
    }
    
    //Constrain the values
    if (ActuatorVelocity < MIN_SPEED) ActuatorVelocity = MIN_SPEED;
    else if (ActuatorVelocity > MAX_SPEED) ActuatorVelocity = MAX_SPEED;

    //Send the Command to the motor     
    smartDriveDuo30.control(ActuatorVelocity, ActuatorVelocity);
    
    
    digitalWrite(BlueLED_pin,BlueLED_state);
    digitalWrite(GreenLED_pin,GreenLED_state);
    digitalWrite(OrangeLED_pin,OrangeLED_state);
    digitalWrite(YellowLED_pin,YellowLED_state);


    // Send data serially at specified intervals
    if (serial_send_timer>= serial_send_time){
      serial_send_timer = 0;
      run_time = run_timer;
      char buffer[40];
      sprintf(buffer, "% 8s,", Direction);
      Serial.printf("% 7d,",run_time);
      Serial.print(buffer);
      Serial.printf("% 4+d,",ActuatorVelocity);
      Serial.printf("% 8.3f,",Displacement);
      Serial.printf("%d,",SwitchApply_Value);
      Serial.printf("%d,",SwitchRetract_Value);
      Serial.printf("%d,",SerialApply_Value);
      Serial.printf("%d,",SerialRetract_Value);
      Serial.printf("% 4d,",ServicePressure_bytes);
      Serial.printf("%7.2f,",ServicePressure_Value);
      Serial.printf("% 4d,",SupplyPressure_bytes);
      Serial.printf("%7.2f,",SupplyPressure_Value);
      Serial.printf("%d,",SafetyRetract_Value);
      Serial.printf("%d,",SafetyApply_Value);
      Serial.printf("%d,",LimitRetract_Value);
      Serial.printf("%d,",LimitApply_Value);
      Serial.printf("%d,",Timeout_Value);
      Serial.printf("%d\n",EmergencyStop_Value);
    }
  } 
}


void readInputs(){
  LimitApply_Value = digitalRead(LimitApply_Input); 
  LimitRetract_Value = !digitalRead(LimitRetract_Input); //This sensor was wired as normally closed.
  SupplyPressure_bytes = analogRead(SupplyPressure_Input);
  ServicePressure_bytes = analogRead(ServicePressure_Input);
  SwitchApply_Value = digitalRead(SwitchApply_Input); 
  SwitchRetract_Value = digitalRead(SwitchRetract_Input);
  ResetButton_Value = digitalRead(ResetButton_Input);
  EmergencyStop_Value = !digitalRead(EmergencyStop_Input); //Normally closed
  SafetyApply_Value =digitalRead(SafetyApply_Input);
  SafetyRetract_Value =digitalRead(SafetyRetract_Input);
  
  // Read encoder value
  mCurPosValue = positionEncoder.read();
  Displacement = double(mCurPosValue)/STEPS_PER_MM;
  
  // Map pressure readings
  
  SupplyPressure_Value  = double(map(SupplyPressure_bytes, SupplyPress_bytes_low,  SupplyPress_bytes_high,  SupplyPres_value_low,  SupplyPres_value_high ))/10.0; //
  ServicePressure_Value = double(map(ServicePressure_bytes,ServicePress_bytes_low, ServicePress_bytes_high, ServicePres_value_low, ServicePres_value_high))/10.0;
}
