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
#define FORK 7
#define GOING_NO_OBSTRUCTION 8

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = INITIAL;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

#define value_for_black 400
#define value_for_white 100
int red, blue, green;
int red_second, blue_second, green_second;

unsigned long timeDetected;
unsigned long timeBlue;
unsigned long timeStationary;
unsigned long time_180;
#define value_for_blue 40
#define time_check 50
#define time_stop 5

#define turn_left 0
#define turn_right 1

int red_array[] = {23, 220, 20, 125, 122,19, 88, 102};
int green_array[] = {80, 185, 62, 12, 10, 74, 73, 11};
int blue_array[] = {22, 8, 74, 11, 7, 67, 9, 10};

int state_fork = 0;
#define UP 1
#define DOWN 2

bool detected = false;
int state_turn = -1;
int turn_count = 0;

int state_corner_turn = 0;
#define turn_right_90 0
#define turn_left_90 1
#define turn_right_180 2
#define turn_left_180 3
int state_count = 0;

int i = 0;

bool blue_spot = false;
bool turn_finished = false;
bool received = false;
bool fork_finished = false;
bool sent = false;
bool fork_is_up = false;

void setup()
{
  SDPsetup();
  Serial.write(9);
}

void loop(){
    read_color();
    switch(state){
    case GOING_NO_OBSTRUCTION:
        detectSpot();
        break;
    case GOING:
        detectSpot();
        if(obstructed())
        {
            state = OBSTRUCTED;
            timeOfObstruction = millis();
        }
      break;
    case OBSTRUCTED:
      motorAllStop();
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
        if(received)
        {      
            state = TURNING;
            received = false;
            timeStationary = millis();
            sent = false;
        }  
        else
        {
          motorAllStop();
        }
        break;
     case ONWAIT:
        if(!received)
        {
          if(!sent)
          {
            Serial.write('n');
            sent = true;
            motorAllStop();
          }
          else
            motorAllStop();
        }
        else if(received)
        {
          received = false;
          sent = false;
          state = TURNING;
          timeStationary = millis();

        }
        break;
     case TURNING:
        stationary_turn();
        break;
     case FORK:
        if(!fork_finished)
          fork();
        else if(fork_finished)
          {
            state = ONWAIT;
            sent = false;
            fork_finished = false;
          }
        break;
  }
}
int obstructed(){
    if(readAnalogSensorData(1) > 200)
        return true;
   return false;
}
void halt(){
 motorAllStop();
}

void read_color()
{
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red,&green,&blue);
}

void serialEvent(){
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
      state = FORK;
      state_fork = UP;
      fork_is_up = true;
      break;
    case 'D':
      state = FORK;
      state_fork = DOWN;
      fork_is_up = false;
      break;
    case 'f': 
      state = GOING_NO_OBSTRUCTION;
      break;
    case 'P':
      Serial.write('n');
      break;
  }
}
 
bool first_black = false;
unsigned long timeFirstBlack;

void detectSpot()
{
  if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black && !first_black)
  {
    motorAllStop();
    first_black = true;
    timeFirstBlack = millis();
  }
  else if(readAnalogSensorData(0) > value_for_black && first_black)
    {
      state = ONWAIT;
      sent = false;
      first_black = false;
    }
  else if(first_black && readAnalogSensorData(0) < value_for_white)
    {
      if(millis() - timeFirstBlack < 500)
        {
          motorAllStop();
        }
       else if(millis() - timeFirstBlack > 500)
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
    if(readAnalogSensorData(3)>value_for_black) /*((!(red < 70 && green < 70 && blue < 20)) && readAnalogSensorData(0) < value_for_white))*/
    {
        state_turn = turn_right;
        timeDetected = millis();
        detected = true;
    }
    else if(readAnalogSensorData(2)>value_for_black) /*|| (readAnalogSensorData(0) > value_for_black))*/
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
    motorBackward(5, 35);
    motorBackward(3, 35);
    motorForward(2, 35);
    motorForward(4, 35);
  }
if(millis()-timeDetected < time_check && detected)
{
  switch(state_turn){
  case turn_right:
    motorBackward(5, 10);
    motorForward(4, 10);
    motorBackward(3, 60);
    motorForward(2, 85);
    break;
  case turn_left:
    motorBackward(3, 10);
    motorForward(2, 10);
    motorBackward(5, 60);
    motorForward(4, 85);
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
    motorBackward(5, 10);
    motorForward(4, 10);
    motorBackward(3, 75);
    motorForward(2, 100);
    break;
  case turn_left:
    motorBackward(3, 10);
    motorForward(2, 10);
    motorBackward(5, 75);
    motorForward(4, 100);
    break;
}
}
else
detected = false;
}

void stationary_turn()
{
    if(state_corner_turn == turn_left_90 || state_corner_turn == turn_right_90)
        stationary_turn_90();
    else if(state_corner_turn == turn_left_180 || state_corner_turn == turn_right_180)
        stationary_turn_180();
}

void stationary_turn_90()
{
    if(millis() - timeStationary < 700 && ((state_corner_turn == turn_right_90)))
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 70 + fork_is_up * 17);
         motorForward(4, 70 + fork_is_up * 17);
     }
    else if(millis() - timeStationary < 700 && ((state_corner_turn == turn_left_90)))
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
      state = ONWAIT;
    }
}

void stationary_turn_180()
{
    if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black)
    {
        motorAllStop();
        timeStationary = millis();
        state_corner_turn = turn_right_90;
    }
    else
    {
        motorBackward(2, 45);
        motorBackward(4, 45);
        motorForward(5, 45);
        motorForward(3, 45);
    }
}

bool timed_turn = false;

void turn_left_func()
{
    if(readAnalogSensorData(3) > value_for_black && !timed_turn)
    {
        motorAllStop();
        delay(150);
        timed_turn = true;
    }
    else if(readAnalogSensorData(3) < value_for_white && !timed_turn)
    {
        motorStop(3);
        motorStop(5);
        motorBackward(4, 70 + fork_is_up * 17);
        motorForward(2, 70 + fork_is_up * 17);
    }
    else if((!(readAnalogSensorData(3) < value_for_white && readAnalogSensorData(2) < value_for_white) && timed_turn))
    {
        motorStop(3);
        motorStop(5);
        motorBackward(4, 70 + fork_is_up * 17);
        motorForward(2, 70 + fork_is_up * 17);
    }
    else if(readAnalogSensorData(3) < value_for_white && readAnalogSensorData(2) < value_for_white && timed_turn)
    {
        motorAllStop();
        turn_finished = true;
        timed_turn = false;
    }
}

void turn_right_func()
{
    if(readAnalogSensorData(2) > value_for_black && !timed_turn)
    {
        motorAllStop();
        delay(150);
        timed_turn = true;
    }
    else if(readAnalogSensorData(2) < value_for_white && !timed_turn)
    {
        motorStop(3);
        motorStop(5);
        motorBackward(2, 70 + fork_is_up * 17);
        motorForward(4, 70 + fork_is_up * 17);
    }
    else if((!(readAnalogSensorData(3) < value_for_white && readAnalogSensorData(2) < value_for_white) && timed_turn))
    {
        motorStop(3);
        motorStop(5);
        motorBackward(2, 70 + fork_is_up * 17);
        motorForward(4, 70 + fork_is_up * 17);
    }
    else if(readAnalogSensorData(3) < value_for_white && readAnalogSensorData(2) < value_for_white && timed_turn)
    {
        motorAllStop();
        turn_finished = true;
        timed_turn = false;
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
