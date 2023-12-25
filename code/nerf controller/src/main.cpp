#include <Arduino.h>
// #include <PID_v1.h>


//* Function declarations
int calcEnergy(int, float, int);
int calcFPS(int, int);

//* IO stuff
//in
const int main_trigger = A0;
const int rev_trigger = 4;
const int top_flywheel_gate = 5;
const int bottom_flywheel_gate = 6;
const int chrono_gate_a = 7;
const int chrono_gate_b = 8;
//out
const int top_flywheel = 9;
const int bottom_flywheel = 10;
const int left_solenoid = 11;
const int right_solenoid = 12;
const int LED = 13;

//*settings for dart energy calc / velocity etc
const float dartMass = 0.001; // 1g
const float flywheel_circumference = 72.2566310326;
const int dist_chrono = 100; //mm
const int manualPWM = 20; //manual setting for PWM flywheel control

//*misc settings
const int main_trigger_threashold = 20;

//* intervals for main loop stuff
unsigned long previousMillisTriggersCheck = 0;
const int intervalTriggersCheck = 50;

unsigned long previousMillisPID = 0;
const int intervalPID = 50;

unsigned long previousMillisFire;
const int intervalFire = 250; //ms between shots

unsigned long previousMillisSolenoidOnTimeL;
unsigned long previousMillisSolenoidOnTimeR;
const int SolenoidOnTime = 300; //ms that the solenoids are powered for per shot !!!must be less than 2*intervalFire!!!

unsigned long previousMillisRev = 0;
const int intervalRev = 50;

//* variables for flywheel / chronometer calcs
unsigned int top_flywheel_gate_millis = 0;
unsigned int bottom_flywheel_gate_millis = 0;
int topFlywheelMillisPerR = 0;
int bottomFlywheelMillisPerR = 0;
unsigned long chrono = 0;
unsigned long chronoDiff = 0;
int fpsMeasured = 0;

//* flags and IO readings
bool revTriggerFlag = 0;
int mainTriggerPosition = 0;
bool leftBarrelFiredLast = 0;
bool firingL = 0;
bool firingR = 0;

//* flags just used for debugging
bool leftSolenoidOn = 0;
bool rightSolenoidOn = 0;
bool triggerOutOfSequence = 0;

//* PID stuff
/*double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
*/

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT); //d2 + d3 are i2c lines
  pinMode(3, INPUT);
  
  //digital IO
  pinMode(rev_trigger, INPUT_PULLUP); //4
  pinMode(top_flywheel_gate, INPUT_PULLUP); //5
  pinMode(bottom_flywheel_gate, INPUT_PULLUP); //6 (pwm)
  pinMode(chrono_gate_a, INPUT_PULLUP); //7
  pinMode(chrono_gate_b, INPUT_PULLUP); //8
  
  pinMode(top_flywheel, OUTPUT); //9 pwm
  pinMode(bottom_flywheel, OUTPUT);  //10 pwm
  pinMode(left_solenoid, OUTPUT);  //11 pwm
  pinMode(right_solenoid, OUTPUT);  //12

  digitalWrite(main_trigger, LOW);  //mosfets off to start
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  
  //analogue IO
  pinMode(main_trigger, INPUT_PULLUP); //A0
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  Serial.begin(38400);
  Serial.println("beep!!!");

}


// TODO: if main trigger is pulled before rev trigger: do nothing
// todo: 


void loop() {
  
  //*read lightgates
  //todo: these need some stuff to account for being low for multiple loops before the dart/slot passes
  // if(!digitalRead(chrono_gate_a)) chrono = currentMillis;
  // if(!digitalRead(chrono_gate_b)) chronoDiff = chrono - currentMillis;
  // if(!digitalRead(top_flywheel_gate)) topFlywheelMillisPerR = currentMillis - top_flywheel_gate_millis;
  // if(!digitalRead(bottom_flywheel_gate)) bottomFlywheelMillisPerR = currentMillis - bottom_flywheel_gate_millis;

  //*read triggers, spinn wheels
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisTriggersCheck >= intervalTriggersCheck) {
    previousMillisTriggersCheck = currentMillis;
    revTriggerFlag = !digitalRead(rev_trigger);
    mainTriggerPosition = analogRead(main_trigger);
    if(mainTriggerPosition>main_trigger_threashold && revTriggerFlag == 0){  //neither trigger has been pulled
      triggerOutOfSequence = 0;
    }else if (mainTriggerPosition>main_trigger_threashold && revTriggerFlag == 1){ //just rev trigger
      triggerOutOfSequence = 0;
    }else if (mainTriggerPosition<main_trigger_threashold && revTriggerFlag == 0){ //just main trigger
      triggerOutOfSequence = 1;
    }

  if(triggerOutOfSequence) revTriggerFlag = 0; //no shooty if you did it wrong!!

    Serial.print(">maintriggerpos:");
    Serial.println(mainTriggerPosition);
    if(revTriggerFlag){
      analogWrite(top_flywheel, manualPWM);
      analogWrite(bottom_flywheel, manualPWM);
      Serial.println(">spinn!:1");
    }
    else
    {
      analogWrite(top_flywheel, 0);
      analogWrite(bottom_flywheel, 0);
      Serial.println(">spinn!:0");
    }
    Serial.print(">left solenoid:");
    Serial.println(leftSolenoidOn);
    Serial.print(">right solenoid:");
    Serial.println(rightSolenoidOn);

    Serial.print(">trigger out of sequence:");
    Serial.println(triggerOutOfSequence);
  }

  /*currentMillis = millis();
  if (currentMillis - previousMillisPID >= intervalPID) {
    previousMillisPID = currentMillis;
    
  }*/




  //*wiggle solenoids
  if(mainTriggerPosition < main_trigger_threashold && revTriggerFlag) {
    currentMillis = millis();
    if(currentMillis - previousMillisFire >= intervalFire) {
      previousMillisFire = currentMillis;
      
      if(leftBarrelFiredLast){
        previousMillisSolenoidOnTimeR = currentMillis;
        digitalWrite(right_solenoid,HIGH);
        //Serial.println(">right solenoid:1");
        rightSolenoidOn = 1;
        firingR = 1;
      }else{
        previousMillisSolenoidOnTimeL = currentMillis;
        digitalWrite(left_solenoid,HIGH);
        //Serial.println(">left solenoid:1");
        leftSolenoidOn = 1;
        firingL = 1;
      }
      leftBarrelFiredLast = !leftBarrelFiredLast;
    }
  }

  //*unwiggle solenoids
  if(firingR){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTimeR >= SolenoidOnTime) {
      previousMillisSolenoidOnTimeR = currentMillis;
        digitalWrite(right_solenoid,LOW);
        //Serial.println(">right solenoid:0");
        rightSolenoidOn = 0;
      firingR = 0;
    }
  }

  if(firingL){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTimeL >= SolenoidOnTime) {
      previousMillisSolenoidOnTimeL = currentMillis;
        digitalWrite(left_solenoid,LOW);
        //Serial.println(">right solenoid:0");
        leftSolenoidOn = 0;
      firingL = 0;
    }
  }

}


int calcEnergy(int dist, float time, int mass){
  return 0.5*mass*pow((dist/time),2);
}

int calcFPS(int dist, int time){
  return (dist/time)*0.00328084;
}
