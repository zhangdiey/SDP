#include "SDPArduino.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2

unsigned long timeUntilThresholdDistance = 4656;
int trigpin = 3;
int echopin = A3;
long duration;

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 100;
int state = HALTED;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

void setup(){
  SDPsetup();
  delay(3000);
  helloWorld();
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
          delay(500);
        }
        break;
      case HALTED: 
        break;
  }
}
int obstructed(){
    for (int attempt = 0;attempt<3;attempt++){
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
void followLine(){
 motorBackward(2, 100);
 motorBackward(4, 100);
 motorForward(5, 100);
 motorForward(3, 100);
 
}
void serialEvent(){
  switch(Serial.read()){
    case 'g':
      state=GOING;
      break;
    case 'h':
      state=HALTED;
      halt()
      break;
  }
}


