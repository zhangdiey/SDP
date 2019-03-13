#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2
#define TURNING 4

#define left 'a'
#define right 'c'
#define forward 'f'
#define backward 'b' //it is actually for turn 180 degrees

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = HALTED;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

#define value_for_black 400
#define value_for_white 100
int i = 0;
int red, blue, green;

unsigned long timeDetected;
unsigned long timeBlue;
unsigned long timeStationary;
#define value_for_blue 40
#define time_check 50
#define time_stop 5

#define turn_left 0
#define turn_right 1
#define go_forward 2
bool detected = false;
int state_turn = -1;
int turn_count = 0;

int state_corner_turn = -1;
#define turn_right_90 0
#define turn_left_90 1
#define turn_right_180 2
#define turn_left_180 3
int state_count = 0;

bool blue_spot = false;
bool turn_finished = false;


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
void detectSpot(){
    GroveColorSensor colorSensor;
    colorSensor.ledStatus = 1;
    colorSensor.readRGB(&red,&green,&blue);
  if(blue > value_for_blue )
  {
    blue_spot = true;
  }
  if(!blue_spot)
  {
    followLine();
  }
  else
  {
     motorAllStop();
     delay(50);
     state = TURNING;
  }
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
      state_corner_turn = turn_left_90;
      break;
    case 'b':

      break;
    case 'c':
     state_corner_turn = turn_right_90
     break;
    case 'f':
      turning_state = 'f';
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
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(2)<value_for_white)
  {
    state_turn = go_forward;
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
  case go_forward:
    break;
}
}
else
detected = false;
}

void stationary_turn()
{
    if(millis() - timeStationary < 500)
    {
        motorBackward(5, 100);
        motorBackward(3, 100);
        motorForward(2, 100);
        motorForward(4, 100);
    }
    switch(state_corner_turn)
    {
        case turn_right_90:
            turn_right_90_func();
            if(turn_finished)
            {
                turn_finished = false;
                break;
            }
        case turn_left_90:
            turn_left_90_func();
            if(turn_finished)
            {
                turn_finished = false;
                break;
            }
        case turn_right_180:
            if(i == 0)
                turn_right_90_func();
            else if(i == 1 && !turn_finished)
                turn_right_90_func();
             else if(i == 1 && turn_finished)
             {
                turn_finished = false;
                i = 0;
                break;
             }
        case turn_left_180:
            if(i == 0)
                turn_right_90_func();
            else if(i == 1 && !turn_finished)
                turn_right_90_func();
             else if(i == 1 && turn_finished)
             {
                turn_finished = false;
                i = 0;
                break;
             }
    }
}

void turn_right_90_func()
{
    if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
     }
     else
     {
         motorForward(2, 100);
         motorBackward(4, 100);
      }
}

void turn_left_90_func()
{
    if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
     }
     else
     {
         motorBackward(2, 100);
         motorForward(4, 100);
      }
}
