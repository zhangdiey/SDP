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
#define value_for_blue 40
#define value_for_red 40
#define time_check 50
#define time_stop 5

#define turn_left 0
#define turn_right 1
#define go_forward 2
bool detected = false;
int state_turn = -1;
int turn_count = 0;

int state_corner_turn = 1;
#define turn_right_90 0
#define turn_left_90 1
#define turn_right_180 2
#define turn_left_180 3
int state_count = 0;

int state = 2;

#define TURNING 1
#define GOING 2

bool blue_spot = false;
bool turn_finished = false;

void setup()
{
  SDPsetup();
  helloWorld();
}

void loop(){
  Serial.print("state:");
  Serial.print(state);
  Serial.print("  red:");
  Serial.print(red);
  Serial.print("  blue:");
  Serial.print(blue);
  Serial.print("  green:");
  Serial.println(green);
      GroveColorSensor colorSensor;
    colorSensor.ledStatus = 1;
    colorSensor.readRGB(&red,&green,&blue);
  //delay(150);
  switch(state){
    case TURNING:
      stationary_turn();
      break;
    case GOING:
      followLine();
      detectSpot();
      break;
  }
}

void detectSpot()
{
  if(readAnalogSensorData(3)>value_for_black && readAnalogSensorData(2)>value_for_black)
  {
    blue_spot = true;
    motorAllStop();
    delay(500);
    timeStationary = millis();
    state = TURNING;
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
    motorBackward(5, 75);
    motorBackward(3, 75);
    motorForward(2, 75);
    motorForward(4, 75);
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
        motorBackward(5, 75);
        motorBackward(3, 75);
        motorForward(2, 75);
        motorForward(4, 75);     
    }
    else if(millis() - timeStationary == 500)
        motorAllStop();
    else if(millis() - timeStationary < 1000 && millis() - timeStationary > 500)
     {
         motorStop(3);
         motorStop(5);
         motorBackward(2, 100);
         motorForward(4, 100);        
     }
    else if(millis() - timeStationary > 1000)
    {
    switch(state_corner_turn)
    {
        case turn_right_90:
            turn_right_90_func();
            if(turn_finished)
            {
                turn_finished = false;
                break;
            }
            break;
        case turn_left_90:
            turn_left_90_func();
            if(turn_finished)
            {
                turn_finished = false;
                break;
            }
            break;
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
             break;
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
         motorStop(3);
         motorStop(5);
         motorForward(2, 75);
         motorBackward(4, 75);
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
         motorStop(3);
         motorStop(5);
         motorBackward(2, 75);
         motorForward(4, 75);
      }
}
