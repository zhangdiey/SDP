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
  timeFork = millis();
}

void loop(){
   /*if(millis() - timeFork < 11000)
  {
        motorForward(0, 65);
  }
  else if(millis() - timeFork == 11000)
  {
        motorAllStop();
  }
  else if(millis() - timeFork > 11000 && millis() - timeFork < 17000)
  {
        motorBackward(0, 65);
  }
  else
  {
        motorAllStop();
        state = TURNING;
  }*/
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red,&green,&blue);
  
  /*switch(state){
    case TURNING:
      stationary_turn();
      break;
    case GOING:
      followLine();
      detectSpot();
      break;
    case FORK:
    if(!fork_finished)
      fork();
    else
      {
        state_corner_turn = turn_left_180;
        state = TURNING;
      }
      break;
    case HALTED:
      motorStop(2);
      motorStop(3);
      motorStop(4);
      motorStop(5);
      fork_down();
      break;
  }*/
}

void detectSpot()
{
  if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black)
  {
    node_count ++;
    motorAllStop();
    delay(500);
    if(readAnalogSensorData(0) > value_for_black)
    {
      motorAllStop();
    }
    else
    {
      motorBackward(5, 100);
      motorBackward(3, 100);
      motorForward(2, 100);
      motorForward(4, 100);
    }
    timeStationary = millis();
    state = TURNING;
    /*switch(node_count)
        {
            case 1:
                state_corner_turn = turn_right_90;
                break;
            case 2:
                state_corner_turn = turn_left_90;
                break;
            case 3:
                state = GOING;
                break;
            case 4:
                state_corner_turn = turn_right_90;
                break;
            case 5:
                state = FORK;
                state_fork = UP;
                break;
            case 6:
                state_corner_turn = turn_left_90;
                break;
            case 7:
                state = GOING;
                break;
            case 8:
                state_corner_turn = turn_right_90;
                break;
            case 9:
                state = HALTED;
                break;                          
        }*/
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
    motorBackward(5, 20);
    motorForward(4, 20);
    motorBackward(3, 45);
    motorForward(2, 65);
    break;
  case turn_left:
    motorBackward(3, 20);
    motorForward(2, 20);
    motorBackward(5, 45);
    motorForward(4, 65);
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
    if(millis() - timeStationary < 300 && ((state_corner_turn == turn_right_90) || (state_corner_turn == turn_left_90)))
    {
        motorBackward(5, 45);
        motorBackward(3, 45);
        motorForward(2, 45);
        motorForward(4, 45);
    }
    else if(millis() - timeStationary < 300 && ((state_corner_turn == turn_right_180) || (state_corner_turn == turn_left_180)))
    {
        motorForward(5, 45);
        motorForward(3, 45);
        motorBackward(2, 45);
        motorBackward(4, 45);
    }   
    else if(millis() - timeStationary == 300)
        motorAllStop();
    else if(millis() - timeStationary < 1000 && millis() - timeStationary > 300 && ((state_corner_turn == turn_right_90) || (state_corner_turn == turn_right_180)))
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 55);
         motorForward(4, 55);        
     }
    else if(millis() - timeStationary < 1000 && millis() - timeStationary > 300 && ((state_corner_turn == turn_left_90) || (state_corner_turn == turn_left_180)))
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 55);
         motorBackward(4, 55);        
     }
   else if(!turn_finished && millis() -timeStationary > 1000)
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

void turn_left_func()
{/*
   if(!move_forward && (readAnalogSensorData(3)>value_for_black) && (readAnalogSensorData(2)>value_for_black))
    {
        motorBackward(5, 55);
        motorBackward(3, 55);
        motorForward(2, 55);
        motorForward(4, 55);
    }
    else if(!move_forward && (readAnalogSensorData(3)<value_for_white) && (readAnalogSensorData(2)<value_for_white))
    {
      motorAllStop();
      delay(45);
      move_forward = true;
    }
    else if(!turn_aside && move_forward && red < 70 && green < 70 && blue < 20)
    {
      motorStop(3);
      motorStop(5);
      motorForward(2, 55);
      motorBackward(4, 55);
    }
    else if(!turn_aside && move_forward &&  red > 150 && green > 150 && blue > 45)
    {
      motorAllStop();
      delay(45);
      turn_aside = true;
    }
    else */if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
        delay(145);
     }
     else
     {
         motorStop(3);
         motorStop(5);
         motorForward(2, 55);
         motorBackward(4, 55);
      }
}

void turn_right_func()
{
   /*if(!move_forward && (readAnalogSensorData(3)>value_for_black) && (readAnalogSensorData(2)>value_for_black))
    {
        motorBackward(5, 55);
        motorBackward(3, 55);
        motorForward(2, 55);
        motorForward(4, 55);
    }
    else if(!move_forward && (readAnalogSensorData(3)<value_for_white) && (readAnalogSensorData(2)<value_for_white))
    {
      motorAllStop();
      delay(45);
      move_forward = true;
    }
    else if(!turn_aside && move_forward && red < 70 && green < 70 && blue < 20)
    {
      motorStop(3);
      motorStop(5);
      motorBackward(2, 55);
      motorForward(4, 55);
    }
    else if(!turn_aside && move_forward &&  red > 150 && green > 150 && blue > 45)
    {
      motorAllStop();
      delay(45);
      turn_aside = true;
    }
    else*/ if(red < 70 && green < 70 && blue < 20)
     {
        motorAllStop();
        turn_finished = true;
        delay(145);
     }
     else
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 55);
         motorForward(4, 55);
      }
}

void fork()
{
  if(fork_finished)
  {
    fork_finished = false;
  }
  else{
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
