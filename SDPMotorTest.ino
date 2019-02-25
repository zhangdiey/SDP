#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2
#define TURNING 4
//blue rgb 50 red

#define value_for_black 600
#define value_for_white 100
#define value_for_red 20
#define value_for_green 50
#define value_for_blue 140

#define value_for_blue_upper 300
#define value_for_blue_lower 200

//it is actually for turn 180 degrees

//the right greyscale sensor number 2
//the left greyscale sensor number 3

/* 
 * 2 is top right whee
 * 3 is bottom right
 * 5 is bottom left
 * and 4 is top left
 */

unsigned long timeUntilThresholdDistance = 3000;
int trigpin = 3;
int echopin = A3;
long duration;

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = GOING;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

int red, green, blue;
GroveColorSensor colorSensor;
char turning_state = forward;

void setup(){
  SDPsetup();
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
       turn();       
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
  
void followLine(){
  if(readAnalogSensorData(3)>value_for_black)
  {
    motorBackward(2, 50);
    motorForward(3, 50);
  }
  else if(readAnalogSensorData(2)>value_for_black)
  {
     motorBackward(4, 50);
     motorForward(5, 50);
  }
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(2)<value_for_white)
  {
    motorBackward(2, 100);
    motorBackward(4, 100);
    motorForward(5, 100);
    motorForward(3, 100);
  }
 
}

void detectStop()
{
  colorSensor.readRGB(&red, &green, &blue);
  if(red < value_for_red && green < value_for_green && blue > value_for_blue)
  {
    Serial.write('n');
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
      turning_state = 'a';
      break;
    case 'b':
      turning_state = 'b';
      break;
    case 'c':
      turning_state = 'c';
      break;
    case 'f':
      turning_state = 'f';
      break;            
  }
}

void turn()
{
   switch(turning_state)
   {
     case 'a':
       turn_left();
       break;
     case 'b':
       backward();
       break;
     case 'c':
       turn_right();
       break;
     case 'f':
       forward();
       break;
   }
}

void turn_right()
{
  if(readAnalogSensorData(3)>value_for_black)
{
  if(readAnalogSensorData(2)>value_for_black)
  {
     motorBackward(4, 50);
     motorForward(5, 50);
  }  
}  
  else if(readAnalogSensorData(3)<value_for_white)
    state = GOING;
}

void turn_left()
{
  if(readAnalogSensorData(2)>value_for_black)
  {
    if(readAnalogSensorData(3)>value_for_black)
  {
    motorBackward(2, 50);
    motorForward(3, 50);
  }
  }
  else if(readAnalogSensorData(2)<value_for_white)
    state = GOING;  
}

void turn_right_reverse()
{
    if(readAnalogSensorData(2)>value_for_black)
  {
    if(readAnalogSensorData(3)>value_for_black)
  {
    motorForward(2, 50);
    motorBackward(3, 50);
  }
  }
  else if(readAnalogSensorData(2) <value_for_white)
    state = GOING;  
}

void forward()
{
  if(readAnalogSensorData(3)>value_for_blue_lower && readAnalogSensorData(3)<value_for_blue_upper)
  {
    motorBackward(2, 50);
    motorForward(3, 50);
  }
  else if(readAnalogSensorData(2)>value_for_blue_lower && readAnalogSensorData(2)<value_for_blue_upper)
  {
     motorBackward(4, 50);
     motorForward(5, 50);
  }
  else if(readAnalogSensorData(2)<value_for_blue_lower || readAnalogSensorData(2)>value_for_blue_upper||readAnalogSensorData(3)<value_for_blue_lower || readAnalogSensorData(3)>value_for_blue_upper)
  {
    motorBackward(2, 100);
    motorBackward(4, 100);
    motorForward(5, 100);
    motorForward(3, 100);
  }
  else if(readAnalogSensorData(2)<value_for_white && readAnalogSensorData(3)>value_for_white)
  {
     motorBackward(4, 50);
     motorForward(5, 50);
  }
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(2)>value_for_white)
  {
     motorBackward(2, 50);
     motorForward(3, 50);
  }
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(2)<value_for_white)
  {
    state = GOING;
  }  
}

void backward()
{
  if(readAnalogSensorData(3)>value_for_white)
  {
  if(readAnalogSensorData(3)>value_for_black)
{
  if(readAnalogSensorData(2)>value_for_black)
  {
     motorBackward(4, 50);
     motorForward(5, 50);
  }  
}  
  }
  else
    turn_right_reverse();
}
  
