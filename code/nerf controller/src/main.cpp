#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

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

  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  flyweels(!digitalRead(rev_trigger)); //if revtrigger is low (pulled) spin da wheels, otherwise donot.
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}