#include <Arduino.h>
// #include <PID_v1.h>


//* IO stuff
//in
const int main_trigger = 8;
const int speed_control = A0;

//out
const int solenoid1 = 9;
const int solenoid2 = 10;
const int solenoid3 = 11;
const int solenoid4 = 12;
const int LED = 13;


//* intervals for main loop stuff
unsigned long previousMillisTriggersCheck = 0;
const int intervalTriggersCheck = 50;

unsigned long previousMillisFire;
const int intervalFireDefault = 150; //ms between shots
int intervalFire = 250; 

unsigned long previousMillisSolenoidOnTime1;
unsigned long previousMillisSolenoidOnTime2;
unsigned long previousMillisSolenoidOnTime3;
unsigned long previousMillisSolenoidOnTime4;
const int SolenoidOnTime = 100; //ms that the solenoids are powered for per shot !!!must be less than 2*intervalFire!!!

unsigned long previousMillisRev = 0;
const int intervalRev = 50;

//* flags and IO readings
bool triggerFlag = 0;
int BarrelFiredLast = 0;
bool firing1 = 0;
bool firing2 = 0;
bool firing3 = 0;
bool firing4 = 0;

//* flags just used for debugging
bool Solenoid1On = 0;
bool Solenoid2On = 0;
bool Solenoid3On = 0;
bool Solenoid4On = 0;
bool triggerOutOfSequence = 0;

void setup() {
  //digital IO
  pinMode(main_trigger, INPUT_PULLUP); //4
  

  pinMode(solenoid1, OUTPUT);  //9
  pinMode(solenoid2, OUTPUT);  //10
  pinMode(solenoid3, OUTPUT);  //11 pwm
  pinMode(solenoid4, OUTPUT);  //12

  digitalWrite(solenoid1, LOW);  //mosfets off to start
  digitalWrite(solenoid2, LOW);
  digitalWrite(solenoid3, LOW);
  digitalWrite(solenoid4, LOW);
  
  //analogue IO
  pinMode(speed_control, INPUT_PULLUP); //A0

  Serial.begin(38400);
  Serial.println("beep!!!");
}

void loop() {
  
  //*read triggers
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisTriggersCheck >= intervalTriggersCheck) {
    previousMillisTriggersCheck = currentMillis;
    triggerFlag = !digitalRead(main_trigger);
    intervalFire = intervalFireDefault + map(analogRead(speed_control), 0, 1023, 0, 500);

    Serial.print(">fire interval:");
    Serial.println(intervalFire);
    Serial.print(">solenoid1:");
    Serial.println(Solenoid1On);
    Serial.print(">solenoid2:");
    Serial.println(Solenoid2On);
    Serial.print(">solenoid3:");
    Serial.println(Solenoid3On);
    Serial.print(">solenoid4:");
    Serial.println(Solenoid4On);
  }


  //*wiggle solenoids
  if(triggerFlag) {
    currentMillis = millis();
    if(currentMillis - previousMillisFire >= intervalFire) {
      previousMillisFire = currentMillis;
      

      switch (BarrelFiredLast) {
        case 1:
          previousMillisSolenoidOnTime1 = currentMillis;
          digitalWrite(solenoid1,HIGH);
          Solenoid1On = 1;
          firing1 = 1;
          break;
        case 2:
          previousMillisSolenoidOnTime2 = currentMillis;
          digitalWrite(solenoid1,HIGH);
          Solenoid2On = 1;
          firing2 = 1;
          break;
        case 3:
          previousMillisSolenoidOnTime3 = currentMillis;
          digitalWrite(solenoid3,HIGH);
          Solenoid3On = 1;
          firing3 = 1;
          break;
        case 4:
          previousMillisSolenoidOnTime4 = currentMillis;
          digitalWrite(solenoid4,HIGH);
          Solenoid4On = 1;
          firing4 = 1;
          break;
      }
      BarrelFiredLast++;
      if(BarrelFiredLast>4) BarrelFiredLast = 1;
    }
  }

  //*unwiggle solenoids
  if(firing1){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTime1 >= SolenoidOnTime) {
      previousMillisSolenoidOnTime1 = currentMillis;
        digitalWrite(solenoid1,LOW);
        Solenoid1On = 0;
      firing1 = 0;
    }
  }
  if(firing2){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTime2 >= SolenoidOnTime) {
      previousMillisSolenoidOnTime2 = currentMillis;
        digitalWrite(solenoid2,LOW);
        Solenoid2On = 0;
      firing2 = 0;
    }
  }  if(firing3){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTime3 >= SolenoidOnTime) {
      previousMillisSolenoidOnTime3 = currentMillis;
        digitalWrite(solenoid3,LOW);
        Solenoid3On = 0;
      firing3 = 0;
    }
  }  if(firing4){
    currentMillis = millis();
    if (currentMillis - previousMillisSolenoidOnTime4 >= SolenoidOnTime) {
      previousMillisSolenoidOnTime4 = currentMillis;
        digitalWrite(solenoid4,LOW);
        Solenoid4On = 0;
      firing4 = 0;
    }
  }
}
