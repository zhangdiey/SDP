#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2
#define TURNING 4
#define ONWAIT 5
#define INITIAL 6

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = INITIAL;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

#define value_for_black 400
#define value_for_white 100
int red, blue, green;

unsigned long timeDetected;
unsigned long timeBlue;
unsigned long timeStationary;
unsigned long time_180;
#define value_for_blue 40
#define time_check 50
#define time_stop 5

#define turn_left 0
#define turn_right 1

bool detected = false;
int state_turn = -1;
int turn_count = 0;
int send_count = 0;

int state_corner_turn = 0;
#define turn_right_90 0
#define turn_left_90 1
#define turn_right_180 2
#define turn_left_180 3
int state_count = 0;

bool blue_spot = false;
bool turn_finished = false;
bool received = false;


void setup(){
  SDPsetup();
  delay(3000);
  helloWorld();
}

void loop(){
  switch(state){
    case GOING:
        detectSpot();
        followLine();
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
     case INITIAL:
         motorAllStop();
        if(received)
        {
          state = TURNING;
        }  
        break;        
     case ONWAIT:
         motorAllStop();
        if(!received&&send_count == 0)
        {
          Serial.write('n');
          send_count++;
        }
        else if(received)
        {
          state = TURNING;
          timeStationary = millis();
          send_count = 0;
        }
        break;
     case TURNING:
        stationary_turn();
        break;
  }
}
int obstructed(){
    if(readAnalogSensorData(1) > 170)
        return true;
   return false;
}
void halt(){
 motorAllStop();
}


void detectSpot()
{
  if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black)
  {
    blue_spot = true;
    motorAllStop();
    delay(500);
    state = ONWAIT;
  }
  else
  {
    followLine();
  }
}


void serialEvent(){
  Serial.println('r');
  switch(Serial.read()){
    case 'A':
      received = true;
      state_corner_turn = turn_left_90;
      break;
    case 'B':
      received = true;
      state_corner_turn = turn_right_180;
      break;
    case 'C':
      received = true;
     state_corner_turn = turn_right_90;
     break;
    case 'F':
      state = GOING;
      break;
    case 'U':
      received = true;
      break;
    case 'D':
      received = true;    
      break;
  }
}

void detectBlack(){
  if(readAnalogSensorData(3)>value_for_black)
  {
    state_turn = turn_right;
    timeDetected = millis();
    detected = true;
  }
  else if(readAnalogSensorData(2)>value_for_black)
  {
    state_turn = turn_left;
    timeDetected = millis();
    detected = true;
  }
}

void followLine()
{
    if(!detected)
  {
    detectBlack();
    motorBackward(5, 100);
    motorBackward(3, 100);
    motorForward(2, 100);
    motorForward(4, 100);
  }
if(millis()-timeDetected < time_check && detected)
{
  switch(state_turn){
  case turn_right:
    motorBackward(5, 25);
    motorForward(4, 25);
    motorBackward(3, 100);
    motorForward(2, 100);
    break;
  case turn_left:
    motorBackward(3, 25);
    motorForward(2, 25);
    motorBackward(5, 100);
    motorForward(4, 100);
    break;
}
}
else
detected = false;
}

void stationary_turn()
{
    if(millis() - timeStationary < 500 && ((state_corner_turn == turn_right_90) || (state_corner_turn == turn_left_90)))
    {
        motorBackward(5, 100);
        motorBackward(3, 100);
        motorForward(2, 100);
        motorForward(4, 100);
    }
    else if(millis() - timeStationary < 500 && ((state_corner_turn == turn_right_180) || (state_corner_turn == turn_left_180)))
    {
        motorForward(5, 100);
        motorForward(3, 100);
        motorBackward(2, 100);
        motorBackward(4, 100);
    }    
    else if(millis() - timeStationary == 500)
        motorAllStop();
    else if(millis() - timeStationary < 1400 && millis() - timeStationary > 500 && ((state_corner_turn == turn_right_90) || (state_corner_turn == turn_right_180)))
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 100);
         motorForward(4, 100);        
     }
    else if(millis() - timeStationary < 1400 && millis() - timeStationary > 500 && ((state_corner_turn == turn_left_90) || (state_corner_turn == turn_left_180)))
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 100);
         motorBackward(4, 100);        
     }
     else if(millis() - timeStationary == 1400)
       motorAllStop;
   else if(millis() - timeStationary > 1400 && !turn_finished)
   { 
    switch(state_corner_turn)
    {
        case turn_right_90: 
            turn_right_func();
            break;
        case turn_left_90:
            turn_left_func();
            break;
        case turn_right_180:
            turn_right_func();
            break;       
        case turn_left_180:
            turn_left_func();
            break; 
      }
    }
    else if(turn_finished)
    {
      state = ONWAIT;
      received = false;
      turn_finished = false;
    }
}

void turn_left_func()
{
    if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
     }
     else
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 75);
         motorBackward(4, 75);
      }
}

void turn_right_func()
{
    if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
     }
     else
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 75);
         motorForward(4, 75);
      }
}
