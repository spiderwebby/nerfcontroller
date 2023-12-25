#include <Arduino.h>
// #include <PID_v1.h>


//* Function declarations
int calcEnergy(int, float, int);
int calcFPS(int, int);

//* IO stuff
//in
const int main_trigger = A1;
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

//* intervals for main loop stuff
unsigned long previousMillisTriggersCheck = 0;
const int intervalTriggersCheck = 50;

unsigned long previousMillisPID = 0;
const int intervalPID = 50;

unsigned long previousMillisFire
const int intervalFire = 250; //ms between shots

unsigned long previousMillisSolenoidOnTime
const int SolenoidOnTime = 100; //ms that the solenoids are powered for per shot

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
bool firing = 0;

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
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

}


// TODO: make minimal working gun FIRST
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
    if(revTriggerFlag){
      analogWrite(top_flywheel, manualPWM);
      analogWrite(bottom_flywheel, manualPWM);
    }
    else
    {
      analogWrite(top_flywheel, 0);
      analogWrite(bottom_flywheel, 0);
    }
  }

  /*currentMillis = millis();
  if (currentMillis - previousMillisPID >= intervalPID) {
    previousMillisPID = currentMillis;
    
  }*/




  //*wiggle solenoids
  if(mainTriggerPosition==0&&revTriggerFlag) {
    currentMillis = millis();
    if(currentMillis - previousMillisFire >= intervalFire) {
      previousMillisFire = currentMillis;
      previousMillisSolenoidOnTime = currentMillis;
      firing = 1;
      if(leftBarrelFiredLast)
        digitalWrite(right_solenoid,HIGH);
      else
        digitalWrite(left_solenoid,High);
      leftBarrelFiredLast++;
    }
  }

  //*unwiggle solenoids
  if(firing){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTime >= SolenoidOnTime) {
      previousMillisSolenoidOnTime = currentMillis;
      if(!leftBarrelFiredLast) // inverted because it's already been incremented by the firing routine
        digitalWrite(right_solenoid,LOW);
      else
        digitalWrite(left_solenoid,LOW);
        firing = 0;
    }
  }

}


int calcEnergy(int dist, float time, int mass){
  return 0.5*mass*pow((dist/time),2);
}

int calcFPS(int dist, int time){
  return (dist/time)*0.00328084;
}
