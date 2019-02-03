#include "SDPArduino.h"
#include <Wire.h>
#define GOING 2
#define HALTED 0
#define OBSTRUCTED 1
int state = HALTED;

void setup(){
  SDPsetup();
}

void loop(){
  switch (state) {
    case GOING:
      if(obstructed()){
        halt()
        Serial.write('o');
        state = OBSTRUCTED;
      }
      else {
        followLine();
      }
      break;
    case OBSTRUCTED:
      if(obstructed()){
        break;
      }
      else{
        Serial.write('c')
        state = GOING;
      }
    case HALTED:
      break;
  }
}

void serialEvent(){
  switch (Serial.read()) {
    case 'g':
      state = GOING;
      break;
    case 'h':
      halt();
      state = HALTED;
}
  
