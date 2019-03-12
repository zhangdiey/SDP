#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2
#define TURNING 4

#define value_for_black 600
#define value_for_white 100
#define value_for_red
#define value_for_green
#define value_for_blue

#define value_for_blue_upper
#define value_for_blue_lower

#define left 'a'
#define right 'c'
#define forward 'f'
#define backward 'b' //it is actually for turn 180 degrees

//the right greyscale sensor number 2
//the left greyscale sensor number 3

unsigned long timeUntilThresholdDistance = 3000;
int trigpin = 3;
int echopin = A3;
long duration;

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = HALTED;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

int red, green, blue;
  GroveColorSensor colorSensor;
char turning_state = forward;

void setup(){
  SDPsetup();
  delay(3000);
  helloWorld();
  colorSensor.ledStatus = 1;
}



void loop(){
  switch(state){
    case GOING:
      if(obstructed()){
        halt();
        state=OBSTRUCTED;
        timeOfObstruction=millis();
        Serial.write('o');
      }
      else{
        detectSpot();
        followLine();
      }
      break;
    case OBSTRUCTED:
      if(obstructed()){
        if(millis()-timeOfObstruction > obstructionTilHalt){
          state=HALTED;
          Serial.write('O');
        }
      }
      else{
        timeOfClearing = millis();
        state=OBSTRUCTED_CLEARING;
      }
      break;
    case OBSTRUCTED_CLEARING:
        if(obstructed()){
          state=OBSTRUCTED;
        }
        else if(millis()-timeOfClearing>timeTilClear ){
          state=GOING;
          Serial.write('c');
        }
        break;
     case HALTED: 
        break;
     case TURNING:
  }
}
int obstructed(){
    for (int attempt = 0;attempt<1;attempt++){
      digitalWrite(trigpin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigpin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigpin, LOW);
      if (pulseIn(echopin, HIGH, timeUntilThresholdDistance)){
        return true;
      }
    }
   return false;
}
void halt(){
 motorAllStop();
}
void detectSpot(){
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red, &green, &blue);
}

void serialEvent(){
  switch(Serial.read()){
    case 'g':
      state=GOING;
      Serial.println("state = going");
      break;
    case 'h':
      halt();
      state=HALTED;
      Serial.println("state = halted");
      break;
    case 'a':
      turning_state = 'a';
      break;
    case 'b':
      turning_state = 'b':
      break;
    case 'c':
      turning_state = 'c';
      break;
    case 'f':
      turning_state = 'f';
      break;            
  }
}
