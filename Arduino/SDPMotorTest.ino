#include "SDPArduino.h"
#include <Wire.h>
#define GOING 2
#define OBSTRUCTED 1
#define HALTED 0
unsigned long obstructionResumeDelay = 500
unsigned long obstructionTilHalt = 10000
unsigned long timerStart;
unsigned long timeElapsed;
int state = HALTED
unsigned long timeOfObstruction;

void setup(){
  SDPsetup();
}

void loop(){
  switch(state){
    case GOING:
      if(obstructed()){
        halt();
        state=OBSTRUCTED;
        timeOfObstruction=millis();
        Serial.write('o')
      }
      else{
        followLine();
      }
      break;
    case OBSTRUCTED:
      if(obstructed()){
        if(millis()-timeOfObstruction > obstructionTilHalt){
          state=HALTED;
          Serial.write('O')
        }
      }
      else{
        Serial.write('c')
        delay(obstructionResumeDelay)
        state=GOING;
      }
      break;
    case HALTED:
      break;
  }
}
int obstructed(unsigned long period){ // Synthetic obstruction Square wave
  timeElapsed=millis()-timerStart;
  if(timeElapsed>period/2){
    return true;
  }
}
void serialEvent(){
  switch(Serial.read()){
    case 'g':
      state=GOING;
      Serial.write("debug: state=GOING")
      timerStart=millis();
      break;
    case 'h':
      state=HALTED;
      Serial.write("debug: state=HATLED")
      break;
}
