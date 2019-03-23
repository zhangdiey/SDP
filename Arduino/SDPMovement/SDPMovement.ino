#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define value_for_black 400
#define value_for_white 100
int i = 0;
int red, blue, green;

unsigned long timeDetected;
unsigned long timeBlue;
unsigned long timeStationary;
unsigned long timeFork;
#define value_for_blue 40
#define value_for_red 40
#define time_check 5
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

int node_count = 0;

int state = 2;

#define TURNING 1
#define GOING 2
#define FORK 3
#define HALTED 4

int fork_is_up = 1;

int state_fork = 0;
#define UP 1
#define DOWN 2

bool blue_spot = false;
bool turn_finished = false;

bool fork_finished = false;

void setup()
{
  SDPsetup();
  helloWorld();
}

void loop(){
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red,&green,&blue);
  switch(state){
    case TURNING:
      stationary_turn();
      break;
    case GOING:
      detectSpot();
      break;
    case FORK:
    if(!fork_finished)
      fork();
    else
      {
        state = TURNING;
        state_corner_turn = turn_right_180;
      }
      break;
    case HALTED:
      motorStop(2);
      motorStop(3);
      motorStop(4);
      motorStop(5);
      state = FORK;
      state_fork = UP;     
      break;
  }
}

bool first_black = false;
unsigned long timeFirstBlack;



void detectSpot()
{
  if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black)
  {
    motorAllStop();
    first_black = true;
    timeFirstBlack = millis();
  }
  else if(readAnalogSensorData(0) > value_for_black && first_black)
    {
      motorAllStop();
      delay(300);
      first_black = false;
      timeStationary = millis();
      switch(node_count)
      {
        case 0:
          state = GOING;
          break;
        case 1:
          state = TURNING;
          state_corner_turn = turn_right_90;
          break;
        case 2:
            motorStop(2);
            motorStop(3);
            motorStop(4);
            motorStop(5);
            state = FORK;
            state_fork = UP;
            break;
          break;
        case 3:
          state = GOING;
          break;
        case 4:
          motorStop(2);
          motorStop(3);
          motorStop(4);
          motorStop(5);
          state = FORK;
          state_fork = DOWN;
          break;
      }      
      node_count++;
  }
    else if(first_black && readAnalogSensorData(0) < value_for_white)
    {
      if(millis() - timeFirstBlack < 700 + fork_is_up * 100)
        {
          motorAllStop();
        }
       else if(millis() - timeFirstBlack > 700 + fork_is_up * 100)
       {
          followLine_slow();
       }
    }
  else
  {
    followLine();
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


void followLine_slow()
{
    if(!detected)
  {
    detectBlack();
    motorBackward(5, 45);
    motorBackward(3, 45);
    motorForward(2, 45);
    motorForward(4, 45);
  }
if(millis()-timeDetected < time_check && detected)
{
  switch(state_turn){
  case turn_right:
    motorBackward(5, 5 + 20);
    motorForward(4, 5 + 20);
    motorBackward(3, 40 + 20);
    motorForward(2, 65 + 20);
    break;
  case turn_left:
    motorBackward(3, 5 + 20);
    motorForward(2, 5 + 20);
    motorBackward(5, 40 + 20);
    motorForward(4, 65 + 20);
    break;
}
}
else
detected = false;
}

void followLine()
{
    if(!detected)
  {
    detectBlack();
    motorBackward(5, 45);
    motorBackward(3, 45);
    motorForward(2, 45);
    motorForward(4, 45);
  }
if(millis()-timeDetected < time_check && detected)
{
  switch(state_turn){
  case turn_right:
    motorBackward(5, 10 + 25);
    motorForward(4, 10 + 25);
    motorBackward(3, 50 + 25);
    motorForward(2, 75 + 25);
    break;
  case turn_left:
    motorBackward(3, 10 + 25);
    motorForward(2, 10 + 25);
    motorBackward(5, 50 + 25);
    motorForward(4, 75 + 25);
    break;
}
}
else
detected = false;
}

bool move_forward = false;
bool turn_aside = false;

void stationary_turn()
{
    if(millis() - timeStationary < 700 && ((state_corner_turn == turn_right_90) || (state_corner_turn == turn_right_180)))
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 70 + fork_is_up * 17);
         motorForward(4, 70 + fork_is_up * 17);        
     }
    else if(millis() - timeStationary < 700 && ((state_corner_turn == turn_left_90) || (state_corner_turn == turn_left_180)))
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 70 + fork_is_up * 17);
         motorBackward(4, 70 + fork_is_up * 17);        
     }
   else if(!turn_finished && millis() -timeStationary > 700)
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
      motorAllStop();
      turn_finished = false;
      state = GOING;
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
         motorBackward(2, 70 + fork_is_up * 17);
         motorForward(4, 70 + fork_is_up * 17);
      }
}

void turn_left_func()
{
  if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
        delay(200 + fork_is_up * 100);
     }
     else
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 70 + fork_is_up * 17);
         motorBackward(4, 70 + fork_is_up * 17);
      }
}

void fork()
{

    switch(state_fork)
  {
    case UP:
      fork_up();
      break;
    case DOWN:
      fork_down();
      break;
  }
 
}

void fork_up()
{
  digitalWrite(6, HIGH);
  if(!digitalRead(6) && !fork_finished)
  {
    fork_finished = true;
    motorStop(0);
  }
  else if(digitalRead(6) && !fork_finished)
    motorForward(0, 75);
}

void fork_down()
{
  digitalWrite(9, HIGH);
  if(!digitalRead(9) && !fork_finished)
  {
    fork_finished = true;
    motorStop(0);
  }
  else if(digitalRead(9) && !fork_finished)
    motorBackward(0, 75);
}
