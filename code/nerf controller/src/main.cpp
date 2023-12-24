#include <Arduino.h>

// put function declarations here:
int calcEnergy(int, float, int);
int calcFPS(int, int);


const int main_trigger = A1;
const int rev_trigger = 4;
const int top_flywheel_gate = 5;
const int bottom_flywheel_gate = 6;
const int chrono_gate_a = 7;
const int chrono_gate_b = 8;


const int top_flywheel = 9;
const int bottom_flywheel = 10;
const int left_solenoid = 11;
const int right_solenoid = 12;

const int LED = 13;



const float dartMass = 0.001; // 1g

unsigned long previousMillisTriggersCheck = 0;
const long intervalTriggersCheck = 50;

const float flywheel_circumference = 72.2566310326;
unsigned int top_flywheel_gate_millis = 0;
unsigned int bottom_flywheel_gate_millis = 0;
int topFlywheelMillisPerR = 0;
int bottomFlywheelMillisPerR = 0;


const int dist_chrono = 100; //mm
unsigned long chrono = 0;
unsigned long chronoDiff = 0;

int revTriggerFlag = 0;
int mainTriggerPosition = 0;
int fpsMeasured = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT); //d2 + d3 are i2c lines
  pinMode(3, INPUT);
  
  //digital IO
  pinMode(rev_trigger, INPUT_PULLUP); //4
  pinMode(top_flywheel_gate, INPUT_PULLUP); //5
  pinMode(bottom_flywheel_gate, INPUT_PULLUP); //6
  pinMode(chrono_gate_a, INPUT_PULLUP); //7
  pinMode(chrono_gate_b, INPUT_PULLUP); //8
  
  pinMode(top_flywheel, OUTPUT); //9
  pinMode(bottom_flywheel, OUTPUT);  //10
  pinMode(left_solenoid, OUTPUT);  //11
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

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();

  
  if(!digitalRead(chrono_gate_a)) chrono = currentMillis;
  if(!digitalRead(chrono_gate_b)) chronoDiff = chrono - currentMillis;
  if(!digitalRead(top_flywheel_gate)) topFlywheelMillisPerR = currentMillis - top_flywheel_gate_millis;
  if(!digitalRead(bottom_flywheel_gate)) bottomFlywheelMillisPerR = currentMillis - bottom_flywheel_gate_millis;

  if (currentMillis - previousMillisTriggersCheck >= intervalTriggersCheck) {
    previousMillisTriggersCheck = currentMillis;
    revTriggerFlag = digitalRead(rev_trigger);
    mainTriggerPosition = analogRead(main_trigger);
  }

 

}


int calcEnergy(int dist, float time, int mass){
  return 0.5*mass*pow((dist/time),2);
}

int calcFPS(int dist, int time){
  return (dist/time)*0.00328084;
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}